// Advent Of Code 2022
// Day 08
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

#ifdef _DEBUG
#include <assert.h>
#define ASSERT(x)   assert(x)
#else
#define ASSERT(x)   ((void)0)
#endif



void Alloc(std::vector<std::vector<int>>& matrix, size_t N)
{
	matrix.resize(N);
	for (size_t i = 0; i < N; ++i)
		matrix[i].resize(N);
}

std::vector<std::vector<int>> ParseMatrix(std::istream& in)
{
	std::vector<std::vector<int>> matrix;

	size_t ir = 0;
	for (std::string line; std::getline(in, line); ++ir)
	{
		if (line.empty())
			break;

		if (ir == 0)
		{
			ASSERT(matrix.empty());
			Alloc(matrix, line.size());
		}
		else
			ASSERT(!matrix.empty() && matrix.size() == line.size());

		for (size_t ic = 0; ic < line.size(); ++ic)
			matrix[ir][ic] = line[ic] - '0';
	}

	return std::move(matrix);
}

bool Visible(const std::vector<std::vector<int>>& matrix, const size_t ir, const size_t ic)
{
	ASSERT(0 <= ir && ir < matrix.size());
	ASSERT(0 <= ic && ic < matrix[ir].size());

	{
		size_t _ir;

		for (_ir = 0; _ir < ir; ++_ir)
			if (matrix[_ir][ic] >= matrix[ir][ic])
				break;
		if (_ir >= ir)
			return true;

		for (_ir = ir + 1; _ir < matrix.size(); ++_ir)
			if (matrix[_ir][ic] >= matrix[ir][ic])
				break;
		if (_ir >= matrix.size())
			return true;
	}

	{
		size_t _ic;

		for (_ic = 0; _ic < ic; ++_ic)
			if (matrix[ir][_ic] >= matrix[ir][ic])
				break;
		if (_ic >= ic)
			return true;

		for (_ic = ic + 1; _ic < matrix[ir].size(); ++_ic)
			if (matrix[ir][_ic] >= matrix[ir][ic])
				break;
		if (_ic >= matrix[ir].size())
			return true;
	}

	return false;
}

int PartOne(std::istream& in)
{
	std::vector<std::vector<int>> matrix = std::move(ParseMatrix(in));

	int count = 0;
	for (size_t ir = 0; ir < matrix.size(); ++ir)
		for (size_t ic = 0; ic < matrix[ir].size(); ++ic)
			if (Visible(matrix, ir, ic))
				++count;

	return count;
}

int ScenicScore(const std::vector<std::vector<int>>& matrix, const size_t ir, const size_t ic)
{
	ASSERT(0 <= ir && ir < matrix.size());
	ASSERT(0 <= ic && ic < matrix[ir].size());

	int cnt, i, scenic_score = 1;

	for (cnt = 0, i = 1; ir >= i; ++i)
	{
		++cnt;
		if (matrix[ir][ic] <= matrix[ir - i][ic])
			break;
	}
	scenic_score *= cnt;

	for (cnt = 0, i = 1; ir+i < matrix.size(); ++i)
	{
		++cnt;
		if (matrix[ir][ic] <= matrix[ir+i][ic])
			break;
	}
	scenic_score *= cnt;

	for (cnt = 0, i = 1; ic >= i; ++i)
	{
		++cnt;
		if (matrix[ir][ic] <= matrix[ir][ic-i])
			break;
	}
	scenic_score *= cnt;

	for (cnt = 0, i = 1; ic+i < matrix[ir].size(); ++i)
	{
		++cnt;
		if (matrix[ir][ic] <= matrix[ir][ic+i])
			break;
	}
	scenic_score *= cnt;

	return scenic_score;
}

int PartTwo(std::istream& in)
{
	std::vector<std::vector<int>> matrix = std::move(ParseMatrix(in));

	int best_score = 0;
	for (size_t ir = 1; ir < matrix.size()-1; ++ir)
		for (size_t ic = 1; ic < matrix[ir].size() - 1; ++ic)
		{
			int score = ScenicScore(matrix, ir, ic);
			if (best_score < score)
				best_score = score;
		}

	return best_score;
}

int main()
{
#if 0
	std::istringstream in(
R"(30373
25512
65332
33549
35390)");
#else
	std::ifstream in("input1.txt");
#endif

#if 0
	int count = PartOne(in);
	std::cout << count << std::endl;  // 1798
#else
	int count = PartTwo(in);
	std::cout << count << std::endl;  // 259308
#endif
}
