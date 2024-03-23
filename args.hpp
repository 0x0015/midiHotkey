#pragma once
#include <span>
#include <optional>
#include <string>

class argParse{
public:
	std::optional<std::string> autoLoadFile = std::nullopt;
	std::optional<int> autoOpenPort = std::nullopt;
	bool silent = false;
	void parse(const std::span<char*> args);
};

