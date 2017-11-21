#version 400
in vec2 textureUV;
in vec3 out_normal;
uniform sampler2D mytexture;
uniform vec3 lightPos;
in vec3 vertexPositionWorldSpace;

void main (void)  
{     
	vec3 lightNormal = normalize(lightPos - vertexPositionWorldSpace);
	float lightAmount = dot(out_normal, lightNormal);			// out and light normals should already be normalized before dot producting them
	lightAmount = clamp(lightAmount, 0,1);
	vec4 Color = texture(mytexture, textureUV);  
	gl_FragColor = vec4(Color.rgb * lightAmount, Color.a);
}