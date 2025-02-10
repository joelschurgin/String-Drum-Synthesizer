# String-Drum-Synthesizer

## Problem
A musical artist brings with them a sense of how to evoke certain emotions or create particular atmospheres, but may lack the technical knowledge to bring it to life. Sound design can be challenging as it involves crafting the unique timbres and textures that define a piece of music, which can be both an art and a science. The learning curve is steep, not only because it requires a grasp of complex technical tools but also because it demands a creative intuition that isn't easily taught. Musicians and producers must learn to navigate a vast array of software and hardware tools, each with its own set of knobs, sliders, and buttons. Understanding how to shape sounds means diving deep into the physics of sound waves, the principles of digital signal processing, and the subtleties of different synthesis methods. This process can feel overwhelming, especially for beginners who might feel lost amidst the jargon and the sheer number of options.

## Solution
Build a synthesizer. The range of sounds are controlled by a set of parameters such as filter cutoffs, oscillator types, low frequency oscillators, etc. The proposed solution is two-fold. The first part of the project is to build emotive knobs that feature descriptive ranges such as soft to aggressive or thin to thick. These knobs would also control the internal parameters of the system to achieve the desired effects on the input sound. The other part is to create a system that records a sound and finds the best way to set the synthesizer's internal parameters to recreate it. The intended work flow of this device for a musical artist is to record a sound, tweak the knobs, and play that sound by pressing a key on a MIDI keyboard. It is up to the musician to record the sound into their own recording software for further manipulation.

# Solution Components
## Subsystem 1: Software Synthesizer
Purpose/Requirements: This subsystem will generate sounds using the Karplus Strong algorithm which features a pulse generator and an echo chamber (modeled with digital delay, feedback, and filtering). When a note is played on a USB keyboard, this component must be able to change the pitch based on which key is pressed. The algorithm’s parameters must be controllable using potentiometers, but several parameters can be determined as a function of one knob. We chose the Karplus Strong algorithm to reduce the computational load, but we are modifying certain aspects to allow for a greater range of sounds. The pulse that we will use is a blend of noise and sine wave oscillator which are added together and distorted. In addition, there will be some soft distortion applied on the output. In addition, the echo chamber will use 3 IIR filters: highpass, lowpass, and a bell. The choice to use IIR filters is to lower the amount of computation.

We made a demo of the synthesizer algorithm using a digital audio workstation in order to play with parameters and determine the names of the emotive knobs. Below are the emotive knobs and how the corresponding synth parameters should change to implement them:

Aggression

- Synth Params: Pulse distortion, output distortion
- Knob Left: Low distortion
- Knob Right: High distortion

Brightness

- Synth Param: Low pass filter cutoff
- Knob Left: 20 Hz
- Knob Right: 20 kHz

Boominess

- Synth Param: High pass Cutoff
- Knob Left: 20 kHz
- Knob Center: Note pitch
- Knob Right: 20 Hz

Dampening

- Synth Param: Bell Q
- Knob Left: High Q
- Knob Right: Low Q

*Not sure what to call this yet, but it affects how the echo chamber resonates. Note: setting this to around 3.6 kHz sounds good, so it could be fine to keep this completely internal and forgo the corresponding knob.

- Synth Param: Bell frequency
- Knob Left: 20 hz
- Knob Right: 20 kHz

Scratchiness

- Synth Param: Length of pulse
- Knob Left: 0
- Knob Center: 1/(Note pitch)
- Knob Right: 4/(Note pitch)

Sharpness

- Synth Param: Mix between pulse noise and oscillator
- Knob Left: Pulse consists of only oscillator
- Knob Right: Pulse consists of only noise

*Strike weight => Corresponds to simulated weight of object striking the string/drum head.

- Synth Param: Pulse oscillator pitch
- Knob Left: 20 kHz
- Knob Right: 20 hz

Notes:
- The value "note pitch" is determined by USB keyboard input.
- Bell gain is fixed and should be set to attenuate the signal.

Components:

- Microprocessor
- DAC
- Potentiometers for parameters (linear, should be able to snap to center position if center value has meaning other should be smooth all the way through)
- USB Port

## Subsystem 3: Amplifier
Purpose/Requirements: This subsystem should output the smallest possible signal to noise ratio through a guitar cable or headphones. In order to achieve this we will experiment with using several inverting op-amp amplifier circuits in parallel and summing them together with an inverting mixer. The gain of each amplifier will be set, but the volume control will be part of the inverting mixer circuit.

Components:

- Mono 1/4in audio jack
- 4 Op-amps
- Logarithmic Potentiometer (for volume control)
