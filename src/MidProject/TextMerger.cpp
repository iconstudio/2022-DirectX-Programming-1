#include "pch.hpp"
#include "TextMerger.hpp"
#include "Vertex.hpp"

TextMerger::TextMerger(const char* text, float size, XMVECTORF32 color)
	: myText(text), lineWidth(size), myColour(color)
	, myStructure(Awake())
	, textWidth(size * 7), textHeight(size * 7)
	, myMerger(nullptr)
{
	myStructure = Awake();
}

std::vector<std::string> TextMerger::Awake()
{
	auto builder = std::back_inserter(myStructure);

	std::transform(myText.cbegin(), myText.cend(), builder
		, [&](const char one) -> std::string {
			return std::string(Build(one));
		});
}

void TextMerger::Start(P3DDevice device, P3DGrpCommandList cmdlist)
{
	RawMesh raw_text{};

	std::for_each(myStructure.cbegin(), myStructure.cend()
		, [&](const std::string& structure) {
			float dx = 0.0f;
			float dy = 0.0f;

			std::for_each(structure.cbegin(), structure.cend()
				, [&](const char& one) {
					if ('��' == one)
					{
						auto positions = MakeRect(XMFLOAT3(dx, dy, 0.0f));

						for (int i = 0; i < 4; ++i)
						{
							const auto& pos = positions[i];
							raw_text.AddVertex(CDiffusedVertex{ pos, myColour });
						}
					}

					dx += lineWidth;
					if (textWidth <= dx)
					{
						dx = 0.0f;
						dy += lineWidth;
					}
				});
		});

	myMerger = new CPlainMesh(device, cmdlist, raw_text);
}

const XMFLOAT3* TextMerger::MakeRect(const XMFLOAT3 offset) const
{
	XMFLOAT3 result[4]{};

	result[0] = offset;

	result[1] = offset;
	result[1].x += (lineWidth);

	result[2] = offset;
	result[2].x += (lineWidth);
	result[2].y += (lineWidth);

	result[3] = offset;
	result[3].y += (lineWidth);

	return result;
}

const char* TextMerger::Build(const char one) const
{
	switch (toupper(one))
	{
		case ' ':
		{
			return "\
������\
������\
������\
������\
������";
		}
		break;

		case '-':
		{
			return "\
������\
������\
������\
������\
������";
		}
		break;

		case '!':
		{
			return "\
������\
������\
������\
������\
������";
		}
		break;

		case '0':
		{
			return "\
������\
������\
������\
������\
������";
		}
		break;

		case '1':
		{
			return "\
������\
������\
������\
������\
������";
		}
		break;

		case '2':
		{
			return "\
������\
������\
������\
������\
������";
		}
		break;

		case '3':
		{
			return "\
������\
������\
������\
������\
������";
		}
		break;

		case '4':
		{
			return "\
������\
������\
������\
������\
������";
		}
		break;

		case '5':
		{
			return "\
������\
������\
������\
������\
������";
		}
		break;

		case '6':
		{
			return "\
������\
������\
������\
������\
������";
		}
		break;

		case '7':
		{
			return "\
������\
������\
������\
������\
������";
		}
		break;

		case '8':
		{
			return "\
������\
������\
������\
������\
������";
		}
		break;

		case '9':
		{
			return "\
������\
������\
������\
������\
������";
		}
		break;

		case 'A':
		{
			return "\
������\
������\
������\
������\
������";
		}
		break;

		default:
		{
			return "\
������\
������\
������\
������\
������";
		}
		break;
	}

	return nullptr;
}
