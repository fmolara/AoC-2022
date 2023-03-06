// Advent Of Code 2022
// Day 25
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




static const std::string digits = "=-012";

int64_t SNAFU2Decimal(const std::string& snafu)
{
	int64_t decimal = 0, pow5 = 1;
	for (auto it = snafu.rbegin(); it != snafu.rend(); ++it, pow5 *= 5)
	{
		ASSERT(digits.find(*it) != std::string::npos);
		int64_t digit = (int64_t)digits.find(*it) - 2;
		ASSERT(-2 <= digit && digit <= 2);

		decimal += digit*pow5;
	}

	return decimal;
}

std::string Decimal2SNAFU(int64_t decimal)
{
	std::string snafu;
	while (decimal > 0)
	{
		int64_t q = decimal/5;
		int64_t r = decimal%5;

		if (r <= 2)
			snafu.insert(0, 1, digits[r+2]);
		else
		{
			++q;
			snafu.insert(0, 1, digits[r-3]);
		}

		decimal = q;
	}

	return snafu;
}

void Test()
{
	static const
	std::vector<std::pair<int64_t, std::string>> map =
	{
			{          1,            "1" },
			{          2,            "2" },
			{          3,           "1=" },
			{          4,           "1-" },
			{          5,           "10" },
			{          6,           "11" },
			{          7,           "12" },
			{          8,           "2=" },
			{         9,            "2-" },
			{        10,            "20" },
			{        15,           "1=0" },
			{        20,           "1-0" },
			{      2022,        "1=11-2" },
			{     12345,       "1-0---0" },
			{ 314159265, "1121-1110-1=0" },
	};

	for (const auto& item : map)
	{
		const int64_t decimal = SNAFU2Decimal(item.second);
		ASSERT(decimal == item.first);

		const std::string snafu = Decimal2SNAFU(item.first);
		ASSERT(snafu == item.second);
	}
}

std::string PartOne(std::istream& in)
{
	int64_t sum = 0;
	for (std::string line; std::getline(in, line); )
	{
		const int64_t snafu = SNAFU2Decimal(line);
		sum += snafu;
	}

	return Decimal2SNAFU(sum);
}

int main()
{
#if 0
	std::istringstream in(
R"(1=-0-2
12111
2=0=
21
2=01
111
20012
112
1=-1=
1-12
12
1=
122)");
#else
	std::ifstream in("input1.txt");
#endif

//	Test();

#if 1
	std::string snafu = PartOne(in);  // 2=-0=1-0012-=-2=0=01
	std::cout << snafu << std::endl;
#else
#endif
}
