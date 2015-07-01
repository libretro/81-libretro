#ifndef MIDI_H
#define MIDI_H

#ifdef __cplusplus
extern "C" {
#endif

void MidiWriteBit(int Bit);
void MidiClockTick(int ts);
void MidiWrite(int Byte);
void MidiStart(void);
void MidiStop(void);

extern int MidiDev;

#ifdef __cplusplus
}
#endif

#endif
