struct Input
{
    float3 Position : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float2 UV : TEXCOORD2;
};

struct Output
{
    float2 TexCoord : TEXCOORD0;
    float4 Position : SV_Position;
};

cbuffer uniforms : register(b0, space1)
{
    float4x4 Transformation;
    float4x4 data;
    float time;
}

Output main(Input input)
{
    Output output;
    output.TexCoord = input.UV;
    output.Position = mul(Transformation, float4(input.Position, 1.0f));
    return output;
}