// Advent Of Code 2022
// Day 15
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

	Coo() : Coo(0, 0) {}
	Coo(int _x, int _y) : x(_x), y(_y) {}
	bool operator <  (const Coo& o) const { return x < o.x || x == o.x && y < o.y; }
	bool operator == (const Coo& o) const { return x == o.x && y == o.y; }
	bool operator != (const Coo& o) const { return x != o.x || y != o.y; }
};

struct Position
{
	Coo sensor;
	Coo beacon;

	int Radius() const
	{
		return std::abs(sensor.x-beacon.x)
			 + std::abs(sensor.y-beacon.y);
	}

	void FillRow(int yRow, std::set<int>& xCoo) const
	{
		const int radius = Radius();
		const int dy = std::abs(sensor.y - yRow);
		if (dy > radius)
			return;
		const int dx = radius-dy;

		Coo coo(0, yRow);
		for (int x = 0; x <= dx; ++x)
		{
			coo.x = sensor.x+x;
			if (coo != beacon)
				xCoo.insert(coo.x);

			coo.x = sensor.x-x;
			if (coo != beacon)
				xCoo.insert(coo.x);
		}
	}

	bool GetInterval(int yRow, int& x0, int& x1) const
	{
		const int radius = Radius();
		const int dy = std::abs(sensor.y - yRow);
		if (dy > radius)
			return false;
		const int dx = radius - dy;

		x0 = sensor.x-dx;
		x1 = sensor.x+dx;
		return true;
	}
};




void ParseField(std::istream& in, std::vector<Position>& field)
{
	field.clear();
	for (std::string line; std::getline(in, line); )
	{
		auto parts = split(line, ":");
		ASSERT(parts.size() == 2);

		Position pos;

		Coo* coo[2] = { &pos.sensor, &pos.beacon };
		for (int i = 0; i < 2; ++i)
		{
			auto coos = split(parts[i], ",");
			ASSERT(coos.size() == 2);

			coo[i]->x = std::atoi(coos[0].c_str());
			coo[i]->y = std::atoi(coos[1].c_str());
		}

		field.push_back(pos);
	}
}

int PartOne(std::istream& in, const int yRow)
{
	std::vector<Position> field;
	ParseField(in, field);

	std::set<int> xCoo;
	for (const Position& p : field)
		p.FillRow(yRow, xCoo);

	return (int)xCoo.size();
}

struct Interval
{
	int x0, x1;  // range [x0 ; x1]

	Interval()
	{
	}
	Interval(int _x0, int _x1)
		: x0(_x0)
		, x1(_x1)
	{
	}
	Interval(const Interval& o)
		: x0(o.x0)
		, x1(o.x1)
	{
	}
	Interval& operator = (const Interval& o)
	{
		x0 = o.x0;
		x1 = o.x1;
		return *this;
	}

	bool Includes(const Interval& other) const
	{
		return x0 <= other.x0 && other.x1 <= x1;
	}
	bool Overlaps(const Interval& other) const
	{
		return x0 <= other.x1 && other.x0 <= x1;
	}
};

void ErodeIntervals(std::vector<Interval>& intervals, const Interval& interval)
{
	for (size_t i = 0; i < intervals.size(); ++i)
	{
		if (interval.Includes(intervals[i]))
			intervals.erase(intervals.begin() + i--);
		else if (intervals[i].Includes(interval))
		{
			if (intervals[i].x0 == interval.x0)
			{
				ASSERT(interval.x1 < intervals[i].x1);  // the previous case should have been true
				intervals[i].x1 = interval.x1;
			}
			else
			{
				ASSERT(intervals[i].x0 < interval.x0);
				if (interval.x1 == intervals[i].x1)
					intervals[i].x0 = interval.x0;
				else
				{
					// Inclusion without overlapping
					intervals.push_back(Interval(interval.x1+1, intervals[i].x1));
					intervals[i].x1 = interval.x0-1;
				}
			}
			break;
		}
		else if (interval.Overlaps(intervals[i]))
		{
			// Here inclusion cases are excluded
			if (interval.x1 <= intervals[i].x1)
			{
				ASSERT(interval.x1 < intervals[i].x1);  // it should have been an "inclusion case", so we shouldn't have been here
				intervals[i].x0 = interval.x1+1;
			}
			else
			{
				ASSERT(intervals[i].x0 < interval.x0);
				intervals[i].x1 = interval.x0-1;
			}
		}
	}
}

long long PartTwo(std::istream& in, const int order)
{
	std::vector<Position> field;
	ParseField(in, field);

	for (int yRow = 0; yRow <= order; ++yRow)
	{
		std::vector<Interval> intervals;
		intervals.push_back(Interval(0, order));

		for (const Position& p : field)
		{
			Interval p_interval;
			if (!p.GetInterval(yRow, p_interval.x0, p_interval.x1))
				continue;

			ErodeIntervals(intervals, p_interval);
		}

		if (intervals.size() == 1 && intervals[0].x0 == intervals[0].x1)
			return (long long)intervals[0].x0 * (long long)order + (long long)yRow;
	}

	ASSERT(false);
	return -1;
}

int main()
{
#if 0
	const int yRow = 10;
	const int order = 20;
	std::istringstream in(
R"(2,18:-2,15
9,16:10,16
13,2:15,3
12,14:10,16
10,20:10,16
14,17:10,16
8,7:2,10
2,0:2,10
0,11:2,10
20,14:25,17
17,20:21,22
16,7:15,3
14,3:15,3
20,1:15,3)");
#else
	const int yRow = 2000000;
	const int order = 4000000;
	std::ifstream in("input1.txt");
#endif

#if 0
	int cnt = PartOne(in, yRow);  // 5040643
	std::cout << cnt << std::endl;
#else
	long long tuning_freq = PartTwo(in, order);  // 11016575214126
	std::cout << tuning_freq << std::endl;
#endif
}
