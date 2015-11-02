//============================================================
//	Main_VS.hlsl
//
//  This vertex shader will: transform the position of a 
//  vertex from object space to screen space, transform the 
//  normal vector from object space to world space, passes the
//  texture coordinate through as-is.
//============================================================

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
// constant buffers batch global data and send to the video card
// pack things together that change at the same frequency
cbuffer cbMatrices : register( b0 )
{
	matrix		g_mWorldViewProjection	: packoffset( c0 );
	matrix		g_mWorld				: packoffset( c4 );
};

cbuffer cbLights : register( b1 )
{
	// lighting data
	float4		g_LightDiffuse[8];           // Light's diffuse color - we can support up to eight lights with this shader.
	float4		g_LightDir[8];               // Light's direction in world space
	float4		g_fAmbient;
	int			g_nNumLights;
};

cbuffer cbObjectColors : register( b2 )
{
	// how the vertex reacts to lighting
	float4		g_vDiffuseObjectColor	: packoffset( c0 );
	float4		g_vAmbientObjectColor	: packoffset( c1 );
	bool		g_bHasTexture			: packoffset( c2.x );
};

//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 vPosition	: POSITION; // POSITION = semantics. to link to data from C++
	float3 vNormal		: NORMAL;
	float2 vTexcoord	: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 vDiffuse		: COLOR0;
	float2 vTexcoord	: TEXCOORD0;
	float4 vPosition	: SV_POSITION; // system value semantic. tells the shader this is the pixel location for the PS
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT Main_VS( VS_INPUT Input )
{
	VS_OUTPUT Output;
	
    float3 vNormalWorldSpace;
	float dotProduct;
	float4 dottedLightColor;

	// multiply the vertex by the world/view/projection matrix to get its coord in screen space
	Output.vPosition = mul( Input.vPosition, g_mWorldViewProjection );

	// convert the normal vector from object to world space
	vNormalWorldSpace = mul( Input.vNormal, (float3x3)g_mWorld );
	Output.vTexcoord = Input.vTexcoord;

    // compute simple directional lighting equation
    float4 vTotalLightDiffuse = float4(0,0,0,0);
    for(int i=0; i<g_nNumLights; i++ )
	{
		// take dot product of normal vector and light direction
		dotProduct = dot(vNormalWorldSpace, g_LightDir[i]);
		dotProduct = max(0, dotProduct);

		// scale the lights contributed diffuse value by the angle of the light
		dottedLightColor = g_LightDiffuse[i] * dotProduct;
		// add this to the total light color
        vTotalLightDiffuse += dottedLightColor;
	}

	// final color for the vertex
    Output.vDiffuse.rgb = g_vDiffuseObjectColor * vTotalLightDiffuse + 
                         g_vAmbientObjectColor * g_fAmbient;   

    Output.vDiffuse.a = g_vDiffuseObjectColor.a; 
	
	return Output;
}

