#version 330

uniform float inputTime;

layout(location = 0) in vec4 vertexPosition;

void main()
{
    vec4 modifiedVertexPosition = vertexPosition;

    // Insert your code for "Slightly-More Advanced Shaders" here.
    // Perofmrs an up-down cycle for the triangles once every two seconds.
    float timeInRange = mod(inputTime, 2.0);
    float deltaPosition = -(timeInRange > 1.0 ? 2.0 - timeInRange : timeInRange);
    modifiedVertexPosition.y += deltaPosition;

    gl_Position = modifiedVertexPosition;
}
