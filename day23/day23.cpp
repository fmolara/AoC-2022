// Advent Of Code 2022
// Day 23
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
#include <map>
#include <set>

#ifdef _DEBUG
#include <assert.h>
#define ASSERT(x)   assert(x)
#else
#define ASSERT(x)   ((void)0)
#endif



const struct Moves
{
	int iCoo, delta;
}
moves[] =
{
	{ 0, -1 },  // y -= 1  -> North
	{ 0, +1 },  // y += 1  -> South
	{ 1, -1 },  // x -= 1  -> West
	{ 1, +1 },  // x += 1  -> East
};

struct Coo
{
	int coo[2];

	Coo(int row = 0, int col = 0)
	{
		coo[0] = row;
		coo[1] = col;
	}

	Coo& operator += (const Coo& other)
	{
		for (int i = 0; i < _countof(coo); ++i)
			coo[i] += other.coo[i];

		return *this;
	}

	Coo& operator += (const Moves& move)
	{
		coo[move.iCoo] += move.delta;

		return *this;
	}

	Coo operator + (const Moves& move)
	{
		Coo res = *this;
		res += move;
		return res;
	}

	auto operator <=> (const Coo&) const = default;
};

struct Elf
{
	Coo coo;
	int iNextMove;

	Elf(int row, int col)
		: coo(row, col)
	{
		iNextMove = -1;
	}
};

typedef std::vector<Elf> board_t;
typedef std::map<Coo, size_t> index_t;

void ParseBoard(std::istream& in, board_t& board)
{
	board.clear();

	int row = 0;
	for (std::string line; std::getline(in, line); ++row)
	{
		for (int col = 0; col < line.length(); ++col)
		{
			if (line[col] != '#')
				continue;

			board.push_back(Elf(row, col));
		}
	}
}

void MakeIndex(const board_t& board, index_t& index)
{
	index.clear();
	for (size_t i = 0; i < board.size(); ++i)
	{
		ASSERT(index.find(board[i].coo) == index.end());
		index.insert(std::make_pair(board[i].coo, i));
	}
}

int GetNeighbourhoodCount(const index_t& index, const Coo& coo)
{
	static const
	Coo neighbour_moves[] = {
		{ -1, -1 }, { -1,  0 }, { -1, +1 },
		{  0, -1 },             {  0, +1 },
		{ +1, -1 }, { +1,  0 }, { +1, +1 },
	};

	int cnt = 0;
	for (int i = 0; i < _countof(neighbour_moves); ++i)
	{
		Coo _coo = coo;
		_coo += neighbour_moves[i];

		if (index.find(_coo) != index.end())
			++cnt;
	}

	return cnt;
}

int GetDirectionCount(const index_t& index, const Coo& coo, int iMove)
{
	ASSERT(index.find(coo) != index.end());
	ASSERT(0 <= iMove && iMove < _countof(moves));

	const auto& m = moves[iMove];

	int cnt = 0;
	for(int other_coo_delta = -1; other_coo_delta <= 1; ++other_coo_delta)
	{
		Coo _coo = coo;
		_coo.coo[m.iCoo] += m.delta;
		_coo.coo[m.iCoo == 0 ? 1 : 0] += other_coo_delta;

		if (index.find(_coo) != index.end())
			++cnt;
	}

	return cnt;
}

void GetCrop(const board_t& board, Coo& topLeft, Coo& bottomRight)
{
	topLeft =     { INT_MAX, INT_MAX };
	bottomRight = { INT_MIN, INT_MIN };
	for (const Elf& elf : board)
	{
		for (int i = 0; i < _countof(topLeft.coo); ++i)
		{
			topLeft.coo[i] =     std::min(topLeft.coo[i],     elf.coo.coo[i]);
			bottomRight.coo[i] = std::max(bottomRight.coo[i], elf.coo.coo[i]);
		}
	}
}

void Print(const board_t& board, int inflate = 0)
{
	index_t index;
	MakeIndex(board, index);

	Coo cooMin, cooMax;
	GetCrop(board, cooMin, cooMax);

	for (int i = 0; i < _countof(cooMin.coo); ++i)
	{
		cooMin.coo[i] -= inflate;
		cooMax.coo[i] += inflate;
	}

	for (int row = cooMin.coo[0]; row <= cooMax.coo[0]; ++row)
	{
		for (int col = cooMin.coo[1]; col <= cooMax.coo[1]; ++col)
		{
			const Coo coo(row, col);

			auto pred = [&](const Elf& elf) -> bool { return elf.coo == coo; };

			auto it = index.find(coo);
			auto jt = std::find_if(board.begin(), board.end(), pred);
			ASSERT(it == index.end() && jt == board.end() ||
			       it != index.end() && jt != board.end());
			ASSERT(jt == board.end() || board.end() == std::find_if(jt+1, board.end(), pred));

			char ch = '.';
			if (jt != board.end())
				ch = '0' + (jt - board.begin()) % 10;

			std::cout << ch;
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

int PerformRounds(board_t& board, int rounds_limit = -1)
{
	for (int round = 0; rounds_limit < 0 || round < rounds_limit; ++round)
	{
//		Print(board);

		index_t index;
		MakeIndex(board, index);

		// First Half : considers the eight positions adjacent to the elf

		std::map<Coo, int> reservations;  // int == number of elves that would move to Coo
		for (size_t i = 0; i < board.size(); ++i)
		{
			Elf& elf = board[i];

			ASSERT(elf.iNextMove < 0);

			if (GetNeighbourhoodCount(index, elf.coo) == 0)
				continue;

			for (int m = 0; m < _countof(moves); ++m)
			{
				const int iNextMove = (round + m) % _countof(moves);
				if (GetDirectionCount(index, elf.coo, iNextMove) == 0)
				{
					elf.iNextMove = iNextMove;
					reservations[elf.coo + moves[iNextMove]]++;
					break;
				}
			}
		}

		if (rounds_limit < 0 && reservations.empty())  // part two stop condition
			return round+1;

		// Second Half : each Elf moves to their proposed destination tile if they were the only Elf to propose moving to that position
		for (size_t i = 0; i < board.size(); ++i)
		{
			Elf& elf = board[i];

			if (elf.iNextMove < 0)
				continue;

			const Coo newCoo = elf.coo + moves[elf.iNextMove];
			elf.iNextMove = -1;

			auto it = reservations.find(newCoo);
			if (it == reservations.end())
			{
				ASSERT(false);
				continue;
			}

			if (it->second != 1)
				continue;

			elf.coo = newCoo;
		}
	}

//	Print(board);

	return rounds_limit;
}

int PartOne(std::istream& in)
{
	board_t board;
	ParseBoard(in, board);

	PerformRounds(board, 10);

	Coo topLeft, bottomRight;
	GetCrop(board, topLeft, bottomRight);

	int cnt = 0;
	for (const Elf& elf : board)
	{
		int i;
		for (i = 0; i < _countof(elf.coo.coo); ++i)
			if (elf.coo.coo[i] < topLeft.coo[i] || bottomRight.coo[i] < elf.coo.coo[i])
				break;
		if (i >= _countof(elf.coo.coo))
			++cnt;
	}

	int area = 1;
	for (int i = 0; i < _countof(bottomRight.coo); ++i)
		area *= bottomRight.coo[i] - topLeft.coo[i] + 1;

	return area - cnt;
}

int PartTwo(std::istream& in)
{
	board_t board;
	ParseBoard(in, board);

	return PerformRounds(board, -1);
}

int main()
{
#if 0
	std::istringstream in(
R"(.....
..##.
..#..
.....
..##.
.....
)");
#elif 0
	std::istringstream in(
R"(..............
..............
.......#......
.....###.#....
...#...#.#....
....#...##....
...#.###......
...##.#.##....
....#..#......
..............
..............
..............)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	int ground_count = PartOne(in);  // 4158
	std::cout << ground_count << std::endl;
#else
	int round_count = PartTwo(in);  // 1014
	std::cout << round_count << std::endl;
#endif
}
