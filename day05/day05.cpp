// Advent Of Code 2022
// Day 05
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



void PartOne(std::istream& in, std::vector<std::string>& stacks)
{
	for (std::string line; std::getline(in, line);)
	{
		if (line.empty())
			break;

		auto sline = split(line, " ");
		ASSERT(sline.size() == 3);

		const int cnt =  std::stoi(sline[0]);
		const int from = std::stoi(sline[1])-1;
		const int to =   std::stoi(sline[2])-1;

		for (int i = 0; i < cnt; ++i)
		{
			stacks[to].insert(stacks[to].begin(), stacks[from].front());
			stacks[from].erase(stacks[from].begin());
		}
	}
}

void PartTwo(std::istream& in, std::vector<std::string>& stacks)
{
	for (std::string line; std::getline(in, line);)
	{
		if (line.empty())
			break;

		auto sline = split(line, " ");
		ASSERT(sline.size() == 3);

		const int cnt =  std::stoi(sline[0]);
		const int from = std::stoi(sline[1])-1;
		const int to =   std::stoi(sline[2])-1;

		stacks[to].insert(stacks[to].begin(), stacks[from].begin(), stacks[from].begin()+cnt);
		stacks[from].erase(stacks[from].begin(), stacks[from].begin()+cnt);
	}
}

int main()
{
#if 0
	std::istringstream in(
R"(1 2 1
3 1 3
2 2 1
1 1 2)");

	std::vector<std::string> stacks =
	{
		"NZ",
		"DCM",
		"P"
	};
#else
	std::ifstream in("input1.txt");
	//     [P]                 [Q]     [T]
	// [F] [N]             [P] [L]     [M]
	// [H] [T] [H]         [M] [H]     [Z]
	// [M] [C] [P]     [Q] [R] [C]     [J]
	// [T] [J] [M] [F] [L] [G] [R]     [Q]
	// [V] [G] [D] [V] [G] [D] [N] [W] [L]
	// [L] [Q] [S] [B] [H] [B] [M] [L] [D]
	// [D] [H] [R] [L] [N] [W] [G] [C] [R]
	std::vector<std::string> stacks =
	{
		"FHMTVLD",
		"PNTCJGQH",
		"HPMDSR",
		"FVBL",
		"QLGHN",
		"PMRGDBW",
		"QLHCRNMG",
		"WLC",
		"TMZJQLDR",
	};
#endif

#if 0
	PartOne(in, stacks);
#else
	PartTwo(in, stacks);
#endif

	for (auto v : stacks)
	{
		if (v.empty())
			std::cout << '-';
		else
			std::cout << v[0];
	}

	std::cout << std::endl;  // HNSNMTLHQ  /  RNLFDJMCT
}