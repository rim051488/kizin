#include <DxLib.h>
#include "NoPE.h"

NoPE::NoPE(int postPS, Vector2 pos, Vector2 rate):PEBase(postPS,pos,rate)
{
}

NoPE::~NoPE()
{
}

void NoPE::Update(float delta)
{
}

void NoPE::Draw(int beforeScr, int afterScr, int depth, int skyScr, int redScr)
{
	SetDrawScreen(afterScr);
	ClsDrawScreen();
	DefaultScreen(beforeScr,afterScr, pos_, rate_);
}
