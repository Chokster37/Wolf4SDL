// WL_PLAY.C

#include "wl_def.h"
#pragma hdrstop

/*
=============================================================================

                                                 LOCAL CONSTANTS

=============================================================================
*/

#define sc_Question     0x35

/*
=============================================================================

                                                 GLOBAL VARIABLES

=============================================================================
*/

boolean madenoise;              // true when shooting or screaming

exit_t playstate;

static musicnames lastmusicchunk = (musicnames) 0;

static int DebugOk;

objtype objlist[MAXACTORS];
objtype *newobj, *obj, *player, *lastobj, *objfreelist, *killerobj;

int extravbls = 0;

byte tilemap[MAPSIZE][MAPSIZE]; // wall values only
byte spotvis[MAPSIZE][MAPSIZE];
objtype *actorat[MAPSIZE][MAPSIZE];

//
// replacing refresh manager
//
unsigned tics;

//
// control info
//
int dirscan[4] = { sc_UpArrow, sc_RightArrow, sc_DownArrow, sc_LeftArrow };
int buttonscan[NUMBUTTONS] = { sc_Control, sc_Alt, sc_LShift, sc_Space, sc_1, sc_2, sc_3, sc_4 };
int viewsize;

boolean buttonheld[NUMBUTTONS];

boolean demoplayback;
int8_t *demoptr, *lastdemoptr;
memptr demobuffer;

//
// current user input
//
int controlx, controly;         // range from -100 to 100 per tic
boolean buttonstate[NUMBUTTONS];


//===========================================================================


void InitObjList (void);
void RemoveObj (objtype * gone);
void PollControls (void);
void StopMusic (void);
void StartMusic (void);
void PlayLoop (void);

/*
=============================================================================

                                                 LOCAL VARIABLES

=============================================================================
*/


//
// LIST OF SONGS FOR EACH VERSION
//
int songs[] = {
#ifndef SPEAR
    //
    // Episode One
    //
    GETTHEM_MUS,
    SEARCHN_MUS,
    POW_MUS,
    SUSPENSE_MUS,
    GETTHEM_MUS,
    SEARCHN_MUS,
    POW_MUS,
    SUSPENSE_MUS,

    WARMARCH_MUS,               // Boss level
    CORNER_MUS,                 // Secret level

    //
    // Episode Two
    //
    NAZI_OMI_MUS,
    PREGNANT_MUS,
    GOINGAFT_MUS,
    HEADACHE_MUS,
    NAZI_OMI_MUS,
    PREGNANT_MUS,
    HEADACHE_MUS,
    GOINGAFT_MUS,

    WARMARCH_MUS,               // Boss level
    DUNGEON_MUS,                // Secret level

    //
    // Episode Three
    //
    INTROCW3_MUS,
    NAZI_RAP_MUS,
    TWELFTH_MUS,
    ZEROHOUR_MUS,
    INTROCW3_MUS,
    NAZI_RAP_MUS,
    TWELFTH_MUS,
    ZEROHOUR_MUS,

    ULTIMATE_MUS,               // Boss level
    PACMAN_MUS,                 // Secret level

    //
    // Episode Four
    //
    GETTHEM_MUS,
    SEARCHN_MUS,
    POW_MUS,
    SUSPENSE_MUS,
    GETTHEM_MUS,
    SEARCHN_MUS,
    POW_MUS,
    SUSPENSE_MUS,

    WARMARCH_MUS,               // Boss level
    CORNER_MUS,                 // Secret level

    //
    // Episode Five
    //
    NAZI_OMI_MUS,
    PREGNANT_MUS,
    GOINGAFT_MUS,
    HEADACHE_MUS,
    NAZI_OMI_MUS,
    PREGNANT_MUS,
    HEADACHE_MUS,
    GOINGAFT_MUS,

    WARMARCH_MUS,               // Boss level
    DUNGEON_MUS,                // Secret level

    //
    // Episode Six
    //
    INTROCW3_MUS,
    NAZI_RAP_MUS,
    TWELFTH_MUS,
    ZEROHOUR_MUS,
    INTROCW3_MUS,
    NAZI_RAP_MUS,
    TWELFTH_MUS,
    ZEROHOUR_MUS,

    ULTIMATE_MUS,               // Boss level
    FUNKYOU_MUS                 // Secret level
#else

    //////////////////////////////////////////////////////////////
    //
    // SPEAR OF DESTINY TRACKS
    //
    //////////////////////////////////////////////////////////////
    XTIPTOE_MUS,
    XFUNKIE_MUS,
    XDEATH_MUS,
    XGETYOU_MUS,                // DON'T KNOW
    ULTIMATE_MUS,               // Trans Grösse

    DUNGEON_MUS,
    GOINGAFT_MUS,
    POW_MUS,
    TWELFTH_MUS,
    ULTIMATE_MUS,               // Barnacle Wilhelm BOSS

    NAZI_OMI_MUS,
    GETTHEM_MUS,
    SUSPENSE_MUS,
    SEARCHN_MUS,
    ZEROHOUR_MUS,
    ULTIMATE_MUS,               // Super Mutant BOSS

    XPUTIT_MUS,
    ULTIMATE_MUS,               // Death Knight BOSS

    XJAZNAZI_MUS,               // Secret level
    XFUNKIE_MUS,                // Secret level (DON'T KNOW)

    XEVIL_MUS                   // Angel of Death BOSS
#endif
};


/*
=============================================================================

                               USER CONTROL

=============================================================================
*/

/*
===================
=
= PollKeyboardButtons
=
===================
*/

void PollKeyboardButtons (void)
{
    int i;

    for (i = 0; i < NUMBUTTONS; i++)
        if (Keyboard[buttonscan[i]])
            buttonstate[i] = true;
}


/*
===================
=
= PollKeyboardMove
=
===================
*/

void PollKeyboardMove (void)
{
    int delta = buttonstate[bt_run] ? RUNMOVE * tics : BASEMOVE * tics;

    if (Keyboard[dirscan[di_north]])
        controly -= delta;
    if (Keyboard[dirscan[di_south]])
        controly += delta;
    if (Keyboard[dirscan[di_west]])
        controlx -= delta;
    if (Keyboard[dirscan[di_east]])
        controlx += delta;
}


/*
===================
=
= PollControls
=
= Gets user or demo input, call once each frame
=
= controlx              set between -100 and 100 per tic
= controly
= buttonheld[]  the state of the buttons LAST frame
= buttonstate[] the state of the buttons THIS frame
=
===================
*/

void PollControls (void)
{
    int max, min, i;
    byte buttonbits;

    IN_ProcessEvents();

//
// get timing info for last frame
//
    if (demoplayback)   // demo recording and playback needs to be constant
    {
        // wait up to DEMOTICS Wolf tics
        uint32_t curtime = SDL_GetTicks();
        lasttimecount += DEMOTICS;
        int32_t timediff = (lasttimecount * 100) / 7 - curtime;
        if(timediff > 0)
            SDL_Delay(timediff);

        if(timediff < -2 * DEMOTICS)       // more than 2-times DEMOTICS behind?
            lasttimecount = (curtime * 7) / 100;    // yes, set to current timecount

        tics = DEMOTICS;
    }
    else
        CalcTics ();

    controlx = 0;
    controly = 0;
    memcpy (buttonheld, buttonstate, sizeof (buttonstate));
    memset (buttonstate, 0, sizeof (buttonstate));

    if (demoplayback)
    {
        //
        // read commands from demo buffer
        //
        buttonbits = *demoptr++;
        for (i = 0; i < NUMBUTTONS; i++)
        {
            buttonstate[i] = buttonbits & 1;
            buttonbits >>= 1;
        }

        controlx = *demoptr++;
        controly = *demoptr++;

        if (demoptr == lastdemoptr)
            playstate = ex_completed;   // demo is done

        controlx *= (int) tics;
        controly *= (int) tics;

        return;
    }


//
// get button states
//
    PollKeyboardButtons ();

//
// get movements
//
    PollKeyboardMove ();

//
// bound movement to a maximum
//
    max = 100 * tics;
    min = -max;
    if (controlx > max)
        controlx = max;
    else if (controlx < min)
        controlx = min;

    if (controly > max)
        controly = max;
    else if (controly < min)
        controly = min;
}



//===========================================================================


/*
=====================
=
= CheckKeys
=
=====================
*/

void CheckKeys (void)
{
    ScanCode scan;


    if (screenfaded || demoplayback)    // don't do anything with a faded screen
        return;

    scan = LastScan;

    if (scan == sc_Escape || buttonstate[bt_esc])
    {
        StopMusic ();
        ClearMemory ();
        VW_FadeOut ();

        US_ControlPanel (buttonstate[bt_esc] ? sc_Escape : scan);

        SETFONTCOLOR (0, 15);
        IN_ClearKeysDown ();
        VW_FadeOut();
        DrawPlayScreen ();
        if (!startgame && !loadedgame)
            StartMusic ();
        if (loadedgame)
            playstate = ex_abort;
        lasttimecount = GetTimeCount();
        return;
    }
}


//===========================================================================

/*
#############################################################################

                                  The objlist data structure

#############################################################################

objlist containt structures for every actor currently playing.  The structure
is accessed as a linked list starting at *player, ending when ob->next ==
NULL.  GetNewObj inserts a new object at the end of the list, meaning that
if an actor spawn another actor, the new one WILL get to think and react the
same frame.  RemoveObj unlinks the given object and returns it to the free
list, but does not damage the objects ->next pointer, so if the current object
removes itself, a linked list following loop can still safely get to the
next element.

<backwardly linked free list>

#############################################################################
*/


/*
=========================
=
= InitActorList
=
= Call to clear out the actor object lists returning them all to the free
= list.  Allocates a special spot for the player.
=
=========================
*/

void InitActorList (void)
{
    int i;

//
// init the actor lists
//
    for (i = 0; i < MAXACTORS; i++)
    {
        objlist[i].prev = &objlist[i + 1];
        objlist[i].next = NULL;
    }

    objlist[MAXACTORS - 1].prev = NULL;

    objfreelist = &objlist[0];
    lastobj = NULL;

//
// give the player the first free spots
//
    GetNewActor ();
    player = newobj;

}

//===========================================================================

/*
=========================
=
= GetNewActor
=
= Sets the global variable new to point to a free spot in objlist.
= The free spot is inserted at the end of the liked list
=
= When the object list is full, the caller can either have it bomb out ot
= return a dummy object pointer that will never get used
=
=========================
*/

void GetNewActor (void)
{
    if (!objfreelist)
        Quit ("GetNewActor: No free spots in objlist!");

    newobj = objfreelist;
    objfreelist = newobj->prev;
    memset (newobj, 0, sizeof (*newobj));

    if (lastobj)
        lastobj->next = newobj;
    newobj->prev = lastobj;     // new->next is allready NULL from memset

    newobj->active = ac_no;
    lastobj = newobj;
}

//===========================================================================

/*
=========================
=
= RemoveObj
=
= Add the given object back into the free list, and unlink it from it's
= neighbors
=
=========================
*/

void RemoveObj (objtype * gone)
{
    if (gone == player)
        Quit ("RemoveObj: Tried to remove the player!");

    gone->state = NULL;

//
// fix the next object's back link
//
    if (gone == lastobj)
        lastobj = (objtype *) gone->prev;
    else
        gone->next->prev = gone->prev;

//
// fix the previous object's forward link
//
    gone->prev->next = gone->next;

//
// add it back in to the free list
//
    gone->prev = objfreelist;
    objfreelist = gone;
}

/*
=============================================================================

                                                MUSIC STUFF

=============================================================================
*/


/*
=================
=
= StopMusic
=
=================
*/
void StopMusic (void)
{
    SD_MusicOff ();

    UNCACHEAUDIOCHUNK (STARTMUSIC + lastmusicchunk);
}

//==========================================================================


/*
=================
=
= StartMusic
=
=================
*/

void StartMusic ()
{
    SD_MusicOff ();
    lastmusicchunk = (musicnames) songs[gamestate.mapon + gamestate.episode * 10];
    SD_StartMusic(STARTMUSIC + lastmusicchunk);
}

/*
=============================================================================

                                        PALETTE SHIFTING STUFF

=============================================================================
*/

#define NUMREDSHIFTS    6
#define REDSTEPS        8

#define NUMWHITESHIFTS  3
#define WHITESTEPS      20
#define WHITETICS       6


SDL_Color redshifts[NUMREDSHIFTS][256];
SDL_Color whiteshifts[NUMWHITESHIFTS][256];

int damagecount, bonuscount;
boolean palshifted;

/*
=====================
=
= InitRedShifts
=
=====================
*/

void InitRedShifts (void)
{
    SDL_Color *workptr, *baseptr;
    int i, j, delta;


//
// fade through intermediate frames
//
    for (i = 1; i <= NUMREDSHIFTS; i++)
    {
        workptr = redshifts[i - 1];
        baseptr = gamepal;

        for (j = 0; j <= 255; j++)
        {
            delta = 256 - baseptr->r;
            workptr->r = baseptr->r + delta * i / REDSTEPS;
            delta = -baseptr->g;
            workptr->g = baseptr->g + delta * i / REDSTEPS;
            delta = -baseptr->b;
            workptr->b = baseptr->b + delta * i / REDSTEPS;
            baseptr++;
            workptr++;
        }
    }

    for (i = 1; i <= NUMWHITESHIFTS; i++)
    {
        workptr = whiteshifts[i - 1];
        baseptr = gamepal;

        for (j = 0; j <= 255; j++)
        {
            delta = 256 - baseptr->r;
            workptr->r = baseptr->r + delta * i / WHITESTEPS;
            delta = 248 - baseptr->g;
            workptr->g = baseptr->g + delta * i / WHITESTEPS;
            delta = 0-baseptr->b;
            workptr->b = baseptr->b + delta * i / WHITESTEPS;
            baseptr++;
            workptr++;
        }
    }
}


/*
=====================
=
= ClearPaletteShifts
=
=====================
*/

void ClearPaletteShifts (void)
{
    bonuscount = damagecount = 0;
    palshifted = false;
}


/*
=====================
=
= StartBonusFlash
=
=====================
*/

void StartBonusFlash (void)
{
    bonuscount = NUMWHITESHIFTS * WHITETICS;    // white shift palette
}


/*
=====================
=
= StartDamageFlash
=
=====================
*/

void StartDamageFlash (int damage)
{
    damagecount += damage;
}


/*
=====================
=
= UpdatePaletteShifts
=
=====================
*/

void UpdatePaletteShifts (void)
{
    int red, white;

    if (bonuscount)
    {
        white = bonuscount / WHITETICS + 1;
        if (white > NUMWHITESHIFTS)
            white = NUMWHITESHIFTS;
        bonuscount -= tics;
        if (bonuscount < 0)
            bonuscount = 0;
    }
    else
        white = 0;


    if (damagecount)
    {
        red = damagecount / 10 + 1;
        if (red > NUMREDSHIFTS)
            red = NUMREDSHIFTS;

        damagecount -= tics;
        if (damagecount < 0)
            damagecount = 0;
    }
    else
        red = 0;

    if (red)
    {
        VL_SetPalette (redshifts[red - 1], false);
        palshifted = true;
    }
    else if (white)
    {
        VL_SetPalette (whiteshifts[white - 1], false);
        palshifted = true;
    }
    else if (palshifted)
    {
        VL_SetPalette (gamepal, false);        // back to normal
        palshifted = false;
    }
}


/*
=====================
=
= FinishPaletteShifts
=
= Resets palette to normal if needed
=
=====================
*/

void FinishPaletteShifts (void)
{
    if (palshifted)
    {
        palshifted = 0;
        VL_SetPalette (gamepal, true);
    }
}


/*
=============================================================================

                                                CORE PLAYLOOP

=============================================================================
*/


/*
=====================
=
= DoActor
=
=====================
*/

void DoActor (objtype * ob)
{
    void (*think) (objtype *);

    if (!ob->active && ob->areanumber < NUMAREAS && !areabyplayer[ob->areanumber])
        return;

    if (!(ob->flags & (FL_NONMARK | FL_NEVERMARK)))
        actorat[ob->tilex][ob->tiley] = NULL;

//
// non transitional object
//

    if (!ob->ticcount)
    {
        think = (void (*)(objtype *)) ob->state->think;
        if (think)
        {
            think (ob);
            if (!ob->state)
            {
                RemoveObj (ob);
                return;
            }
        }

        if (ob->flags & FL_NEVERMARK)
            return;

        if ((ob->flags & FL_NONMARK) && actorat[ob->tilex][ob->tiley])
            return;

        actorat[ob->tilex][ob->tiley] = ob;
        return;
    }

//
// transitional object
//
    ob->ticcount -= (short) tics;
    while (ob->ticcount <= 0)
    {
        think = (void (*)(objtype *)) ob->state->action;        // end of state action
        if (think)
        {
            think (ob);
            if (!ob->state)
            {
                RemoveObj (ob);
                return;
            }
        }

        ob->state = ob->state->next;

        if (!ob->state)
        {
            RemoveObj (ob);
            return;
        }

        if (!ob->state->tictime)
        {
            ob->ticcount = 0;
            goto think;
        }

        ob->ticcount += ob->state->tictime;
    }

think:
    //
    // think
    //
    think = (void (*)(objtype *)) ob->state->think;
    if (think)
    {
        think (ob);
        if (!ob->state)
        {
            RemoveObj (ob);
            return;
        }
    }

    if (ob->flags & FL_NEVERMARK)
        return;

    if ((ob->flags & FL_NONMARK) && actorat[ob->tilex][ob->tiley])
        return;

    actorat[ob->tilex][ob->tiley] = ob;
}

//==========================================================================


/*
===================
=
= PlayLoop
=
===================
*/
#ifdef SPEAR
int32_t funnyticount;
#endif

void PlayLoop (void)
{
    playstate = ex_stillplaying;
    lasttimecount = GetTimeCount();
    anglefrac = 0;
    facecount = 0;
#ifdef SPEAR
    funnyticount = 0;
#endif
    memset (buttonstate, 0, sizeof (buttonstate));
    ClearPaletteShifts ();

    if (demoplayback)
        IN_StartAck ();

    do
    {
        PollControls ();

//
// actor thinking
//
        madenoise = false;

        MoveDoors ();
        MovePWalls ();

        for (obj = player; obj; obj = obj->next)
            DoActor (obj);

        UpdatePaletteShifts ();

        ThreeDRefresh ();

        //
        // MAKE FUNNY FACE IF BJ DOESN'T MOVE FOR AWHILE
        //
#ifdef SPEAR
        funnyticount += tics;
        if (funnyticount > 30l * 70)
        {
            funnyticount = 0;
            StatusDrawFace(BJWAITING1PIC + (US_RndT () & 1));
            facecount = 0;
        }
#endif

        gamestate.TimeCount += tics;

        UpdateSoundLoc ();      // JAB
        if (screenfaded)
            VW_FadeIn ();

        CheckKeys ();

        if (extravbls)
            VW_WaitVBL (extravbls);

        if (demoplayback)
        {
            if (IN_CheckAck ())
            {
                IN_ClearKeysDown ();
                playstate = ex_abort;
            }
        }
    }
    while (!playstate && !startgame);

    if (playstate != ex_died)
        FinishPaletteShifts ();
}
