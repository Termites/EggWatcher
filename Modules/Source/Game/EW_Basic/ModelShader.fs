/*=======================================
	EggWatcher : 
		ModelShader
=======================================*/

#version 120

//================================
// Program-defined 
//================================

uniform sampler2D uShadowTexture;
uniform sampler2D uDiffuseTexture;

uniform vec3 uLightDirection;
uniform vec3 uLightColor;

uniform int uOverlayTexturesCount;

uniform vec4 uOverlayTextureOffset0;
uniform vec4 uOverlayTextureOffset1;

uniform sampler2D uOverlayTextureSample0;
uniform sampler2D uOverlayTextureSample1;
//================================
// Inputs
//================================

varying vec4 vViewLocation;
varying vec3 vNormal;
varying vec3 vEyeVec;
varying vec2 vTexCoords;
varying vec4 vShadowCoords;

//================================
// Output
//================================

#define PARAM_FILTER_SHADOWS_ENABLED
#define PARAM_FILTER_DIV 1200.0

//[ 0 ] : Color RGB | Alpha
//[ 1 ] : Normal XYZ | Unused

float Overlay(float Src, float Dst)
{
	if (Src < 0.5)
		return 2 * Src * Dst;
	
	return 1.0 - 2.0 * (1.0 - Src)*(1.0 - Dst);
}

vec3 Overlay(in vec3 Source, in vec3 Target)
{
	return vec3(Overlay(Source.x,Target.x), Overlay(Source.y,Target.y), Overlay(Source.z,Target.z));
}

vec3 LinearBurn(in vec3 Source, in vec3 Target)
{
	return Source + Target - vec3(1.0);
}

void main()
{	
	float nDotL = dot(uLightDirection, vNormal);
	
	
	vec4 Color =texture2D(uDiffuseTexture, vTexCoords); 
	
	Color.xyz = Overlay(Color.xyz, vec3(0.2,0.7,1.0));
	
	//--------------------
	// Blend with overlays
	
	vec4 AddColor;
	
	if (uOverlayTexturesCount == 1)
	{
		AddColor = texture2D ( uOverlayTextureSample0 , (vTexCoords * uOverlayTextureOffset0.zw) + uOverlayTextureOffset0.xy);
		Color.rgb = (1.0-AddColor.w) * Color.rgb + (AddColor.w) * AddColor.rgb;
	}
	/*if (uOverlayTexturesCount  == 2)
	{
		AddColor = texture2D ( uOverlayTextureSample1 , (vTexCoords  * uOverlayTextureOffset1.zw) +  uOverlayTextureOffset1.xy);
		Color.rgb = (1.0-AddColor.w) * Color.rgb + (AddColor.w) * AddColor.rgb;
	}*/
	//--------------------
	
	
	Color.xyz *=  (step(0.15,nDotL) *0.5 + 0.5);//smoothstep(0.153,0.16,nDotL)*0.5 + 0.5; 

	//Color.xyz = LinearBurn(Color.xyz, uLightColor);
	
	if (nDotL >= 0.15)
	{
		#ifdef PARAM_FILTER_SHADOWS_ENABLED
			// 4 sample poisson disk : 
			float z = 0.0;
			float Visibility = 1.0;
			
			z = texture2D( uShadowTexture, vShadowCoords.xy).z;
			if ( z < vShadowCoords.z  )
				Visibility = 0.4;
				
			z = texture2D( uShadowTexture, vShadowCoords.xy + vec2(-0.94201624, -0.39906216 ) / PARAM_FILTER_DIV).z;
			if  ( z < vShadowCoords.z ) 
				Visibility -= 0.1;

			z = texture2D( uShadowTexture, vShadowCoords.xy + vec2(0.94558609, -0.76890725 ) / PARAM_FILTER_DIV).z;
			if  ( z < vShadowCoords.z ) 
				Visibility -= 0.1;

			z = texture2D( uShadowTexture, vShadowCoords.xy + vec2(-0.094184101, -0.92938870 ) / PARAM_FILTER_DIV).z;
			if  ( z < vShadowCoords.z ) 
				Visibility -= 0.1;

			z = texture2D( uShadowTexture, vShadowCoords.xy + vec2(0.34495938, 0.29387760 ) / PARAM_FILTER_DIV).z;
			if  ( z < vShadowCoords.z ) 
				Visibility -= 0.1;			

			Color.xyz *= (Visibility*0.5 + 0.5);
		#else
			float z = texture2D( uShadowTexture, vShadowCoords.xy).z;

			if ( z < vShadowCoords.z  )
				Color.xyz *= vec3(0.5);
		#endif
	}
		
	gl_FragData[0] = Color;
	gl_FragData[1] = vec4( vNormal.xyz*0.5+0.5 , 0.0);

}