#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in uint aIndex;

uniform mat4 MVP;
uniform vec4 ourColor;

void main()
{
    // note that we read the multiplication from right to left
    gl_Position = MVP * vec4(aPos, 1.0);
}