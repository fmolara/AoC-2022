// Advent Of Code 2022
// Day 12
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



typedef std::vector<std::string> board_t;

board_t ParseBoard(std::istream& in)
{
	board_t board;

	for (std::string line; std::getline(in, line);)
	{
		ASSERT(board.empty() || board.back().size() == line.size());
		board.push_back(line);
	}

	return std::move(board);
}

inline size_t RowCol2Ind(const board_t& board, size_t r, size_t c)              { return r * board.front().size() + c; }
inline void Ind2RowCol(const board_t& board, size_t& r, size_t& c, size_t ind ) { r = ind / board.front().size();
                                                                                  c = ind % board.front().size();}
#include "..\stuffs\Dijkstra.h"

void AddEgdeHelper(const board_t& board, DirectedGraph<char, int>& g, int r, int c, int dr, int dc)
{
	ASSERT(0 <= r || r < (int)board.size());
	ASSERT(0 <= c || c < (int)board[r].size());

	const int _r = r + dr;
	const int _c = c + dc;
	if (_r < 0 || (int)board.size() <= _r)
		return;
	if (_c < 0 || (int)board[_r].size() <= _c)
		return;

	if (board[_r][_c] > board[r][c] + 1)
		return;

	g.AddEdge( RowCol2Ind(board, _r, _c)
	         , RowCol2Ind(board, r, c)
			 , 1 );
}

void PartCommon(std::istream& in, size_t& iS, size_t& iE, DirectedGraph<char, int>& g)
{
	board_t board = std::move(ParseBoard(in));

	iS = iE = g.npos;
	for (size_t r = 0; r < board.size(); ++r)
	{
		for (size_t c = 0; c < board[r].size(); ++c)
		{
			size_t ind = g.AddNode(board[r][c]);
			ASSERT(ind == RowCol2Ind(board, r, c));

			if (board[r][c] == 'S')
			{
				ASSERT(iS == std::string::npos);
				iS = RowCol2Ind(board, r, c);
				board[r][c] = 'a';
			}
			else if (board[r][c] == 'E')
			{
				ASSERT(iE == std::string::npos);
				iE = RowCol2Ind(board, r, c);
				board[r][c] = 'z';
			}
		}
	}
	ASSERT(iS != std::string::npos);
	ASSERT(iE != std::string::npos);

	for (int r = 0; r < board.size(); ++r)
	{
		for (int c = 0; c < board[r].size(); ++c)
		{
			AddEgdeHelper(board, g, r, c, -1,  0);
			AddEgdeHelper(board, g, r, c, +1,  0);
			AddEgdeHelper(board, g, r, c,  0, -1);
			AddEgdeHelper(board, g, r, c,  0, +1);
		}
	}
}

int PartOne(std::istream& in)
{
	size_t iS, iE;
	DirectedGraph<char, int> g;
	PartCommon(in, iS, iE, g);

	std::vector<size_t> path;
	std::vector<int> dist;
	Dijkstra(g, iE, iS, path, dist);

	return dist[iS];
}

int PartTwo(std::istream& in)
{
	size_t iS, iE;
	DirectedGraph<char, int> g;
	PartCommon(in, iS, iE, g);

	std::vector<size_t> path;
	std::vector<int> dist;
	Dijkstra(g, iE, g.npos, path, dist);

	int min_cnt = INT_MAX;
	for (size_t iNode = 0; iNode < g.NodeCount(); ++iNode)
	{
		if (g[iNode] != 'a')
			continue;
		if (min_cnt > dist[iNode])
			min_cnt = dist[iNode];
	}

	return min_cnt;
}

int main()
{
#if 0
	std::istringstream in(
R"(Sabqponm
abcryxxl
accszExk
acctuvwj
abdefghi)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	int pah_len = PartOne(in);  // 484
	std::cout << pah_len  << std::endl;
#else
	int min_pah_len = PartTwo(in);  // 478
	std::cout << min_pah_len << std::endl;
#endif
}
