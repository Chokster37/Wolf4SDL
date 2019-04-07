// WL_MAIN.C

#ifdef _WIN32
    #include <io.h>
#else
    #include <unistd.h>
#endif

#include "wl_def.h"
#pragma hdrstop
#include <SDL_syswm.h>


/*
=============================================================================

                             WOLFENSTEIN 3-D

                        An Id Software production

                             by John Carmack

=============================================================================
*/

extern byte signon[];

/*
=============================================================================

                             LOCAL CONSTANTS

=============================================================================
*/


#define FOCALLENGTH     (0x5700l)               // in global coordinates
#define VIEWGLOBAL      0x10000                 // globals visable flush to wall

#define VIEWWIDTH       256                     // size of view window
#define VIEWHEIGHT      144

/*
=============================================================================

                            GLOBAL VARIABLES

=============================================================================
*/

char    str[80];
int     dirangle[9] = {0,ANGLES/8,2*ANGLES/8,3*ANGLES/8,4*ANGLES/8,
                       5*ANGLES/8,6*ANGLES/8,7*ANGLES/8,ANGLES};

//
// proejection variables
//
fixed    focallength;
unsigned screenofs;
int      viewscreenx, viewscreeny;
int      viewwidth;
int      viewheight;
int      anglewidth;
short    centerx;
int      shootdelta;           // pixels away from centerx a target can be
fixed    scale;
int32_t  heightnumerator;


void    Quit (const char *error,...);

boolean startgame;

char    configdir[256] = "";
char    configname[13] = "config.";
char    savename[13] = "savegame.";
word    configver = 0xfefc;
boolean levelrestore;

//
// Command line parameter variables
//
int     param_samplerate = 44100;
int     param_audiobuffer = 2048 / (44100 / param_samplerate);

/*
=============================================================================

                            LOCAL VARIABLES

=============================================================================
*/


/*
====================
=
= ReadConfig
=
====================
*/

void ReadConfig(void)
{
    SDMode  sd;
    SMMode  sm;

    char configpath[300];

    if(configdir[0])
        snprintf(configpath, sizeof(configpath), "%s/%s", configdir, configname);
    else
        strcpy(configpath, configname);

    const int file = open(configpath, O_RDONLY | O_BINARY);
    if (file != -1)
    {
        //
        // valid config file
        //
        word tmp;
        read(file,&tmp,sizeof(tmp));
        if(tmp!=configver)
        {
            close(file);
            goto noconfig;
        }
        read(file,Scores,sizeof(HighScore) * MaxScores);

        read(file,&sd,sizeof(sd));
        read(file,&sm,sizeof(sm));

        read(file,dirscan,sizeof(dirscan));
        read(file,buttonscan,sizeof(buttonscan));
        read(file,&viewsize,sizeof(viewsize));

        close(file);

        // make sure values are correct
        if(viewsize<4) viewsize=4;
        else if(viewsize>19) viewsize=19;

        MainMenu[6].active=1;
        MainItems.curpos=0;
    }
    else
    {
        //
        // no config file, so select by hardware
        //
noconfig:
        sd = sdm_PC;
        sm = smm_AdLib;

        viewsize = 19;                          // start with a good size
    }

    SD_SetMusicMode (sm);
    SD_SetSoundMode (sd);
}

/*
====================
=
= WriteConfig
=
====================
*/

void WriteConfig(void)
{
    char configpath[300];

    if(configdir[0])
        snprintf(configpath, sizeof(configpath), "%s/%s", configdir, configname);
    else
        strcpy(configpath, configname);

    const int file = open(configpath, O_CREAT | O_WRONLY | O_BINARY, 0644);
    if (file != -1)
    {
        word tmp=configver;
        write(file,&tmp,sizeof(tmp));
        write(file,Scores,sizeof(HighScore) * MaxScores);

        write(file,&SoundMode,sizeof(SoundMode));
        write(file,&MusicMode,sizeof(MusicMode));

        write(file,dirscan,sizeof(dirscan));
        write(file,buttonscan,sizeof(buttonscan));
        write(file,&viewsize,sizeof(viewsize));

        close(file);
    }
}


//===========================================================================

/*
=====================
=
= NewGame
=
= Set up new game to start from the beginning
=
=====================
*/

void NewGame (int difficulty,int episode)
{
    memset (&gamestate,0,sizeof(gamestate));
    gamestate.difficulty = difficulty;
    gamestate.weapon = gamestate.bestweapon
            = gamestate.chosenweapon = wp_pistol;
    gamestate.health = 100;
    gamestate.ammo = STARTAMMO;
    gamestate.lives = 3;
    gamestate.nextextra = EXTRAPOINTS;
    gamestate.episode=episode;
    
    faceframe = attackframe =
    attackcount = weaponframe = victoryflag = 0;

    startgame = true;
}

//===========================================================================


int32_t DoChecksum(byte *source,unsigned size,int32_t checksum)
{
    unsigned i;

    for (i=0;i<size-1;i++)
    checksum += source[i]^source[i+1];

    return checksum;
}


//===========================================================================

/*
======================
=
= SetupLevelRestore
=
======================
*/

void SetupLevelRestore(boolean import)
{
    word tmp;
    FILE *file;
    char loadpath[300];
    int32_t checksum=0,oldchecksum;

    levelrestore = false;

    if(configdir[0])
        snprintf(loadpath, sizeof(loadpath), "%s/%s", configdir, savename);
    else
        strcpy(loadpath, savename);

    //
    // enable the menu option if valid
    //
    if (!import)
    {
        const int handle = open(loadpath, O_RDONLY | O_BINARY);

        if(handle >= 0)
        {
            read(handle, &tmp, sizeof(tmp));
            if (tmp == configver)
                levelrestore = true;
            close(handle);
        }
        return;
    }

    //
    // actually load in the data
    //
    file = fopen (loadpath, "rb");

    fread (&tmp,sizeof(tmp),1,file);
    if(tmp==configver)
    {
        checksum = DoChecksum((byte *)&tmp,sizeof(tmp),checksum);

        fread (&gamestate,sizeof(gamestate),1,file);
        checksum = DoChecksum((byte *)&gamestate,sizeof(gamestate),checksum);

        fread (&LevelRatios[0],sizeof(LRstruct)*LRpack,1,file);
        checksum = DoChecksum((byte *)&LevelRatios[0],sizeof(LRstruct)*LRpack,checksum);

        fread (&oldchecksum,sizeof(oldchecksum),1,file);

        if (oldchecksum == checksum)
        {
	    	SetupGameLevel ();
	    	startgame = true;
            levelrestore = true;
	    }
    }
    fclose (file);
}


/*
======================
=
= UpdateLevelRestore
=
======================
*/

void UpdateLevelRestore(boolean valid)
{
    FILE *file;
    char savepath[300];
    int32_t checksum=0;

    if(configdir[0])
        snprintf(savepath, sizeof(savepath), "%s/%s", configdir, savename);
    else
        strcpy(savepath, savename);

    unlink (savepath);
    file = fopen (savepath, "wb");

    //
    // if true update the data, if false kill it
    //
    if (valid)
    {            
        fwrite(&configver,sizeof(configver),1,file);
        checksum = DoChecksum((byte *)&configver,sizeof(configver),checksum);

        fwrite(&gamestate,sizeof(gamestate),1,file);
        checksum = DoChecksum((byte *)&gamestate,sizeof(gamestate),checksum);

        fwrite(&LevelRatios[0],sizeof(LRstruct)*LRpack,1,file);
        checksum = DoChecksum((byte *)&LevelRatios[0],sizeof(LRstruct)*LRpack,checksum);

        fwrite (&checksum,sizeof(checksum),1,file);
    }
    else
        levelrestore = false;

    fclose (file);
}


//===========================================================================

/*
==========================
=
= ShutdownId
=
= Shuts down all ID_?? managers
=
==========================
*/

void ShutdownId (void)
{
    US_Shutdown ();         // This line is completely useless...
    SD_Shutdown ();
    PM_Shutdown ();
    IN_Shutdown ();
    VW_Shutdown ();
    CA_Shutdown ();
}


//===========================================================================

/*
==================
=
= BuildTables
=
= Calculates:
=
= scale                 projection constant
= sintable/costable     overlapping fractional tables
=
==================
*/

const float radtoint = (float)(FINEANGLES/2/PI);

void BuildTables (void)
{
    //
    // calculate fine tangents
    //

    int i;
    for(i=0;i<FINEANGLES/8;i++)
    {
        double tang=tan((i+0.5)/radtoint);
        finetangent[i]=(int32_t)(tang*GLOBAL1);
        finetangent[FINEANGLES/4-1-i]=(int32_t)((1/tang)*GLOBAL1);
    }

    //
    // costable overlays sintable with a quarter phase shift
    // ANGLES is assumed to be divisable by four
    //

    float angle=0;
    float anglestep=(float)(PI/2/ANGLEQUAD);
    for(i=0; i<ANGLEQUAD; i++)
    {
        fixed value=(int32_t)(GLOBAL1*sin(angle));
        sintable[i]=sintable[i+ANGLES]=sintable[ANGLES/2-i]=value;
        sintable[ANGLES-i]=sintable[ANGLES/2+i]=-value;
        angle+=anglestep;
    }
    sintable[ANGLEQUAD] = 65536;
    sintable[3*ANGLEQUAD] = -65536;
}

//===========================================================================


/*
====================
=
= CalcProjection
=
= Uses focallength
=
====================
*/

void CalcProjection (int32_t focal)
{
    int     i;
    int    intang;
    float   angle;
    double  tang;
    int     halfview;
    double  facedist;

    focallength = focal;
    facedist = focal+MINDIST;
    halfview = viewwidth/2;                                 // half view in pixels

    //
    // calculate scale value for vertical height calculations
    // and sprite x calculations
    //
    scale = (fixed) (halfview*facedist/(VIEWGLOBAL/2));

    //
    // divide heightnumerator by a posts distance to get the posts height for
    // the heightbuffer.  The pixel height is height>>2
    //
    heightnumerator = (TILEGLOBAL*scale)>>6;

    //
    // calculate the angle offset from view angle of each pixel's ray
    //

    for (i=0;i<halfview;i++)
    {
        // start 1/2 pixel over, so viewangle bisects two middle pixels
        tang = (int32_t)i*VIEWGLOBAL/viewwidth/facedist;
        angle = (float) atan(tang);
        intang = (int) (angle*radtoint);
        pixelangle[halfview-1-i] = intang;
        pixelangle[halfview+i] = -intang;
    }
}



//===========================================================================

/*
===================
=
= SetupWalls
=
= Map tile values to scaled pics
=
===================
*/

void SetupWalls (void)
{
    int     i;

    horizwall[0]=0;
    vertwall[0]=0;

    for (i=1;i<MAXWALLTILES;i++)
    {
        horizwall[i]=(i-1)*2;
        vertwall[i]=(i-1)*2+1;
    }
}

//===========================================================================

/*
==========================
=
= SignonScreen
=
==========================
*/

void SignonScreen (void)                        // VGA version
{
    VL_SetVGAPlaneMode ();

    VL_MungePic (signon,320,200);
    VL_MemToScreen (signon,320,200,0,0);
}


/*
==========================
=
= FinishSignon
=
==========================
*/

void FinishSignon (void)
{
    VW_Bar (0,189,300,11,VL_GetPixel(0,0));
    WindowX = 0;
    WindowW = 320;
    PrintY = 190;

    SETFONTCOLOR(14,4);

    US_CPrint ("Press a key");

    VH_UpdateScreen();

    IN_Ack ();

    VW_Bar (0,189,300,11,VL_GetPixel(0,0));

    PrintY = 190;
    SETFONTCOLOR(10,4);

    US_CPrint ("Working...");

    VH_UpdateScreen();

    SETFONTCOLOR(0,15);
}

//===========================================================================


/*
==========================
=
= InitGame
=
= Load a few things right away
=
==========================
*/

static void InitGame()
{
    // initialize SDL
#if defined _WIN32
    putenv("SDL_VIDEODRIVER=directx");
#endif
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

    SignonScreen ();

#if defined _WIN32
    if(!fullscreen)
    {
        struct SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);

        if(SDL_GetWMInfo(&wmInfo) != -1)
        {
            HWND hwndSDL = wmInfo.window;
            DWORD style = GetWindowLong(hwndSDL, GWL_STYLE) & ~WS_SYSMENU;
            SetWindowLong(hwndSDL, GWL_STYLE, style);
            SetWindowPos(hwndSDL, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        }
    }
#endif
	VW_UpdateScreen();

    VH_Startup ();
    IN_Startup ();
    PM_Startup ();
    SD_Startup ();
    CA_Startup ();
    US_Startup ();

//
// build some tables
//
    ReadConfig ();

    SetupLevelRestore (false);

    CA_LoadAllSounds ();

//
// draw intro screen stuff
//
    IntroScreen ();

//
// load in and lock down some basic chunks
//

    CA_CacheGrChunk(STARTFONT);
    CA_CacheGrChunk(STATUSBARPIC);

    LoadLatchMem ();
    BuildTables ();          // trig tables
    SetupWalls ();

    NewViewSize (viewsize);

//
// initialize variables
//
    InitRedShifts ();
    FinishSignon();
}

//===========================================================================

/*
==========================
=
= SetViewSize
=
==========================
*/

boolean SetViewSize (unsigned width, unsigned height)
{
    viewwidth = width&~15;                  // must be divisable by 16
    viewheight = height&~1;                 // must be even
    anglewidth = viewwidth*8/304;           // for CalcRotate
    centerx = viewwidth/2-1;
    shootdelta = viewwidth/10;
    if((unsigned) viewheight == screenHeight)
        viewscreenx = viewscreeny = screenofs = 0;
    else
    {
        viewscreenx = (screenWidth-viewwidth) / 2;
        viewscreeny = (screenHeight-scaleFactor*STATUSLINES-viewheight)/2;
        screenofs = viewscreeny*screenWidth+viewscreenx;
    }

//
// calculate trace angles and projection constants
//
    CalcProjection (FOCALLENGTH);

    return true;
}


void ShowViewSize (int width)
{
    int oldwidth,oldheight;

    oldwidth = viewwidth;
    oldheight = viewheight;

    viewwidth = width*16*scaleFactor;
    viewheight =  width*8*scaleFactor;
    DrawPlayBorder ();

    viewwidth = oldwidth;
    viewheight = oldheight;
}


void NewViewSize (int width)
{
    viewsize = width;
    SetViewSize(width*16*scaleFactor, width*8*scaleFactor);
}



//===========================================================================

/*
==========================
=
= Quit
=
==========================
*/

void Quit (const char *errorStr, ...)
{
    char error[256];
    if(errorStr != NULL)
    {
        va_list vlist;
        va_start(vlist, errorStr);
        vsprintf(error, errorStr, vlist);
        va_end(vlist);
    }
    else error[0] = 0;

    if (!pictable)  // don't try to display the red box before it's loaded
    {
        ShutdownId();
        if (error && *error)
            puts(error);

        exit(1);
    }

    if (!error || !*error)
        WriteConfig ();

    ShutdownId ();

    if (error && *error)
    {
        puts(error);
        exit(1);
    }

    exit(0);
}

//===========================================================================



/*
=====================
=
= DemoLoop
=
=====================
*/


static void DemoLoop()
{
    int LastDemo = 0;

//
// main game cycle
//

    StartCPMusic(INTROSONG);

    PG13 ();

    while (1)
    {
        while (1)
        {
//
// title page
//
            CA_CacheScreen (TITLEPIC);
            VW_UpdateScreen ();
            VW_FadeIn();
            if (IN_UserInput(TickBase*15))
                break;
            VW_FadeOut();
//
// credits page
//
            CA_CacheScreen (CREDITSPIC);
            VW_UpdateScreen();
            VW_FadeIn ();
            if (IN_UserInput(TickBase*10))
                break;
            VW_FadeOut ();
//
// high scores
//
            DrawHighScores ();
            VW_UpdateScreen ();
            VW_FadeIn ();

            if (IN_UserInput(TickBase*10))
                break;
//
// demo
//
            PlayDemo (LastDemo++%4);

            if (playstate == ex_abort)
                break;
            VW_FadeOut();
            StartCPMusic(INTROSONG);
        }

        VW_FadeOut ();

        US_ControlPanel (0);

        if (startgame)
        {
            GameLoop ();
            VW_FadeOut();
            StartCPMusic(INTROSONG);
        }
    }
}


//===========================================================================

#define IFARG(str) if(!strcmp(arg, (str)))

void CheckParameters(int argc, char *argv[])
{
    bool hasError = false, showHelp = false;
    bool sampleRateGiven = false, audioBufferGiven = false;
    int defaultSampleRate = param_samplerate;

    for(int i = 1; i < argc; i++)
    {
        char *arg = argv[i];

        IFARG("--windowed")
            fullscreen = false;
        else IFARG("--res")
        {
            if(i + 2 >= argc)
            {
                printf("The res option needs the width and/or the height argument!\n");
                hasError = true;
            }
            else
            {
                screenWidth = atoi(argv[++i]);
                screenHeight = atoi(argv[++i]);
                unsigned factor = screenWidth / 320;
                if(screenWidth % 320 || screenHeight != 200 * factor)
                    printf("Screen size must be a multiple of 320x200!\n"), hasError = true;
            }
        }
        else IFARG("--bits")
        {
            if(++i >= argc)
            {
                printf("The bits option is missing the color depth argument!\n");
                hasError = true;
            }
            else
            {
                screenBits = atoi(argv[i]);
                switch(screenBits)
                {
                    case 8:
                    case 16:
                    case 24:
                    case 32:
                        break;

                    default:
                        printf("Screen color depth must be 8, 16, 24, or 32!\n");
                        hasError = true;
                        break;
                }
            }
        }
        else IFARG("--nodblbuf")
            usedoublebuffering = false;
        else IFARG("--extravbls")
        {
            if(++i >= argc)
            {
                printf("The extravbls option is missing the vbls argument!\n");
                hasError = true;
            }
            else
            {
                extravbls = atoi(argv[i]);
                if(extravbls < 0)
                {
                    printf("Extravbls must be positive!\n");
                    hasError = true;
                }
            }
        }
        else IFARG("--samplerate")
        {
            if(++i >= argc)
            {
                printf("The samplerate option is missing the rate argument!\n");
                hasError = true;
            }
            else param_samplerate = atoi(argv[i]);
            sampleRateGiven = true;
        }
        else IFARG("--audiobuffer")
        {
            if(++i >= argc)
            {
                printf("The audiobuffer option is missing the size argument!\n");
                hasError = true;
            }
            else param_audiobuffer = atoi(argv[i]);
            audioBufferGiven = true;
        }
        else IFARG("--configdir")
        {
            if(++i >= argc)
            {
                printf("The configdir option is missing the dir argument!\n");
                hasError = true;
            }
            else
            {
                size_t len = strlen(argv[i]);
                if(len + 2 > sizeof(configdir))
                {
                    printf("The config directory is too long!\n");
                    hasError = true;
                }
                else
                {
                    strcpy(configdir, argv[i]);
                    if(argv[i][len] != '/' && argv[i][len] != '\\')
                        strcat(configdir, "/");
                }
            }
        }
        else IFARG("--help")
            showHelp = true;
        else hasError = true;
    }
    if(hasError || showHelp)
    {
        if(hasError) printf("\n");
        printf(
            "Wolf4SDL v1.7 ($Revision$)\n"
            "Ported by Chaos-Software (http://www.chaos-software.de.vu)\n"
            "Original Wolfenstein 3D by id Software\n\n"
            "Usage: Wolf4SDL [options]\n"
            "Options:\n"
            " --help                 This help page\n"
            " --windowed             Starts the game in a window\n"
            " --res <width> <height> Sets the screen resolution\n"
            "                        (must be multiple of 320x200)\n"
            " --bits <b>             Sets the screen color depth\n"
            "                        (use this when you have palette/fading problems\n"
            "                        allowed: 8, 16, 24, 32, default: \"best\" depth)\n"
            " --nodblbuf             Don't use SDL's double buffering\n"
            " --extravbls <vbls>     Sets a delay after each frame, which may help to\n"
            "                        reduce flickering (unit is currently 8 ms, default: 0)\n"
            " --samplerate <rate>    Sets the sound sample rate (given in Hz, default: %i)\n"
            " --audiobuffer <size>   Sets the size of the audio buffer (-> sound latency)\n"
            "                        (given in bytes, default: 2048 / (44100 / samplerate))\n"
            " --configdir <dir>      Directory where config file and save games are stored\n"
#if defined(_WIN32)
            "                        (default: current directory)\n"
#else
            "                        (default: $HOME/.wolf4sdl)\n"
#endif
            , defaultSampleRate
        );
        exit(1);
    }

    if(sampleRateGiven && !audioBufferGiven)
        param_audiobuffer = 2048 / (44100 / param_samplerate);
}

/*
==========================
=
= main
=
==========================
*/

int main (int argc, char *argv[])
{
    CheckParameters(argc, argv);

    CheckForEpisodes();

    InitGame();

    DemoLoop();

    Quit("Demo loop exited???");
    return 1;
}
