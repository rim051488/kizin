#include <array>
#include <DxLib.h>
#include "ColortoMono.h"

ColortoMono::ColortoMono(int postPS, Vector2 pos, Vector2 rate):PEBase(postPS, pos, rate)
{
	timeBuff_ = CreateShaderConstantBuffer(sizeof(float) * 4);
	time_ = static_cast<float*>(GetBufferShaderConstantBuffer(timeBuff_));
	time = 0.0f;
}

ColortoMono::~ColortoMono()
{
}

void ColortoMono::Update(float delta)
{
	if (time < 1.0f)
	{
		time += delta /2;
	}
	else
	{
		time = 1.0f;
	}
}

void ColortoMono::Draw(int beforeScr, int afterScr, int depth, int skyScr, int redScr)
{
	SetDrawScreen(afterScr);
	ClsDrawScreen();
	time_[0] = time;
	SetBuffer(timeBuff_);
	SetPostEffect(beforeScr, -1, -1, -1, postPS_, pos_, rate_);
}
