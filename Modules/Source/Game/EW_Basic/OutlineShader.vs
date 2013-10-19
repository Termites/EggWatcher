/*=======================================
	EggWatcher : 
		Outline Shader
=======================================*/

#version 120

//================================
// Program-defined 
//================================

uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;

//================================
// Vertex attributes 
//================================

attribute vec3 aLocation;
attribute vec3 aNormal;
attribute vec2 aTexCoords;

//================================
// Output
//================================

varying vec4 vViewLocation;

void main()
{	
	gl_Position = uProjectionMatrix *  uModelViewMatrix * vec4(aLocation + aNormal*0.02  ,1.0);
}