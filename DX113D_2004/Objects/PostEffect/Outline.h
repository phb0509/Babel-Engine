#pragma once

class Outline : public UIImage
{
private:
	class OutlineBuffer : public ConstBuffer
	{
	public:
		struct Data
		{
			int value;
			int range;
			Float2 size;
		}data;

		OutlineBuffer() : ConstBuffer(&data, sizeof(Data))
		{
			data.value = 0;
			data.range = 0;
			data.size = { 0, 0 };
		}
	};
	OutlineBuffer* buffer;

public:
	Outline();
	~Outline();

	void Render() override;
};