// Advent Of Code 2022
// Day 09
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



struct Coo
{
	int x, y;

	Coo() : x(0), y(0) {}
	bool operator < (const Coo& o) const { return x < o.x || x == o.x && y < o.y;  }
};


inline
void MoveHelper(int& main_i1, const int main_i0, int& slave_i1, const int slave_i0)
{
	ASSERT(std::abs(main_i1-main_i0) == 2);
	ASSERT(std::abs(slave_i1-slave_i0) <= 2);

	main_i1 += (main_i1 > main_i0 ? -1 : +1);
	if (std::abs(slave_i1-slave_i0) > 0)
		slave_i1 += (slave_i1 > slave_i0 ? -1 : +1);
}

void Move(int dx, int dy, std::vector<Coo>& pos)
{
	ASSERT(-1 <= dx && dx <= 1);
	ASSERT(-1 <= dy && dy <= 1);
	for (size_t i = 1; i < pos.size(); ++i)
	{
		ASSERT(std::abs(pos[i].x - pos[i-1].x) <= 1);
		ASSERT(std::abs(pos[i].y - pos[i-1].y) <= 1);
	}

	pos.front().x += dx;
	pos.front().y += dy;

	for (size_t i = 1; i < pos.size(); ++i)
	{
		if (std::abs(pos[i].x - pos[i-1].x) > 1)
			MoveHelper(pos[i].x, pos[i-1].x, pos[i].y, pos[i-1].y);
		else if (std::abs(pos[i].y - pos[i-1].y) > 1)
			MoveHelper(pos[i].y, pos[i-1].y, pos[i].x, pos[i-1].x);

		ASSERT(std::abs(pos[i].x - pos[i-1].x) <= 1);
		ASSERT(std::abs(pos[i].y - pos[i-1].y) <= 1);
	}
}

int PartCommon(std::istream& in, int len)
{
	std::set<Coo> visited;
	std::vector<Coo> pos(len+1, Coo());  // .front() == Head, .back() == Tail
	visited.insert(pos.back());

	for (std::string line; std::getline(in, line);)
	{
		ASSERT(line.length() >= 3 && line[1] == ' ');
		char dir = line[0];
		int magn = std::atoi(line.c_str() + 2);
		ASSERT(std::string("RLUD").find(dir) != std::string::npos && magn > 0);

		for (int i = 0; i < magn; ++i)
		{
			switch (dir)
			{
				case 'R': Move(+1,  0, pos); break;
				case 'L': Move(-1,  0, pos); break;
				case 'U': Move( 0, +1, pos); break;
				case 'D': Move( 0, -1, pos); break;
				default: ASSERT(false);
			}

			visited.insert(pos.back());
		}
	}

	return (int)visited.size();
}

int PartOne(std::istream& in) { return PartCommon(in, 1); }
int PartTwo(std::istream& in) { return PartCommon(in, 9); }

int main()
{
#if 0
	std::istringstream in(
R"(R 4
U 4
L 3
D 1
R 4
D 1
L 5
R 2)");
#elif 0
	std::istringstream in(
R"(R 5
U 8
L 8
D 3
R 17
D 10
L 25
U 20)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	int count = PartOne(in);
	std::cout << count << std::endl;  // 6384
#else
	int count = PartTwo(in);
	std::cout << count << std::endl;  // 2734
#endif
}
