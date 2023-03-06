// Advent Of Code 2022
// Day 06
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
#include <set>

#ifdef _DEBUG
	#include <assert.h>
	#define ASSERT(x)   assert(x)
#else
	#define ASSERT(x)   ((void)0)
#endif



size_t FindMarker(const std::string& buff, int len)
{
	if (buff.size() < len)
	{
		ASSERT(false);
	}

	for (size_t i = 0; i < buff.size()- len; ++i)
	{
		std::set sub(buff.begin()+i, buff.begin()+i+len);
		if (sub.size() == len)
		{
			return i+len;
		}
	}

	ASSERT(false);
	return 0;
}

void FindMarker(std::istream& in, int len)
{
	for (std::string line; std::getline(in, line);)
	{
		if (line.empty())
			break;

		std::cout << FindMarker(line, len) << std::endl;
	}
}

void PartTwo(std::istream& in)
{
}

int main()
{
#if 0
	std::istringstream in(
R"(bvwbjplbgvbhsrlpgdmjqwftvncz
nppdvjthqldpwncqszvftbrmjlhg
nznrnfrfntjfmvfwmzdfjlvtqnbhcprs
zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw)");
#elif 0
	std::istringstream in(
R"(mjqjpqmgbljsphdztnvjfqwrcgsmlb
bvwbjplbgvbhsrlpgdmjqwftvncz
nppdvjthqldpwncqszvftbrmjlhg
nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg
zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	FindMarker(in, 4);  // 1920
#else
	FindMarker(in, 14);  // 2334
#endif
}