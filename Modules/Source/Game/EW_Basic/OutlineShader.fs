/*=======================================
	EggWatcher : 
		Outline Shader
=======================================*/

#version 120

//================================
// Program-defined 
//================================

//================================
// Inputs
//================================

//================================
// Output
//================================


//[ 0 ] : Color RGB | Alpha
//[ 1 ] : Normal XYZ | Unused


void main()
{	
	gl_FragData[0] = vec4( vec3(0.0) , 1.0 );
	gl_FragData[1] = vec4( vec3(0.0) , 1.0 );
}