#ifndef JAMULSOUND_H
#define JAMULSOUND_H

#include "jamultypes.h"

// external fun sound playing flags for everyone to use
enum {
	SND_CUTOFF = 1,			// cut off same sound if needed
	SND_MAXPRIORITY = 2,	// ignore priority value, this sound is a must-have
	SND_ONE = 4,			// only one copy may play at once
	SND_PLAYING = 8,		// well, it's not for everyone, but it goes here
	SND_LOOPING = 32,
	SND_SUSTAIN = 64,		// only used by instruments
	SND_RANDOM = 128, 		// randomize pitch up or down 10%
	SND_BACKWARDS = 256,
	SND_DOUBLESPEED = 512,
	SND_MUSICVOLUME = 1024, // use music volume instead of sound volume, for Stockboy
};

const int MAX_SNDPRIORITY = 65536;

void SetJamulSoundEnabled(bool enabled, int numSounds);
bool JamulSoundInit(int numBuffers);
void JamulSoundExit(void);
bool SoundIsAvailable();

// --------------------------------
// here's the fun and easy sound manager
// it assumes there is a subdirectory "\sounds" that contains snd000.wav - sndXXX.wav,
// for as many sounds as you'll try to play.  It will load them if they aren't in memory already.

// call this fairly often to free up unused buffers, otherwise no new sounds can be played
void JamulSoundUpdate(void);

// call this to wipe the sounds from memory
void JamulSoundPurge(void);

// call this a lot, it plays sounds
void GoPlaySound(int num, long pan, long vol, int flags, int priority);

// pass in the sound # you're sick of hearing
bool JamulSoundStop(int which);

void JamulSoundVolume(int v);
void JamulSoundChangeSoundVolume(int snd, int volume);

// ----------------------------------------------------------------------------

void SetHamMusicEnabled(bool enabled);

void UpdateMusic();

void SetMusicVolume(int vol);

// Accepts full path, including leading "sound/" or "music/".
byte PlaySongFile(const char* path, byte layer=0);	// defaults to base layer, can be used to play layer 1 as well!
void StopSong(void);

void PauseSong(); // pauses all layers at same time
void ResumeSong(); // resumes all layers at same time

bool IsSongPlaying();

void SetMusicFrequency(float f); // speed up or slow down the song - shifts pitch AND tempo!

// Music layer functions

byte PlaySongLayer(int layer, const char* filename);
void StopSongLayer(int layer);
void PauseMusicLayer(int layer);
void ResumeMusicLayer(int layer);

void SetMusicLayerVolume(int layer, int volume);
void SetMusicLayerTarget(int layer, int target);

byte PlayLayeredSong(const char* files[8]);
void StopEntireSong();
void PauseEntireSong();
void ResumeEntireSong();
int GetBaseLayerPosition();
void SetEntireSongPosition(int position);

byte IsMusicLayerLoaded(int layer);
byte IsMusicLayerPaused(int layer);


#endif  // JAMULSOUND_H
