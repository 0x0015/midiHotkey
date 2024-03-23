#include "load.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

void saveCommands(const std::string& filename, const std::vector<std::pair<uint8_t, std::string>>& commands){
	json output;
	for(const auto& o : commands){
		json object;
		object["key"] = o.first;
		object["command"] = o.second;
		output.push_back(object);
	}
	std::ofstream file(filename);
	file << std::setw(1) << std::setfill('\t') << output <<std::endl;
}

void loadCommands(const std::string& filename, std::vector<std::pair<uint8_t, std::string>>& commands){
	std::ifstream file(filename);
	json input = json::parse(file);
	commands.clear();
	for(const auto& entry : input){
		if(!entry.contains("key") || !entry.contains("command"))
			continue;
		uint8_t key = entry["key"].get<uint8_t>();
		std::string command = entry["command"].get<std::string>();
		commands.push_back({key, command});
	}
}

