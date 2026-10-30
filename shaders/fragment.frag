#version 330 core
out vec4 FragColour;

in vec2 TexCoord;
in vec3 VertexColour;

uniform sampler2D texture1;

uniform int currentMode;
uniform int nextMode;
uniform float blendAmount;

vec3 getModeColour(int mode) {
    if (mode == 1) {
        return VertexColour;
    }
    if (mode == 2) {
        return texture(texture1, TexCoord).rgb;
    }
    return vec3(0.7f); // light grey
}

void main()
{
    vec3 currentColour = getModeColour(currentMode);
    vec3 nextColour = getModeColour(nextMode);

    FragColour = vec4(mix(currentColour, nextColour, blendAmount), 1.0f);
}

