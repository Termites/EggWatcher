/*=======================================
	EggWatcher : 
		ModelShader
=======================================*/

#version 120

//================================
// Program-defined 
//================================

uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uShadowMatrix;


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
varying vec3 vNormal;
varying vec3 vEyeVec;
varying vec2 vTexCoords;
varying vec4 vShadowCoords;

void main()
{

	mat3 NormalMatrix = mat3(uModelViewMatrix);

	vViewLocation =  uModelViewMatrix * vec4(aLocation  ,1.0) ;
	
	vShadowCoords = (uShadowMatrix * vec4(aLocation,1.0));
	
	vNormal = normalize(NormalMatrix * aNormal);
	
	vEyeVec = normalize(-vViewLocation.xyz);
	
	vTexCoords = aTexCoords;
	
	gl_Position = uProjectionMatrix *  vViewLocation;
}