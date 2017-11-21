#version 400
in vec3 position;
in vec3 normal;
in vec2 texUV;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightPos;
out vec3 out_normal;
out vec2 textureUV;

out vec3 vertexPositionWorldSpace;

void main(void)
{
   vec4 a = vec4(position, 1.0);

   //transform the vertices by the modelMatrix
   vertexPositionWorldSpace = (modelMatrix * a).xyz;
   a =  projectionMatrix * viewMatrix * modelMatrix * a;
   textureUV = texUV;
   out_normal = (modelMatrix * vec4(normal, 0)).xyz;

   gl_Position = a;
} 