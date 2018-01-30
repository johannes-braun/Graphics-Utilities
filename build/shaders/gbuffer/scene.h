#pragma once

struct scene
{
    mat4 view;
    mat4 projection;
    mat4 view_projection;
    mat4 inverse_view;
    mat4 inverse_projection;
    mat4 inverse_view_projection;

    float time;
    float sin_time;
};