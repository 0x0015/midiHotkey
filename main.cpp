#include <iostream>
#include <simpleGUI/simpleGUI.hpp>
#include "midiHandler.hpp"
#include "load.hpp"
#include "args.hpp"

constexpr bool midiMessageDebug = false;
constexpr bool midiMessageNotes = false;

std::string hexToStr(uint8_t val){
	std::stringstream hex_str;
	hex_str<<std::hex<<(int)val;
	return hex_str.str();
}

int main(int argc, char** argv){
	argParse args;
	args.parse(std::span<char*>{argv, (size_t)argc});

	simpleGUI::init();
	midiHandler midi;
	if(args.autoOpenPort){
		std::cout<<"Opening port "<<*args.autoOpenPort<<" by user request"<<std::endl;
		midi.selectedPort = *args.autoOpenPort;
	}else{
		std::cout<<"Opening default midi port 0"<<std::endl;
	}
	midi.openSelectedPort();

	std::vector<std::pair<uint8_t, std::string>> commands;
	if(args.autoLoadFile)
		loadCommands(*args.autoLoadFile, commands);

	std::optional<uint8_t> lastNote = std::nullopt;
	double lastNoteDeltaTime = -1;
	std::chrono::time_point<std::chrono::steady_clock> lastNoteTime;
	std::chrono::time_point<std::chrono::steady_clock> timePointZero;

	auto processNoteEvents = [&](){
		for(auto& message = midi.getMessage();!message.empty();midi.getMessage()){
			if constexpr(midiMessageDebug){
				std::cout<<"Got midi message of size "<<message.size()<<": ";
				for(auto& o : message)
					std::cout<<std::hex<<" "<<(int)o;
				std::cout<<" at "<<std::chrono::duration<double>{midi.lastMessageTime-timePointZero}<<" seconds"<<std::endl;
			}

			if(midi.justChangedPort)
				continue;//wait just a bit after changing ports to prevent old midi events from being tracked

			if(message.size() < 2)
				continue;

			if(!(message[0] == 0x99 || message[0] == 0x90))//not sure why some devices make different note on events (haven't looked at the midi spec or anything though)
				continue;

			if constexpr(midiMessageNotes) std::cout<<"got midi note on for note number "<<std::hex<<(int)message[1]<<std::endl;

			lastNote = message[1];
			lastNoteTime = midi.lastMessageTime;
			lastNoteDeltaTime = midi.lastMessageDeltaTime;

			for(auto& command : commands)
				if(command.first == message[1])
					system(command.second.c_str());

		}
		midi.justChangedPort = false;//a little messy, but best solution I could find (didn't even have to wait n time)
	};

	//this is a little messy, but will do
	
	if(args.silent){
		while(true){
			processNoteEvents();
			usleep(200);
		}
		return 0;
	}

	while(simpleGUI::isRunning()){
		simpleGUI::pollEvents();

		processNoteEvents();

		if(simpleGUI::combo("Select midi port", &midi.selectedPort, midi.ports)){
			std::cout<<"Opening midi port "<<midi.selectedPort<<std::endl;
			midi.openSelectedPort();
			timePointZero = std::chrono::steady_clock::now();
		}
		if(simpleGUI::button("Refresh midi devices"))
			midi.refreshMidiSignals();

		if(lastNote){
			std::chrono::duration<double> elapsed{std::chrono::steady_clock::now() - lastNoteTime};
			double elapsedSeconds = elapsed.count();//- lastNoteDeltaTime; //seems to mess things up (and works well enough without)
			simpleGUI::text("Last midi note received: 0x" + hexToStr(*lastNote) + " (" + std::to_string(elapsedSeconds) + " seconds ago)");
		}

		simpleGUI::text("Commands:");
		for(unsigned int i=0;i<commands.size();i++){
			//simpleGUI::text("Midi 0x" + hexToStr(commands[i].first) + ":  " + commands[i].second);
			simpleGUI::textInput("Midi 0x" + hexToStr(commands[i].first) + ":  ", commands[i].second);
			if(simpleGUI::button("Remove")){
				commands.erase(commands.begin()+i);
				i--;
			}
		}

		if(lastNote){
			static std::string commandInput;
			simpleGUI::textInput("Command Input:", commandInput);
			if(simpleGUI::button("Add command for last note (0x" + hexToStr(*lastNote) + ")"))
				commands.push_back({*lastNote, commandInput});
		}

		static std::string saveFilename = "commands.json";
		simpleGUI::textInput("Filename:", saveFilename);

		if(!commands.empty()){
			if(simpleGUI::button("Save command list to file"))
				saveCommands(saveFilename, commands);
		}

		if(simpleGUI::button("Load command list to file"))
			loadCommands(saveFilename, commands);

		usleep(200);
		simpleGUI::render();
	}

	simpleGUI::exit();
}
