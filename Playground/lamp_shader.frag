#version 330 core
out vec4 color;

uniform vec3 lampColor;

void main()
{
    vec4 theColor = vec4(lampColor, 1.0);
    color = theColor;
}
//0.82 0.26 0.0
//1.0 0.84 0.2
