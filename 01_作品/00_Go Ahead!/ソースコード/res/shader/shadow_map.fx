//--------------------------------------------------------------------------------
// 
// シャドウマップ [sgadow_map.fx]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// Vertex Shader の型定義
//---------------------------------------------------
struct VS_OUT
{
	float4 Pos : POSITION;
	float3 Light : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	float3 View : TEXCOORD2;
	float2 DecalTC : TEXCOORD3;
	float4 ShadowTC : TEXCOORD4;
	float4 DepthValue : TEXCOORD5;
};

//---------------------------------------------------
// グローバル変数宣言
//---------------------------------------------------
float4x4 mW;
float4x4 mWCP;
float4x4 mWLP;
float4x4 mWLPT;

float4 Diffuse;		// 拡散色
float4 Ambient;		// 環境色
float4 Specular;	// 鏡面色
float4 Emissive;	// 放射色
float Power;		// 鏡面の強さ

float4 vLightDir;
float4 vEye;

float TexWidth = 1024;
float TexHeight = 1024;

texture DepthTex;
sampler DepthSampler = sampler_state
{
	Texture = (DepthTex);
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = NONE;
	AddressU = Clamp;
	AddressV = Clamp;
};

texture DecalTexture;
sampler DecalSampler = sampler_state
{
	Texture = (DecalTexture);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;
	AddressU = Clamp;
	AddressV = Clamp;
};

//---------------------------------------------------
// Vertex Shader (シャドウマップ作成用)
//---------------------------------------------------
VS_OUT VS_Depth(float4 Position : POSITION)
{
	VS_OUT Out = (VS_OUT) 0;

	Out.Pos = mul(Position, mWLP);
	Out.DepthValue = Out.Pos;

	return Out;
}

//---------------------------------------------------
// Pixel Shader (シャドウマップ作成用)
//---------------------------------------------------
float4 PS_Depth(VS_OUT In) : COLOR
{
	float4 FinalColor = In.DepthValue.z / In.DepthValue.w;
	return FinalColor;
}

//---------------------------------------------------
// technique (シャドウマップ作成用)
//---------------------------------------------------
technique tecMakeDepthTexture
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS_Depth();
		PixelShader = compile ps_2_0 PS_Depth();
	}
}

//---------------------------------------------------
// Vertex Shader (シーンレンダリング用)
//---------------------------------------------------
VS_OUT VS_Scene(float4 Position : POSITION, float3 Normal : NORMAL, float2 TC : TEXCOORD0)
{
	VS_OUT Out = (VS_OUT) 0;

	Out.Pos = mul(Position, mWCP);
	Out.Light = vLightDir;
	Out.View = vEye - normalize(mul(Position, mW));
	Out.Normal = mul(Normal, mW);
	Out.DecalTC = TC;
	Out.ShadowTC = mul(Position, mWLPT);
	Out.DepthValue = mul(Position, mWLP) - 0.5;
	
	return Out;
}

//---------------------------------------------------
// Pixel Shader (シーンレンダリング用)
//---------------------------------------------------
float4 PS_Scene(VS_OUT In) : COLOR
{
	float3 Normal = normalize(In.Normal);
	float3 LightDir = normalize(In.Light);
	float3 ViewDir = normalize(In.View);

	float4 NL = saturate(dot(Normal, LightDir));
	float3 Reflect = normalize(2 * NL * Normal - LightDir);
	float4 SpecularHighlight = pow(saturate(dot(Reflect, ViewDir)), Power);

	float4 AmbientColor = Ambient;
	float4 DiffuseColor = Diffuse;
	float4 SpecularColor = Specular;
	float4 EmissiveColor = Emissive;

	float4 FinalColor = (DiffuseColor + tex2D(DecalSampler, In.DecalTC)) * NL + SpecularHighlight;
	FinalColor += AmbientColor + EmissiveColor;

	float Shadow = tex2Dproj(DepthSampler, In.ShadowTC).r;
	if (Shadow * In.DepthValue.w < In.DepthValue.z)
	{
		FinalColor /= 4;
	}

	return FinalColor;
}

//---------------------------------------------------
// technique (シーンレンダリング用)
//---------------------------------------------------
technique tecRenderScene
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader = compile ps_2_0 PS_Scene();
	}
}
