#version 400 

// Epic (Unreal Engine 4) BRDF from their SIGGRAPH 2013 Course:
// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec4 vertexColor;
layout(location = 4) in vec3 vertexTangent;
layout(location = 5) in vec3 vertexBitangent;

out vec4 fragmentColor;
out vec4 vertexWorldPosition;
out vec3 vertexWorldNormal;
out vec2 fragmentUV;

out vec3 vertexWorldTangent;
out vec3 vertexWorldBitangent;

uniform sampler2D displacementTexture;
uniform bool useDisplacementTexture;
uniform float maxDisplacement;

void main()
{
    vertexWorldPosition =  modelMatrix * vertexPosition;
    vertexWorldNormal = normalize(vec3(transpose(inverse(modelMatrix)) * vec4(vertexNormal, 0)));
    
    float displacement = 0.f;
    if (useDisplacementTexture) {
        displacement = texture(displacementTexture, vertexUV).r * maxDisplacement;
    }
    
    gl_Position = projectionMatrix * viewMatrix * (vertexWorldPosition + vec4(displacement * vertexWorldNormal, 0.f));
    fragmentUV = vertexUV;
    
    vertexWorldTangent = normalize(vec3(transpose(inverse(modelMatrix)) * vec4(vertexTangent, 0)));
    vertexWorldBitangent = normalize(vec3(transpose(inverse(modelMatrix)) * vec4(vertexBitangent, 0)));
}