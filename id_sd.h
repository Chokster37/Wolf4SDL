//
//      ID Engine
//      ID_SD.h - Sound Manager Header
//      Version for Wolfenstein
//      By Jason Blochowiak
//

#ifndef __ID_SD__
#define __ID_SD__

#define alOut(n,b) YM3812Write(oplChip, n, b)

#define TickBase        70      // 70Hz per tick - used as a base for timer 0

typedef enum
{
    sdm_Off,
    sdm_PC
} SDMode;

typedef enum
{
    smm_Off,smm_AdLib
} SMMode;

typedef struct
{
    longword        length;
    word            priority;
} SoundCommon;

//      PC Sound stuff
typedef struct
{
    SoundCommon     common;
    byte            data[1];
} PCSound;

//      Register addresses
// Channel stuff
#define alFreqL         0xa0
#define alFreqH         0xb0
// Global stuff
#define alEffects       0xbd

typedef struct
{
    byte    mChar,cChar,
            mScale,cScale,
            mAttack,cAttack,
            mSus,cSus,
            mWave,cWave,
            nConn,

            // These are only for Muse - these bytes are really unused
            voice,
            mode;
    byte    unused[3];
} Instrument;

//
//      Sequencing stuff
//
#define sqMaxTracks     10

// Global variables
extern  SDMode          SoundMode;
extern  SMMode          MusicMode;

#define GetTimeCount()  ((SDL_GetTicks()*7)/100)

inline void Delay(int wolfticks)
{
    if(wolfticks>0) SDL_Delay(wolfticks * 100 / 7);
}

// Function prototypes
extern  void    SD_Startup(void),
                SD_Shutdown(void);

extern  boolean SD_PlaySound(soundnames sound);
extern  void    SD_StopSound(void),
                SD_WaitSoundDone(void);

extern  void    SD_StartMusic(int chunk);
extern  void    SD_MusicOn(void),
                SD_FadeOutMusic(void);
extern  void    SD_MusicOff(void);

extern  boolean SD_MusicPlaying(void);
extern  boolean SD_SetSoundMode(SDMode mode);
extern  boolean SD_SetMusicMode(SMMode mode);
extern  word    SD_SoundPlaying(void);

#endif
