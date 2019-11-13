#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 MVP;
uniform vec4 ourColor;

out vec4 outColor;

void main()
{
    // note that we read the multiplication from right to left
    gl_Position = MVP * vec4(aPos, 1.0);
    float offset = 0.01 * mod(floor(gl_VertexID / 4), 3.0);
    outColor = vec4(ourColor[0] + offset, ourColor[1] + offset, ourColor[2] + offset, ourColor[3]);
}