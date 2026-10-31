#version 330 core
out vec4 fragColour;

in vec2 texCoord;
in vec3 vertexColour;
in vec3 materialColour;

uniform sampler2D tex;

uniform int currentMode;
uniform int nextMode;
uniform float blendAmount;

vec3 getModeColour(int mode) {
    if (mode == 1) {
        return vertexColour;
    }
    if (mode == 2) {
        return texture(tex, texCoord).rgb;
    }
    //return vec3(0.7f); // light grey
    return materialColour;
}

void main()
{
    vec3 currentColour = getModeColour(currentMode);
    vec3 nextColour = getModeColour(nextMode);

    fragColour = vec4(mix(currentColour, nextColour, blendAmount), 1.0f);
}
