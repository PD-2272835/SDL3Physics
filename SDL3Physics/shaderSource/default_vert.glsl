#version 460

//inputs
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uv;

//outputs
layout(location = 0) out vec4 v_color;


//matrix storage buffer
layout(std140, set = 0, binding = 0) readonly buffer ssbo {
	mat4 data[];
};

layout(std140, set = 1, binding = 0) uniform UniformBlock {
	mat4 View;
	mat4 Model;
	float time;
};


//projection matrix is the first item in the matrix storage buffer
mat4 GetProjection()
{
	return data[0];
};

mat4 GetModel(int index)
{
	return data[index];
}


void main()
{
	float pulse = 1;//sin(time * 2.0) * 0.5 + 0.5;
	gl_Position = GetProjection() * View * Model * vec4(a_position, 1.0f); //projection, view, model
	v_color = vec4(a_normal.xyz, a_uv.x * pulse);
}