#version 150

in vec3 vNormal;

out vec4 vFragColor;

void main(void)
{
	vFragColor = vec4(vNormal, 1.0);
	//vFragColor = vec4(1.0);
}
