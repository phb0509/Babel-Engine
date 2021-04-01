#pragma once

struct FontBrushDesc
{
	Float4 color;
	ID2D1SolidColorBrush* brush;

	bool operator==(const FontBrushDesc& value)
	{
		return color.x == value.color.x && color.y == value.color.y &&
			color.z == value.color.z && color.w == value.color.w;
	}
};

struct FontTextDesc
{
	wstring font;
	float fontSize;

	DWRITE_FONT_WEIGHT weight;
	DWRITE_FONT_STYLE style;
	DWRITE_FONT_STRETCH stretch;

	IDWriteTextFormat* format;

	bool operator==(const FontTextDesc& value)
	{
		bool b = true;
		b &= font == value.font;
		b &= fontSize == value.fontSize;
		b &= weight == value.weight;
		b &= style == value.style;
		b &= stretch == value.stretch;

		return b;
	}
};

class DirectWrite : public Singleton<DirectWrite>
{
private:
	friend class Singleton;

	ID2D1Factory1* factory;
	IDWriteFactory* writeFactory;

	ID2D1Device* device;
	ID2D1DeviceContext* context;

	ID2D1Bitmap1* targetBitmap;

	vector<FontBrushDesc> fontBrush;
	vector<FontTextDesc> fontText;

	DirectWrite();
	~DirectWrite();

public:
	void RenderText(wstring text, RECT rect, float size = 20.0f,
		wstring font = L"배달의 민족 한나는 열한살", Float4 color = Float4(1, 1, 1, 1),
		DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH stretch = DWRITE_FONT_STRETCH_NORMAL);

	ID2D1DeviceContext* GetDC() { return context; }
};
