// Advent Of Code 2022
// Day 11
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



enum OperandType { Value, Old };
struct Operand
{
	OperandType type;
	int         value;

	Operand()
		: Operand(Value, 0)
	{
	}

	Operand(OperandType _type, int _value)
		: type(_type)
		, value(_value)
	{
	}
};

class Monkey
{
public:
	static std::vector<Monkey> monkeys;
	static long long worry_divider;
	static long long worry_mod;

public:
	Monkey(int _id, const std::vector<long long>& _items, char _op_operator, const Operand& _op_operand, int _divisible_by, int _thow_true, int _thow_false)
		: id(_id)
		, items(_items)
		, op_operator(_op_operator), op_operand(_op_operand)
		, divisible_by(_divisible_by)
		, thow_true(_thow_true), thow_false(_thow_false)
		, inspection_cnt(0)
	{
	}

	void Turn()
	{
		while (items.size() > 0)
		{
			++inspection_cnt;

			long long item = items.front();
			items.erase(items.begin());

			item = DoOperation(item);
			item /= worry_divider;
			item %= worry_mod;
			monkeys[item % divisible_by == 0 ? thow_true : thow_false].items.push_back(item);
		}
	}

	long InspectionCounter() const { return inspection_cnt; }

protected:
	long long DoOperation(long long item)
	{
		long long value = (op_operand.type == OperandType::Old ? item : op_operand.value);
		switch (op_operator)
		{
			case '+': item += value; break;
			case '-': item -= value; break;
			case '*': item *= value; break;
			case '/': item /= value; break;
			default: ASSERT(false);
		}

		return item;
	}

	const int id;
	std::vector<long long> items;
	const char op_operator;
	const Operand op_operand;
	const int divisible_by;
	const int thow_true;
	const int thow_false;

	long inspection_cnt;
};

std::vector<Monkey> Monkey::monkeys;
long long Monkey::worry_divider = 3;
long long Monkey::worry_mod = 1;

bool ParseMonkey(std::istream& in, int& id, std::vector<long long>& items, char& op_operator, Operand& op_operand, int& divisible_by, int& thow_true, int& thow_false)
{
	std::string line;

	// Monkey NN:
	{
		if (!std::getline(in, line))
			return false;
		static const std::string prefix = "Monkey ";
		ASSERT(line.find(prefix) == 0);
		line.erase(line.begin(), line.begin() + prefix.size());
		id = std::atoi(line.c_str());
	}

	// Starting items :
	{
		if (!std::getline(in, line))
			return false;
		static const std::string prefix = "  Starting items: ";
		ASSERT(line.find(prefix) == 0);
		line.erase(line.begin(), line.begin() + prefix.size());
		auto sline = split(line, ", ");

		items.clear();
		for (auto s : sline)
			items.push_back(std::atoi(s.c_str()));
	}

	// Operation: new = old * 19
	{
		if (!std::getline(in, line))
			return false;
		static const std::string prefix = "  Operation: new = old ";
		ASSERT(line.find(prefix) == 0);
		line.erase(line.begin(), line.begin() + prefix.size());

		auto sline = split(line, " ");
		ASSERT(sline.size() == 2 && sline[0].size() == 1);
		op_operator = sline[0][0];
		op_operand.type = (sline[1] == "old" ? OperandType::Old : OperandType::Value);
		if (op_operand.type == OperandType::Value)
			op_operand.value = std::atoi(sline[1].c_str());
	}

	// Test: divisible by 23
	{
		if (!std::getline(in, line))
			return false;
		static const std::string prefix = "  Test: divisible by ";
		ASSERT(line.find(prefix) == 0);
		line.erase(line.begin(), line.begin() + prefix.size());

		divisible_by = std::atoi(line.c_str());
	}

	// If true: throw to monkey 2
	{
		if (!std::getline(in, line))
			return false;
		static const std::string prefix = "    If true: throw to monkey ";
		ASSERT(line.find(prefix) == 0);
		line.erase(line.begin(), line.begin() + prefix.size());

		thow_true = std::atoi(line.c_str());
	}

	// If false: throw to monkey 3
	{
		if (!std::getline(in, line))
			return false;
		static const std::string prefix = "    If false: throw to monkey ";
		ASSERT(line.find(prefix) == 0);
		line.erase(line.begin(), line.begin() + prefix.size());

		thow_false = std::atoi(line.c_str());
	}

	// \newline
	{
		if (std::getline(in, line))
			ASSERT(line.empty());
	}

	return true;
}

void ParseMonkeys(std::istream& in)
{
	Monkey::monkeys.clear();
	while (true)
	{
		int id; std::vector<long long> items; char op_operator;  Operand op_operand; int divisible_by; int thow_true, thow_false;
		if (!ParseMonkey(in, id, items, op_operator, op_operand, divisible_by, thow_true, thow_false))
			break;

		Monkey::monkeys.push_back(Monkey(id, items, op_operator, op_operand, divisible_by, thow_true, thow_false));
		Monkey::worry_mod *= divisible_by;
	}
}

long long PartCommon(std::istream& in, int TURNS)
{
	ParseMonkeys(in);

	for (int turn = 0; turn < TURNS; ++turn)
	{
		for (auto it = Monkey::monkeys.begin(); it != Monkey::monkeys.end(); ++it)
		{
			it->Turn();
		}
	}

	int max1 = -1, max2 = -1;
	for (auto it = Monkey::monkeys.begin(); it != Monkey::monkeys.end(); ++it)
	{
		if (it->InspectionCounter() > max1)
		{
			max2 = max1;
			max1 = it->InspectionCounter();
		}
		else if (it->InspectionCounter() > max2)
		{
			max2 = it->InspectionCounter();
		}
	}
	ASSERT(max1 >= 0 && max2 >= 0);

	return (long long)max1 * (long long)max2;
}

long long PartOne(std::istream& in) { return PartCommon(in, 20); }
long long PartTwo(std::istream& in) { return PartCommon(in, 10000); }

int main()
{
#if 0
	std::istringstream in(
R"(Monkey 0:
  Starting items: 79, 98
  Operation: new = old * 19
  Test: divisible by 23
    If true: throw to monkey 2
    If false: throw to monkey 3

Monkey 1:
  Starting items: 54, 65, 75, 74
  Operation: new = old + 6
  Test: divisible by 19
    If true: throw to monkey 2
    If false: throw to monkey 0

Monkey 2:
  Starting items: 79, 60, 97
  Operation: new = old * old
  Test: divisible by 13
    If true: throw to monkey 1
    If false: throw to monkey 3

Monkey 3:
  Starting items: 74
  Operation: new = old + 3
  Test: divisible by 17
    If true: throw to monkey 0
    If false: throw to monkey 1
)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	long long total_strengths = PartOne(in);  // 110220
	std::cout << total_strengths << std::endl;
#else
	Monkey::worry_divider = 1;
	long long total_strengths = PartTwo(in);  // 19457438264
	std::cout << total_strengths << std::endl;
#endif
}
