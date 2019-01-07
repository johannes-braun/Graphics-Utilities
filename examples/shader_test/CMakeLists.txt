include(spv/spv_to_dll)
file(GLOB_RECURSE SPVGEN_FILES *.vert *.frag *.tese *.tesc *.geom *.comp)
add_shader(test_shader ${SPVGEN_FILES})