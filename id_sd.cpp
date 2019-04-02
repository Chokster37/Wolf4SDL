//
//      ID Engine
//      ID_SD.c - Sound Manager for Wolfenstein 3D
//      v1.2
//      By Jason Blochowiak
//

//
//      This module handles dealing with generating sound on the appropriate
//              hardware
//
//      Depends on: User Mgr (for parm checking)
//
//      Globals:
//              For User Mgr:
//                      SoundMode - What device is used for sound effects
//                              (Use SM_SetSoundMode() to set)
//                      MusicMode - What device is used for music
//                              (Use SM_SetMusicMode() to set)
//
//

#include "wl_def.h"
#include <SDL_mixer.h>
#include "fmopl.h"

//      Global variables
        SDMode          SoundMode;
        SMMode          MusicMode;
static  byte          **SoundTable;

//      Internal variables
static  boolean                 SD_Started;
static  soundnames              SoundNumber;
static  word                    SoundPriority;

//      PC Sound variables
static  volatile byte           pcLastSample;
static  byte * volatile         pcSound;
static  longword                pcLengthLeft;

//      AdLib variables
static  longword                alTimeCount;

//      Sequencer variables
static  volatile boolean        sqActive;
static  word                   *sqHack;
static  word                   *sqHackPtr;
static  int                     sqHackLen;
static  int                     sqHackSeqLen;
static  longword                sqHackTime;
static  const int               oplChip = 0;


///////////////////////////////////////////////////////////////////////////
//
//      SDL_PCPlaySound() - Plays the specified sound on the PC speaker
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_PCPlaySound(PCSound *sound)
{
        pcLastSample = (byte)-1;
        pcLengthLeft = sound->common.length;
        pcSound = sound->data;
}

// Adapted from Chocolate Doom (chocolate-doom/pcsound/pcsound_sdl.c)
#define SQUARE_WAVE_AMP 0x1800

static void SDL_PCMixCallback(void *udata, Uint8 *stream, int len)
{
    static int current_remaining = 0;
    static int current_freq = 0;
    static int phase_offset = 0;

    Sint16 *leftptr;
    Sint16 *rightptr;
    Sint16 this_value;
    int oldfreq;
    int i;
    int nsamples;

    // Number of samples is quadrupled, because of 16-bit and stereo

    nsamples = len / 4;

    leftptr = (Sint16 *) stream;
    rightptr = ((Sint16 *) stream) + 1;

    // Fill the output buffer

    for (i=0; i<nsamples; ++i)
    {
        // Has this sound expired? If so, retrieve the next frequency

        while (current_remaining == 0) 
        {
            oldfreq = current_freq;
            phase_offset = 0;

            // Get the next frequency to play

            if(pcSound)
            {
                // The PC speaker sample rate is 140Hz (see SDL_t0SlowAsmService)
                current_remaining = param_samplerate / 140;

                if(*pcSound!=pcLastSample)
                {
                    pcLastSample=*pcSound;
					
                    if(pcLastSample)
                        // The PC PIC counts down at 1.193180MHz
                        // So pwm_freq = counter_freq / reload_value
                        // reload_value = pcLastSample * 60 (see SDL_DoFX)
                        current_freq = 1193180 / (pcLastSample * 60);
                    else
                        current_freq = 0;
						
                }
                pcSound++;
                pcLengthLeft--;
                if(!pcLengthLeft)
                {
                    pcSound=0;
                    SoundNumber=(soundnames)0;
                    SoundPriority=0;
                }
            }
            else
            {	
                current_freq = 0;
                current_remaining = 1;
            }
        }

        // Set the value for this sample.

        if (current_freq == 0)
        {
            // Silence

            this_value = 0;
        }
        else 
        {
            int frac;

            // Determine whether we are at a peak or trough in the current
            // sound.  Multiply by 2 so that frac % 2 will give 0 or 1 
            // depending on whether we are at a peak or trough.

            frac = (phase_offset * current_freq * 2) / param_samplerate;

            if ((frac % 2) == 0) 
            {
                this_value = SQUARE_WAVE_AMP;
            }
            else
            {
                this_value = -SQUARE_WAVE_AMP;
            }

            ++phase_offset;
        }

        --current_remaining;

        // Use the same value for the left and right channels.

        *leftptr += this_value;
        *rightptr += this_value;

        leftptr += 2;
        rightptr += 2;
    }
}


//      Public routines

///////////////////////////////////////////////////////////////////////////
//
//      SD_SetSoundMode() - Sets which sound hardware to use for sound effects
//
///////////////////////////////////////////////////////////////////////////
boolean
SD_SetSoundMode(SDMode mode)
{
    SD_StopSound();

    SoundMode = mode;

    return true;
}

///////////////////////////////////////////////////////////////////////////
//
//      SD_SetMusicMode() - sets the device to use for background music
//
///////////////////////////////////////////////////////////////////////////
boolean
SD_SetMusicMode(SMMode mode)
{
	SD_MusicOff();

    MusicMode = mode;

    return true;
}

int numreadysamples = 0;
int samplesPerMusicTick;

void SDL_IMFMusicPlayer(void *udata, Uint8 *stream, int len)
{
    int stereolen = len>>1;
    int sampleslen = stereolen>>1;
    int16_t *stream16 = (int16_t *) (void *) stream;    // expect correct alignment

    while(1)
    {
        if(numreadysamples)
        {
            if(numreadysamples<sampleslen)
            {
                YM3812UpdateOne(oplChip, stream16, numreadysamples);
                stream16 += numreadysamples*2;
                sampleslen -= numreadysamples;
            }
            else
            {
                YM3812UpdateOne(oplChip, stream16, sampleslen);
                numreadysamples -= sampleslen;
                return;
            }
        }
        if(sqActive)
        {
            do
            {
                if(sqHackTime > alTimeCount) break;
                sqHackTime = alTimeCount + *(sqHackPtr+1);
                alOut(*(byte *) sqHackPtr, *(((byte *) sqHackPtr)+1));
                sqHackPtr += 2;
                sqHackLen -= 4;
            }
            while(sqHackLen>0);
            alTimeCount++;
            if(!sqHackLen)
            {
                sqHackPtr = sqHack;
                sqHackLen = sqHackSeqLen;
                sqHackTime = 0;
                alTimeCount = 0;
            }
        }
        numreadysamples = samplesPerMusicTick;
    }
}

///////////////////////////////////////////////////////////////////////////
//
//      SD_Startup() - starts up the Sound Mgr
//              Detects all additional sound hardware and installs my ISR
//
///////////////////////////////////////////////////////////////////////////
void
SD_Startup(void)
{
    int     i;

    if (SD_Started)
        return;

    if(Mix_OpenAudio(param_samplerate, AUDIO_S16, 2, param_audiobuffer))
    {
        printf("Unable to open audio: %s\n", Mix_GetError());
        return;
    }

    // Init music

    samplesPerMusicTick = param_samplerate / 700;    // SDL_t0FastAsmService played at 700Hz

    if(YM3812Init(1,3579545,param_samplerate))
    {
        printf("Unable to create virtual OPL!!\n");
    }

    for(i=1;i<0xf6;i++)
        YM3812Write(oplChip,i,0);

    YM3812Write(oplChip,1,0x20); // Set WSE=1
//    YM3812Write(0,8,0); // Set CSM=0 & SEL=0		 // already set in for statement

    Mix_HookMusic(SDL_IMFMusicPlayer, 0);

    alTimeCount = 0;
	
    // Add PC speaker sound mixer
    Mix_SetPostMix(SDL_PCMixCallback, NULL);
    SoundTable = &audiosegs[STARTPCSOUNDS];

    SD_SetSoundMode(sdm_Off);
    SD_SetMusicMode(smm_Off);

    SD_Started = true;
}

///////////////////////////////////////////////////////////////////////////
//
//      SD_Shutdown() - shuts down the Sound Mgr
//              Removes sound ISR and turns off whatever sound hardware was active
//
///////////////////////////////////////////////////////////////////////////
void
SD_Shutdown(void)
{
    if (!SD_Started)
        return;

    SD_MusicOff();
    SD_StopSound();

    SD_Started = false;
}

///////////////////////////////////////////////////////////////////////////
//
//      SD_PlaySound() - plays the specified sound on the appropriate hardware
//
///////////////////////////////////////////////////////////////////////////
void
SD_PlaySound(soundnames sound)
{
    SoundCommon     *s;

    if (sound == -1 || SoundMode == sdm_Off)
        return;

    s = (SoundCommon *) SoundTable[sound];

    if (!s)
        Quit("SD_PlaySound() - Uncached sound");

    if (!s->length)
        Quit("SD_PlaySound() - Zero length sound");

    if (s->priority < SoundPriority)
        return;

    SDL_PCPlaySound((PCSound *)s);
    SoundNumber = sound;
    SoundPriority = s->priority;
}

///////////////////////////////////////////////////////////////////////////
//
//      SD_SoundPlaying() - returns the sound number that's playing, or 0 if
//              no sound is playing
//
///////////////////////////////////////////////////////////////////////////
word
SD_SoundPlaying(void)
{
    if (SoundMode == sdm_PC && pcSound)
        return true;
		
    return false;
}

///////////////////////////////////////////////////////////////////////////
//
//      SD_StopSound() - if a sound is playing, stops it
//
///////////////////////////////////////////////////////////////////////////
void
SD_StopSound(void)
{
    pcSound = 0;
    SoundNumber   = (soundnames)0;
    SoundPriority = 0;
}

///////////////////////////////////////////////////////////////////////////
//
//      SD_WaitSoundDone() - waits until the current sound is done playing
//
///////////////////////////////////////////////////////////////////////////
void
SD_WaitSoundDone(void)
{
    while (SD_SoundPlaying())
        SDL_Delay(5);
}

///////////////////////////////////////////////////////////////////////////
//
//      SD_MusicOn() - turns on the sequencer
//
///////////////////////////////////////////////////////////////////////////
void
SD_MusicOn(void)
{
    sqActive = true;
}

///////////////////////////////////////////////////////////////////////////
//
//      SD_MusicOff() - turns off the sequencer and any playing notes
//
///////////////////////////////////////////////////////////////////////////
void
SD_MusicOff(void)
{
    word    i;

    sqActive = false;
    if (MusicMode == smm_AdLib)
    {
        alOut(alEffects, 0);
        for (i = 0;i < sqMaxTracks;i++)
            alOut(alFreqH + i + 1, 0);
    }
}

///////////////////////////////////////////////////////////////////////////
//
//      SD_StartMusic() - starts playing the music pointed to
//
///////////////////////////////////////////////////////////////////////////
void
SD_StartMusic(int chunk)
{
    SD_MusicOff();

    if (MusicMode == smm_AdLib)
    {
        int32_t chunkLen = CA_CacheAudioChunk(chunk);
        sqHack = (word *)(void *) audiosegs[chunk];     // alignment is correct
        if(*sqHack == 0) sqHackLen = sqHackSeqLen = chunkLen;
        else sqHackLen = sqHackSeqLen = *sqHack++;
        sqHackPtr = sqHack;
        sqHackTime = 0;
        alTimeCount = 0;
        SD_MusicOn();
    }
}

