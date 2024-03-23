#include "midiHandler.hpp"

midiHandler::midiHandler(){
	try{
		midiin = std::make_shared<RtMidiIn>();
	}catch(RtMidiError& error){
		error.printMessage();
		exit(-1);
	}
	refreshMidiSignals();
	lastMessageTime = std::chrono::steady_clock::now();
}

void midiHandler::refreshMidiSignals(){
	unsigned int nPorts = midiin->getPortCount();
	ports.resize(nPorts);
	for(unsigned int i=0;i<nPorts;i++){
		ports[i] = midiin->getPortName(i);
	}
}

void midiHandler::openSelectedPort(){
	if(midiin->isPortOpen())
		midiin->closePort();
	midiin->openPort(selectedPort);
	midiin->ignoreTypes(false, false, false);

	//process all stored messages, so there isn't a sudden burst when switching
	//std::vector<uint8_t> ignoredMessages = {0};
	//while(!ignoredMessages.empty()){
	//	midiin->getMessage(&ignoredMessages);
	justChangedPort = true;

}

const std::vector<uint8_t>& midiHandler::getMessage(){
	lastMessageDeltaTime = midiin->getMessage(&midiMessage);
	lastMessageTime = std::chrono::steady_clock::now();
	return midiMessage;
}

