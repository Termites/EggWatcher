/*=======================================
	EggWatcher : 
		QuadShader
=======================================*/

#version 120

//================================
// Vertex attributes 
//================================

attribute vec3 aLocation;
attribute vec3 aNormal;
attribute vec2 aTexCoords;

//================================
// Output
//================================

varying vec2 vTexCoords;

void main()
{
	vTexCoords = vec2(aLocation.xy * 0.5 + 0.5);
	gl_Position = vec4(aLocation,1.0);
	

}