#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aVertexColour;
layout (location = 4) in vec3 aMaterialColour;

out vec2 texCoord;
out vec3 vertexColour;
out vec3 materialColour;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0f);
   texCoord = aTexCoord;
   vertexColour = aVertexColour;
   materialColour = aMaterialColour;
}
