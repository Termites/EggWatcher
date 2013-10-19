/*=======================================
	EggWatcher : 
		Cell-Shading : 
			Line Tracer
=======================================*/

#version 120

//================================
// Inputs
//================================

uniform sampler2D uDepthBuffer;
uniform sampler2D uNormalBuffer;
uniform vec2 uTexelSize;

varying vec2 vTexCoords;

//================================
// Output
//================================

//[ 0 ] : Line (RGB)


const vec2 Offsets[9] = 
{
	vec2( 0, 0),
	vec2(-1,-1),
	vec2( 0,-1),
	vec2( 1,-1),
	vec2( 1, 0),
	vec2( 1, 1),
	vec2( 0, 1),
	vec2(-1, 1),
	vec2(-1, 0)
	
};

float LinearizeDepth(float D)
{
	const float zNear = 0.1;
	const float zFar = 1000.0;
	return (2.0 * zNear) / ( zFar + zNear - D * (zFar - zNear) );
}


/*
	From Tabula Rasa :
		http://http.developer.nvidia.com/GPUGems3/gpugems3_ch19.html
*/

float GetPixelWeight(in vec2 UV)
{
	float Depth[9];
	vec3 Normal[9];
	
	float W = 1.5;
	
	for (int i = 0;i<9;++i)
	{
		vec2 _uv = UV + Offsets[i] * uTexelSize * W;
		Depth[i] = texture2D(uDepthBuffer, _uv).x;
		Normal[i] = 2.0 * (texture2D(uNormalBuffer,_uv).xyz) - 1.0;
	}
	
	vec4 Deltas1,Deltas2;
	
	Deltas1.x = Depth[1];
	Deltas1.y = Depth[2];
	Deltas1.z = Depth[3];
	Deltas1.w = Depth[4];
	Deltas2.x = Depth[5];
	Deltas2.y = Depth[6];
	Deltas2.z = Depth[7];
	Deltas2.w = Depth[8];
	
	Deltas1 = abs(Deltas1 - Depth[0]);
	Deltas2 = abs(Depth[0] - Deltas2);
	
	vec4 MaxDeltas = max(Deltas1,Deltas2);
	vec4 MinDeltas = max(min(Deltas1,Deltas2),0.00001);
	
	vec4 DepthResult = step(MinDeltas * 25.0,MaxDeltas);
	
	Deltas1.x = dot(Normal[1], Normal[0]);
	Deltas1.y = dot(Normal[2], Normal[0]);
	Deltas1.z = dot(Normal[3], Normal[0]);
	Deltas1.w = dot(Normal[4], Normal[0]);
	Deltas2.x = dot(Normal[5], Normal[0]);
	Deltas2.y = dot(Normal[6], Normal[0]);
	Deltas2.z = dot(Normal[7], Normal[0]);
	Deltas2.w = dot(Normal[8], Normal[0]);
	
	Deltas1 = abs(Deltas1 - Deltas2);

	vec4 NormalResult = step(0.25,Deltas1);
	NormalResult = max(NormalResult, DepthResult);
	
	return (NormalResult.x + NormalResult.y + NormalResult.z + NormalResult.w) * 0.7;
}

void main()
{	
/*
	//vec3 Color = texture2D(uDiffuseBuffer, vTexCoords).xyz;
	//==========================
	// Delta-Depth 
	float W = 2.0;
	float Depth,Depth0,Depth1,Depth2,Depth3;
	
	Depth = LinearizeDepth(texture2D(uDepthBuffer, vTexCoords).x);
	Depth0 =  abs( LinearizeDepth(texture2D(uDepthBuffer, vTexCoords + vec2( 1, 0) * uTexelSize * W).x) - Depth);
	Depth1 =  abs( LinearizeDepth(texture2D(uDepthBuffer, vTexCoords + vec2(-1, 0) * uTexelSize * W).x) - Depth);
	Depth2 =  abs( LinearizeDepth(texture2D(uDepthBuffer, vTexCoords + vec2( 0, 1) * uTexelSize * W).x) - Depth);
	Depth3 =  abs( LinearizeDepth(texture2D(uDepthBuffer, vTexCoords + vec2( 0,-1) * uTexelSize * W).x) - Depth);
	
	
	float MinDepth,MaxDepth;
	
	MaxDepth = max(Depth0, max(Depth1,max(Depth2,Depth3)));
	MinDepth = min(Depth0, min(Depth1,min(Depth2,Depth3)));
	
	//==========================
	
	//==========================
	// Delta-Normal
	vec3 tNormal,tNormal0,tNormal1,tNormal2,tNormal3;
	
	tNormal = texture2D(uNormalBuffer,vTexCoords).xyz*2.0 - 1.0;
	tNormal0 = (texture2D(uNormalBuffer,vTexCoords+ vec2( 1, 0) * uTexelSize * W).xyz)*2.0-1.0;
	tNormal1 = (texture2D(uNormalBuffer,vTexCoords+ vec2(-1, 0) * uTexelSize * W).xyz)*2.0-1.0;
	tNormal2 = (texture2D(uNormalBuffer,vTexCoords+ vec2( 0, 1) * uTexelSize * W).xyz)*2.0-1.0;
	tNormal3 = (texture2D(uNormalBuffer,vTexCoords+ vec2( 0,-1) * uTexelSize * W).xyz)*2.0-1.0;
	
	float fNormal0 = abs(dot(tNormal,tNormal0));
	float fNormal1 = abs(dot(tNormal,tNormal1));
	float fNormal2 = abs(dot(tNormal,tNormal2));
	float fNormal3 = abs(dot(tNormal,tNormal3));
	

	float MinNormal = min(fNormal0, min(fNormal1,min(fNormal2,fNormal3)));
	float MaxNormal = max(fNormal0, max(fNormal1,max(fNormal2,fNormal3)));
	
	float AvgN = (MaxNormal + MinNormal) / 2.0;

	
	float Outline = 0.0; //;

	if (Depth >= 0.9999999 || (MinNormal > 0.4 && MaxNormal > 0.6) )
		Outline = 1.0;
		
	//Outline *= MaxDepth>0.001 ? 0 : 1;*/
	float Outline = 1.0 - GetPixelWeight(vTexCoords);
	gl_FragColor = Outline;

}