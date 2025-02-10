#pragma once

#define KNOB_SELECT_C 23
#define KNOB_SELECT_B 24
#define KNOB_SELECT_A 25

class EmotiveKnob {
public:
    EmotiveKnob(const short _id);
    ~EmotiveKnob();

    static void setup();
    static void cleanup();

    unsigned char readValue();

private:
    void selectKnob();

private:
    short id;
};