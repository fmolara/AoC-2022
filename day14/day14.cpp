// Advent Of Code 2022
// Day 14
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

#include "..\stuffs\Splitter.h"

#ifdef _DEBUG
#include <assert.h>
#define ASSERT(x)   assert(x)
#else
#define ASSERT(x)   ((void)0)
#endif




struct Coo
{
	int x, y;

	Coo() : Coo(0,0) {}
	Coo(int _x, int _y) : x(_x), y(_y) {}
	bool operator < (const Coo& o) const { return x < o.x || x == o.x && y < o.y; }
};




void ParseField(std::istream& in, std::map<Coo, char>& field, Coo& cMin, Coo& cMax)
{
	cMin.x = cMin.y = INT_MAX;
	cMax.x = cMax.y = INT_MIN;

	field.clear();
	for (std::string line; std::getline(in, line); )
	{
		int x0 = -1, y0 = -1;

		auto parts = split(line, " -> ");
		for (auto s : parts)
		{
			auto coos = split(s, ",");
			ASSERT(coos.size() == 2);

			const int x1 = std::atoi(coos[0].c_str());
			const int y1 = std::atoi(coos[1].c_str());

			cMin.x = std::min(cMin.x, x1);
			cMin.y = std::min(cMin.y, y1);
			cMax.x = std::max(cMax.x, x1);
			cMax.y = std::max(cMax.y, y1);

			if (x0 >= 0)
			{
				ASSERT(y0 >= 0);
				if (x0 == x1)
				{
					for (int y = std::min(y0, y1); y <= std::max(y0, y1); ++y)
						field.insert(std::make_pair(Coo(x0, y), '#'));
				}
				else if (y0 == y1)
				{
					for (int x = std::min(x0, x1); x <= std::max(x0, x1); ++x)
						field.insert(std::make_pair(Coo(x, y0), '#'));
				}
				else
					ASSERT(false);
			}
			else
				ASSERT(y0 < 0);

			x0 = x1;
			y0 = y1;
		}
	}
}

bool DropSand(std::map<Coo, char>& field, const Coo& source, const int y_max)
{
	if (field.contains(source))
		return false;

	for (Coo coo = source, coo_next; coo.y < y_max; coo = coo_next)
	{
		if (!field.contains(coo_next = Coo(coo.x+0, coo.y+1)))
			continue;
		if (!field.contains(coo_next = Coo(coo.x-1, coo.y+1)))
			continue;
		if (!field.contains(coo_next = Coo(coo.x+1, coo.y+1)))
			continue;

		field.insert(std::make_pair(coo, 'o'));
		return true;
	}

	return false;
}

int DropSandCount(std::map<Coo, char>& field, const Coo& source, const int y_max)
{
	int cnt = 0;
	while (DropSand(field, source, y_max))
		++cnt;

	return cnt;
}

int PartOne(std::istream& in)
{
	Coo cMin, cMax;
	std::map<Coo, char> field;
	ParseField(in, field, cMin, cMax);

	Coo source(500, 0);
	return DropSandCount(field, source, cMax.y);
}

void AddFloor(std::map<Coo, char>& field, const Coo& source, const int y_floor)
{
	const int height = y_floor - source.y;
	ASSERT(height > 0);

	for (int dx = -2 * height; dx <= 2 * height; ++dx)
	{
		Coo coo(source.x+dx, y_floor);
		ASSERT(!field.contains(coo));
		field.insert(std::make_pair(coo, '#'));
	}
}

int PartTwo(std::istream& in)
{
	Coo cMin, cMax;
	std::map<Coo, char> field;
	ParseField(in, field, cMin, cMax);

	Coo source(500, 0);
	AddFloor(field, source, cMax.y+2);
	return DropSandCount(field, source, cMax.y+2);
}

int main()
{
#if 0
	std::istringstream in(
R"(498,4 -> 498,6 -> 496,6
503,4 -> 502,4 -> 502,9 -> 494,9)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	int cnt = PartOne(in);  // 618
	std::cout << cnt << std::endl;
#else
	int cnt = PartTwo(in);  // 26358
	std::cout << cnt << std::endl;
#endif
}
