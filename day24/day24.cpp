// Advent Of Code 2022
// Day 24
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

#include "..\stuffs\Dijkstra.h"

#ifdef _DEBUG
#include <assert.h>
#define ASSERT(x)   assert(x)
#else
#define ASSERT(x)   ((void)0)
#endif



const struct Moves
{
	int d_row, d_col;
}
moves[] =
{
	{ -1,  0 },  // ^  up
	{ +1,  0 },  // v  down
	{  0, -1 },  // <  left
	{  0, +1 },  // >  right
};

struct Coo
{
	int row, col;

	Coo(int _row = 0, int _col = 0)
		: row(_row)
		, col(_col)
	{
	}

	bool IsValid(int width, int height) const
	{
		if (row < 0 || height <= row)
			return false;
		if (col < 0 || width <= col)
			return false;
		return true;
	}

	auto operator <=> (const Coo&) const = default;
};

struct Blizzard
{
	Coo    coo;
	size_t iMove;  // valid index in moves[]

	Blizzard(int row, int col, size_t _iMove)
		: coo(row, col)
		, iMove(_iMove)
	{
	}
};

typedef std::vector<Blizzard> blizzards_t;
typedef std::set<Coo>         board_t;        // 2D board (row / col coordinates)
typedef std::vector<board_t>  timed_board_t;  // add time dimension (minutes)

void ParseInitialBoard(std::istream& in, blizzards_t& blizzards, int& width, int& height, Coo& entrance, Coo& exit)
{
	blizzards.clear();
	width = height = 0;

	std::string line;

	// parse top border
	{
		if (!std::getline(in, line))
		{
			ASSERT(false);
			return;
		}
		ASSERT(line.size() > 2 && line.front() == '#' && line.back() == '#');

		const size_t iEntrance = line.find('.', 1);
		ASSERT(iEntrance != std::string::npos);
		ASSERT(std::all_of(line.begin()+iEntrance+1, line.end(), [](char ch) -> bool { return ch == '#'; }));

		entrance = { -1, (int)iEntrance-1 };
		width = (int)line.length()-2;
	}

	// parse until bottom border
	for (; std::getline(in, line); ++height)
	{
		ASSERT(line.size() == width+2 && line.front() == '#' && line.back() == '#');

		if (line.find('#', 1) < line.size()-2)  // true for bottom border
			break;

		const int row = height;

		for (size_t i = 1; i < line.size()-1; ++i)
		{
			const int col = (int)i-1;
			const auto ch = line[i];

			if (ch == '.')
				continue;

			static const std::string arrow2move = "^v<>";  // same order of moves[] array
			const size_t type = arrow2move.find(ch);
			ASSERT(type != std::string::npos);

			blizzards.push_back(Blizzard(row, col, type));
		}
	}

	// parse bottom border
	{
		ASSERT(line.size() == width + 2 && line.front() == '#' && line.back() == '#');

		const size_t iExit = line.rfind('.', line.size());
		ASSERT(iExit != std::string::npos);
		ASSERT(std::all_of(line.begin(), line.begin()+iExit, [](char ch) -> bool { return ch == '#'; }));

		exit = { height, (int)iExit-1 };
	}
}

void MakeBoard(const blizzards_t& blizzards, board_t& board)
{
	board.clear();
	for (const auto& blz : blizzards)
	{
		board.insert(blz.coo);
	}
}

void MoveBlizzards(blizzards_t& blizzards, const int width, const int height)
{
	for (auto& blz : blizzards)
	{
		ASSERT(0 <= blz.coo.row && blz.coo.row < height && 0 <= blz.coo.col && blz.coo.col < width);

		const auto& move = moves[blz.iMove];
		blz.coo.row = (blz.coo.row + move.d_row + height) % height;
		blz.coo.col = (blz.coo.col + move.d_col + width) % width;

		ASSERT(0 <= blz.coo.row && blz.coo.row < height && 0 <= blz.coo.col && blz.coo.col < width);
	}
}

struct TimedCoo
	: Coo
{
	int minute;

	TimedCoo(int row = 0, int col = 0, int _minute = 0)
		: Coo(row, col)
		, minute(_minute)
	{
	}
	TimedCoo(const Coo& coo, int _minute)
		: Coo(coo)
		, minute(_minute)
	{
	}

	TimedCoo operator + (const Moves& move) const
	{
		return TimedCoo(row+move.d_row, col+move.d_col, minute);
	}

	auto operator <=> (const TimedCoo&) const = default;
};

typedef DirectedGraph<TimedCoo, int> graph_t;

inline
size_t GetNodeIndex(const graph_t& graph, const std::map <TimedCoo, size_t>& coo2node, TimedCoo tcoo)
{
	const auto it = coo2node.find(tcoo);
	if (it == coo2node.end())
		return graph.npos;

	ASSERT(graph.GetNodeValue(it->second) == tcoo);
	return it->second;
}

void AddNodesToGraph(graph_t& graph, std::map <TimedCoo, size_t>& coo2node, const int width, const int height, const board_t& board, const int minute)
{
	for (int row = 0; row < height; ++row)
		for (int col = 0; col < width; ++col)
		{
			TimedCoo tcoo(row, col, minute);

			if (board.find(tcoo) != board.end())  // skip non-empty cells
				continue;

			coo2node[tcoo] = graph.AddNode(tcoo);
		}
}

void AddEdgesToGraph(graph_t& graph, std::map <TimedCoo, size_t>& coo2node, const int width, const int height, const board_t& board0, const int minute0, const board_t& board1, const int minute1)
{
	for (int row = 0; row < height; ++row)
		for (int col = 0; col < width; ++col)
		{
			TimedCoo tcoo0(row, col, minute0);

			if (board0.find(tcoo0) != board0.end())  // skip non-empty cells
				continue;

			const auto i_tcoo0 = GetNodeIndex(graph, coo2node, tcoo0);
			ASSERT(i_tcoo0 != graph.npos);

			TimedCoo tcoo1(row, col, minute1);
			for (int m = 0; m < _countof(moves); ++m)
			{
				TimedCoo new_tcoo1 = tcoo1+moves[m];

				if(!new_tcoo1.IsValid(width, height))
					continue;
				if (board1.find(new_tcoo1) != board1.end())  // skip non-empty cells
					continue;

				const auto i_tcoo1 = GetNodeIndex(graph, coo2node, new_tcoo1);
				ASSERT(i_tcoo1 != graph.npos);

				graph.AddEdge(i_tcoo0, i_tcoo1, 1);
			}

			// Add the "you can wait in place" edge
			if (board1.find(tcoo1) == board1.end())
			{
				const auto i_tcoo1 = GetNodeIndex(graph, coo2node, tcoo1);
				ASSERT(i_tcoo1 != graph.npos);

				graph.AddEdge(i_tcoo0, i_tcoo1, 1);
			}
		}
}

void BoardsToGraph(const timed_board_t& boards, const int width, const int height, const size_t iCycle, const Coo& entrance, const Coo& exit, graph_t& graph, size_t& iEntrance, size_t& iExit)
{
	graph.Clear();
	std::map <TimedCoo, size_t> coo2node;

	for (size_t i = 0; i < boards.size(); ++i)
	{
		AddNodesToGraph(graph, coo2node, width, height, boards[i], (int)i);

		if (i > 0)
			AddEdgesToGraph(graph, coo2node, width, height, boards[i-1], (int)i-1, boards[i], (int)i);
	}

	// Close cycle
	AddEdgesToGraph(graph, coo2node, width, height, boards.back(), (int)boards.size()-1, boards[iCycle], (int)iCycle);

	// Add entrance
	iEntrance = graph.AddNode(TimedCoo(entrance, -1));
	for (size_t i = 1; i < boards.size(); ++i)
	{
		const auto& board = boards[i];

		TimedCoo tcoo(entrance.row+(entrance.row < 0 ? +1 : -1), entrance.col, (int)i);

		if (board.find(tcoo) != board.end())  // skip non-empty cells
			continue;

		const auto i_tcoo = GetNodeIndex(graph, coo2node, tcoo);
		ASSERT(i_tcoo != graph.npos);

		graph.AddEdge(iEntrance, i_tcoo, (int)i);
		/*
		* No!! It is theoretically possible to wait at the entrance!

			break;
		*/
	}

	// Add exit
	iExit = graph.AddNode(TimedCoo(exit, -1));
	for (size_t i = 0; i < boards.size(); ++i)
	{
		const auto& board = boards[i];

		TimedCoo tcoo(exit.row+(exit.row < 0 ? +1 : -1), exit.col, (int)i);

		if (board.find(tcoo) != board.end())  // skip non-empty cells
			continue;

		const auto i_tcoo = GetNodeIndex(graph, coo2node, tcoo);
		ASSERT(i_tcoo != graph.npos);

		graph.AddEdge(i_tcoo, iExit, 1);
	}
}

void Print(const board_t& board, const int width, const int height)
{
	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			const Coo coo(row, col);

			auto it = board.find(coo);
			char ch = (it != board.end() ? '@' : '.');

			std::cout << ch;
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

int Solve(const blizzards_t& _blizzards, const int width, const int height, const Coo& entrance, const Coo& exit, const int initial_minute)
{
	blizzards_t blizzards = _blizzards;

	for (int minute = 0; minute < initial_minute; ++minute)
		MoveBlizzards(blizzards, width, height);

	const int time_limit = 10000;

	size_t iCycle = (size_t)-1;
	timed_board_t boards;
	for (int minute = 0; minute < time_limit; ++minute)
	{
		board_t board;
		MakeBoard(blizzards, board);

#if 0
		std::cout << "--- minute " << minute << " ---" << std::endl;
		Print(board, width, height);
#endif

		// check if 'board' closes a cycle into boards sequence
		auto it = std::find(boards.begin(), boards.end(), board);
		if (it != boards.end())
		{
			// bingo!
			iCycle = it-boards.begin();
			break;
		}

		boards.push_back(std::move(board));

		MoveBlizzards(blizzards, width, height);
	}

	if (iCycle == (size_t)-1)
	{
		ASSERT(false);  // no cycles in blizzards movements!
		return -1;
	}

	// Translate boards into a directed graph
	graph_t graph;
	size_t iEntrance, iExit;
	BoardsToGraph(boards, width, height, iCycle, entrance, exit, graph, iEntrance, iExit);

	std::vector<int> dist;
	std::vector<size_t> path;
	Dijkstra(graph, iEntrance, iExit, path, dist);
	ASSERT(path[iExit] != graph.npos);

	return dist[iExit];
}

int PartOne(std::istream& in)
{
	int width, height;
	Coo entrance, exit;
	blizzards_t blizzards;
	ParseInitialBoard(in, blizzards, width, height, entrance, exit);

	return Solve(blizzards, width, height, entrance, exit, 0);
}

int PartTwo(std::istream& in)
{
	int width, height;
	Coo entrance, exit;
	blizzards_t blizzards;
	ParseInitialBoard(in, blizzards, width, height, entrance, exit);

	int initial_minute = 0;
	const int step0 = Solve(blizzards, width, height, entrance, exit, initial_minute);  initial_minute += step0;
	const int step1 = Solve(blizzards, width, height, exit, entrance, initial_minute);  initial_minute += step1;
	const int step2 = Solve(blizzards, width, height, entrance, exit, initial_minute);

	return step0 + step1 + step2;
}

int main()
{
#if 0
	std::istringstream in(
R"(#.######
#>>.<^<#
#.<..<<#
#>v.><>#
#<^v^^>#
######.#)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	int best_minutes = PartOne(in);  // 290
	std::cout << best_minutes << std::endl;
#else
	int best_minutes = PartTwo(in);  // 842
	std::cout << best_minutes << std::endl;
#endif
}
