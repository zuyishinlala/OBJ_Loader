#version 330 core

uniform vec3 fillColor;

out vec4 FragColor;


void main()
{
    FragColor = vec4(fillColor, 1.0);
}
