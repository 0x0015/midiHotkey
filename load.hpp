#pragma once
#include <iostream>
#include <vector>
#include <cstdint>

void saveCommands(const std::string& filename, const std::vector<std::pair<uint8_t, std::string>>& commands);
void loadCommands(const std::string& filename, std::vector<std::pair<uint8_t, std::string>>& commands);


