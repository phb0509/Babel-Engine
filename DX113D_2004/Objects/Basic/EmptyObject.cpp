#include "Framework.h"

EmptyObject::EmptyObject()
{
}

EmptyObject::~EmptyObject()
{
}

void EmptyObject::Update()
{
	UpdateWorld();
}

void EmptyObject::Render()
{
	SetWorldBuffer();
}
