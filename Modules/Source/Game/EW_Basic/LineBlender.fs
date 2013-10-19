/*=======================================
	EggWatcher : 
		Cell-Shading : 
			Blend lines
=======================================*/

#version 120

//================================
// Inputs
//================================

uniform sampler2D uColorBuffer;
uniform sampler2D uLineBuffer;

varying vec2 vTexCoords;

//================================
// Output
//================================

//[ 0 ] : Line (RGB)


void main()
{	
	vec3 Color = texture2D(uColorBuffer, vTexCoords).xyz;
	gl_FragColor =  texture2D(uColorBuffer, vTexCoords) * texture2D(uLineBuffer, vTexCoords).x;
}