/*=======================================
	EggWatcher : 
		ShadowMap Generator 
=======================================*/

#version 120

//================================
// Program-defined 
//================================

void main()
{
		gl_FragDepth = gl_FragCoord.z / gl_FragCoord.w + 0.0005;
		//gl_FragColor = vec4(vNrm*0.5+0.5,1.0);
}