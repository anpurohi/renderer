Texture2D shaderTexture;
SamplerState SampleType;

struct PixelInputType
{
	float4 position: SV_POSITION;
	float2 tex: TEXCOORD0;
};

float4 texPs_main(PixelInputType input) : SV_TARGET
{
	float4 texColor;

	// Sample the pixel color from the texture,
	// using the sampler at this texture coordinate location
	texColor = shaderTexture.Sample(SampleType, input.tex);

	return texColor;
}