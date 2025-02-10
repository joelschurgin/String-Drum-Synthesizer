#include <iostream>
#include <cstdlib>
#include <string.h>
#include <math.h>
#include <thread>
#include <pthread.h>
#include <wiringPi.h>
#include <unistd.h>
#include "rtmidi/RtMidi.h"
#include "AudioDevice.h"
#include "EmotiveKnob.h"

short knob_vals[8] = {};
AudioDevice audio_device;

unsigned char prevMidiMsgEvent = 0;
unsigned char prevMidiMsgPitch = 0;

enum MidiEvents {
	NOTE_ON = 144,
	NOTE_OFF = 128,
};

void HandleMidiMessage(double deltatime, std::vector< unsigned char > *message, void *userData) {
	unsigned int nBytes = message->size();
	switch ((int)message->at(0)) {
		case MidiEvents::NOTE_ON:
			if (message->at(1) == prevMidiMsgPitch && deltatime < 0.0025) {
				break;
			}

			audio_device.getSynth().startLatencyTimer();

			prevMidiMsgPitch = message->at(1);
			const double newPitch = 440.0 * pow(2.0, static_cast<double>((int)message->at(1) - 69) / 12.0);
			audio_device.getSynth().playNote(newPitch);
			break;
		// case MidiEvents::NOTE_OFF:
		// 	break;
	}
}

int getMidiPort(RtMidiIn *midi) {
	unsigned int nPorts = midi->getPortCount();
	if ( nPorts == 0 ) {
		std::cerr << "No ports available!\n";
		return -1;
	}
	for (int port = 0; port < nPorts; port++) {
		std::string portName = midi->getPortName(port);
		if (portName.find("MPK") != std::string::npos) {
			std::cout << "Port " << port << ": " << portName << std::endl;
			return port;
		}
	}
	return -1;
}

RtMidiIn* connectToMidiDevice() {
	RtMidiIn *midi = new RtMidiIn();

	const int midiPort = getMidiPort(midi);
	if (midiPort < 0) {
		delete midi;
		return NULL;
	}
	midi->openPort( midiPort );

	// This should be done immediately after opening the port to avoid 
	// having incoming messages written to the queue.
	midi->setCallback( &HandleMidiMessage );

	// Do we ignore sysex, timing, or active sensing messages.
	// midi->ignoreTypes( false, true, false );

	return midi;
}

void playAudio() {
	audio_device.play();
}
 
int main()
{
	if (audio_device.initiallize() < 0) {
        return -1;
    }

	RtMidiIn* midi = connectToMidiDevice();
	while (!midi) {
		midi = connectToMidiDevice();
		printf("Waiting for MIDI device...\n");
		usleep(250000);
	}

	printf("Reading MIDI input.\n");

	std::thread audio_thread(playAudio);

	sched_param param;
    int policy;
    pthread_getschedparam(audio_thread.native_handle(), &policy, &param);
    param.sched_priority = sched_get_priority_max(SCHED_FIFO); // Use SCHED_FIFO for real-time scheduling
    pthread_setschedparam(audio_thread.native_handle(), SCHED_FIFO, &param);


	EmotiveKnob::setup();
	
	EmotiveKnob knobs[8] = {EmotiveKnob(0), 
							EmotiveKnob(1),
							EmotiveKnob(2),
							EmotiveKnob(3),
							EmotiveKnob(4),
							EmotiveKnob(5),
							EmotiveKnob(6),
							EmotiveKnob(7)};

	printf("\n\n\n");
	while (1) {
		for (int i = 0; i < 8; i++){
			knob_vals[i] = knobs[i].readValue();
		}

		audio_device.getSynth().updateParams(knob_vals);

		printf( "Sharpness     | %3d        \n"
				"Strike Weight | %3d        \n"
				"Dampening     | %3d        \n"
				"Brightness    | %3d        \n"
				"Boominess     | %3d        \n"
				"Scratchiness  | %3d        \r\033[A\033[A\033[A\033[A\033[A", 
				knob_vals[0], knob_vals[1], knob_vals[4], knob_vals[5], knob_vals[6], knob_vals[7]);
	}

	audio_thread.join();

	EmotiveKnob::cleanup();

	// Clean up
	delete midi;

	return 0;
}