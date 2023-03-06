// Advent Of Code 2022
// Day 19
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



enum ResourceTypes
{
	ore,
	clay,
	obsidian,
	geode,

	LAST
};

const std::map<std::string, ResourceTypes> res_map = {
	{ "ore",      ore      },
	{ "clay",     clay     },
	{ "obsidian", obsidian },
	{ "geode",    geode    },
};

struct Cost
{
	int            amount;
	ResourceTypes  type;
};

struct BluePrint
{
	int                id;
	std::vector<Cost>  costs[4];   // costs[i]  where i == ResourceType
	int                robots[4];  // robots[i]  where i == ResourceType
	int                avail[4];   // avail[i]  where i == ResourceType

	int                best_geode;

	BluePrint()
	{
		Clear();
	}

	void Clear()
	{
		id = -1;
		best_geode = 0;
		for (int i = 0; i < _countof(costs); ++i)
		{
			costs[i].clear();
			robots[i] = 0;
			avail[i] = 0;
		}

		robots[ore] = 1;  // Fortunately, you have exactly one ore-collecting robot in your pack that you can use to kickstart the whole operation.
	}
};

BluePrint ParseBluePrint(const std::string& line)
{
	BluePrint res;

	auto main_parts = split(line, ":");

	ASSERT(main_parts.size() == 2);
	res.id = std::atoi(main_parts[0].c_str());

	auto quadruple = split(main_parts[1], ".");

	ASSERT(quadruple.size() == 5 && quadruple[4].empty());
	for (auto it = quadruple.cbegin(); it != quadruple.cend(); ++it)
	{
		if (it->empty())
			continue;

		auto clauses = split(*it, " ");
		ASSERT(clauses.size() >= 3);

		ASSERT(res_map.contains(clauses[0]));
		const ResourceTypes robot_type = res_map.find(clauses[0])->second;

		size_t i;
		for (i = 1; i+1 < clauses.size(); i += 2)
		{
			Cost cost;
			cost.amount = std::atoi(clauses[i].c_str());
			cost.type = res_map.find(clauses[i + 1])->second;
			ASSERT(0 <= cost.type && cost.type < ResourceTypes::LAST);

			res.costs[robot_type].push_back(cost);

			if (i+2 < clauses.size())
			{
				ASSERT(clauses[i+2] == "and");
				i += 1;
			}
		}
		ASSERT(i == clauses.size());
	}

	return res;
}

void ParseAllBluePrints(std::istream& in, std::vector< BluePrint>& blueprints)
{
	blueprints.clear();

	for (std::string line; std::getline(in, line); )
	{
		BluePrint bp = ParseBluePrint(line);
		ASSERT(bp.id == blueprints.size()+1);
		blueprints.push_back(bp);
	}
}

bool CanBuild(const BluePrint& bp, ResourceTypes type)
{
	ASSERT(0 <= type && type < _countof(bp.costs));
	for (const auto& c : bp.costs[type])
	{
		if (bp.avail[c.type] < c.amount)
			return false;
	}

	return true;
}

void UpdateAvail(BluePrint& bp, bool increase)
{
	// Each robot can collect 1 of its resource type per minute
	for (int type = ResourceTypes::geode; type >= ResourceTypes::ore; --type)
		bp.avail[type] += (increase ? +bp.robots[type] : -bp.robots[type]);
}

void Build(BluePrint& bp, ResourceTypes type, bool increase)
{
	ASSERT(0 <= type && type < _countof(bp.costs));
	for (const auto& c : bp.costs[type])
	{
		if (increase)
		{
			ASSERT(bp.avail[c.type] >= c.amount);
			bp.avail[c.type] -= c.amount;
		}
		else
		{
			bp.avail[c.type] += c.amount;
			ASSERT(bp.avail[c.type] >= c.amount);
		}
	}

	bp.robots[type] += (increase ? +1 : -1);
}

int ExtimateBestGeode(const BluePrint& bp, int minutes)
{
	// Supponiamo di produrre un geode-robot al minuto
	return bp.avail[geode] + (2 * bp.robots[geode] + minutes - 1) * minutes / 2;
}

void TestBluePrint(BluePrint& bp, int minutes)
{
	ASSERT(minutes >= 0);

	if (minutes <= 0)
	{
		if (bp.best_geode < bp.avail[geode])
			bp.best_geode = bp.avail[geode];
		return;
	}

	for (int type = ResourceTypes::geode; type >= ResourceTypes::ore; --type)
	{
		if (ExtimateBestGeode(bp, minutes) < bp.best_geode)
			return;

		if (CanBuild(bp, (ResourceTypes)type))
		{
			UpdateAvail(bp, true);
				Build(bp, (ResourceTypes)type, true);
					TestBluePrint(bp, minutes-1);
				Build(bp, (ResourceTypes)type, false);
			UpdateAvail(bp, false);
		}
	}

	if (ExtimateBestGeode(bp, minutes) < bp.best_geode)
		return;

	// Ricorsiona anche senza buildare nulla
	UpdateAvail(bp, true);
		TestBluePrint(bp, minutes - 1);
	UpdateAvail(bp, false);
}

int PartOne(std::istream& in, const int available_minutes = 24)
{
	std::vector< BluePrint> blueprints;
	ParseAllBluePrints(in, blueprints);

	int sum = 0;
	for (size_t i = 0; i < blueprints.size(); ++i)
	{
		std::cout << i+1 << '/' << blueprints.size() << " " << sum << std::endl;

		auto& bp = blueprints[i];

		TestBluePrint(bp, available_minutes);
		const int quality_level = bp.id * bp.best_geode;
		sum += quality_level;
	}

	return sum;
}

int PartTwo(std::istream& in, const int available_minutes = 32)
{
	std::vector< BluePrint> blueprints;
	ParseAllBluePrints(in, blueprints);

	int mult = 1;
	for (size_t i = 0; i < blueprints.size(); ++i)
	{
		if (i >= 3)
			break;

		auto& bp = blueprints[i];

		TestBluePrint(bp, available_minutes);
		mult *= bp.best_geode;
	}

	return mult;
}

int main()
{
#if 0
	std::istringstream in(
R"(1:ore 4 ore.clay 2 ore.obsidian 3 ore and 14 clay.geode 2 ore and 7 obsidian.
2:ore 2 ore.clay 3 ore.obsidian 3 ore and 8 clay.geode 3 ore and 12 obsidian.
)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	int sum = PartOne(in);  // 1624
	std::cout << sum << std::endl;
#else
	int mult = PartTwo(in);  // 12628
	std::cout << mult << std::endl;
#endif
}
