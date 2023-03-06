// Advent Of Code 2022
// Day 03
//
// (c) 2023 Federico Molara <federico@molara.net>
// This code is licensed under MIT license (see LICENSE.txt for details)

#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
//#include <vector>
//#include <string>
//#include <ranges>
//#include <string_view>
#include <set>
#include <algorithm>

#ifdef _DEBUG
	#include <assert.h>
	#define ASSERT(x)   assert(x)
#else
	#define ASSERT(x)   ((void)0)
#endif



int Score(std::string::value_type ch)
{
	if ('a' <= ch && ch <= 'z')
		return ch - 'a' + 1;

	ASSERT('A' <= ch && ch <= 'Z');
	return ch - 'A' + 27;
}

int PartOne(std::istream& in)
{
	int score = 0;

	for (std::string line; std::getline(in, line);)
	{
		std::set s0(line.begin(), line.begin() + line.size() / 2);
		std::set s1(              line.begin() + line.size() / 2, line.end());

		std::string inters;
		std::set_intersection(s0.begin(), s0.end(),
		                      s1.begin(), s1.end(),
		                      std::back_inserter(inters));

		ASSERT(inters.size() == 1);
		score += Score(inters[0]);
	}

	return score;
}

int PartTwo(std::istream& in)
{
	int score = 0;

	for (std::string line; in;)
	{
		if (!std::getline(in, line))
			break;
		std::set s0(line.begin(), line.end());

		if (!std::getline(in, line))
			break;
		std::set s1(line.begin(), line.end());

		if (!std::getline(in, line))
			break;
		std::set s2(line.begin(), line.end());

		std::string inters;
		std::set_intersection(s0.begin(), s0.end(),
			                  s1.begin(), s1.end(),
			                  std::back_inserter(inters));
		s0.clear();
		s0.insert(inters.begin(), inters.end());
		inters.clear();

		std::set_intersection(s0.begin(), s0.end(),
			                  s2.begin(), s2.end(),
			                  std::back_inserter(inters));

		ASSERT(inters.size() == 1);
		score += Score(inters[0]);
	}

	return score;
}

int main()
{
#if 0
	std::istringstream in(
R"(vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg
wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	int score = PartOne(in);
	std::cout << score << std::endl;  // 8139
#else
	int score = PartTwo(in);
	std::cout << score << std::endl;  // 2668
#endif
}