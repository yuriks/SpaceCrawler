#pragma once
#include <string>

std::string getNextCsvField(const std::string& str, std::string::size_type& cur_pos);
