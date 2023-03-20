struct PSInput {
	float4 svpos:SV_POSITION;
	float3 pos:POSITION;
	float3 norm:NORMAL;
	float2 uv:TECOORD;
	float4 diff:COLOR0;
	float4 spec:COLOR1;
	float3 tan:TANGENT;
	float3 bin:BINORMAL;
};

SamplerState sam:register(s0);
Texture2D<float4> tex:register(t0);

// �f�B���N�V�������C�g�p�̒萔�o�b�t�@
cbuffer DirectionLightCb : register(b0)
{
	float3 ligDirection;	//���C�g�̕���
	float3 ligColor;		//���C�g�̃J���[
}

float4 main(PSInput input) : SV_TARGET
{
	// Lambert�g�U���˂�K�p���Ă���
	// �s�N�Z���̖@���ƃ��C�g�̕����̓��ς��v�Z����
	float  t = dot(input.norm,ligDirection);
	// ���ς̌��ʂ�-1����Z����
	t *= -1.0f;
	if (t < 0.0f)
	{
		t = 0.0f;
	}
	float3 diffuseLig = ligColor * t;
	float4 finalColor = tex.Sample(sam, input.uv);
	finalColor.xyz *= diffuseLig;
	return finalColor;
}