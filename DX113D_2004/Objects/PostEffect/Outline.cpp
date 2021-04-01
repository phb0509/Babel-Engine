#include "Framework.h"

Outline::Outline()
	: UIImage(L"Outline")
{
	buffer = new OutlineBuffer();
}

Outline::~Outline()
{
	delete buffer;
}

void Outline::Render()
{
	buffer->data.size.x = scale.x;
	buffer->data.size.y = scale.y;

	buffer->SetPSBuffer(10);
	UIImage::Render();

	ImGui::SliderInt("Value", &buffer->data.value, 0, 8);
	ImGui::SliderInt("Range", &buffer->data.range, 0, 8);
	ImGui::ColorEdit4("Color", (float*)&material->GetBuffer()->data.diffuse);
}
