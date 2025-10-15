#version 330 core
layout(location = 0) in vec2 quad_position;   //Position of vertex (in screen corner)

uniform vec2 bounds;        //Bounds of simualtion given as (x_range, y_range). Given in screen space (-1 to 1)

void main()
{
    //Convert to world space (scale using bounds)
    gl_Position = vec4(quad_position * bounds, 0.0, 1.0);
}