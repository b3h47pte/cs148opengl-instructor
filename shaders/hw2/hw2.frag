#version 330

uniform float inputTime;
out vec4 fragColor;

float ComputeColor(float off) 
{
    float t = 3.0 - mod(inputTime + off, 3.0);
    float color = (t < 1.5 ? t : 3.0 - t) / 1.5;
    return color;
}

void main() 
{
    vec4 finalColor = vec4(1);

    // Insert your code for "Slightly-More Advanced Shaders" here.
    float red = ComputeColor(0.0);
    float green = ComputeColor(1.0);
    float blue = ComputeColor(2.0);
    finalColor.rgb = vec3(red, green, blue);

    fragColor = finalColor;
}
