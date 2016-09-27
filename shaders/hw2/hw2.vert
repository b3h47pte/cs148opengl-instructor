#version 330

layout(location = 0) in vec4 vertexPosition;

invariant gl_Position;

void main()
{
    gl_Position = vertexPosition;
}
