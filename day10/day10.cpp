// Advent Of Code 2022
// Day 10
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



class CPU
{
public:
	CPU()
		: x(1)
		, cycle_cnt(1)
	{
	}

	void Exec(const std::string& line)
	{
		auto sline = split(line, " ");
		ASSERT(sline.size() >= 1);

		if (sline[0] == "addx")
		{
			ASSERT(sline.size() == 2);
			int op = std::atoi(sline[1].c_str());

			CycleStep(+2);
			x += op;
		}
		else if (sline[0] == "noop")
		{
			ASSERT(sline.size() == 1);
			CycleStep(+1);
		}
		else
			ASSERT(false);
	}

protected:
	virtual void CycleStep(int cycles) = 0;

	int x;
	int cycle_cnt;
};

class CPU_1 : public CPU
{
public:
	CPU_1()
		: CPU()
		, milestones { 20, 60, 100, 140, 180, 220, INT_MAX }
		, curr_milestone(0)
		, total_strengths(0)
	{
	}

	int TotalStrengths() const { return total_strengths;}

private:
	virtual void CycleStep(int cycles) override
	{
		ASSERT(cycle_cnt <= milestones[curr_milestone]);
		cycle_cnt += cycles;
		if (cycle_cnt > milestones[curr_milestone])
		{
			const int strength = x * milestones[curr_milestone];
			total_strengths += strength;

			++curr_milestone;
		}
	}

	std::vector<int> milestones;
	size_t           curr_milestone;
	int              total_strengths;
};

int PartOne(std::istream& in)
{
	CPU_1 cpu;
	for (std::string line; std::getline(in, line);)
	{
		cpu.Exec(line);
	}

	return cpu.TotalStrengths();
}

class CPU_2 : public CPU
{
public:
	CPU_2()
		: CPU()
	{
	}

	const std::string& CRT() const { return sCRT; }

private:
	virtual void CycleStep(int cycles) override
	{
		for (; cycles > 0; --cycles, ++cycle_cnt)
		{
			const int _cycle_cnt = (cycle_cnt - 1) % 40 + 1;
			sCRT.append(1, x <= _cycle_cnt && _cycle_cnt <= x + 2 ? '#' : '.');
		}
	}

	std::string sCRT;
};

std::string PartTwo(std::istream& in)
{
	CPU_2 cpu;
	for (std::string line; std::getline(in, line);)
	{
		cpu.Exec(line);
	}

	return cpu.CRT();
}

int main()
{
#if 0
	std::istringstream in(
R"(addx 15
addx -11
addx 6
addx -3
addx 5
addx -1
addx -8
addx 13
addx 4
noop
addx -1
addx 5
addx -1
addx 5
addx -1
addx 5
addx -1
addx 5
addx -1
addx -35
addx 1
addx 24
addx -19
addx 1
addx 16
addx -11
noop
noop
addx 21
addx -15
noop
noop
addx -3
addx 9
addx 1
addx -3
addx 8
addx 1
addx 5
noop
noop
noop
noop
noop
addx -36
noop
addx 1
addx 7
noop
noop
noop
addx 2
addx 6
noop
noop
noop
noop
noop
addx 1
noop
noop
addx 7
addx 1
noop
addx -13
addx 13
addx 7
noop
addx 1
addx -33
noop
noop
noop
addx 2
noop
noop
noop
addx 8
noop
addx -1
addx 2
addx 1
noop
addx 17
addx -9
addx 1
addx 1
addx -3
addx 11
noop
noop
addx 1
noop
addx 1
noop
noop
addx -13
addx -19
addx 1
addx 3
addx 26
addx -30
addx 12
addx -1
addx 3
addx 1
noop
noop
noop
addx -9
addx 18
addx 1
addx 2
noop
noop
addx 9
noop
noop
noop
addx -1
addx 2
addx -37
addx 1
addx 3
noop
addx 15
addx -21
addx 22
addx -6
addx 1
noop
addx 2
addx 1
noop
addx -10
noop
noop
addx 20
addx 1
addx 2
addx 2
addx -6
addx -11
noop
noop
noop)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	int total_strengths = PartOne(in);  // 14540
	std::cout << total_strengths << std::endl;
#else
	std::string sCRT = PartTwo(in);

	for (size_t i = 0; i < sCRT.size(); ++i)
	{
		if (i % 40 == 0)
			std::cout << std::endl;
		std::cout << sCRT[i];
	}
	// EHZFZHCZ
#endif

}
