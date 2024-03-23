all:
	g++ -g -std=c++20 main.cpp midiHandler.cpp load.cpp args.cpp -o midiHotkey -lsimpleGUI -lrtmidi

clean:
	rm -f midiHotkey
