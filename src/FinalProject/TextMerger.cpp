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
			if ('O' == one)
			{
				const auto positions = MakeRect(XMFLOAT3(dx, dy, 1.0f));

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
XXXXX\
XXXXX\
XXXXX\
XXXXX\
XXXXX";
		}
		break;

		case '?':
		{
			return "\
OOOOO\
XXXXO\
XXOOO\
XXXXX\
XXOXX";
		}
		break;

		case '(':
		{
			return "\
XXOXX\
XOXXX\
OOXXX\
XOXXX\
XXOXX";
		}
		break;

		case ')':
		{
			return "\
XXOXX\
XXXOX\
XXXOO\
XXXOX\
XXOXX";
		}
		break;
		case '-':
		{
			return "\
XXXXX\
XXXXX\
OOOOO\
XXXXX\
XXXXX";
		}
		break;

		case '!':
		{
			return "\
XXOXX\
XXOXX\
XXOXX\
XXXXX\
XXOXX";
		}
		break;

		case '0':
		{
			return "\
OOOOO\
OXXXO\
OXXXO\
OXXXO\
OOOOO";
		}
		break;

		case '1':
		{
			return "\
OOOXX\
XXOXX\
XXOXX\
XXOXX\
OOOOO";
		}
		break;

		case '2':
		{
			return "\
OOOOO\
XXXXO\
OOOOO\
OXXXX\
OOOOO";
		}
		break;

		case '3':
		{
			return "\
OOOOO\
XXXXO\
OOOOO\
XXXXO\
OOOOO";
		}
		break;

		case '4':
		{
			return "\
OXXOX\
OXXOX\
OXXOX\
OOOOO\
XXXOX";
		}
		break;

		case '5':
		{
			return "\
OOOOO\
OXXXX\
OOOOO\
XXXXO\
OOOOO";
		}
		break;

		case '6':
		{
			return "\
OOOOO\
OXXXX\
OOOOO\
OXXXO\
OOOOO";
		}
		break;

		case '7':
		{
			return "\
OOOOO\
XXXXO\
XXXXO\
XXXXO\
XXXXO";
		}
		break;

		case '8':
		{
			return "\
OOOOO\
OXXXO\
OOOOO\
OXXXO\
OOOOO";
		}
		break;

		case '9':
		{
			return "\
OOOOO\
OXXXO\
OOOOO\
XXXXO\
OOOOO";
		}
		break;

		case 'A':
		{
			return "\
XOOOX\
OXXXO\
OOOOO\
OXXXO\
OXXXO";
		}
		break;

		case 'B':
		{
			return "\
OOOOX\
OXXXO\
OOOOX\
OXXXO\
OOOOX";
		}
		break;

		case 'C':
		{
			return "\
XOOOO\
OXXXX\
OXXXX\
OXXXX\
XOOOO";
		}
		break;

		case 'D':
		{
			return "\
OOOOX\
OXXXO\
OXXXO\
OXXXO\
OOOOX";
		}
		break;

		case 'E':
		{
			return "\
OOOOO\
OXXXX\
OOOOO\
OXXXX\
OOOOO";
		}
		break;

		case 'F':
		{
			return "\
OOOOO\
OXXXX\
OOOOO\
OXXXX\
OXXXX";
		}
		break;

		case 'G':
		{
			return "\
OOOOO\
OXXXX\
OXXOO\
OXXXO\
OOOOO";
		}
		break;

		case 'H':
		{
			return "\
OXXXO\
OXXXO\
OOOOO\
OXXXO\
OXXXO";
		}
		break;

		case 'I':
		{
			return "\
OOOOO\
XXOXX\
XXOXX\
XXOXX\
OOOOO";
		}
		break;

		case 'J':
		{
			return "\
OOOOO\
XXOXX\
XXOXX\
XXOXX\
OOOXX";
		}
		break;

		case 'K':
		{
			return "\
OXXXO\
OXXOX\
OXOXX\
OOXOX\
OXXXO";
		}
		break;

		case 'L':
		{
			return "\
OXXXX\
OXXXX\
OXXXX\
OXXXX\
OOOOO";
		}
		break;

		case 'M':
		{
			return "\
OOOOO\
OXOXO\
OXOXO\
OXOXO\
OXOXO";
		}
		break;

		case 'N':
		{
			return "\
OXXXO\
OOXXO\
OXOXO\
OXXOO\
OXXXO";
		}
		break;

		case 'O':
		{
			return "\
XOOOX\
OXXXO\
OXXXO\
OXXXO\
XOOOX";
		}
		break;

		case 'P':
		{
			return "\
OOOOX\
OXXXO\
OOOOX\
OXXXX\
OXXXX";
		}
		break;

		case 'Q':
		{
			return "\
XOOOX\
OXXXO\
OXOXO\
OXXOO\
XOOXO";
		}
		break;

		case 'R':
		{
			return "\
OOOOX\
OXXXO\
OOOOX\
OXXXO\
OXXXO";
		}
		break;

		case 'S':
		{
			return "\
OOOOO\
OXXXX\
OOOOO\
XXXXO\
OOOOO";
		}
		break;

		case 'T':
		{
			return "\
OOOOO\
XXOXX\
XXOXX\
XXOXX\
XXOXX";
		}
		break;

		case 'U':
		{
			return "\
OXXXO\
OXXXO\
OXXXO\
OXXXO\
XOOOX";
		}
		break;

		case 'V':
		{
			return "\
OXXXO\
OXXXO\
OXXXO\
XOXOX\
XXOXX";
		}
		break;

		case 'W':
		{
			return "\
OXOXO\
OXOXO\
OXOXO\
OOOOO\
XOXOX";
		}
		break;

		case 'X':
		{
			return "\
OXXXO\
XOXOX\
XXOXX\
XOXOX\
OXXXO";
		}
		break;

		case 'Y':
		{
			return "\
OXXXO\
XOXOX\
XXOXX\
XXOXX\
XXOXX";
		}
		break;

		case 'Z':
		{
			return "\
OOOOO\
XXXOX\
XXOXX\
XOXXX\
OOOOO";
		}
		break;

		default:
		{
			return "\
XXXXX\
XXXXX\
XXXXX\
XXXXX\
XXXXX";
		}
		break;
	}

	return nullptr;
}
