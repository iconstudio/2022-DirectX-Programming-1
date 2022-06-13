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
					if ('¡á' == one)
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
¡à¡à¡à¡à¡à\
¡à¡à¡à¡à¡à\
¡à¡à¡à¡à¡à\
¡à¡à¡à¡à¡à\
¡à¡à¡à¡à¡à";
		}
		break;

		case '-':
		{
			return "\
¡à¡à¡à¡à¡à\
¡à¡à¡à¡à¡à\
¡á¡á¡á¡á¡á\
¡à¡à¡à¡à¡à\
¡à¡à¡à¡à¡à";
		}
		break;

		case '!':
		{
			return "\
¡à¡à¡á¡à¡à\
¡à¡à¡á¡à¡à\
¡à¡à¡á¡à¡à\
¡à¡à¡à¡à¡à\
¡à¡à¡á¡à¡à";
		}
		break;

		case '0':
		{
			return "\
¡á¡á¡á¡á¡á\
¡á¡à¡à¡à¡á\
¡á¡à¡à¡à¡á\
¡á¡à¡à¡à¡á\
¡á¡á¡á¡á¡á";
		}
		break;

		case '1':
		{
			return "\
¡á¡á¡á¡à¡à\
¡à¡à¡á¡à¡à\
¡à¡à¡á¡à¡à\
¡à¡à¡á¡à¡à\
¡á¡á¡á¡á¡á";
		}
		break;

		case '2':
		{
			return "\
¡á¡á¡á¡á¡á\
¡à¡à¡à¡à¡á\
¡á¡á¡á¡á¡á\
¡á¡à¡à¡à¡à\
¡á¡á¡á¡á¡á";
		}
		break;

		case '3':
		{
			return "\
¡á¡á¡á¡á¡á\
¡à¡à¡à¡à¡á\
¡á¡á¡á¡á¡á\
¡à¡à¡à¡à¡á\
¡á¡á¡á¡á¡á";
		}
		break;

		case '4':
		{
			return "\
¡á¡à¡à¡á¡à\
¡á¡à¡à¡á¡à\
¡á¡à¡à¡á¡à\
¡á¡á¡á¡á¡á\
¡à¡à¡à¡á¡à";
		}
		break;

		case '5':
		{
			return "\
¡á¡á¡á¡á¡á\
¡á¡à¡à¡à¡à\
¡á¡á¡á¡á¡á\
¡à¡à¡à¡à¡á\
¡á¡á¡á¡á¡á";
		}
		break;

		case '6':
		{
			return "\
¡á¡á¡á¡á¡á\
¡á¡à¡à¡à¡à\
¡á¡á¡á¡á¡á\
¡á¡à¡à¡à¡á\
¡á¡á¡á¡á¡á";
		}
		break;

		case '7':
		{
			return "\
¡á¡á¡á¡á¡á\
¡à¡à¡à¡à¡á\
¡à¡à¡à¡à¡á\
¡à¡à¡à¡à¡á\
¡à¡à¡à¡à¡á";
		}
		break;

		case '8':
		{
			return "\
¡á¡á¡á¡á¡á\
¡á¡à¡à¡à¡á\
¡á¡á¡á¡á¡á\
¡á¡à¡à¡à¡á\
¡á¡á¡á¡á¡á";
		}
		break;

		case '9':
		{
			return "\
¡á¡á¡á¡á¡á\
¡á¡à¡à¡à¡á\
¡á¡á¡á¡á¡á\
¡à¡à¡à¡à¡á\
¡á¡á¡á¡á¡á";
		}
		break;

		case 'A':
		{
			return "\
¡á¡á¡á¡á¡á\
¡á¡à¡à¡à¡á\
¡á¡á¡á¡á¡á\
¡á¡à¡à¡à¡á\
¡á¡á¡á¡á¡á";
		}
		break;

		default:
		{
			return "\
¡à¡à¡à¡à¡à\
¡à¡à¡à¡à¡à\
¡à¡à¡à¡à¡à\
¡à¡à¡à¡à¡à\
¡à¡à¡à¡à¡à";
		}
		break;
	}

	return nullptr;
}
