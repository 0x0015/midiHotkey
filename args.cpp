#include "args.hpp"
#include <iostream>
#include <charconv>

constexpr bool argParseDebug = false;

void argParse::parse(const std::span<char*> args){
	//an initial pass to make sure help isn't requested before spitting out other messages or whatever
	for(const auto& o : args){
		if(std::string_view(o) == "-h"){
			std::cout<<"midiHotkey Help:\n\t-p {int} \t Open up the midi port with the corrosponding number upon startup\n\t-f {string} \t Load the given command file on startup\n\t-s \t\t Do not open the gui (but still listen for midi)\n\t-h \t\t See this help message"<<std::endl;
			exit(0);
		}
	}

	bool lastArgWasFile = false;
	bool lastArgWasPort = false;
	for(unsigned int i=0;i<args.size();i++){
		std::string_view arg(args[i]);
		if constexpr(argParseDebug) std::cout<<arg<<std::endl;

		if(i == 0)
			continue;

		if(lastArgWasPort){
			int portAssignment;
			auto result = std::from_chars<int>(arg.data(), arg.data()+arg.size(), portAssignment);
			if(result.ec == std::errc::invalid_argument){
				std::cerr<<"Could not convert \""<<arg<<"\" into a port number (integer)"<<std::endl;
			}else{
				autoOpenPort = portAssignment;
			}
			lastArgWasPort = false;
			continue;
		}

		if(lastArgWasFile){
			autoLoadFile = std::string(arg);
			lastArgWasFile = false;
			continue;
		}

		if(arg == "-p"){
			lastArgWasPort = true;
			continue;
		}

		if(arg == "-f"){
			lastArgWasFile = true;
			continue;
		}

		if(arg == "-s"){
			silent = true;
			continue;
		}

		if(arg == "-h"){
			continue;
		}

		std::cerr<<"Unknown argument \""<<arg<<"\""<<std::endl;
	}
}
