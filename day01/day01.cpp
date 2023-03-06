// Advent Of Code 2022
// Day 01
//
// (c) 2023 Federico Molara <federico@molara.net>
// This code is licensed under MIT license (see LICENSE.txt for details)

#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <vector>
#include <string>

#ifdef _DEBUG
	#include <assert.h>
	#define ASSERT(x)   assert(x)
#else
	#define ASSERT(x)   ((void)0)
#endif



struct Elf
{
	std::vector<int> food;

	Elf() {}
	Elf(std::vector<int> v) : food(v) {}
	int Sum() { return std::reduce(food.begin(), food.end()); }
};


int main()
{
#if 0
	std::istringstream in(
R"(1000
2000
3000

4000

5000
6000

7000
8000
9000

10000)");
#else
	std::ifstream in("input1.txt");
#endif


	std::vector<Elf> elves;

	while (in)
	{
		Elf elf;

		for (std::string line; std::getline(in, line);)
		{
			if (line.empty())
				break;
			elf.food.push_back(std::stoi(line));
		}

		elves.push_back(elf);
	}

	int max[] = {0, 0, 0};
	for(auto elf : elves)
	{
		int sum = elf.Sum();
		if (sum > max[0])
		{
			max[2] = max[1];
			max[1] = max[0];
			max[0] = sum;
		}
		else if (sum > max[1])
		{
			max[2] = max[1];
			max[1] = sum;
		}
		else if (sum > max[2])
		{
			max[2] = sum;
		}
	}

	std::cout << max[0] << std::endl;  // 67450
	std::cout << max[1] << std::endl;
	std::cout << max[2] << std::endl;
	std::cout << std::endl;
	std::cout << max[0]+max[1]+max[2] << std::endl;  // 199357
}