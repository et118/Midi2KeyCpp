#pragma once
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <bitset>
#include <map>
#pragma comment(lib,"winmm.lib")
using namespace std;

//https://www.recordingblogs.com/wiki/status-byte-of-a-midi-message

const enum MidiMessageType {
	NOTE_ON,
	NOTE_OFF,
	HANDLE_SENSE
};
const map<unsigned int, MidiMessageType> midiMessageTypeMap = {
	{0x80,NOTE_OFF},
	{0x90,NOTE_ON},
	{0xFE,HANDLE_SENSE}
};
const map<unsigned int, char> virtualPianoMap = {
	{36,'1'},
	{37,'!'},
	{38,'2'},
	{39,'@'},
	{40,'3'},

	{41,'4'},
	{42,'$'},
	{43,'5'},
	{44,'%'},
	{45,'6'},
	{46,'^'},
	{47,'7'},

	{48,'8'},
	{49,'*'},
	{50,'9'},
	{51,'('},
	{52,'0'},

	{53,'q'},
	{54,'Q'},
	{55,'w'},
	{56,'W'},
	{57,'e'},
	{58,'E'},
	{59,'r'},

	{60,'t'},
	{61,'T'},
	{62,'y'},
	{63,'Y'},
	{64,'u'},

	{65,'i'},
	{66,'I'},
	{67,'o'},
	{68,'O'},
	{69,'p'},
	{70,'P'},
	{71,'a'},

	{72,'s'},
	{73,'S'},
	{74,'d'},
	{75,'D'},
	{76,'f'},

	{77,'g'},
	{78,'G'},
	{79,'h'},
	{80,'H'},
	{81,'j'},
	{82,'J'},
	{83,'k'},

	{84,'l'},
	{85,'L'},
	{86,'z'},
	{87,'Z'},
	{88,'x'},

	{89,'c'},
	{90,'C'},
	{91,'v'},
	{92,'V'},
	{93,'b'},
	{94,'B'},
	{95,'n'},

	{96,'m'}

};


int getDeviceID();
void openPort(int deviceID, HMIDIIN* devicePtr);
void closePort(HMIDIIN device);
void loop();
void sendKeyPress(char c);
void sendKeyRelease(char c);
void keyHandler(MidiMessageType type, unsigned int key, unsigned int velocity);
void CALLBACK midiCallback(HMIDIIN device, UINT msg, DWORD_PTR instanceData, DWORD message, DWORD timestamp);
