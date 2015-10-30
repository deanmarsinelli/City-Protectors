//============================================================
//	Main_PS.hlsl
//
//  This pixel shader will combine an objects color,
//  texture color (if it exists), and lighting to,
//  create a final rasterization of each triangle
//============================================================

// GLOBALS
// constant buffers batch global data and send to the video card
// pack things together that change at the same frequency
cbuffer cbObjectColors : register(b0)
{
	// how the vertex reacts to lighting
	float4 g_DiffuseObjectColor	: packoffset(c0);
	float4 g_AmbientObjectColor : packoffset(c1);
	bool   g_HasTexture : packoffset(c2.x);
}

// Textures and samplers
Texture2D g_txDiffuse    : register(t0);
SamplerState g_samLinear : register(s0);


// Input/Output structures
struct PS_INPUT
{
	float4 vDiffuse	 : COLOR0;
	float2 vTexcoord : TEXCOORD0;
};


// Pixel Shader
float4 Main_PS(PS_INPUT Input) : SV_TARGET // sv_target is the final pixel color
{
	float4 vOutputColor;

	// if there is a texture, sample the texture coordinate and multiply it by the diffuse
	// color from the input (vertex shader)
	if (g_HasTexture)
		vOutputColor = g_txDiffuse.Sample(g_samLinear, Input.vTexcoord) * Input.vDiffuse;
	else
		vOutputColor = Input.vDiffuse;

	return vOutputColor;
}
