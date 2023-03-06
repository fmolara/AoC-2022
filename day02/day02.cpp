// Advent Of Code 2022
// Day 02
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

#include "..\stuffs\Splitter.h"

#ifdef _DEBUG
	#include <assert.h>
	#define ASSERT(x)   assert(x)
#else
	#define ASSERT(x)   ((void)0)
#endif



const int R = 0;
const int P = 1;
const int S = 2;

const int SCORES[3][3] =  // [im][io]  im == index "me", io = index "opponent"
{
	//    R  P  S
		{ 3, 0, 6 },  // R
		{ 6, 3, 0 },  // P
		{ 0, 6, 3 },  // S
};

const int MOVES[3][3] =  // [imr][io]  imr == index "my result", io = index "opponent"
{
	//    R  P  S
		{ S, R, P },  // L
		{ R, P, S },  // D
		{ P, S, R },  // W
};

int PartOne(std::istream& in)
{
	int score = 0;
	for (std::string line; std::getline(in, line);)
	{
		if (line.empty())
			break;

		auto sline = split(line, " ");
		ASSERT(sline.size() == 2);

		ASSERT(sline[0].size() == 1);
		int om = sline[0][0] - 'A';
		ASSERT( 0 <= om && om <= 2);

		ASSERT(sline[1].size() == 1);
		int im = sline[1][0] - 'X';
		ASSERT(0 <= im && im <= 2);

		score += SCORES[im][om] + (im+1);
	}

	return score;
}

int PartTwo(std::istream& in)
{
	int score = 0;
	for (std::string line; std::getline(in, line);)
	{
		if (line.empty())
			break;

		auto sline = split(line, " ");
		ASSERT(sline.size() == 2);

		ASSERT(sline[0].size() == 1);
		int om = sline[0][0] - 'A';
		ASSERT(0 <= om && om <= 2);

		ASSERT(sline[1].size() == 1);
		int imr = sline[1][0] - 'X';
		ASSERT(0 <= imr && imr <= 2);
		int im = MOVES[imr][om];

		score += SCORES[im][om] + (im + 1);
}

	return score;
}

int main()
{
#if 0
	std::istringstream in(
R"(A Y
B X
C Z)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	int score = PartOne(in);
	std::cout << score << std::endl;  // 11666
#else
	int score = PartTwo(in);
	std::cout << score << std::endl;  // 12767
#endif
}