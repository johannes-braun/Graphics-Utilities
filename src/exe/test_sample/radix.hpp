#pragma once
#include <glm/glm.hpp>


namespace radix
{
    using namespace glm;
    /*
    *	Implementation of 'Philippe HELLUY, A portable implementation of the radix sort algorithm in OpenCL, 2011' from https://github.com/phelluy/ocl-radix-sort
    *	(with some changes)
    */


#define BITS   4 // number of bits processed each pass
#define RADIX (1 << BITS) // 2^BITS

    typedef struct
    {
        uint key;
    } Key;

    // way faster than enqueueWriteBuffer. Here is no host->gpu data transfer
    void radixSort_initPermutation(uint
        * permut)
    {
        uint gid = get_global_id(0);
        permut[gid] =
            gid;
    }

    /*
    * w = work_item; w.r number of radix element
    * local_histo: w0.0, w1.0, ... , w0.1, w1.1, ... , w0.2, w1.2, ...
    */
    void radixSort_count(Key
        * keys,
        uint
        * histogram,
        uint
        * local_histo,
        const uint pass,
        const uint offset,
        const uint num_hits,
        const uint num_hits_rounded
    )
    {
        uint gid = get_global_id(0);
        uint lid = get_local_id(0);

        uint group_id = get_group_id(0);
        uint num_groups = get_num_groups(0);
        uint lsize = get_local_size(0);

        // clear local_histo - each work item clears its values
        for (
            uint i = 0u;
            i < RADIX; i++)
            local_histo[
                i *lsize
                    + lid] = 0u;

        barrier(CLK_LOCAL_MEM_FENCE);

        // range of keys for this work item
        uint size = num_hits_rounded / num_groups / lsize; // size of the sub-list
        uint start_pos = gid * size + offset; // beginning of the sub-list

                                              // count keys
        for (
            uint i = 0;
            i<size;
            i++)
        {
            if (start_pos<num_hits)
            {
                uint key = keys[start_pos].key;
                uint short_key = ((key >> (pass * BITS)) & (RADIX - 1u));
                local_histo[
                    short_key *lsize
                        + lid]++;
            }
            ++
                start_pos;
        }

        barrier(CLK_LOCAL_MEM_FENCE);


        // copy result to global mem
        for (
            uint i = 0u;
            i < 16; i++)
            histogram[
                lsize *(i
                    * num_groups + group_id) + lid] = local_histo[
                        i *lsize
                            + lid];
    }

    // algorithm from http://http.developer.nvidia.com/GPUGems3/gpugems3_ch39.html
    void radixSort_scan(uint
        * histogram,
        uint
        * temp,
        uint
        * sum)
    {
        uint gid = get_global_id(0);
        uint lid = get_local_id(0);
        uint group_id = get_group_id(0);

        uint n = get_local_size(0) * 2u;

        // process two items (up sweep phase)
        temp[2u * lid] = histogram[2u * gid];
        temp[2u * lid + 1u] = histogram[2u * gid + 1u];

        // parallel prefix sum (algorithm of Blelloch 1990)
        uint offset = 1u;
        for (
            uint d = n >> 1;
            d > 0; d >>= 1)
        {
            barrier(CLK_LOCAL_MEM_FENCE);
            if (lid<d)
            {
                uint ai = offset * (2u * lid + 1u) - 1u;
                uint bi = offset * (2u * lid + 2u) - 1u;
                temp[bi] += temp[ai];
            }
            offset *= 2u;
        }

        // store last element in global sum
        if (lid == 0u)
        {
            sum[group_id] = temp[n - 1u];
            temp[n - 1u] = 0u;
        }

        // traverse down tree & build scan (down sweep phase)
        for (
            uint d = 1u;
            d<n;
            d *= 2u)
        {
            offset >>= 1;
            barrier(CLK_LOCAL_MEM_FENCE);

            if (lid<d)
            {
                int ai = offset * (2u * lid + 1u) - 1u;
                int bi = offset * (2u * lid + 2u) - 1u;

                uint t = temp[ai];
                temp[ai] = temp[bi];
                temp[bi] +=
                    t;
            }

        }
        barrier(CLK_LOCAL_MEM_FENCE);

        // write results to global memory
        histogram[2u * gid] = temp[2u * lid];
        histogram[2u * gid + 1u] = temp[2u * lid + 1u];

        //barrier(CLK_GLOBAL_MEM_FENCE);
    }

    void radixSort_paste(uint
        * histogram,
        uint
        * sum)
    {


        uint gid = get_global_id(0);
        uint group_id = get_group_id(0);

        uint s = sum[group_id];

        // write results to device memory
        histogram[2u * gid] +=
            s;
        histogram[2u * gid + 1u] +=
            s;
    }

    void radixSort_reorder(Key
        * keys,
        Key
        * out_keys,
        uint
        * histogram,
        uint
        * local_histo,
        uint
        * index,
        uint
        * permut_in,
        uint
        * permut_out,
        const uint pass,
        const uint offset,
        const uint num_hits,
        const uint num_hits_rounded
    )
    {
        uint gid = get_global_id(0);
        uint lid = get_local_id(0);

        uint group_id = get_group_id(0);
        uint num_groups = get_num_groups(0);
        uint lsize = get_local_size(0);

        // range of keys for this work item
        uint size = num_hits_rounded / num_groups / lsize; // size of the sub-list
        uint start_pos = gid * size + offset; // beginning of the sub-list

                                              // save the histogram in the cache
        for (
            int i = 0;
            i < RADIX; i++)
        {
            local_histo[
                i *lsize
                    + lid] = histogram[
                        lsize *(i
                            * num_groups + group_id) + lid];
        }
        barrier(CLK_LOCAL_MEM_FENCE);

        Key key;
        uint short_key, new_pos, index_pos;

        //uint pos = start;
        for (
            int i = 0;
            i<size;
            i++)
        {
            if (start_pos<num_hits)
            {
                // load values
                key = keys[start_pos];
                index_pos = permut_in[start_pos];

                short_key = ((key.key >> (pass * BITS)) & (RADIX - 1u));
                new_pos = local_histo[short_key * lsize + lid] + offset;

                // write result
                out_keys[new_pos] =
                    key;
                index[index_pos] =
                    new_pos;
                permut_out[start_pos] =
                    index_pos;

                ++
                    new_pos;
                local_histo[
                    short_key *lsize
                        + lid] =
                    new_pos;
            }
            ++
                start_pos;
        }
    }
}