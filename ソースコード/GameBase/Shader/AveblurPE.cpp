#include <DxLib.h>
#include "AveblurPE.h"

AveblurPE::AveblurPE(int postPS, Vector2 pos, Vector2 rate) :PEBase(postPS, pos, rate)
{
}

AveblurPE::~AveblurPE()
{
}

void AveblurPE::Update(float delta)
{
}

void AveblurPE::Draw(int beforeScr, int afterScr, int depth, int skyScr, int redScr)
{
	SetDrawScreen(afterScr);
	ClsDrawScreen();
	SetPostEffect(beforeScr,-1,-1,-1, postPS_, pos_, rate_);
}
