/*=======================================
	EggWatcher : 
		FXAAShader
	From https://code.google.com/p/processing/source/browse/trunk/processing/java/libraries/opengl/examples/Shaders/FXAA/data/fxaa.glsl?r=9668
=======================================*/

uniform sampler2D uSampleBuffer;
uniform vec2 uTexelSize;

varying vec2 vTexCoords;


float GetLuma(in vec3 Color)
{
	/*
		From http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf
	*/
	
	return Color.y * (0.587/0.299) + Color.x;
}


void main()
{
	const float FXAA_SPAN_MAX = 2.0;
	const float FXAA_REDUCE_MUL = 1.0/8.0;
	const float FXAA_REDUCE_MIN = (1.0/512.0);
	
	vec3 Sample0 = texture2D(uSampleBuffer, vTexCoords + (vec2(-1,-1) * uTexelSize)).xyz;
	vec3 Sample1 = texture2D(uSampleBuffer, vTexCoords + (vec2( 1,-1) * uTexelSize)).xyz;
	vec3 Sample2 = texture2D(uSampleBuffer, vTexCoords + (vec2(-1, 1) * uTexelSize)).xyz;
	vec3 Sample3 = texture2D(uSampleBuffer, vTexCoords + (vec2( 1, 1) * uTexelSize)).xyz;
	vec3 Sample4 = texture2D(uSampleBuffer, vTexCoords ).xyz;
	
	float Luma0 = GetLuma(Sample0);
	float Luma1 = GetLuma(Sample1);
	float Luma2 = GetLuma(Sample2);
	float Luma3 = GetLuma(Sample3);
	float Luma4 = GetLuma(Sample4);
	
	float LumaMin = min(Luma4, min(min(Luma3,Luma2), min(Luma1,Luma0)));
	float LumaMax = max(Luma4, max(max(Luma3,Luma2), max(Luma1,Luma0)));
	
	vec2 dir;
	dir.x = - ((Luma0 + Luma1) - (Luma2 + Luma3));
	dir.y =   ((Luma0 + Luma2) - (Luma1 + Luma3));
	
	float DirReduce = max((Luma0 + Luma1 + Luma2 + Luma3) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
	
	float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + DirReduce);
	
	dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), 
		  max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir*rcpDirMin)) * uTexelSize;
	
	vec3 RGBA = 0.5 * (texture2D(uSampleBuffer, vTexCoords + dir * (1.0/3.0 - 0.5)).xyz + 
					   texture2D(uSampleBuffer, vTexCoords + dir * (2.0/3.0 - 0.5)).xyz );
					
	vec3 RGBB = RGBA * 0.5 + 0.25 * (texture2D(uSampleBuffer, vTexCoords + dir * (- 0.5    )).xyz + 
									 texture2D(uSampleBuffer, vTexCoords + dir * (1.0 - 0.5)).xyz );		

	float LumaB = GetLuma(RGBB); 
	
	if ((LumaB < LumaMin) || (LumaB > LumaMax))
		gl_FragColor.xyz = RGBA;
	else
		gl_FragColor.xyz = RGBB;
	
	gl_FragColor.w = 1.0;
}
