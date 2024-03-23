#pragma once
#include <memory>
#include <vector>
#include <chrono>
#include <rtmidi/RtMidi.h>

class midiHandler{
public:
	std::shared_ptr<RtMidiIn> midiin;
	std::vector<std::string> ports;
	int selectedPort = 0;
	double lastMessageDeltaTime = -1;
	std::chrono::time_point<std::chrono::steady_clock> lastMessageTime;
	bool justChangedPort = false;
	midiHandler();
	void refreshMidiSignals();
	void openSelectedPort();
	const std::vector<uint8_t>& getMessage();

private:
	std::vector<uint8_t> midiMessage;
};
