#version 150

// OF default uniforms and attributes
uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec3 normal;

// App uniforms and attributes
uniform mat4 uNormalMatrix;

out vec3 vNormal;

void main()
{
	// Normal matrix is 4x4 but we only need 3x3
	vNormal = (uNormalMatrix * vec4(normal, 0.0)).xyz;
	vNormal = normalize(vNormal);
	vNormal = vNormal * 0.5 + 0.5;
	
	gl_Position = modelViewProjectionMatrix * position;
}

