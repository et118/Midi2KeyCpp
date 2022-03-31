#include "main.h";

int getDeviceID() {
	UINT numDevices = midiInGetNumDevs();
	if (numDevices == 0) {
		printf("No Devices Connected\n");
		exit(0);
	}

	for (unsigned int i = 0; i < numDevices; i++)
	{
		MIDIINCAPS info;
		MMRESULT result = midiInGetDevCaps(i, &info, sizeof(MIDIINCAPS));
		printf("|ID| %d | Name | %ls\n", i, info.szPname);
	}

	printf("Select an input device 0->%d: ", numDevices - 1);
	unsigned int deviceID;
	cin >> deviceID;
	while (deviceID < 0 || deviceID > numDevices - 1) {
		printf("Invalid input! Try again: ");
		cin >> deviceID;
	}
	return deviceID;
}

void openPort(int deviceID, HMIDIIN* devicePtr) {
	MMRESULT result = midiInOpen(devicePtr, deviceID, (DWORD_PTR)(void*)midiCallback, 0, CALLBACK_FUNCTION);
	if (result != MMSYSERR_NOERROR) {
		printf("Failed to open port. Error: %d", result);
		exit(0);
	}
	printf("Successfully opened port %d\n", deviceID);
	return;
}

void closePort(HMIDIIN device) {
	MMRESULT result = midiInClose(device);
	if (result != MMSYSERR_NOERROR) {
		printf("Failed to close port. Error: %d", result);
		exit(0);
	}
	printf("Successfully closed port");
}

void loop() {
	while (true) {
		int ch = _getch();
		if (ch == VK_ESCAPE || ch == 'q') {
			break;
		}
	}
}

void sendKeyPress(unsigned int i) {
	char c = virtualPianoMap.at(i);
	INPUT input;
	HKL keyboardLayout = GetKeyboardLayout(0);

	input.type = INPUT_KEYBOARD;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = 0;

	if (isupper(c) || (!((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) && !isdigit(c))) {
		//Is upper and special characters
		WORD key = VkKeyScanExA(virtualPianoMap.at(i - 1), keyboardLayout);

		input.ki.dwFlags = KEYEVENTF_SCANCODE;
		input.ki.wScan = MapVirtualKeyA(VK_SHIFT, MAPVK_VK_TO_VSC);
		//input.ki.wScan = 0;
		SendInput(1, &input, sizeof(INPUT));

		input.ki.dwFlags = KEYEVENTF_SCANCODE;
		input.ki.wScan = MapVirtualKeyA(key, MAPVK_VK_TO_VSC);
		//input.ki.wVk = key;
		SendInput(1, &input, sizeof(INPUT));

		input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		//input.ki.wVk = VK_SHIFT;
		input.ki.wScan = MapVirtualKeyA(VK_SHIFT, MAPVK_VK_TO_VSC);
		SendInput(1, &input, sizeof(INPUT));

	}
	else {
		//Is lower characters and numbers
		WORD key = VkKeyScanExA(c, keyboardLayout);
		input.ki.dwFlags = KEYEVENTF_SCANCODE;
		//input.ki.wVk = key;
		input.ki.wScan = MapVirtualKeyA(key, MAPVK_VK_TO_VSC);
		SendInput(1, &input, sizeof(INPUT));

	}
}

void sendKeyRelease(unsigned int i) {
	char c = virtualPianoMap.at(i);
	INPUT input;
	HKL keyboardLayout = GetKeyboardLayout(0);

	input.type = INPUT_KEYBOARD;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = 0;

	if (isupper(c) || (!((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) && !isdigit(c))) {
		//Is upper and special characters
		WORD key = VkKeyScanExA(virtualPianoMap.at(i - 1), keyboardLayout);
		input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		//input.ki.wVk = key;
		input.ki.wScan = MapVirtualKeyA(key, MAPVK_VK_TO_VSC);
		SendInput(1, &input, sizeof(INPUT));
	}
	else {
		//Is lower characters and numbers
		WORD key = VkKeyScanExA(c, keyboardLayout);
		input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		//input.ki.wVk = key;
		input.ki.wScan = MapVirtualKeyA(key, MAPVK_VK_TO_VSC);
		SendInput(1, &input, sizeof(INPUT));
	}
}

void newKeyPress(unsigned int i) {
	char c = virtualPianoMap.at(i);
	HKL keyboardLayout = GetKeyboardLayout(0);

	if (isupper(c) || (!((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) && !isdigit(c))) {
		WORD key = VkKeyScanExA(virtualPianoMap.at(i - 1), keyboardLayout);
		keybd_event(VK_SHIFT, 0, 0, 0);
		keybd_event(key, 0, 0, 0);
		keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
	}
	else {
		WORD key = VkKeyScanExA(c, keyboardLayout);
		keybd_event(key, 0, 0, 0);
	}
}

void newKeyRelease(unsigned int i) {
	char c = virtualPianoMap.at(i);
	HKL keyboardLayout = GetKeyboardLayout(0);

	if (isupper(c) || (!((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) && !isdigit(c))) {
		WORD key = VkKeyScanExA(virtualPianoMap.at(i - 1), keyboardLayout);
		keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
	}
	else {
		WORD key = VkKeyScanExA(c, keyboardLayout);
		keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
	}
}

void keyHandler(MidiMessageType type, unsigned int key, unsigned int velocity) {
	switch (type) {
	case NOTE_ON: {
		sendKeyPress(key);
		//newKeyPress(key);
		break;
	}
	case NOTE_OFF: {
		//newKeyRelease(key);
		sendKeyRelease(key);
		break;
	}
	}
}

void CALLBACK midiCallback(HMIDIIN device, UINT msgType, DWORD_PTR instanceData, DWORD message, DWORD timestamp) {
	switch (msgType) {//Message: [Empty 8b][Velocity 8b][Key 8b][Status 8b]
	case MIM_DATA: {
		unsigned int status = (unsigned char)message;
		unsigned int key = (unsigned char)(message >> 8);
		unsigned int velocity = (unsigned char)(message >> 16);
		if (midiMessageTypeMap.count(status) == 0) return;

		MidiMessageType messageType = midiMessageTypeMap.at(status);
		if (messageType == HANDLE_SENSE) break;
		if (velocity == 0) messageType = NOTE_OFF;
		keyHandler(messageType, key, velocity);
		break;
	}
	}
}

int main(int argc, char const* argv[])
{
	HMIDIIN device;
	int deviceID = getDeviceID();
	openPort(deviceID, &device);
	midiInStart(device);
	loop();
	midiInStop(device);
	closePort(device);
	return 0;
}
