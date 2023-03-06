// Advent Of Code 2022
// Day 20
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



void Move(const std::vector<int64_t>& values, std::vector<size_t>& perm, std::vector<size_t>& perm_inv, size_t i)
{
	ASSERT(values.size() == perm.size());

	int64_t value = values[i];
	const size_t index = perm_inv[i];

	if (value < 0)
	{
		const int64_t cnt = -(value / ((int64_t)values.size() - 1));
		value += (cnt+1) * ((int64_t)values.size()-1);
	}

	ASSERT(value >= 0);
	const size_t new_index = (index + (size_t)value) % (values.size()-1);

	if (index < new_index)
	{
		size_t tmp = perm[index];
		for (size_t k = index; k < new_index; ++k)
		{
			perm[k] = perm[k+1];
			perm_inv[perm[k]] = k;
		}
		perm[new_index] = tmp;
		perm_inv[perm[new_index]] = new_index;
	}
	else if (new_index < index)
	{
		size_t tmp = perm[index];
		size_t tmp_inv = perm_inv[index];
		for (size_t k = index; k > new_index; --k)
		{
			perm[k] = perm[k-1];
			perm_inv[perm[k]] = k;
		}
		perm[new_index] = tmp;
		perm_inv[perm[new_index]] = new_index;
	}
}

void Print(const std::vector<int64_t>& values, const std::vector<size_t>& perm)
{
	ASSERT(values.size() == perm.size());

	for (auto it = perm.begin(); it != perm.end(); ++it)
	{
		if (it != perm.begin())
			std::cout << ", ";
		std::cout << values[*it];
	}
	std::cout << std::endl;
}

inline
int64_t GetAt(const std::vector<int64_t>& values, std::vector<size_t>& perm, size_t i)
{
	ASSERT(values.size() == perm.size());

	const size_t zero_index = std::find(values.begin(), values.end(), 0) - values.begin();
	ASSERT(0 <= zero_index && zero_index < values.size());

	const size_t index = std::find(perm.begin(), perm.end(), zero_index) - perm.begin();
	ASSERT(0 <= index && index < perm.size());

	const size_t j = (i+index) % perm.size();
	return values[perm[j]];
}

int64_t Common(std::istream& in, int64_t key = 1, int shuffle_times = 1)
{
	std::vector<int64_t> values;
	for (std::string line; std::getline(in, line); )
	{
		const int64_t value = std::atoi(line.c_str());
		values.push_back(value * key);
	}

	std::vector<size_t> perm(values.size(), 0);
	std::vector<size_t> perm_inv(values.size(), 0);
	for (size_t i = 0; i < perm.size(); ++i)
	{
		perm[i] = i;
		perm_inv[i] = i;
	}

	for (; shuffle_times > 0; --shuffle_times)
		for (size_t i = 0; i < values.size(); ++i)
			Move(values, perm, perm_inv, i);

//	Print(values, perm);

	int64_t sum = GetAt(values, perm, 1000)  // -2520
	            + GetAt(values, perm, 2000)  // 1833
	            + GetAt(values, perm, 3000); // 8082

	return sum;
}

int64_t PartOne(std::istream& in) { return Common(in,         1,  1); }
int64_t PartTwo(std::istream& in) { return Common(in, 811589153, 10); }

int main()
{
#if 0
	std::istringstream in(
R"(1
2
-3
3
-2
0
4)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	int64_t sum = PartOne(in);  // 7395
	std::cout << sum << std::endl;
#else
	int64_t sum = PartTwo(in);  // 1640221678213
	std::cout << sum << std::endl;
#endif
}
