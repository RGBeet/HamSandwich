#include "music.h"
#include "mgldraw.h"
#include "progress.h"
#include "config.h"
#include "editor.h"
#include "jamulsound.h"
#include "shop.h"
#include "player.h"

char curSongName[64];
char reserveSongName[64];

int songPosition;
byte lastSong	= 255;
byte pauseMenu	= 0;

void RestartCurrentSong()
{
	PlaySongForce(curSongName);
}

void ChooseNextSong(void)
{
	if(!config.music)
		return;

	if((profile.musicMode&MUSIC_LISTBITS)==MUSIC_OFFICIAL ||
	   (profile.musicMode&MUSIC_PLAYMODE)==MUSIC_REPEAT)
	{
		// official or repeat mode: just replay this song again

		if((profile.musicMode&MUSIC_LISTBITS)==MUSIC_OFFICIAL)
			PlaySongForce(curSongName);
		else
		{
			// in repeat mode we just loop the first song of the playlist
			// it's not very useful honestly
			if(profile.playList[(profile.musicMode&MUSIC_LISTBITS)-1].numSongs==0)
			{
				StopSong();
				return;
			}
			lastSong=0;
			PlaySongForce(&profile.playList[(profile.musicMode&MUSIC_LISTBITS)-1].song[0]);
		}
	}
	else
	{
		byte pl=(profile.musicMode&MUSIC_LISTBITS)-1;
		// not playing the official songs, and not on repeat
		if((profile.musicMode&MUSIC_PLAYMODE)==MUSIC_SEQUENTIAL)
		{
			lastSong++;
			if(lastSong>=profile.playList[pl].numSongs)
			{
				lastSong=0;
				if(profile.playList[pl].numSongs==0)
				{
					StopSong();
					return;
				}
			}
			PlaySongForce(&profile.playList[pl].song[lastSong*SONGNAME_LEN]);
		}
		else	// on random
		{
			if(profile.playList[pl].numSongs==0)
			{
				StopSong();
				return;
			}
			if(profile.playList[pl].numSongs>1)
			{
				byte sng=lastSong;
				while(sng==lastSong)
					sng=Random(profile.playList[pl].numSongs);
				lastSong=sng;
			}
			else
				lastSong=0;
			PlaySongForce(&profile.playList[pl].song[lastSong*SONGNAME_LEN]);
		}
	}
}

void PlaySong(const char *fname)
{
	if(!config.music)
		return;

	if((profile.musicMode&MUSIC_LISTBITS)!=MUSIC_OFFICIAL && !editing)
	{
		// if not playing the official music, we ignore the song asked for
		if(!IsSongPlaying())
		{
			// but we need to play something, since there are no songs playing at all
			ChooseNextSong();
		}
		return;
	}

	PlaySongForce(fname);
}

void PlaySongForce(const char* fname)
{
	char fullname[128];
	char basename[128];

	if (!config.music)
		return;

	strncpy(curSongName, fname, sizeof(curSongName) - 1);
	curSongName[sizeof(curSongName) - 1] = '\0';

	if (!fname[0])
	{
		StopSong();
		return;
	}

	CalculateMusicSpeed();

	const char* layers[MAX_MUSIC_LAYERS] = {};
	static char filenames[MAX_MUSIC_LAYERS][128];

	// Main song
	snprintf(
		filenames[0],
		sizeof(filenames[0]),
		"music/%s",
		fname
	);

	layers[0] = filenames[0];

	// Remove extension for layer names
	strncpy(basename, fname, sizeof(basename) - 1);
	basename[sizeof(basename) - 1] = '\0';

	char* ext = strrchr(basename, '.');
	if (ext)
		*ext = '\0';


	// Extra layers
	for (int i = 1; i < MAX_MUSIC_LAYERS; i++)
	{
		snprintf(
			filenames[i],
			sizeof(filenames[i]),
			"music/layers/%s_layer%d.ogg",
			basename,
			i
		);

		if (filenames[i])
			layers[i] = filenames[i];
		else
			break;
	}

	PlayLayeredSong(layers);
}

const char *CurSongTitle(void)
{
	return curSongName;
}

void PlayNextSong(void)
{
	byte pl;

	if(!config.music)
		return;

	if((profile.musicMode&MUSIC_LISTBITS)==MUSIC_OFFICIAL)
		return;	// can't

	pl=(profile.musicMode&MUSIC_LISTBITS)-1;

	lastSong++;
	if(lastSong>=profile.playList[pl].numSongs)
	{
		lastSong=0;
		if(profile.playList[pl].numSongs==0)
		{
			StopSong();
			return;
		}
	}
	PlaySongForce(&profile.playList[pl].song[lastSong*SONGNAME_LEN]);
}

void PlayPrevSong(void)
{
	byte pl;

	if(!config.music)
		return;

	if((profile.musicMode&MUSIC_LISTBITS)==MUSIC_OFFICIAL)
		return;	// can't

	pl=(profile.musicMode&MUSIC_LISTBITS)-1;

	lastSong--;
	if(lastSong>250)
	{
		lastSong=profile.playList[pl].numSongs-1;
		if(profile.playList[pl].numSongs==0)
		{
			StopSong();
			return;
		}
	}
	PlaySongForce(&profile.playList[pl].song[lastSong*SONGNAME_LEN]);
}

void UpdateSong(void)
{
	UpdateMusicLayers();
	CalculateMusicSpeed();
}

void CalculateMusicSpeed()
{
	float speed = 1.0f;

	if (PlayerGetTimeStop() > 1)
		speed *= 0.5;

	if (PlayerGetAccelerate() > 1)
		speed *= 1.5;

	if (profile.progress.purchase[modeShopNum[MODE_MANIC]] & SIF_ACTIVE)
		speed *= 2;

	SetMusicFrequency(speed);
}

void InitPauseMenuSong()
{
	sprintf(reserveSongName, curSongName);
	songPosition = GetBaseLayerPosition();
	StopEntireSong();
	PlaySongForce("pause.ogg");
	pauseMenu = 1;
}

void ExitPauseMenuSong(bool resume)
{
	StopSongLayer(0); // exit pause music
	if (!resume)
		return;
	PlaySongForce(reserveSongName);
	SetEntireSongPosition(songPosition);
	pauseMenu = 0;
}
