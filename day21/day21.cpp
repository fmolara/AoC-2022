// Advent Of Code 2022
// Day 21
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




struct Rational
{
	int64_t num, den;

	Rational()
		: Rational(0, 1)
	{
	}

	Rational(int64_t value)
		: Rational(value, 1)
	{
	}

	Rational(int64_t _num, int64_t _den)
		: num(_num), den(_den)
	{
		ASSERT(den != 0);
		Normalize();
	}

	void Normalize()
	{
		if (den < 0)
		{
			num = -num;
			den = -den;
		}
		const int64_t gcd = Euclide(num, den);
		num /= gcd;
		den /= gcd;
	}

	static int64_t Euclide(int64_t a, int64_t b)
	{
		int64_t r;
		while (b != 0) //ripetere finché non riduciamo a zero
		{
			r = a % b;
			a = b;
			b = r; //scambiamo il ruolo di a e b
		}

		return a; //... e quando b è (o è diventato) 0, il risultato è a
	}
};

Rational operator + (const Rational& l, const Rational& r) { return Rational(l.num*r.den + r.num*l.den, l.den*r.den); }
Rational operator - (const Rational& l, const Rational& r) { return Rational(l.num*r.den - r.num*l.den, l.den*r.den); }
Rational operator * (const Rational& l, const Rational& r) { return Rational(l.num*r.num, l.den*r.den); }
Rational operator / (const Rational& l, const Rational& r) { return Rational(l.num*r.den, l.den*r.num); }

bool operator == (const Rational& l, const Rational& r) { return l.num == r.num && l.den == r.den; }

struct Value
{
	std::vector<Rational>  poly;

	Value()              { poly.push_back(Rational()); }
	Value(int64_t value) { poly.push_back(Rational(value)); }
};

Value operator + (const Value& l, const Value& r)
{
	ASSERT(l.poly.size() > 0 && r.poly.size() > 0);

	Value res;
	res.poly.resize(std::max(l.poly.size(), r.poly.size()));
	for (size_t i = 0; i < res.poly.size(); ++i)
	{
		res.poly[i] = (i < l.poly.size() ? l.poly[i] : 0)
			        + (i < r.poly.size() ? r.poly[i] : 0);
	}

	return res;
}

Value operator - (const Value& l, const Value& r)
{
	ASSERT(l.poly.size() > 0 && r.poly.size() > 0);

	Value res;
	res.poly.resize(std::max(l.poly.size(), r.poly.size()));
	for (size_t i = 0; i < res.poly.size(); ++i)
	{
		res.poly[i] = (i < l.poly.size() ? l.poly[i] : 0)
			        - (i < r.poly.size() ? r.poly[i] : 0);
	}

	while (res.poly.size() > 1  &&  res.poly.back() == Rational(0))
		res.poly.pop_back();

	return res;
}

Value operator * (const Value& l, const Value& r)
{
	ASSERT(l.poly.size() > 0 && r.poly.size() > 0);

	if (l.poly.size() < r.poly.size())
		return r * l;

	Value res;
	res.poly.resize(r.poly.size() + l.poly.size() - 1);
	for (size_t i = 0; i < l.poly.size(); ++i)
	{
		for (size_t j = 0; j < r.poly.size(); ++j)
		{
			res.poly[j+i] = res.poly[j + i] + r.poly[j] * l.poly[i];
		}
	}

	return res;
}

Value operator / (const Value& l, const Value& r)
{
	ASSERT(l.poly.size() > 0 && r.poly.size() == 1);

	Value res = l;
	for (size_t i = 0; i < res.poly.size(); ++i)
	{
		res.poly[i] = res.poly[i] / r.poly[0];
	}

	return res;
}

struct Monkey
{
	std::string name;

	std::string left;
	std::string right;
	Value       value;
	char        op;

	Monkey(const std::string& _name, int64_t _value)
		: name(_name)
		, op('\0'), value(_value)
	{}
	Monkey(const std::string& _name, const std::string& _left, char _op, const std::string& _right)
		: name(_name)
		, left(_left), op(_op), right(_right)
	{}
};

void ParseMonkeys(std::istream& in, std::map<std::string, Monkey>& monkeys)
{
	monkeys.clear();

	for (std::string line; std::getline(in, line); )
	{
		auto parts = split(line, ": ");

		ASSERT(parts.size() == 2);
		std::string& name = parts[0];

		auto subparts = split(parts[1], " ");
		ASSERT(subparts.size() == 1 || (subparts.size() == 3 && (subparts[1] == "+" || subparts[1] == "-" || subparts[1] == "*" || subparts[1] == "/")));

		if (subparts.size() == 1)
			monkeys.insert(std::make_pair(name, Monkey(name, std::atoi(subparts[0].c_str()))));
		else
			monkeys.insert(std::make_pair(name, Monkey(name, subparts[0], subparts[1][0], subparts[2])));
	}
}

Value Eval(const std::map<std::string, Monkey>& monkeys, const std::string& name)
{
	auto it = monkeys.find(name);
	ASSERT(it != monkeys.end() && it->second.name == name);

	if (it->second.op == '\0')
		return it->second.value;

	const Value lvalue = Eval(monkeys, it->second.left);
	const Value rvalue = Eval(monkeys, it->second.right);

	switch (it->second.op)
	{
	case '+':  return lvalue + rvalue;
	case '-':  return lvalue - rvalue;
	case '*':  return lvalue * rvalue;
	case '/':  return lvalue / rvalue;
	}

	ASSERT(false);
	return Value();
}

int64_t PartOne(std::istream& in)
{
	std::map<std::string, Monkey> monkeys;
	ParseMonkeys(in, monkeys);

	const Value res = Eval(monkeys, "root");
	ASSERT(res.poly.size() == 1);

	ASSERT(res.poly[0].den == 1);
	return res.poly[0].num;
}

int64_t PartTwo(std::istream& in)
{
	std::map<std::string, Monkey> monkeys;
	ParseMonkeys(in, monkeys);

	ASSERT(monkeys.at("humn").op == '\0');
	ASSERT(monkeys.at("humn").value.poly.size()  == 1);
	monkeys.at("humn").value.poly[0] = 0;
	monkeys.at("humn").value.poly.push_back(1);

	const Value lvalue = Eval(monkeys, monkeys.at("root").left);
	const Value rvalue = Eval(monkeys, monkeys.at("root").right);

	const Value eq = lvalue - rvalue;
	ASSERT(eq.poly.size() == 2);

	auto res = eq.poly[0] / eq.poly[1];
	ASSERT(res.den == 1);

	return -res.num;
}

int main()
{
#if 0
	std::istringstream in(
R"(root: pppw + sjmn
dbpl: 5
cczh: sllz + lgvd
zczc: 2
ptdq: humn - dvpt
dvpt: 3
lfqf: 4
humn: 5
ljgn: 2
sjmn: drzm * dbpl
sllz: 4
pppw: cczh / lfqf
lgvd: ljgn * ptdq
drzm: hmdt - zczc
hmdt: 32
)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	int64_t eval = PartOne(in);  // 291425799367130
	std::cout << eval << std::endl;
#else
	int64_t eval = PartTwo(in);  // 3219579395609
	std::cout << eval << std::endl;
#endif
}
