////////////////////////////////////////////////////////////////////
//
// WL_MENU.C
// by John Romero (C) 1992 Id Software, Inc.
//
////////////////////////////////////////////////////////////////////

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "wl_def.h"
#pragma hdrstop

//
// PRIVATE PROTOTYPES
//
#define STARTITEM       newgame

CP_itemtype MainMenu[] = {
    {1, STR_NG, CP_NewGame},
    {1, STR_CL, CP_Control},
    {1, STR_VS, CP_ViewScores},
    {1, STR_BD, 0},
    {1, STR_QT, 0}
};

CP_itemtype SndMenu[] = {
    {1, STR_NONE, 0},
    {1, STR_PC, 0},
    {0, "", 0},
    {0, "", 0},
    {1, STR_NONE, 0},
    {1, STR_ALSB, 0}
};

CP_itemtype NewEmenu[] = {
    {1, "Episode 1\n" "Escape from Wolfenstein", 0},
    {0, "", 0},
    {1, "Episode 2\n" "Operation: Eisenfaust", 0},
    {0, "", 0},
    {1, "Episode 3\n" "Die, Fuhrer, Die!", 0},
    {0, "", 0},
    {1, "Episode 4\n" "A Dark Secret", 0},
    {0, "", 0},
    {1, "Episode 5\n" "Trail of the Madman", 0},
    {0, "", 0},
    {1, "Episode 6\n" "Confrontation", 0}
};


CP_itemtype NewMenu[] = {
    {1, STR_DADDY, 0},
    {1, STR_HURTME, 0},
    {1, STR_BRINGEM, 0},
    {1, STR_DEATH, 0}
};

CP_itemtype CusMenu[] = {
    {0, "", 0},
    {0, "", 0},
    {0, "", 0},
    {0, "", 0},
    {0, "", 0},
    {0, "", 0},
    {1, "", 0},
    {0, "", 0},
    {1, "", 0}
};

// CP_iteminfo struct format: short x, y, amount, curpos, indent;
CP_iteminfo MainItems = { MENU_X, MENU_Y, lengthof(MainMenu), STARTITEM, 24 },
            SndItems  = { SM_X, SM_Y1, lengthof(SndMenu), 0, 52 },
            CusItems  = { CST_X, CST_Y + 13 * 2, lengthof(CusMenu), -1, 0},
            NewEitems = { NE_X, NE_Y, lengthof(NewEmenu), 0, 88 },
            NewItems  = { NM_X, NM_Y, lengthof(NewMenu), 2, 24 };

int color_hlite[] = {
    DEACTIVE,
    HIGHLIGHT,
    READHCOLOR,
    0x67
};

int color_norml[] = {
    DEACTIVE,
    TEXTCOLOR,
    READCOLOR,
    0x6b
};

// ENDSTRx constants are defined in foreign.h
char endStrings[9][80] = {
    ENDSTR1,
    ENDSTR2,
    ENDSTR3,
    ENDSTR4,
    ENDSTR5,
    ENDSTR6,
    ENDSTR7,
    ENDSTR8,
    ENDSTR9
};

static int StartGame;
static int SoundStatus = 1;


////////////////////////////////////////////////////////////////////
//
// INPUT MANAGER SCANCODE TABLES
//
////////////////////////////////////////////////////////////////////

static const char* const ScanNames[SDLK_LAST] =
    {
        "?","?","?","?","?","?","?","?",                                //   0
        "BkSp","Tab","?","?","?","Return","?","?",                      //   8
        "?","?","?","Pause","?","?","?","?",                            //  16
        "?","?","?","Esc","?","?","?","?",                              //  24
        "Space","!","\"","#","$","?","&","'",                           //  32
        "(",")","*","+",",","-",".","/",                                //  40
        "0","1","2","3","4","5","6","7",                                //  48
        "8","9",":",";","<","=",">","?",                                //  56
        "@","A","B","C","D","E","F","G",                                //  64
        "H","I","J","K","L","M","N","O",                                //  72
        "P","Q","R","S","T","U","V","W",                                //  80
        "X","Y","Z","[","\\","]","^","_",                               //  88
        "`","a","b","c","d","e","f","h",                                //  96
        "h","i","j","k","l","m","n","o",                                // 104
        "p","q","r","s","t","u","v","w",                                // 112
        "x","y","z","{","|","}","~","?",                                // 120
        "?","?","?","?","?","?","?","?",                                // 128
        "?","?","?","?","?","?","?","?",                                // 136
        "?","?","?","?","?","?","?","?",                                // 144
        "?","?","?","?","?","?","?","?",                                // 152
        "?","?","?","?","?","?","?","?",                                // 160
        "?","?","?","?","?","?","?","?",                                // 168
        "?","?","?","?","?","?","?","?",                                // 176
        "?","?","?","?","?","?","?","?",                                // 184
        "?","?","?","?","?","?","?","?",                                // 192
        "?","?","?","?","?","?","?","?",                                // 200
        "?","?","?","?","?","?","?","?",                                // 208
        "?","?","?","?","?","?","?","?",                                // 216
        "?","?","?","?","?","?","?","?",                                // 224
        "?","?","?","?","?","?","?","?",                                // 232
        "?","?","?","?","?","?","?","?",                                // 240
        "?","?","?","?","?","?","?","?",                                // 248
        "?","?","?","?","?","?","?","?",                                // 256
        "?","?","?","?","?","?","?","Enter",                            // 264
        "?","Up","Down","Right","Left","Ins","Home","End",              // 272
        "PgUp","PgDn","F1","F2","F3","F4","F5","F6",                    // 280
        "F7","F8","F9","F10","F11","F12","?","?",                       // 288
        "?","?","?","?","NumLk","CapsLk","ScrlLk","RShft",              // 296
        "Shift","RCtrl","Ctrl","RAlt","Alt","?","?","?",                // 304
        "?","?","?","?","PrtSc","?","?","?",                            // 312
        "?","?"                                                         // 320
    };


////////////////////////////////////////////////////////////////////
//
// Wolfenstein Control Panel!  Ta Da!
//
////////////////////////////////////////////////////////////////////
void
US_ControlPanel (ScanCode scancode)
{
    int which;

    if (ingame)
        StartCPMusic (MENUSONG);
    else
        StartCPMusic (MENUSONG);

    SetupControlPanel ();

    DrawMainMenu ();
    MenuFadeIn ();
    StartGame = 0;

    //
    // MAIN MENU LOOP
    //
    do
    {
        which = HandleMenu (&MainItems, &MainMenu[0], NULL);

        switch (which)
        {
            case viewscores:
                if (MainMenu[viewscores].routine == NULL)
                {
                    if (CP_EndGame (0))
                        StartGame = 1;
                }
                else
                {
                    DrawMainMenu();
                    MenuFadeIn ();
                }
                break;

            case backtodemo:
                StartGame = 1;
                if (!ingame)
                {
                    if (levelrestore)
                        SetupLevelRestore (true);
                    else				    
                        StartCPMusic (INTROSONG);
                }
                VL_FadeOut (0, 255, 0, 0, 0, 10);
                break;

            case -1:
            case quit:
                CP_Quit (0);
                break;

            default:
                if (!StartGame)
                {
                    DrawMainMenu ();
                    MenuFadeIn ();
                }
        }

        //
        // "EXIT OPTIONS" OR "NEW GAME" EXITS
        //
    }
    while (!StartGame);

    //
    // DEALLOCATE EVERYTHING
    //
    CleanupControlPanel ();

    //
    // CHANGE MAINMENU ITEM
    //
    if (startgame)
        EnableEndGameMenuItem();

    // RETURN/START GAME EXECUTION
}

void EnableEndGameMenuItem()
{
    MainMenu[viewscores].routine = NULL;
    strcpy (MainMenu[viewscores].string, STR_EG);
}

////////////////////////
//
// DRAW MAIN MENU SCREEN
//
void
DrawMainMenu (void)
{
    ClearMScreen ();

    VWB_DrawPic (112, 184, C_MOUSELBACKPIC);
    DrawStripes (10);
    VWB_DrawPic (84, 0, C_OPTIONSPIC);

    DrawWindow (MENU_X - 8, MENU_Y - 3, MENU_W, MENU_H, BKGDCOLOR);

    //
    // CHANGE "GAME" AND "DEMO"
    //
    if (ingame || levelrestore)
    {
        strcpy (&MainMenu[backtodemo].string[8], STR_GAME);
        MainMenu[backtodemo].active = 2;
    }
    else
    {
        strcpy (&MainMenu[backtodemo].string[8], STR_DEMO);
        MainMenu[backtodemo].active = 1;
    }

    DrawMenu (&MainItems, &MainMenu[0]);
    VW_UpdateScreen ();
}


////////////////////////////////////////////////////////////////////
//
// END THE CURRENT GAME
//
////////////////////////////////////////////////////////////////////
int
CP_EndGame (int)
{
    int res;
    res = Confirm (ENDGAMESTR);
    DrawMainMenu();
    if(!res) return 0;

    gamestate.lives = 0;
    playstate = ex_died;
    killerobj = NULL;

    UpdateLevelRestore (false);
    MainMenu[viewscores].routine = CP_ViewScores;
    strcpy (MainMenu[viewscores].string, STR_VS);

    return 1;
}


////////////////////////////////////////////////////////////////////
//
// VIEW THE HIGH SCORES
//
////////////////////////////////////////////////////////////////////
int
CP_ViewScores (int)
{
    fontnumber = 0;

    StartCPMusic (ROSTER_MUS);

    DrawHighScores ();
    VW_UpdateScreen ();
    MenuFadeIn ();
    fontnumber = 1;

    IN_Ack ();

    StartCPMusic (MENUSONG);
    MenuFadeOut ();

    return 0;
}


////////////////////////////////////////////////////////////////////
//
// START A NEW GAME
//
////////////////////////////////////////////////////////////////////
int
CP_NewGame (int)
{
    int which, episode;

  firstpart:

    DrawNewEpisode ();
    do
    {
        which = HandleMenu (&NewEitems, &NewEmenu[0], NULL);
        switch (which)
        {
            case -1:
                MenuFadeOut ();
                return 0;

            default:
                episode = which / 2;
                which = 1;
                break;
        }

    }
    while (!which);

    ShootSnd ();

    //
    // ALREADY IN A GAME?
    //
    if (ingame || levelrestore)
        if (!Confirm (CURGAME))
        {
            MenuFadeOut ();
            return 0;
        }

    MenuFadeOut ();

    DrawNewGame ();
    which = HandleMenu (&NewItems, &NewMenu[0], DrawNewGameDiff);
    if (which < 0)
    {
        MenuFadeOut ();
        goto firstpart;
    }

    ShootSnd ();
    UpdateLevelRestore (false);
    NewGame (which, episode);
    StartGame = 1;
    MenuFadeOut ();

    return 0;
}


/////////////////////
//
// DRAW NEW EPISODE MENU
//
void
DrawNewEpisode (void)
{
    int i;

    ClearMScreen ();
    VWB_DrawPic (112, 184, C_MOUSELBACKPIC);

    DrawWindow (NE_X - 4, NE_Y - 4, NE_W + 8, NE_H + 8, BKGDCOLOR);
    SETFONTCOLOR (READHCOLOR, BKGDCOLOR);
    PrintY = 2;
    WindowX = 0;
    US_CPrint ("Which episode to play?");

    SETFONTCOLOR (TEXTCOLOR, BKGDCOLOR);
    DrawMenu (&NewEitems, &NewEmenu[0]);

    for (i = 0; i < 6; i++)
        VWB_DrawPic (NE_X + 32, NE_Y + i * 26, C_EPISODE1PIC + i);

    VW_UpdateScreen ();
    MenuFadeIn ();
    WaitKeyUp ();
}

/////////////////////
//
// DRAW NEW GAME MENU
//
void
DrawNewGame (void)
{
    ClearMScreen ();
    VWB_DrawPic (112, 184, C_MOUSELBACKPIC);

    SETFONTCOLOR (READHCOLOR, BKGDCOLOR);
    PrintX = NM_X + 20;
    PrintY = NM_Y - 32;

    US_Print ("How tough are you?");

    DrawWindow (NM_X - 5, NM_Y - 10, NM_W, NM_H, BKGDCOLOR);

    DrawMenu (&NewItems, &NewMenu[0]);
    DrawNewGameDiff (NewItems.curpos);
    VW_UpdateScreen ();
    MenuFadeIn ();
    WaitKeyUp ();
}


////////////////////////
//
// DRAW NEW GAME GRAPHIC
//
void
DrawNewGameDiff (int w)
{
    VWB_DrawPic (NM_X + 185, NM_Y + 7, w + C_BABYMODEPIC);
}


////////////////////////////////////////////////////////////////////
//
// CUSTOMIZE CONTROLS
//
////////////////////////////////////////////////////////////////////
enum
{ FIRE, STRAFE, RUN, OPEN };
int8_t order[4] = { RUN, OPEN, FIRE, STRAFE };

void
DrawVolumeBar (void)
{
    VWB_Bar (44, 123, 240, 10, TEXTCOLOR);
    DrawOutline (44, 123, 240, 10, 0, HIGHLIGHT);
    if (SoundMode == sdm_PC)
    {
        DrawOutline (44 + 30 * (pcvolume-1), 123, 30, 10, 0, READCOLOR);
        VWB_Bar (45 + 30 * (pcvolume-1), 124, 29, 9, READHCOLOR);
    }
}


int
CP_Control (int)
{
    int which, exit=0;
    ControlInfo ci;

    DrawControlScreen ();
    MenuFadeIn ();
    WaitKeyUp ();

    //
    // HANDLE SOUND SECTION
    //
    do
    {
        which = HandleMenu (&SndItems, &SndMenu[0], NULL);

        switch (which)
        {
                //
                // SOUND EFFECTS
                //
            case 0:
                if (SoundMode != sdm_Off)
                {
                    SD_WaitSoundDone ();
                    SD_SetSoundMode (sdm_Off);
                    DrawControlScreen ();
                }
                break;
            case 1:
                if (SoundMode != sdm_PC)
                {
                    SD_WaitSoundDone ();
                    SD_SetSoundMode (sdm_PC);
                    DrawControlScreen ();
                    ShootSnd ();
                }
                break;

                //
                // MUSIC
                //
            case 4:
                if (MusicMode != smm_Off)
                {
                    SD_SetMusicMode (smm_Off);
                    DrawControlScreen ();
                    ShootSnd ();
                }
                break;
            case 5:
                if (MusicMode != smm_AdLib)
                {
                    SD_SetMusicMode (smm_AdLib);
                    DrawControlScreen ();
                    ShootSnd ();
                    StartCPMusic (MENUSONG);
                }
                break;
        }
    }
    while (which >= 0);

    //
    // HANDLE KEYBOARD SECTION
    //
    do
    {
        which = HandleMenu (&CusItems, &CusMenu[0], FixupCustom);
        switch (which)
        {
            case 6:
                DefineKeyBtns ();
                DrawCustKeybd (0);
                break;
            case 8:
                DefineKeyMove ();
                DrawCustKeys (0);
        }
    }
    while (which >= 0);

    //
    // HANDLE VOLUME SECTION
    //
    if (SoundMode != sdm_PC)
    {
        MenuFadeOut ();
        return 0;
    }

    do
    {
        SDL_Delay(5);
        ReadAnyControl (&ci);
        
        switch (ci.dir)
        {
            case dir_North:
            case dir_West:
                if (pcvolume > 1)
                {
                    pcvolume--;
                    DrawVolumeBar ();
                    VW_UpdateScreen ();
                    SD_PlaySound (MOVEGUN1SND);
                    TicDelay(20);
                }
                break;

            case dir_South:
            case dir_East:
                if (pcvolume < 8)
                {
                    pcvolume++;
                    DrawVolumeBar();
                    VW_UpdateScreen ();
                    SD_PlaySound (MOVEGUN1SND);
                    TicDelay(20);
                }
                break;
        }

        if (ci.button0 || Keyboard[sc_Space] || Keyboard[sc_Enter])
            exit = 1;
        else if (ci.button1 || Keyboard[sc_Escape])
            exit = 2;            
    }
    while (!exit);
    ShootSnd ();

    MenuFadeOut ();

    return 0;
}


////////////////////////
//
// DEFINE THE KEYBOARD BUTTONS
//
void
DefineKeyBtns (void)
{
    CustomCtrls keyallowed = { 1, 1, 1, 1 };
    EnterCtrlData (8, &keyallowed, DrawCustKeybd, PrintCustKeybd, KEYBOARDBTNS);
}


////////////////////////
//
// DEFINE THE KEYBOARD BUTTONS
//
void
DefineKeyMove (void)
{
    CustomCtrls keyallowed = { 1, 1, 1, 1 };
    EnterCtrlData (10, &keyallowed, DrawCustKeys, PrintCustKeys, KEYBOARDMOVE);
}


////////////////////////
//
// ENTER CONTROL DATA FOR ANY TYPE OF CONTROL
//
enum
{ FWRD, RIGHT, BKWD, LEFT };
int moveorder[4] = { LEFT, RIGHT, FWRD, BKWD };

void
EnterCtrlData (int index, CustomCtrls * cust, void (*DrawRtn) (int), void (*PrintRtn) (int),
               int type)
{
    int j, exit, tick, redraw, which, x, picked, lastFlashTime;
    ControlInfo ci;


    ShootSnd ();
    PrintY = CST_Y + 13 * index;
    IN_ClearKeysDown ();
    exit = 0;
    redraw = 1;
    //
    // FIND FIRST SPOT IN ALLOWED ARRAY
    //
    for (j = 0; j < 4; j++)
        if (cust->allowed[j])
        {
            which = j;
            break;
        }

    do
    {
        if (redraw)
        {
            x = CST_START + CST_SPC * which;
            DrawWindow (5, PrintY - 1, 310, 13, BKGDCOLOR);

            DrawRtn (1);
            DrawWindow (x - 2, PrintY, CST_SPC, 11, TEXTCOLOR);
            DrawOutline (x - 2, PrintY, CST_SPC, 11, 0, HIGHLIGHT);
            SETFONTCOLOR (0, TEXTCOLOR);
            PrintRtn (which);
            PrintX = x;
            SETFONTCOLOR (TEXTCOLOR, BKGDCOLOR);
            VW_UpdateScreen ();
            WaitKeyUp ();
            redraw = 0;
        }

        SDL_Delay(5);
        ReadAnyControl (&ci);

        //
        // CHANGE BUTTON VALUE?
        //
        if ((type != KEYBOARDBTNS && type != KEYBOARDMOVE) && (ci.button0 | ci.button1 | ci.button2 | ci.button3) ||
            ((type == KEYBOARDBTNS || type == KEYBOARDMOVE) && LastScan == sc_Enter))
        {
            lastFlashTime = GetTimeCount();
            tick = picked = 0;
            SETFONTCOLOR (0, TEXTCOLOR);

            if (type == KEYBOARDBTNS || type == KEYBOARDMOVE)
                IN_ClearKeysDown ();

            while(1)
            {
                int button, result = 0;

                //
                // FLASH CURSOR
                //
                if (GetTimeCount() - lastFlashTime > 10)
                {
                    switch (tick)
                    {
                        case 0:
                            VWB_Bar (x, PrintY + 1, CST_SPC - 2, 10, TEXTCOLOR);
                            break;
                        case 1:
                            PrintX = x;
                            US_Print ("?");
                            SD_PlaySound (HITWALLSND);
                    }
                    tick ^= 1;
                    lastFlashTime = GetTimeCount();
                    VW_UpdateScreen ();
                }
                else SDL_Delay(5);

                //
                // WHICH TYPE OF INPUT DO WE PROCESS?
                //
                switch (type)
                {
                    case KEYBOARDBTNS:
                        if (LastScan && LastScan != sc_Escape)
                        {
                            buttonscan[order[which]] = LastScan;
                            picked = 1;
                            ShootSnd ();
                            IN_ClearKeysDown ();
                        }
                        break;

                    case KEYBOARDMOVE:
                        if (LastScan && LastScan != sc_Escape)
                        {
                            dirscan[moveorder[which]] = LastScan;
                            picked = 1;
                            ShootSnd ();
                            IN_ClearKeysDown ();
                        }
                        break;
                }

                //
                // EXIT INPUT?
                //
                if (IN_KeyDown (sc_Escape))
                {
                    picked = 1;
                    SD_PlaySound (ESCPRESSEDSND);
                }

                if(picked) break;

                ReadAnyControl (&ci);
            }

            SETFONTCOLOR (TEXTCOLOR, BKGDCOLOR);
            redraw = 1;
            WaitKeyUp ();
            continue;
        }

        if (ci.button1 || IN_KeyDown (sc_Escape))
            exit = 1;

        //
        // MOVE TO ANOTHER SPOT?
        //
        switch (ci.dir)
        {
            case dir_West:
                do
                {
                    which--;
                    if (which < 0)
                        which = 3;
                }
                while (!cust->allowed[which]);
                redraw = 1;
                SD_PlaySound (MOVEGUN1SND);
                while (ReadAnyControl (&ci), ci.dir != dir_None) SDL_Delay(5);
                IN_ClearKeysDown ();
                break;

            case dir_East:
                do
                {
                    which++;
                    if (which > 3)
                        which = 0;
                }
                while (!cust->allowed[which]);
                redraw = 1;
                SD_PlaySound (MOVEGUN1SND);
                while (ReadAnyControl (&ci), ci.dir != dir_None) SDL_Delay(5);
                IN_ClearKeysDown ();
                break;
            case dir_North:
            case dir_South:
                exit = 1;
        }
    }
    while (!exit);

    SD_PlaySound (ESCPRESSEDSND);
    WaitKeyUp ();
    DrawWindow (5, PrintY - 1, 310, 13, BKGDCOLOR);
}


////////////////////////
//
// FIXUP GUN CURSOR OVERDRAW SHIT
//
void
FixupCustom (int w)
{
    static int lastwhich = -1;
    int y = CST_Y + 26 + w * 13;


    VWB_Hlin (7, 32, y - 1, DEACTIVE);
    VWB_Hlin (7, 32, y + 12, BORD2COLOR);
    VWB_Hlin (7, 32, y - 2, BORDCOLOR);
    VWB_Hlin (7, 32, y + 13, BORDCOLOR);

    switch (w)
    {
        case 6:
            DrawCustKeybd (1);
            break;
        case 8:
            DrawCustKeys (1);
    }


    if (lastwhich >= 0)
    {
        y = CST_Y + 26 + lastwhich * 13;
        VWB_Hlin (7, 32, y - 1, DEACTIVE);
        VWB_Hlin (7, 32, y + 12, BORD2COLOR);
        VWB_Hlin (7, 32, y - 2, BORDCOLOR);
        VWB_Hlin (7, 32, y + 13, BORDCOLOR);

        if (lastwhich != w)
            switch (lastwhich)
            {
                case 6:
                    DrawCustKeybd (0);
                    break;
                case 8:
                    DrawCustKeys (0);
            }
    }

    lastwhich = w;
}


////////////////////////
//
// DRAW CONTROL SCREEN
//
void
DrawControlScreen (void)
{
    int i, on;

    ClearMScreen ();
    WindowX = 0;
    WindowW = 320;
    VWB_DrawPic (112, 184, C_MOUSELBACKPIC);


    //
    // SOUND SECTION
    //
    DrawWindow (SM_X - 8, SM_Y1 - 3, SM_W, SM_H1, BKGDCOLOR);
    DrawWindow (SM_X - 8, SM_Y2 - 3, SM_W, SM_H2, BKGDCOLOR);

    DrawMenu (&SndItems, &SndMenu[0]);
    VWB_DrawPic (100, SM_Y1 - 20, C_FXTITLEPIC);
    VWB_DrawPic (100, SM_Y2 - 20, C_MUSICTITLEPIC);

    for (i = 0; i < SndItems.amount; i++)
        if (SndMenu[i].string[0])
        {
            //
            // DRAW SELECTED/NOT SELECTED GRAPHIC BUTTONS
            //
            on = 0;
            switch (i)
            {
                    //
                    // SOUND EFFECTS
                    //
                case 0:
                    if (SoundMode == sdm_Off)
                        on = 1;
                    break;
                case 1:
                    if (SoundMode == sdm_PC)
                        on = 1;
                    break;

                    //
                    // MUSIC
                    //
                case 4:
                    if (MusicMode == smm_Off)
                        on = 1;
                    break;
                case 5:
                    if (MusicMode == smm_AdLib)
                        on = 1;
                    break;
            }

            if (on)
                VWB_DrawPic (SM_X + 24, SM_Y1 + i * 13 + 2, C_SELECTEDPIC);
            else
                VWB_DrawPic (SM_X + 24, SM_Y1 + i * 13 + 2, C_NOTSELECTEDPIC);
        }

    DrawMenuGun (&SndItems);


    //
    // CHANGEVIEW SECTION
    //
    WindowX = 0;
    PrintY = 108;
    SETFONTCOLOR (READCOLOR, BKGDCOLOR);
    US_CPrint (STR_VOLUME);
    DrawVolumeBar ();


    //
    // KEYBOARD SECTION
    //
    PrintY = CST_Y + 13 * 7;
    SETFONTCOLOR (TEXTCOLOR, BKGDCOLOR);
    PrintX = CST_START;
    US_Print (STR_CRUN);
    PrintX = CST_START + CST_SPC * 1;
    US_Print (STR_COPEN);
    PrintX = CST_START + CST_SPC * 2;
    US_Print (STR_CFIRE);
    PrintX = CST_START + CST_SPC * 3;
    US_Print (STR_CSTRAFE "\n");
    DrawWindow (5, PrintY - 1, 310, 13, BKGDCOLOR);
    DrawCustKeybd (0);
    US_Print ("\n");

    SETFONTCOLOR (TEXTCOLOR, BKGDCOLOR);
    PrintX = CST_START;
    US_Print (STR_LEFT);
    PrintX = CST_START + CST_SPC * 1;
    US_Print (STR_RIGHT);
    PrintX = CST_START + CST_SPC * 2;
    US_Print (STR_FRWD);
    PrintX = CST_START + CST_SPC * 3;
    US_Print (STR_BKWD "\n");
    DrawWindow (5, PrintY - 1, 310, 13, BKGDCOLOR);
    DrawCustKeys (0);

    //
    // PICK STARTING POINT IN MENU
    //
    if (CusItems.curpos < 0)
        for (i = 0; i < CusItems.amount; i++)
            if (CusMenu[i].active)
            {
                CusItems.curpos = i;
                break;
            }


    VW_UpdateScreen ();
    MenuFadeIn ();
}


void
PrintCustKeybd (int i)
{
    PrintX = CST_START + CST_SPC * i;
    US_Print ((const char *) IN_GetScanName (buttonscan[order[i]]));
}

void
DrawCustKeybd (int hilight)
{
    int i, color;


    color = TEXTCOLOR;
    if (hilight)
        color = HIGHLIGHT;
    SETFONTCOLOR (color, BKGDCOLOR);

    PrintY = CST_Y + 13 * 8;
    for (i = 0; i < 4; i++)
        PrintCustKeybd (i);
}

void
PrintCustKeys (int i)
{
    PrintX = CST_START + CST_SPC * i;
    US_Print ((const char *) IN_GetScanName (dirscan[moveorder[i]]));
}

void
DrawCustKeys (int hilight)
{
    int i, color;


    color = TEXTCOLOR;
    if (hilight)
        color = HIGHLIGHT;
    SETFONTCOLOR (color, BKGDCOLOR);

    PrintY = CST_Y + 13 * 10;
    for (i = 0; i < 4; i++)
        PrintCustKeys (i);
}


////////////////////////////////////////////////////////////////////
//
// QUIT THIS INFERNAL GAME!
//
////////////////////////////////////////////////////////////////////
int
CP_Quit (int)
{
    if (Confirm (endStrings[US_RndT () & 0x7 + (US_RndT () & 1)]))
    {
        VW_UpdateScreen ();
        SD_MusicOff ();
        SD_StopSound ();
        MenuFadeOut ();
        Quit (NULL);
    }

    DrawMainMenu ();
    return 0;
}


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//
// SUPPORT ROUTINES
//
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//
// Clear Menu screens to dark red
//
////////////////////////////////////////////////////////////////////
void
ClearMScreen (void)
{
    VWB_Bar (0, 0, 320, 200, BORDCOLOR);
}


////////////////////////////////////////////////////////////////////
//
// Un/Cache a LUMP of graphics
//
////////////////////////////////////////////////////////////////////
void
CacheLump (int lumpstart, int lumpend)
{
    int i;

    for (i = lumpstart; i <= lumpend; i++)
        CA_CacheGrChunk (i);
}


void
UnCacheLump (int lumpstart, int lumpend)
{
    int i;

    for (i = lumpstart; i <= lumpend; i++)
        if (grsegs[i])
            UNCACHEGRCHUNK (i);
}


////////////////////////////////////////////////////////////////////
//
// Draw a window for a menu
//
////////////////////////////////////////////////////////////////////
void
DrawWindow (int x, int y, int w, int h, int wcolor)
{
    VWB_Bar (x, y, w, h, wcolor);
    DrawOutline (x, y, w, h, BORD2COLOR, DEACTIVE);
}


void
DrawOutline (int x, int y, int w, int h, int color1, int color2)
{
    VWB_Hlin (x, x + w, y, color2);
    VWB_Vlin (y, y + h, x, color2);
    VWB_Hlin (x, x + w, y + h, color1);
    VWB_Vlin (y, y + h, x + w, color1);
}


////////////////////////////////////////////////////////////////////
//
// Setup Control Panel stuff - graphics, etc.
//
////////////////////////////////////////////////////////////////////
void
SetupControlPanel (void)
{
    //
    // CACHE GRAPHICS & SOUNDS
    //
    CA_CacheGrChunk (STARTFONT + 1);
    CacheLump (CONTROLS_LUMP_START, CONTROLS_LUMP_END);

    SETFONTCOLOR (TEXTCOLOR, BKGDCOLOR);
    fontnumber = 1;
    WindowH = 200;
}


////////////////////////////////////////////////////////////////////
//
// Clean up all the Control Panel stuff
//
////////////////////////////////////////////////////////////////////
void
CleanupControlPanel (void)
{
    UnCacheLump (CONTROLS_LUMP_START, CONTROLS_LUMP_END);

    fontnumber = 0;
}


////////////////////////////////////////////////////////////////////
//
// Handle moving gun around a menu
//
////////////////////////////////////////////////////////////////////
int
HandleMenu (CP_iteminfo * item_i, CP_itemtype * items, void (*routine) (int w))
{
    char key;
    int i, x, y, basey, exit, which, shape;
    int32_t lastBlinkTime, timer;
    ControlInfo ci;


    which = item_i->curpos;
    x = item_i->x & -8;
    basey = item_i->y - 2;
    y = basey + which * 13;

    VWB_DrawPic (x, y, C_CURSOR1PIC);
    SetTextColor (items + which, 1);
    PrintX = item_i->x + item_i->indent;
    PrintY = item_i->y + which * 13;
    US_Print ((items + which)->string);

    //
    // CALL CUSTOM ROUTINE IF IT IS NEEDED
    //
    if (routine)
        routine (which);
    VW_UpdateScreen ();

    shape = C_CURSOR1PIC;
    timer = 8;
    exit = 0;
    lastBlinkTime = GetTimeCount ();
    IN_ClearKeysDown ();


    do
    {
        //
        // CHANGE GUN SHAPE
        //
        if ((int32_t)GetTimeCount () - lastBlinkTime > timer)
        {
            lastBlinkTime = GetTimeCount ();
            if (shape == C_CURSOR1PIC)
            {
                shape = C_CURSOR2PIC;
                timer = 8;
            }
            else
            {
                shape = C_CURSOR1PIC;
                timer = 70;
            }
            VWB_DrawPic (x, y, shape);
            if (routine)
                routine (which);
            VW_UpdateScreen ();
        }
        else SDL_Delay(5);

        CheckPause ();

        //
        // SEE IF ANY KEYS ARE PRESSED FOR INITIAL CHAR FINDING
        //
        key = LastASCII;
        if (key)
        {
            int ok = 0;

            if (key >= 'a')
                key -= 'a' - 'A';

            for (i = which + 1; i < item_i->amount; i++)
                if ((items + i)->active && (items + i)->string[0] == key)
                {
                    EraseGun (item_i, items, x, y, which);
                    which = i;
                    DrawGun (item_i, items, x, &y, which, basey, routine);
                    ok = 1;
                    IN_ClearKeysDown ();
                    break;
                }

            //
            // DIDN'T FIND A MATCH FIRST TIME THRU. CHECK AGAIN.
            //
            if (!ok)
            {
                for (i = 0; i < which; i++)
                    if ((items + i)->active && (items + i)->string[0] == key)
                    {
                        EraseGun (item_i, items, x, y, which);
                        which = i;
                        DrawGun (item_i, items, x, &y, which, basey, routine);
                        IN_ClearKeysDown ();
                        break;
                    }
            }
        }

        //
        // GET INPUT
        //
        ReadAnyControl (&ci);
        switch (ci.dir)
        {
                ////////////////////////////////////////////////
                //
                // MOVE UP
                //
            case dir_North:

                EraseGun (item_i, items, x, y, which);

                //
                // ANIMATE HALF-STEP
                //
                if (which && (items + which - 1)->active)
                {
                    y -= 6;
                    DrawHalfStep (x, y);
                }

                //
                // MOVE TO NEXT AVAILABLE SPOT
                //
                do
                {
                    if (!which)
                        which = item_i->amount - 1;
                    else
                        which--;
                }
                while (!(items + which)->active);

                DrawGun (item_i, items, x, &y, which, basey, routine);
                //
                // WAIT FOR BUTTON-UP OR DELAY NEXT MOVE
                //
                TicDelay (20);
                break;

                ////////////////////////////////////////////////
                //
                // MOVE DOWN
                //
            case dir_South:

                EraseGun (item_i, items, x, y, which);
                //
                // ANIMATE HALF-STEP
                //
                if (which != item_i->amount - 1 && (items + which + 1)->active)
                {
                    y += 6;
                    DrawHalfStep (x, y);
                }

                do
                {
                    if (which == item_i->amount - 1)
                        which = 0;
                    else
                        which++;
                }
                while (!(items + which)->active);

                DrawGun (item_i, items, x, &y, which, basey, routine);

                //
                // WAIT FOR BUTTON-UP OR DELAY NEXT MOVE
                //
                TicDelay (20);
                break;
        }

        if (ci.button0 || Keyboard[sc_Space] || Keyboard[sc_Enter])
            exit = 1;

        if (ci.button1 && !Keyboard[sc_Alt] || Keyboard[sc_Escape])
            exit = 2;

    }
    while (!exit);


    IN_ClearKeysDown ();

    //
    // ERASE EVERYTHING
    //
    VWB_Bar (x - 1, y, 25, 16, BKGDCOLOR);
    PrintX = item_i->x + item_i->indent;
    PrintY = item_i->y + which * 13;
    US_Print ((items + which)->string);

    if (routine)
        routine (which);
    VW_UpdateScreen ();

    item_i->curpos = which;

    switch (exit)
    {
        case 1:
            //
            // CALL THE ROUTINE
            //
            if ((items + which)->routine != NULL)
            {
                ShootSnd ();
                MenuFadeOut ();
                (items + which)->routine (0);
            }
            return which;

        case 2:
            SD_PlaySound (ESCPRESSEDSND);
            return -1;
    }

    return 0;                   // JUST TO SHUT UP THE ERROR MESSAGES!
}


//
// ERASE GUN & DE-HIGHLIGHT STRING
//
void
EraseGun (CP_iteminfo * item_i, CP_itemtype * items, int x, int y, int which)
{
    VWB_Bar (x - 1, y, 25, 16, BKGDCOLOR);
    SetTextColor (items + which, 0);

    PrintX = item_i->x + item_i->indent;
    PrintY = item_i->y + which * 13;
    US_Print ((items + which)->string);
    VW_UpdateScreen ();
}


//
// DRAW HALF STEP OF GUN TO NEXT POSITION
//
void
DrawHalfStep (int x, int y)
{
    VWB_DrawPic (x, y, C_CURSOR1PIC);
    VW_UpdateScreen ();
    SD_PlaySound (MOVEGUN1SND);
    SDL_Delay (8 * 100 / 7);
}


//
// DRAW GUN AT NEW POSITION
//
void
DrawGun (CP_iteminfo * item_i, CP_itemtype * items, int x, int *y, int which, int basey,
         void (*routine) (int w))
{
    VWB_Bar (x - 1, *y, 25, 16, BKGDCOLOR);
    *y = basey + which * 13;
    VWB_DrawPic (x, *y, C_CURSOR1PIC);
    SetTextColor (items + which, 1);

    PrintX = item_i->x + item_i->indent;
    PrintY = item_i->y + which * 13;
    US_Print ((items + which)->string);

    //
    // CALL CUSTOM ROUTINE IF IT IS NEEDED
    //
    if (routine)
        routine (which);
    VW_UpdateScreen ();
    SD_PlaySound (MOVEGUN2SND);
}

////////////////////////////////////////////////////////////////////
//
// DELAY FOR AN AMOUNT OF TICS OR UNTIL CONTROLS ARE INACTIVE
//
////////////////////////////////////////////////////////////////////
void
TicDelay (int count)
{
    ControlInfo ci;

    int32_t startTime = GetTimeCount ();
    do
    {
        SDL_Delay(5);
        ReadAnyControl (&ci);
    }
    while ((int32_t) GetTimeCount () - startTime < count && ci.dir != dir_None);
}


////////////////////////////////////////////////////////////////////
//
// Draw a menu
//
////////////////////////////////////////////////////////////////////
void
DrawMenu (CP_iteminfo * item_i, CP_itemtype * items)
{
    int i, which = item_i->curpos;


    WindowX = PrintX = item_i->x + item_i->indent;
    WindowY = PrintY = item_i->y;
    WindowW = 320;
    WindowH = 200;

    for (i = 0; i < item_i->amount; i++)
    {
        SetTextColor (items + i, which == i);

        PrintY = item_i->y + i * 13;
        if ((items + i)->active)
            US_Print ((items + i)->string);
        else
        {
            SETFONTCOLOR (DEACTIVE, BKGDCOLOR);
            US_Print ((items + i)->string);
            SETFONTCOLOR (TEXTCOLOR, BKGDCOLOR);
        }

        US_Print ("\n");
    }
}


////////////////////////////////////////////////////////////////////
//
// SET TEXT COLOR (HIGHLIGHT OR NO)
//
////////////////////////////////////////////////////////////////////
void
SetTextColor (CP_itemtype * items, int hlight)
{
    if (hlight)
    {
        SETFONTCOLOR (color_hlite[items->active], BKGDCOLOR);
    }
    else
    {
        SETFONTCOLOR (color_norml[items->active], BKGDCOLOR);
    }
}


////////////////////////////////////////////////////////////////////
//
// WAIT FOR CTRLKEY-UP OR BUTTON-UP
//
////////////////////////////////////////////////////////////////////
void
WaitKeyUp (void)
{
    ControlInfo ci;
    while (ReadAnyControl (&ci), ci.button0 |
           ci.button1 |
           ci.button2 | ci.button3 | Keyboard[sc_Space] | Keyboard[sc_Enter] | Keyboard[sc_Escape])
    {
        IN_WaitAndProcessEvents();
    }
}


////////////////////////////////////////////////////////////////////
//
// READ KEYBOARD FOR INPUT
//
////////////////////////////////////////////////////////////////////
void
ReadAnyControl (ControlInfo * ci)
{
    IN_ReadControl (0, ci);
}


////////////////////////////////////////////////////////////////////
//
// DRAW DIALOG AND CONFIRM YES OR NO TO QUESTION
//
////////////////////////////////////////////////////////////////////
int
Confirm (const char *string)
{
    int xit = 0, x, y, tick = 0, lastBlinkTime;
    int whichsnd[2] = { ESCPRESSEDSND, SHOOTSND };
    ControlInfo ci;

    Message (string);
    IN_ClearKeysDown ();
    WaitKeyUp ();

    //
    // BLINK CURSOR
    //
    x = PrintX;
    y = PrintY;
    lastBlinkTime = GetTimeCount();

    do
    {
        ReadAnyControl(&ci);

        if (GetTimeCount() - lastBlinkTime >= 10)
        {
            switch (tick)
            {
                case 0:
                    VWB_Bar (x, y, 8, 13, TEXTCOLOR);
                    break;
                case 1:
                    PrintX = x;
                    PrintY = y;
                    US_Print ("_");
            }
            VW_UpdateScreen ();
            tick ^= 1;
            lastBlinkTime = GetTimeCount();
        }
        else SDL_Delay(5);
    }
    while (!Keyboard[sc_Y] && !Keyboard[sc_N] && !Keyboard[sc_Escape] && !ci.button0 && !ci.button1);

    if (Keyboard[sc_Y] || ci.button0)
    {
        xit = 1;
        ShootSnd ();
    }

    IN_ClearKeysDown ();
    WaitKeyUp ();

    SD_PlaySound ((soundnames) whichsnd[xit]);

    return xit;
}


////////////////////////////////////////////////////////////////////
//
// PRINT A MESSAGE IN A WINDOW
//
////////////////////////////////////////////////////////////////////
void
Message (const char *string)
{
    int h = 0, w = 0, mw = 0, i, len = (int) strlen(string);
    fontstruct *font;


    CA_CacheGrChunk (STARTFONT + 1);
    fontnumber = 1;
    font = (fontstruct *) grsegs[STARTFONT + fontnumber];
    h = font->height;
    for (i = 0; i < len; i++)
    {
        if (string[i] == '\n')
        {
            if (w > mw)
                mw = w;
            w = 0;
            h += font->height;
        }
        else
            w += font->width[string[i]];
    }

    if (w + 10 > mw)
        mw = w + 10;

    PrintY = (WindowH / 2) - h / 2;
    PrintX = WindowX = 160 - mw / 2;

    DrawWindow (WindowX - 5, PrintY - 5, mw + 10, h + 10, TEXTCOLOR);
    DrawOutline (WindowX - 5, PrintY - 5, mw + 10, h + 10, 0, HIGHLIGHT);
    SETFONTCOLOR (0, TEXTCOLOR);
    US_Print (string);
    VW_UpdateScreen ();
}

////////////////////////////////////////////////////////////////////
//
// THIS MAY BE FIXED A LITTLE LATER...
//
////////////////////////////////////////////////////////////////////
static int lastmusic;

void
StartCPMusic (int song)
{
    lastmusic = song;
    SD_MusicOff ();
    UNCACHEAUDIOCHUNK (STARTMUSIC + lastmusic);

    SD_StartMusic(STARTMUSIC + song);
}

void
FreeMusic (void)
{
    UNCACHEAUDIOCHUNK (STARTMUSIC + lastmusic);
}


///////////////////////////////////////////////////////////////////////////
//
//      IN_GetScanName() - Returns a string containing the name of the
//              specified scan code
//
///////////////////////////////////////////////////////////////////////////
const char *
IN_GetScanName (ScanCode scan)
{
/*    const char **p;
    ScanCode *s;

    for (s = ExtScanCodes, p = ExtScanNames; *s; p++, s++)
        if (*s == scan)
            return (*p);*/

    return (ScanNames[scan]);
}


///////////////////////////////////////////////////////////////////////////
//
// CHECK FOR PAUSE KEY (FOR MUSIC ONLY)
//
///////////////////////////////////////////////////////////////////////////
void
CheckPause (void)
{
    if (Paused)
    {
        switch (SoundStatus)
        {
            case 0:
                SD_MusicOn ();
                break;
            case 1:
                SD_MusicOff ();
                break;
        }

        SoundStatus ^= 1;
        VW_WaitVBL (3);
        IN_ClearKeysDown ();
        Paused = false;
    }
}


///////////////////////////////////////////////////////////////////////////
//
// DRAW GUN CURSOR AT CORRECT POSITION IN MENU
//
///////////////////////////////////////////////////////////////////////////
void
DrawMenuGun (CP_iteminfo * iteminfo)
{
    int x, y;


    x = iteminfo->x;
    y = iteminfo->y + iteminfo->curpos * 13 - 2;
    VWB_DrawPic (x, y, C_CURSOR1PIC);
}


///////////////////////////////////////////////////////////////////////////
//
// DRAW SCREEN TITLE STRIPES
//
///////////////////////////////////////////////////////////////////////////
void
DrawStripes (int y)
{
    VWB_Bar (0, y, 320, 24, 0);
    VWB_Hlin (0, 319, y + 22, STRIPE);
}

void
ShootSnd (void)
{
    SD_PlaySound (SHOOTSND);
}


///////////////////////////////////////////////////////////////////////////
//
// CHECK FOR EPISODES
//
///////////////////////////////////////////////////////////////////////////
void
CheckForEpisodes (void)
{
    struct stat statbuf;

    // On Linux like systems, the configdir defaults to $HOME/.wolf4sdl
    if(configdir[0] == 0)
    {
        // Set config location to home directory for multi-user support
        char *homedir = getenv("HOME");
        if(homedir == NULL)
        {
            Quit("Your $HOME directory is not defined. You must set this before playing.");
        }
        #define WOLFDIR "/.wolf4sdl"
        if(strlen(homedir) + sizeof(WOLFDIR) > sizeof(configdir))
        {
            Quit("Your $HOME directory path is too long. It cannot be used for saving games.");
        }
        snprintf(configdir, sizeof(configdir), "%s" WOLFDIR, homedir);
    }

    if(configdir[0] != 0)
    {
        // Ensure config directory exists and create if necessary
        if(stat(configdir, &statbuf) != 0)
        {
            if(mkdir(configdir, 0755) != 0)
            {
                Quit("The configuration directory \"%s\" could not be created.", configdir);
            }
        }
    }

    if(!stat("vswap.wl6", &statbuf))
        strcpy (extension, "wl6");
    else
        Quit ("NO WOLFENSTEIN 3-D DATA FILES to be found!");

    strcpy (graphext, extension);
    strcpy (audioext, extension);
    strcat (configname, extension);
    strcat (savename, extension);
}
