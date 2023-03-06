// Advent Of Code 2022
// Day 18
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




struct Cube
{
	int x, y, z;

	Cube() : Cube(0, 0, 0) {}
	Cube(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}

	bool operator < (const Cube& other) const { return CompareTo(other) < 0;  }

	int CompareTo(const Cube& other) const
	{
		int cmp;
		if (0 != (cmp = this->x-other.x)) return cmp;
		if (0 != (cmp = this->y-other.y)) return cmp;
		if (0 != (cmp = this->z-other.z)) return cmp;
		return 0;
	}
};

void ParseCubes(std::istream& in, std::set<Cube>& cubes)
{
	cubes.clear();

	for (std::string line; std::getline(in, line); )
	{
		auto triple = split(line, ",");

		ASSERT(triple.size() == 3);
		int x = std::atoi(triple[0].c_str());
		int y = std::atoi(triple[1].c_str());
		int z = std::atoi(triple[2].c_str());

		cubes.insert(Cube(x, y, z));
	}
}

inline
bool TestFace(const std::set<Cube>& cubes, int x, int y, int z)
{
	return !cubes.contains(Cube(x, y, z));
}

int PartOne(std::istream& in)
{
	std::set<Cube> cubes;
	ParseCubes(in, cubes);

	int face_cnt = 0;
	for (const auto cube : cubes)
	{
		if (TestFace(cubes, cube.x-1, cube.y,   cube.z  )) ++face_cnt;
		if (TestFace(cubes, cube.x+1, cube.y,   cube.z  )) ++face_cnt;
		if (TestFace(cubes, cube.x,   cube.y-1, cube.z  )) ++face_cnt;
		if (TestFace(cubes, cube.x,   cube.y+1, cube.z  )) ++face_cnt;
		if (TestFace(cubes, cube.x,   cube.y,   cube.z-1)) ++face_cnt;
		if (TestFace(cubes, cube.x,   cube.y,   cube.z+1)) ++face_cnt;
	}

	return face_cnt;
}

inline
bool TestFace(const std::set<Cube>& cubes, const std::map<Cube, bool>& closure, int x, int y, int z)
{
	const Cube cube(x, y, z);
	if (cubes.contains(cube))
		return false;

	auto it = closure.find(cube);
	ASSERT(it != closure.end());
	if (it != closure.end() && !it->second)
		return false;

	return true;
}

int PartTwo(std::istream& in)
{
	std::set<Cube> cubes;
	ParseCubes(in, cubes);

	Cube min(INT_MAX, INT_MAX, INT_MAX);
	Cube max(INT_MIN, INT_MIN, INT_MIN);
	for (const auto cube : cubes)
	{
		min.x = std::min(min.x, cube.x);
		max.x = std::max(max.x, cube.x);
		min.y = std::min(min.y, cube.y);
		max.y = std::max(max.y, cube.y);
		min.z = std::min(min.z, cube.z);
		max.z = std::max(max.z, cube.z);
	}

	std::map<Cube, bool> closure;
	for (int x = min.x-1; x <= max.x+1; ++x)
		for (int y = min.y-1; y <= max.y+1; ++y)
			for (int z = min.z-1; z <= max.z+1; ++z)
			{
				const Cube cube(x, y, z);
				if (cubes.contains(Cube(cube)))
					continue;

				closure.insert(std::make_pair(cube, false));
			}

	ASSERT(!cubes.contains(Cube(min.x-1, min.y-1, min.z-1)));
	closure.find(          Cube(min.x-1, min.y-1, min.z-1))->second = true;
	for (bool flask = true; flask; )
	{
		flask = false;

		for (int x = min.x-1; x <= max.x+1; ++x)
			for (int y = min.y-1; y <= max.y+1; ++y)
				for (int z = min.z-1; z <= max.z+1; ++z)
				{
					auto it = closure.find(Cube(x, y, z));
					if (it == closure.end() || it->second)
						continue;

					std::map<Cube, bool>::const_iterator jt;

					if ((jt = closure.find(Cube(x-1, y,   z  ))) != closure.end() && jt->second) { it->second = flask = true; continue; }
					if ((jt = closure.find(Cube(x+1, y,   z  ))) != closure.end() && jt->second) { it->second = flask = true; continue; }
					if ((jt = closure.find(Cube(x,   y-1, z  ))) != closure.end() && jt->second) { it->second = flask = true; continue; }
					if ((jt = closure.find(Cube(x,   y+1, z  ))) != closure.end() && jt->second) { it->second = flask = true; continue; }
					if ((jt = closure.find(Cube(x,   y,   z-1))) != closure.end() && jt->second) { it->second = flask = true; continue; }
					if ((jt = closure.find(Cube(x,   y,   z+1))) != closure.end() && jt->second) { it->second = flask = true; continue; }
				}
	}

	int face_cnt = 0;
	for (const auto cube : cubes)
	{
		if (TestFace(cubes, closure, cube.x-1, cube.y,   cube.z  )) ++face_cnt;
		if (TestFace(cubes, closure, cube.x+1, cube.y,   cube.z  )) ++face_cnt;
		if (TestFace(cubes, closure, cube.x,   cube.y-1, cube.z  )) ++face_cnt;
		if (TestFace(cubes, closure, cube.x,   cube.y+1, cube.z  )) ++face_cnt;
		if (TestFace(cubes, closure, cube.x,   cube.y,   cube.z-1)) ++face_cnt;
		if (TestFace(cubes, closure, cube.x,   cube.y,   cube.z+1)) ++face_cnt;
	}

	return face_cnt;
}

int main()
{
#if 0
	std::istringstream in(
R"(2,2,2
1,2,2
3,2,2
2,1,2
2,3,2
2,2,1
2,2,3
2,2,4
2,2,6
1,2,5
3,2,5
2,1,5
2,3,5)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	int faces_cnt = PartOne(in);  // 3586
	std::cout << faces_cnt << std::endl;
#else
	int faces_cnt = PartTwo(in);  // 2072
	std::cout << faces_cnt << std::endl;
#endif
}
