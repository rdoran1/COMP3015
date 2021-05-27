#version 460

layout (location = 0) in vec3 VertexPosition; 
layout (location = 1) in vec3 VertexNormal; 

out vec3 Position;
out vec3 Normal;

uniform mat4 ModelViewMatrix;   //model view matrix
uniform mat3 NormalMatrix;		//normal matrix
uniform mat4 MVP;				//model view projection matrix
 

void main() 
{ 
Normal = normalize( NormalMatrix * VertexNormal);
Position = ( ModelViewMatrix * vec4(VertexPosition,1.0) ).xyz;
gl_Position = MVP * vec4(VertexPosition,1.0);
} 

