// Advent Of Code 2022
// Day 16
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

#include "..\stuffs\Dijkstra.h"
#include "..\stuffs\Splitter.h"

#ifdef _DEBUG
#include <assert.h>
#define ASSERT(x)   assert(x)
#else
#define ASSERT(x)   ((void)0)
#endif




struct Valve
{
	std::string               name;
	int                       flow_rate;
	bool                      open;
	std::vector<std::string>  lead_to;
};

struct Cave
{
	std::vector<Valve*>           valves;
	std::map<std::string, Valve*> by_names;

	~Cave()
	{
		Clear();
	}
	void Clear()
	{
		for (size_t i = 0; i < valves.size(); ++i)
		{
			delete valves[i];
			valves[i] = nullptr;
		}
		valves.clear();
		by_names.clear();
	}
};

struct ReducedCave
{
	std::vector<Valve*>           valves;
	std::vector<std::vector<int>> matrix;

	void Clear()
	{
		valves.clear();
		matrix.clear();
	}

	static constexpr auto npos{ static_cast<size_t>(-1) };

	size_t ValveToIndex(const Valve* valve) const
	{
		auto jt = std::find(valves.begin(), valves.end(), valve);
		if (jt == valves.end())
			return npos;

		return jt - valves.begin();
	}
	size_t ValveToIndex(const std::string name) const
	{
		auto jt = std::find_if(valves.begin(), valves.end()
			, [&name](const Valve* valve) { return valve->name == name; });
		if (jt == valves.end())
			return npos;

		return jt - valves.begin();
	}
};

void ParseCave(std::istream& in, Cave& cave)
{
	cave.Clear();
	for (std::string line; std::getline(in, line); )
	{
		auto parts = split(line, ";");
		ASSERT(parts.size() == 2);

		Valve* valve = new Valve;

		// Left part
		{
			auto l_parts = split(parts[0], "=");
			ASSERT(l_parts.size() == 2);
			valve->name = l_parts[0];
			valve->flow_rate = std::atoi(l_parts[1].c_str());
		}

		// Right part
		{
			auto r_parts = split(parts[1], ",");
			ASSERT(r_parts.size() > 0);  // may be not!

			for (auto lead_to : r_parts)
				valve->lead_to.push_back(lead_to);
		}

		valve->open = false;
		cave.valves.push_back(valve);

		ASSERT(!cave.by_names.contains(valve->name));
		cave.by_names.insert(std::make_pair(valve->name, valve));
	}

	std::sort(
		  cave.valves.begin()
		, cave.valves.end()
		, [](const Valve* l, const Valve* r) { return l->flow_rate > r->flow_rate; }
	);

	for (auto it = cave.valves.begin(); it != cave.valves.end(); ++it)
	{
		Valve* const valve = *it;

		if (valve->lead_to.size() <= 1)
			continue;

		std::sort(
			  valve->lead_to.begin()
			, valve->lead_to.end()
			, [&cave](const std::string& l, const std::string& r) {
				ASSERT(cave.by_names.find(l) != cave.by_names.end());
				ASSERT(cave.by_names.find(r) != cave.by_names.end());
				return cave.by_names.find(l)->second->flow_rate > cave.by_names.find(r)->second->flow_rate;
			}
		);
	}
}

int BestExtimate(const ReducedCave& reduced, int remaining)
{
	int total_pressure = 0;
	for (auto it = reduced.valves.cbegin(); it != reduced.valves.cend() && remaining > 0; ++it)
	{
		if ((*it)->open)
			continue;

		if ((*it)->flow_rate == 0)
			break;

		--remaining;  // open valve
		total_pressure += (*it)->flow_rate * remaining;
		--remaining;  // go-next
	}
	return total_pressure;
}

void MakePerm(const ReducedCave& reduced, std::vector<size_t>& perm, size_t node_idx, int remaining)
{
	ASSERT(reduced.valves.size() == reduced.matrix.size());
	perm.resize(0);
	for (size_t i = 0; i < reduced.valves.size(); ++i)
	{
		if (i == node_idx)
			continue;
		perm.push_back(i);
	}

	std::sort(
		perm.begin()
		, perm.end()
		, [&](size_t lnode, size_t rnode) {
			const int l_flow = (reduced.valves[lnode]->open ? 0 : reduced.valves[lnode]->flow_rate);
			const int r_flow = (reduced.valves[rnode]->open ? 0 : reduced.valves[rnode]->flow_rate);
			return (remaining - reduced.matrix[node_idx][lnode]) * l_flow > (remaining - reduced.matrix[node_idx][rnode]) * r_flow;
		}
	);
}

void Visit(ReducedCave& reduced, const size_t curr_node, int remaining, int curr_pressure, int& best_pressure_ever)
{
	ASSERT(remaining >= 0);

	if (remaining <= 1)
	{
		if (best_pressure_ever < curr_pressure)
			best_pressure_ever = curr_pressure;
		return;
	}

	Valve* curr_valve = reduced.valves[curr_node];

	if (curr_valve->flow_rate > 0 && !curr_valve->open)
	{
		curr_valve->open = true;
		remaining -= 1;
		curr_pressure += remaining * curr_valve->flow_rate;

		if (best_pressure_ever < curr_pressure)
			best_pressure_ever = curr_pressure;
	}

	std::vector<size_t> perm;
	MakePerm(reduced, perm, curr_node, remaining);

	const int best_extimate = BestExtimate(reduced, remaining);
	for (const auto& next_node : perm)
	{
		if (curr_pressure + best_extimate < best_pressure_ever)
			break;
		if (reduced.valves[next_node]->open || reduced.valves[next_node]->flow_rate == 0)
			break;
		Visit(reduced, next_node, remaining - reduced.matrix[curr_node][next_node], curr_pressure, best_pressure_ever);
	}

	if (curr_valve->flow_rate > 0 && curr_valve->open)
	{
		curr_pressure -= remaining * curr_valve->flow_rate;
		remaining -= 1;
		curr_valve->open = false;
	}
}

void CaveToGraph(const Cave& cave, DirectedGraph<Valve*, int>& graph)
{
	graph.Clear();
	for (auto it = cave.valves.begin(); it != cave.valves.end(); ++it)
		graph.AddNode(*it);

	for (auto it = cave.valves.begin(); it != cave.valves.end(); ++it)
	{
		Valve* const valve = *it;

		size_t i = graph.GetNodeIndex(valve);
		for (auto jt = valve->lead_to.cbegin(); jt != valve->lead_to.cend(); ++jt)
		{
			size_t j = graph.GetNodeIndex(cave.by_names.find(*jt)->second);
			graph.AddEdge(i, j, 1);
		}
	}
}

void GraphToReduced(const Cave& cave, const DirectedGraph<Valve*, int>& graph, ReducedCave& reduced)
{
	reduced.Clear();
	for (auto it = cave.valves.begin(); it != cave.valves.end(); ++it)
	{
		Valve* const valve = *it;

		if (valve->name != "AA" && valve->flow_rate == 0)
			continue;

		reduced.valves.push_back(valve);
	}

	reduced.matrix.resize(reduced.valves.size());
	for (auto it = reduced.matrix.begin(); it != reduced.matrix.end(); ++it)
		it->assign(reduced.valves.size(), 0);

	std::vector<int> dist;
	std::vector<size_t> path;
	for (auto it = reduced.valves.begin(); it != reduced.valves.end(); ++it)
	{
		Valve* const valve_i = *it;

		Dijkstra(graph, graph.GetNodeIndex(valve_i), graph.npos, path, dist);

		size_t startNodeIndex = reduced.ValveToIndex(valve_i);
		ASSERT(startNodeIndex != reduced.npos);
		for (size_t j = 0; j < dist.size(); ++j)
		{
			Valve* const valve_j = graph.GetNodeValue(j);
			size_t endNodeIndex = reduced.ValveToIndex(valve_j);
			if (endNodeIndex != reduced.npos)
				reduced.matrix[startNodeIndex][endNodeIndex] = dist[j];
			else
				ASSERT(valve_j->flow_rate == 0);
		}
	}
}

int PartOne(std::istream& in)
{
	Cave cave;
	ParseCave(in, cave);

	ReducedCave reduced;
	{
		DirectedGraph<Valve*, int> graph;
		CaveToGraph(cave, graph);

		GraphToReduced(cave, graph, reduced);
	}

	const size_t node_AA = reduced.ValveToIndex("AA");
	const int remaining = 30;

	ASSERT(node_AA != reduced.npos && reduced.valves[node_AA]->name == "AA");

	int best_pressure_ever = 0;
	Visit(reduced, node_AA, remaining, 0, best_pressure_ever);

	return best_pressure_ever;
}

void BestExtimate2_DoStep(const ReducedCave& reduced, int& total_pressure, int& remainingNN)
{
	ASSERT(remainingNN >= 0);

	for (auto it = reduced.valves.cbegin(); it != reduced.valves.cend(); ++it)
	{
		if ((*it)->open)
			continue;

		if ((*it)->flow_rate == 0)
		{
			remainingNN = 0;
			return;
		}

		--remainingNN;  // open valve
		total_pressure += (*it)->flow_rate * remainingNN;
		--remainingNN;  // go-next
		return;
	}
	ASSERT(false);
}

int BestExtimate2(const ReducedCave& reduced, int remaining0, int remaining1)
{
	ASSERT(remaining0 >= remaining1);

	int total_pressure = 0;
	while (remaining0 > 0 || remaining1 > 0)
		if (remaining0 >= remaining1)
			BestExtimate2_DoStep(reduced, total_pressure, remaining0);
		else
			BestExtimate2_DoStep(reduced, total_pressure, remaining1);
	return total_pressure;
}

void Visit2(ReducedCave& reduced, const size_t curr_node0, int remaining0, const size_t curr_node1, int remaining1, int curr_pressure, int& best_pressure_ever);

void Visit2_(ReducedCave& reduced, const size_t curr_node0, int remaining0, const size_t curr_node1, int remaining1, int curr_pressure, int& best_pressure_ever)
{
	Valve* curr_valve0 = reduced.valves[curr_node0];
	Valve* curr_valve1 = reduced.valves[curr_node1];

	ASSERT(curr_valve0->flow_rate > 0 && !curr_valve0->open);

	curr_valve0->open = true;
	remaining0 -= 1;
	curr_pressure += remaining0 * curr_valve0->flow_rate;

	if (best_pressure_ever < curr_pressure)
		best_pressure_ever = curr_pressure;

	if (remaining0 >= remaining1)
		Visit2(reduced, curr_node0, remaining0, curr_node1, remaining1, curr_pressure, best_pressure_ever);
	else
		Visit2(reduced, curr_node1, remaining1, curr_node0, remaining0, curr_pressure, best_pressure_ever);

	curr_pressure -= remaining0 * curr_valve0->flow_rate;
	remaining0 += 1;
	curr_valve0->open = false;
}

void Visit2(ReducedCave& reduced, const size_t curr_node0, int remaining0, const size_t curr_node1, int remaining1, int curr_pressure, int& best_pressure_ever)
{
	ASSERT(remaining0 >= remaining1);

	Valve* curr_valve0 = reduced.valves[curr_node0];
	Valve* curr_valve1 = reduced.valves[curr_node1];

	if (remaining0 <= 1)
	{
		ASSERT(remaining1 <= 1);
		if (best_pressure_ever < curr_pressure)
			best_pressure_ever = curr_pressure;
		return;
	}

	std::vector<size_t> perm;

	if (remaining0 > remaining1)
	{
		const int best_extimate = BestExtimate2(reduced, remaining0, remaining1);
		MakePerm(reduced, perm, curr_node0, remaining0);
		for (const auto& next_node0 : perm)
		{
			if (curr_pressure + best_extimate < best_pressure_ever)
				break;
			if (reduced.valves[next_node0]->open || reduced.valves[next_node0]->flow_rate == 0)
				break;
			const int dist = reduced.matrix[curr_node0][next_node0];
			ASSERT(dist > 0);
			Visit2_(reduced, next_node0, remaining0 - dist, curr_node1, remaining1, curr_pressure, best_pressure_ever);
		}
	}
	else if (remaining0 == remaining1)
	{
		const int best_extimate = BestExtimate2(reduced, remaining0, remaining1);
		MakePerm(reduced, perm, curr_node1, remaining1);
		for (const auto& next_node1 : perm)
		{
			if (curr_pressure + best_extimate < best_pressure_ever)
				break;
			if (reduced.valves[next_node1]->open || reduced.valves[next_node1]->flow_rate == 0)
				break;
			const int dist = reduced.matrix[curr_node1][next_node1];
			ASSERT(dist > 0);
			Visit2_(reduced, next_node1, remaining1 - dist, curr_node0, remaining0, curr_pressure, best_pressure_ever);
		}
	}
	else
	{
		const int best_extimate = BestExtimate2(reduced, remaining1, remaining0);
		MakePerm(reduced, perm, curr_node1, remaining1);
		for (const auto& next_node1 : perm)
		{
			if (curr_pressure + best_extimate < best_pressure_ever)
				break;
			if (reduced.valves[next_node1]->open || reduced.valves[next_node1]->flow_rate == 0)
				break;
			const int dist = reduced.matrix[curr_node1][next_node1];
			ASSERT(dist > 0);
			Visit2_(reduced, next_node1, remaining1 - dist, curr_node0, remaining0, curr_pressure, best_pressure_ever);
		}
	}
	if (best_pressure_ever < curr_pressure)
		best_pressure_ever = curr_pressure;
}

int PartTwo(std::istream& in)
{
	Cave cave;
	ParseCave(in, cave);

	ReducedCave reduced;
	{
		DirectedGraph<Valve*, int> graph;
		CaveToGraph(cave, graph);

		GraphToReduced(cave, graph, reduced);
	}

	const size_t node_AA = reduced.ValveToIndex("AA");
	const int remaining = 26;

	ASSERT(node_AA != reduced.npos && reduced.valves[node_AA]->name == "AA");

	int best_pressure_ever = 0;
	Visit2(reduced, node_AA, remaining, node_AA, remaining, 0, best_pressure_ever);

	return best_pressure_ever;
}

int main()
{
#if 0
	std::istringstream in(
R"(AA=0;DD,II,BB
BB=13;CC,AA
CC=2;DD,BB
DD=20;CC,AA,EE
EE=3;FF,DD
FF=0;EE,GG
GG=0;FF,HH
HH=22;GG
II=0;AA,JJ
JJ=21;II
)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	int best_pressure = PartOne(in);  // 2330
	std::cout << best_pressure << std::endl;
#else
	int best_pressure = PartTwo(in);  // 2675
	std::cout << best_pressure << std::endl;
#endif
}
