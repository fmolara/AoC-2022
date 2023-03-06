// Advent Of Code 2022
// Day 04
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



struct Elf
{
	int from;
	int to;

	Elf()
		: Elf(0, 0)
	{
	}

	Elf(int _from, int _to)
		: from(_from), to(_to)
	{
		ASSERT(from <= to);
	}

	bool Includes(const Elf& e) const { return from <= e.from && e.to <= to; }
	bool Overlaps(const Elf& e) const { return from <= e.to && e.from <= to; }
};


int PartOne(std::istream& in)
{
	int count = 0;
	for (std::string line; std::getline(in, line);)
	{
		if (line.empty())
			break;

		auto sline = split(line, ",");
		ASSERT(sline.size() == 2);

		Elf elf[2];
		{
			auto elve_parts = split(sline[0], "-");
			ASSERT(elve_parts.size() == 2);
			elf[0] = Elf(std::stoi(elve_parts[0]),
			               std::stoi(elve_parts[1]));
		}
		{
			auto elve_parts = split(sline[1], "-");
			ASSERT(elve_parts.size() == 2);
			elf[1] = Elf(std::stoi(elve_parts[0]),
			               std::stoi(elve_parts[1]));
		}

		if (elf[0].Includes(elf[1]) || elf[1].Includes(elf[0]))
			++count;
	}

	return count;
}

int PartTwo(std::istream& in)
{
	int count = 0;
	for (std::string line; std::getline(in, line);)
	{
		if (line.empty())
			break;

		auto sline = split(line, ",");
		ASSERT(sline.size() == 2);

		Elf elf[2];
		{
			auto elve_parts = split(sline[0], "-");
			ASSERT(elve_parts.size() == 2);
			elf[0] = Elf(std::stoi(elve_parts[0]),
			               std::stoi(elve_parts[1]));
	}
		{
			auto elve_parts = split(sline[1], "-");
			ASSERT(elve_parts.size() == 2);
			elf[1] = Elf(std::stoi(elve_parts[0]),
			               std::stoi(elve_parts[1]));
		}

		if (elf[0].Overlaps(elf[1]))
		{
			ASSERT(elf[1].Overlaps(elf[0]));
			++count;
		}
}

	return count;
}

int main()
{
#if 0
	std::istringstream in(
R"(2-4,6-8
2-3,4-5
5-7,7-9
2-8,3-7
6-6,4-6
2-6,4-8)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	int score = PartOne(in);
	std::cout << score << std::endl;  // 448
#else
	int score = PartTwo(in);
	std::cout << score << std::endl;  // 794
#endif
}