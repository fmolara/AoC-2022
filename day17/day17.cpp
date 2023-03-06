// Advent Of Code 2022
// Day 17
//
// (c) 2023 Federico Molara <federico@molara.net>
// This code is licensed under MIT license (see LICENSE.txt for details)

#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <vector>
#include <string>
#include <ranges>
#include <string_view>
#include <algorithm>
#include <functional>
#include <tuple>
#include <map>
#include <set>

#include "..\stuffs\Splitter.h"

#ifdef _DEBUG
#include <assert.h>
#define ASSERT(x)   assert(x)
#else
#define ASSERT(x)   ((void)0)
#endif




struct Coo
{
	int       x;
	long long y;

	Coo() : Coo(0, 0) {}
	Coo(int _x, long long _y) : x(_x), y(_y) {}
	bool operator < (const Coo& o) const { return x < o.x || x == o.x && y < o.y; }
};


typedef std::vector<std::string>  Piece;
std::vector<Piece> pieces =
{
	{ "####" },

	{ " # ",
	  "###",
	  " # ", },

	{ "  #",
	  "  #",
	  "###", },

	{ "#",
	  "#",
	  "#",
	  "#", },

	{ "##",
	  "##", },
};





bool Overlaps(const std::map<Coo, char>& board, const Piece& piece, int x, long long y)
{
	for (size_t py = 0; py < piece.size(); ++py)
		for (size_t px = 0; px < piece[py].size(); ++px)
		{
			if (piece[py][px] == '#'  &&  board.contains(Coo(x+(int)px, y-(long long)py)))
				return true;
		}

	return false;
}

void Copy(std::map<Coo, char>& board, const Piece& piece, int x, long long y)
{
	for (size_t py = 0; py < piece.size(); ++py)
		for (size_t px = 0; px < piece[py].size(); ++px)
		{
			if (piece[py][px] != ' ')
				board.insert(std::make_pair(Coo(x+(int)px, y-(long long)py), '#'));
		}
}

void AddBottom(std::map<Coo, char>& board, long long y_bottom)
{
	for (int x = 0; x < 9; ++x)
		board.insert(std::make_pair(Coo(x, y_bottom), '#'));
}

void AddBorders(std::map<Coo, char>& board, long long y_from, long long y_to)
{
	ASSERT(y_from <= y_to);

	for (long long y = y_from; y <= y_to; ++y)
	{
		board.insert(std::make_pair(Coo(0, y), '#'));
		board.insert(std::make_pair(Coo(8, y), '#'));
	}
}

#include <iomanip>

void Print(const std::map<Coo, char>& board, long long y_last)
{
	std::cout << std::endl;
	std::cout << std::endl;

	for (long long y = y_last+3; y >= 0; --y)
	{
		std::cout << std::setw(3) << y << (y == y_last ? "<< " : "   ");
		for (size_t x = 0; x <= 8; ++x)
		{
			std::cout << (board.contains(Coo((int)x, y)) ? board.at(Coo((int)x, y)) : ' ');
		}
		std::cout << std::endl;
	}
}

bool Memoize(const std::map<Coo, char>& board, size_t cnt, size_t ch_index, size_t piece_index, int x_coo, long long y_coo, size_t& delta_cnt, long long& delta_y_coo)
{
	static std::map<std::tuple<size_t, size_t, int>, std::pair<size_t, long long>> cycles;
	static size_t prev_delta_cnt = 0;
	static long long prev_delta_y_coo = -1;
	static int contigous_cnt = 0;

	const auto key = std::make_tuple(ch_index, piece_index, x_coo);
	if (cycles.contains(key))
	{
		auto value = cycles[key];
		delta_cnt =   cnt   - value.first;
		delta_y_coo = y_coo - value.second;

		if (prev_delta_cnt == delta_cnt && prev_delta_y_coo == delta_y_coo)
		{
			if (++contigous_cnt > 100)
			{
				// Bingo!
				return true;
			}
		}
		else
		{
			prev_delta_cnt = delta_cnt;
			prev_delta_y_coo = delta_y_coo;
			contigous_cnt = 0;
		}
	}
	cycles[key] = std::make_pair(cnt, y_coo);
	return false;
}

long long PartOne(std::istream& in, size_t num_rocks)
{
	std::string line;
	if (!std::getline(in, line))
	{
		ASSERT(false);
		return 0;
	}

	std::map<Coo, char> board;

	int x = 0;
	long long y = 0;
	AddBottom(board, y);

	long long y_last = 0;  // ultima riga piena
	for (size_t i = (size_t)-1, p = 0, cnt = 0; cnt < num_rocks; p = (p + 1) % pieces.size(), ++cnt)
	{
		const Piece &piece = pieces[p];

		AddBorders(board, y_last+1, y_last+7);

		x = 3;  // Each rock appears so that its left edge is two units away from the left wall
		y = y_last + 3 + piece.size();  // its bottom edge is three units above the highest rock in the room
		ASSERT(!Overlaps(board, piece, x, y));

		const size_t firts_ch_index = (i + 1) % line.size();
		for (i = firts_ch_index; true; i = (i + 1) % line.size())
		{
			const auto ch = line[i];
			switch (ch)
			{
			case '<':
				if (!Overlaps(board, piece, x-1, y))
					x -= 1;
				break;
			case '>':
				if (!Overlaps(board, piece, x+1, y))
					x += 1;
				break;
			default:
				ASSERT(false);
			}

			if (!Overlaps(board, piece, x, y-1))
				y -= 1;
			else
				break;
		}

		Copy(board, piece, x, y);
		y_last = std::max(y_last, y);

		size_t delta_cnt; long long delta_y_coo;
		if (Memoize(board, cnt, firts_ch_index, p, x, y, delta_cnt, delta_y_coo))
		{
			const auto remaining = num_rocks - cnt;
			const auto full_cycle = remaining / delta_cnt;
			const auto last_cycle = remaining % delta_cnt;
			if (last_cycle == 0)
				return y_last + (full_cycle * delta_y_coo) - 1;
		}
	}

	return y_last;
}

int main()
{
#if 0
	std::istringstream in(
R"(>>><<><>><<<>><>>><<<>>><<<><<<>><>><<>>
)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	long long height = PartOne(in, 2022);  // 3119
	std::cout << height << std::endl;
#else
	long long height = PartOne(in, 1000000000000);  // 1536994219669
	std::cout << height << std::endl;
#endif
}
