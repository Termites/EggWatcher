/*=======================================
	EggWatcher : 
		ShadowMap Generator 
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

//================================
// Output
//================================

void main()
{
	gl_Position = uProjectionMatrix *  uModelViewMatrix * vec4(aLocation,1.0) ;	
}