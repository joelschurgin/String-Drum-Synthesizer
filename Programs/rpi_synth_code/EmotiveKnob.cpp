#include "EmotiveKnob.h"

#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>

EmotiveKnob::EmotiveKnob(const short _id) 
    : id{_id} {
    selectKnob(); // JUST FOR DEBUGGING
}

EmotiveKnob::~EmotiveKnob() {}

void EmotiveKnob::setup() {
    wiringPiSetup();

    pinMode(KNOB_SELECT_C, OUTPUT); 
    pinMode(KNOB_SELECT_B, OUTPUT);
    pinMode(KNOB_SELECT_A, OUTPUT);

    pinMode(0, INPUT);
    pinMode(1, INPUT);
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    pinMode(4, INPUT);
    pinMode(5, INPUT);
    pinMode(6, INPUT);
    pinMode(7, INPUT);
}

void EmotiveKnob::cleanup() {
    pinMode(KNOB_SELECT_C, INPUT); 
    pinMode(KNOB_SELECT_B, INPUT);
    pinMode(KNOB_SELECT_A, INPUT);
}

unsigned char EmotiveKnob::readValue()
{
    selectKnob();

    unsigned char value = 0;
    for (int i = 0; i < 8; i++) {
        value |= digitalRead(i) << i;
    }
    
    return value;
}

void EmotiveKnob::selectKnob() {
    digitalWrite(KNOB_SELECT_C, (int)((id & 0b001)));
    digitalWrite(KNOB_SELECT_B, (int)((id & 0b010) > 0));
    digitalWrite(KNOB_SELECT_A, (int)((id & 0b100) > 0));
    usleep(2500);
}
