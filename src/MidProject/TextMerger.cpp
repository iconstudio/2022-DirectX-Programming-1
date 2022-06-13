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
	std::vector<std::string> result{};
	auto builder = std::back_inserter(result);

	std::transform(myText.cbegin(), myText.cend(), builder
		, [&](const char one) -> std::string {
		return std::string(Build(one));
	});

	return result;
}

void TextMerger::Start(P3DDevice device, P3DGrpCommandList cmdlist)
{
	RawMesh raw_text{};

	std::for_each(myStructure.cbegin(), myStructure.cend()
		, [&](const std::string& structure) {
		float dx = 0.0f;
		float dy = 0.0f;

		std::for_each(structure.cbegin(), structure.cend()
			, [&](const unsigned char& one) {
			if ('95' == one)
			{
				const auto positions = MakeRect(XMFLOAT3(dx, dy, 0.0f));

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

std::array<XMFLOAT3, 4> TextMerger::MakeRect(const XMFLOAT3 offset) const
{
	std::array<XMFLOAT3, 4> result{};

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
กเกเกเกเกเ\
กเกเกเกเกเ\
กเกเกเกเกเ\
กเกเกเกเกเ\
กเกเกเกเกเ";
		}
		break;

		case '?':
		{
			return "\
กแกแกแกแกแ\
กเกเกเกเกแ\
กเกเกแกแกแ\
กเกเกเกเกเ\
กเกเกแกเกเ";
		}
		break;

		case '(':
		{
			return "\
กเกเกแกเกเ\
กเกแกเกเกเ\
กแกแกเกเกเ\
กเกแกเกเกเ\
กเกเกแกเกเ";
		}
		break;

		case ')':
		{
			return "\
กเกเกแกเกเ\
กเกเกเกแกเ\
กเกเกเกแกแ\
กเกเกเกแกเ\
กเกเกแกเกเ";
		}
		break;
		case '-':
		{
			return "\
กเกเกเกเกเ\
กเกเกเกเกเ\
กแกแกแกแกแ\
กเกเกเกเกเ\
กเกเกเกเกเ";
		}
		break;

		case '!':
		{
			return "\
กเกเกแกเกเ\
กเกเกแกเกเ\
กเกเกแกเกเ\
กเกเกเกเกเ\
กเกเกแกเกเ";
		}
		break;

		case '0':
		{
			return "\
กแกแกแกแกแ\
กแกเกเกเกแ\
กแกเกเกเกแ\
กแกเกเกเกแ\
กแกแกแกแกแ";
		}
		break;

		case '1':
		{
			return "\
กแกแกแกเกเ\
กเกเกแกเกเ\
กเกเกแกเกเ\
กเกเกแกเกเ\
กแกแกแกแกแ";
		}
		break;

		case '2':
		{
			return "\
กแกแกแกแกแ\
กเกเกเกเกแ\
กแกแกแกแกแ\
กแกเกเกเกเ\
กแกแกแกแกแ";
		}
		break;

		case '3':
		{
			return "\
กแกแกแกแกแ\
กเกเกเกเกแ\
กแกแกแกแกแ\
กเกเกเกเกแ\
กแกแกแกแกแ";
		}
		break;

		case '4':
		{
			return "\
กแกเกเกแกเ\
กแกเกเกแกเ\
กแกเกเกแกเ\
กแกแกแกแกแ\
กเกเกเกแกเ";
		}
		break;

		case '5':
		{
			return "\
กแกแกแกแกแ\
กแกเกเกเกเ\
กแกแกแกแกแ\
กเกเกเกเกแ\
กแกแกแกแกแ";
		}
		break;

		case '6':
		{
			return "\
กแกแกแกแกแ\
กแกเกเกเกเ\
กแกแกแกแกแ\
กแกเกเกเกแ\
กแกแกแกแกแ";
		}
		break;

		case '7':
		{
			return "\
กแกแกแกแกแ\
กเกเกเกเกแ\
กเกเกเกเกแ\
กเกเกเกเกแ\
กเกเกเกเกแ";
		}
		break;

		case '8':
		{
			return "\
กแกแกแกแกแ\
กแกเกเกเกแ\
กแกแกแกแกแ\
กแกเกเกเกแ\
กแกแกแกแกแ";
		}
		break;

		case '9':
		{
			return "\
กแกแกแกแกแ\
กแกเกเกเกแ\
กแกแกแกแกแ\
กเกเกเกเกแ\
กแกแกแกแกแ";
		}
		break;

		case 'A':
		{
			return "\
กเกแกแกแกเ\
กแกเกเกเกแ\
กแกแกแกแกแ\
กแกเกเกเกแ\
กแกเกเกเกแ";
		}
		break;

		case 'B':
		{
			return "\
กแกแกแกแกเ\
กแกเกเกเกแ\
กแกแกแกแกเ\
กแกเกเกเกแ\
กแกแกแกแกเ";
		}
		break;

		case 'C':
		{
			return "\
กเกแกแกแกแ\
กแกเกเกเกเ\
กแกเกเกเกเ\
กแกเกเกเกเ\
กเกแกแกแกแ";
		}
		break;

		case 'D':
		{
			return "\
กแกแกแกแกเ\
กแกเกเกเกแ\
กแกเกเกเกแ\
กแกเกเกเกแ\
กแกแกแกแกเ";
		}
		break;

		case 'E':
		{
			return "\
กแกแกแกแกแ\
กแกเกเกเกเ\
กแกแกแกแกแ\
กแกเกเกเกเ\
กแกแกแกแกแ";
		}
		break;

		case 'F':
		{
			return "\
กแกแกแกแกแ\
กแกเกเกเกเ\
กแกแกแกแกแ\
กแกเกเกเกเ\
กแกเกเกเกเ";
		}
		break;

		case 'G':
		{
			return "\
กแกแกแกแกแ\
กแกเกเกเกเ\
กแกเกเกแกแ\
กแกเกเกเกแ\
กแกแกแกแกแ";
		}
		break;

		case 'H':
		{
			return "\
กแกเกเกเกแ\
กแกเกเกเกแ\
กแกแกแกแกแ\
กแกเกเกเกแ\
กแกเกเกเกแ";
		}
		break;

		case 'I':
		{
			return "\
กแกแกแกแกแ\
กเกเกแกเกเ\
กเกเกแกเกเ\
กเกเกแกเกเ\
กแกแกแกแกแ";
		}
		break;

		case 'J':
		{
			return "\
กแกแกแกแกแ\
กเกเกแกเกเ\
กเกเกแกเกเ\
กเกเกแกเกเ\
กแกแกแกเกเ";
		}
		break;

		case 'K':
		{
			return "\
กแกเกเกเกแ\
กแกเกเกแกเ\
กแกเกแกเกเ\
กแกแกเกแกเ\
กแกเกเกเกแ";
		}
		break;

		case 'L':
		{
			return "\
กแกเกเกเกเ\
กแกเกเกเกเ\
กแกเกเกเกเ\
กแกเกเกเกเ\
กแกแกแกแกแ";
		}
		break;

		case 'M':
		{
			return "\
กแกแกแกแกแ\
กแกเกแกเกแ\
กแกเกแกเกแ\
กแกเกแกเกแ\
กแกเกแกเกแ";
		}
		break;

		case 'N':
		{
			return "\
กแกเกเกเกแ\
กแกแกเกเกแ\
กแกเกแกเกแ\
กแกเกเกแกแ\
กแกเกเกเกแ";
		}
		break;

		case 'O':
		{
			return "\
กเกแกแกแกเ\
กแกเกเกเกแ\
กแกเกเกเกแ\
กแกเกเกเกแ\
กเกแกแกแกเ";
		}
		break;

		case 'P':
		{
			return "\
กแกแกแกแกเ\
กแกเกเกเกแ\
กแกแกแกแกเ\
กแกเกเกเกเ\
กแกเกเกเกเ";
		}
		break;

		case 'Q':
		{
			return "\
กเกแกแกแกเ\
กแกเกเกเกแ\
กแกเกแกเกแ\
กแกเกเกแกแ\
กเกแกแกเกแ";
		}
		break;

		case 'R':
		{
			return "\
กแกแกแกแกเ\
กแกเกเกเกแ\
กแกแกแกแกเ\
กแกเกเกเกแ\
กแกเกเกเกแ";
		}
		break;

		case 'S':
		{
			return "\
กแกแกแกแกแ\
กแกเกเกเกเ\
กแกแกแกแกแ\
กเกเกเกเกแ\
กแกแกแกแกแ";
		}
		break;

		case 'T':
		{
			return "\
กแกแกแกแกแ\
กเกเกแกเกเ\
กเกเกแกเกเ\
กเกเกแกเกเ\
กเกเกแกเกเ";
		}
		break;

		case 'U':
		{
			return "\
กแกเกเกเกแ\
กแกเกเกเกแ\
กแกเกเกเกแ\
กแกเกเกเกแ\
กเกแกแกแกเ";
		}
		break;

		case 'V':
		{
			return "\
กแกเกเกเกแ\
กแกเกเกเกแ\
กแกเกเกเกแ\
กเกแกเกแกเ\
กเกเกแกเกเ";
		}
		break;

		case 'W':
		{
			return "\
กแกเกแกเกแ\
กแกเกแกเกแ\
กแกเกแกเกแ\
กแกแกแกแกแ\
กเกแกเกแกเ";
		}
		break;

		case 'X':
		{
			return "\
กแกเกเกเกแ\
กเกแกเกแกเ\
กเกเกแกเกเ\
กเกแกเกแกเ\
กแกเกเกเกแ";
		}
		break;

		case 'Y':
		{
			return "\
กแกเกเกเกแ\
กเกแกเกแกเ\
กเกเกแกเกเ\
กเกเกแกเกเ\
กเกเกแกเกเ";
		}
		break;

		case 'Z':
		{
			return "\
กแกแกแกแกแ\
กเกเกเกแกเ\
กเกเกแกเกเ\
กเกแกเกเกเ\
กแกแกแกแกแ";
		}
		break;

		default:
		{
			return "\
กเกเกเกเกเ\
กเกเกเกเกเ\
กเกเกเกเกเ\
กเกเกเกเกเ\
กเกเกเกเกเ";
		}
		break;
	}

	return nullptr;
}
