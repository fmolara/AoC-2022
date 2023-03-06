// (c) 2023 Federico Molara <federico@molara.net>
// This code is licensed under MIT license (see LICENSE.txt for details)

#pragma once

#include <string_view>

std::vector<std::string> split(const std::string str, const std::string delim)
{
	std::vector<std::string> result;
	for (const auto word : std::views::split(str, delim))
		result.emplace_back(std::string(word.begin(), word.end()));
	return result;
}
