#version 430
layout(lines) in;
//TODO 
//prima data generati o curba (cu line strip), apoi o suprafata de rotatie/translatie cu triangle_strip
layout(line_strip, max_vertices = 256) out;

uniform mat4 View;
uniform mat4 Projection;
uniform vec3 control_p1, control_p2, control_p3, control_p4;
uniform int no_of_instances;
uniform int no_of_generated_points;


in int instance[2];


vec3 rotateY(vec3 point, float u)
{
	float x = point.x * cos(u) - point.z *sin(u);
	float z = point.x * sin(u) + point.z *cos(u);
	return vec3(x, point.y, z);
}

vec3 translateX(vec3 point, float t)
{
	return vec3(point.x + t, point.y, point.z);
}

vec3 bezier(float t)
{
	return control_p1 * pow((1 - t), 3) + control_p2 * 3 * t * pow((1 - t), 2) + control_p3 * 3 * pow(t, 2) * (1 - t) + control_p4 * pow(t, 3);
}

float da = 2 * 3.14159 / no_of_instances;
float dx = 10f / no_of_instances;
float dt = 1f / (1 + no_of_generated_points);


//TODO - incercati sa creati si o curba Hermite/Bspline

void translatie(){
	for(float t = 0f; t <= 1f; t += dt){
		gl_Position = Projection* View * vec4(translateX(bezier(t), instance[0] * dx), 1);	EmitVertex();
	}

	EndPrimitive();

	for(float t = 0f; t <= 1f; t += dt){
		gl_Position = Projection* View * vec4(translateX(bezier(t), (instance[0] + 1) * dx), 1);	EmitVertex();
	}

	EndPrimitive();

	for(float t = 0f; t <= 1f; t += dt){
		gl_Position = Projection* View * vec4(translateX(bezier(t), instance[0] * dx), 1);	EmitVertex();
		gl_Position = Projection* View * vec4(translateX(bezier(t), (instance[0] + 1) * dx), 1);	EmitVertex();
	}

	EndPrimitive();
}

void rotatie(){
	for(float t = 0f; t <= 1f; t += dt){
		gl_Position = Projection* View * vec4(rotateY(bezier(t), instance[0] * da), 1);	EmitVertex();
	}

	EndPrimitive();

	for(float t = 0f; t <= 1f; t += dt){
		gl_Position = Projection* View * vec4(rotateY(bezier(t), (instance[0] + 1) * da), 1);	EmitVertex();
	}

	EndPrimitive();

	for(float t = 0f; t <= 1f; t += dt){
		gl_Position = Projection* View * vec4(rotateY(bezier(t), instance[0] * da), 1);	EmitVertex();
		gl_Position = Projection* View * vec4(rotateY(bezier(t), (instance[0] + 1) * da), 1);	EmitVertex();
	}

	EndPrimitive();
}

void main()
{
	if (instance[0] < no_of_instances)
	{
		//translatie();
		rotatie();
	}
}
