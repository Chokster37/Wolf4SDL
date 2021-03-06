// WL_GAME.C

#include <math.h>
#include "wl_def.h"
#include <SDL_mixer.h>
#pragma hdrstop


/*
=============================================================================

                             LOCAL CONSTANTS

=============================================================================
*/


/*
=============================================================================

                             GLOBAL VARIABLES

=============================================================================
*/

boolean         ingame,fizzlein;
gametype        gamestate;
byte            bordercol=VIEWCOLOR;        // color of the Change View/Ingame border

short       faceframe;
short       attackframe,attackcount,weaponframe;
short       killcount,secretcount,treasurecount,
            killtotal,secrettotal,treasuretotal;
int32_t     TimeCount;
int32_t     killx,killy;
boolean     victoryflag;            // set during victory animations

//
// ELEVATOR BACK MAPS - REMEMBER (-1)!!
//
int ElevatorBackTo[]={1,1,7,3,5,3};

void SetupGameLevel (void);
void DrawPlayScreen (void);
void LoadLatchMem (void);
void GameLoop (void);

/*
=============================================================================

                             LOCAL VARIABLES

=============================================================================
*/


/*
==========================
=
= ScanInfoPlane
=
= Spawn all actors and mark down special places
=
==========================
*/

static void ScanInfoPlane(void)
{
    unsigned x,y;
    int      tile;
    word     *start;

    start = mapsegs[1];
    for (y=0;y<mapheight;y++)
    {
        for (x=0;x<mapwidth;x++)
        {
            tile = *start++;
            if (!tile)
                continue;

            switch (tile)
            {
                case 19:
                case 20:
                case 21:
                case 22:
                    SpawnPlayer(x,y,NORTH+tile-19);
                    break;

                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                case 28:
                case 29:
                case 30:

                case 31:
                case 32:
                case 33:
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:

                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:

                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:

                case 55:
                case 56:
                case 57:
                case 58:
                case 59:
                case 60:
                case 61:
                case 62:

                case 63:
                case 64:
                case 65:
                case 66:
                case 67:
                case 68:
                case 69:
                case 70:
                case 71:
                case 72:
                    SpawnStatic(x,y,tile-23);
                    break;

//
// P wall
//
                case 98:
                    secrettotal++;
                    break;

//
// guard
//
                case 180:
                case 181:
                case 182:
                case 183:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 144:
                case 145:
                case 146:
                case 147:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 108:
                case 109:
                case 110:
                case 111:
                    SpawnStand(en_guard,x,y,tile-108);
                    break;


                case 184:
                case 185:
                case 186:
                case 187:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 148:
                case 149:
                case 150:
                case 151:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 112:
                case 113:
                case 114:
                case 115:
                    SpawnPatrol(en_guard,x,y,tile-112);
                    break;

                case 124:
                    SpawnDeadGuard (x,y);
                    break;
//
// officer
//
                case 188:
                case 189:
                case 190:
                case 191:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 152:
                case 153:
                case 154:
                case 155:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 116:
                case 117:
                case 118:
                case 119:
                    SpawnStand(en_officer,x,y,tile-116);
                    break;


                case 192:
                case 193:
                case 194:
                case 195:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 156:
                case 157:
                case 158:
                case 159:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 120:
                case 121:
                case 122:
                case 123:
                    SpawnPatrol(en_officer,x,y,tile-120);
                    break;


//
// ss
//
                case 198:
                case 199:
                case 200:
                case 201:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 162:
                case 163:
                case 164:
                case 165:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 126:
                case 127:
                case 128:
                case 129:
                    SpawnStand(en_ss,x,y,tile-126);
                    break;


                case 202:
                case 203:
                case 204:
                case 205:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 166:
                case 167:
                case 168:
                case 169:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 130:
                case 131:
                case 132:
                case 133:
                    SpawnPatrol(en_ss,x,y,tile-130);
                    break;

//
// dogs
//
                case 206:
                case 207:
                case 208:
                case 209:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 170:
                case 171:
                case 172:
                case 173:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 134:
                case 135:
                case 136:
                case 137:
                    SpawnStand(en_dog,x,y,tile-134);
                    break;


                case 210:
                case 211:
                case 212:
                case 213:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 36;
                case 174:
                case 175:
                case 176:
                case 177:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 36;
                case 138:
                case 139:
                case 140:
                case 141:
                    SpawnPatrol(en_dog,x,y,tile-138);
                    break;

//
// boss
//
                case 214:
                    SpawnBoss (x,y);
                    break;
                case 197:
                    SpawnGretel (x,y);
                    break;
                case 215:
                    SpawnGift (x,y);
                    break;
                case 179:
                    SpawnFat (x,y);
                    break;
                case 196:
                    SpawnSchabbs (x,y);
                    break;
                case 160:
                    SpawnFakeHitler (x,y);
                    break;
                case 178:
                    SpawnHitler (x,y);
                    break;

//
// mutants
//
                case 252:
                case 253:
                case 254:
                case 255:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 18;
                case 234:
                case 235:
                case 236:
                case 237:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 18;
                case 216:
                case 217:
                case 218:
                case 219:
                    SpawnStand(en_mutant,x,y,tile-216);
                    break;

                case 256:
                case 257:
                case 258:
                case 259:
                    if (gamestate.difficulty<gd_hard)
                        break;
                    tile -= 18;
                case 238:
                case 239:
                case 240:
                case 241:
                    if (gamestate.difficulty<gd_medium)
                        break;
                    tile -= 18;
                case 220:
                case 221:
                case 222:
                case 223:
                    SpawnPatrol(en_mutant,x,y,tile-220);
                    break;

//
// ghosts
//
                case 224:
                    SpawnGhosts (en_blinky,x,y);
                    break;
                case 225:
                    SpawnGhosts (en_clyde,x,y);
                    break;
                case 226:
                    SpawnGhosts (en_pinky,x,y);
                    break;
                case 227:
                    SpawnGhosts (en_inky,x,y);
                    break;
            }
        }
    }
}

//==========================================================================

/*
==================
=
= SetupGameLevel
=
==================
*/

void SetupGameLevel (void)
{
    int  x,y;
    word *map;
    word tile;


    TimeCount
            = secrettotal
            = killtotal
            = treasuretotal
            = secretcount
            = killcount
            = treasurecount
            = pwallstate = pwallpos = 0;
        LastAttacker = NULL;
        killerobj = NULL;

    if (demoplayback)
        US_InitRndT (false);
    else
        US_InitRndT (true);

//
// load the level
//
    CA_CacheMap (gamestate.mapon+10*gamestate.episode);
    mapon-=gamestate.episode*10;

//
// copy the wall data to a data segment array
//
    memset (tilemap,0,sizeof(tilemap));
    memset (actorat,0,sizeof(actorat));
    map = mapsegs[0];
    for (y=0;y<mapheight;y++)
    {
        for (x=0;x<mapwidth;x++)
        {
            tile = *map++;
            if (tile<AMBUSHTILE)
            {
                // solid wall
                tilemap[x][y] = (byte) tile;
                actorat[x][y] = (objtype *)(uintptr_t) tile;
            }
        }
    }

//
// spawn doors
//
    InitActorList ();                       // start spawning things with a clean slate
    InitDoorList ();
    InitStaticList ();

    map = mapsegs[0];
    for (y=0;y<mapheight;y++)
    {
        for (x=0;x<mapwidth;x++)
        {
            tile = *map++;
            if (tile >= 90 && tile <= 101)
            {
                // door
                switch (tile)
                {
                    case 90:
                    case 92:
                    case 94:
                    case 96:
                    case 98:
                    case 100:
                        SpawnDoor (x,y,1,(tile-90)/2);
                        break;
                    case 91:
                    case 93:
                    case 95:
                    case 97:
                    case 99:
                    case 101:
                        SpawnDoor (x,y,0,(tile-91)/2);
                        break;
                }
            }
        }
    }

//
// spawn actors
//
    ScanInfoPlane ();


//
// take out the ambush markers
//
    map = mapsegs[0];
    for (y=0;y<mapheight;y++)
    {
        for (x=0;x<mapwidth;x++)
        {
            tile = *map++;
            if (tile == AMBUSHTILE)
            {
                if (*map >= AREATILE)
                    tile = *map;
                if (*(map-1-mapwidth) >= AREATILE)
                    tile = *(map-1-mapwidth);
                if (*(map-1+mapwidth) >= AREATILE)
                    tile = *(map-1+mapwidth);
                if ( *(map-2) >= AREATILE)
                    tile = *(map-2);

                *(map-1) = tile;
            }
        }
    }
}


//==========================================================================


/*
===================
=
= DrawPlayBorder
=
===================
*/

void DrawPlayBorder (void)
{
	const int px = scaleFactor; // size of one "pixel"
    const int statusborderw = (screenWidth-px*320)/2;
    VWB_BarScaledCoord (0, screenHeight-px*STATUSLINES,
        statusborderw+px*8, px*STATUSLINES, bordercol);
    VWB_BarScaledCoord (screenWidth-statusborderw-px*8, screenHeight-px*STATUSLINES,
        statusborderw+px*8, px*STATUSLINES, bordercol);

    if((unsigned) viewheight == screenHeight) return;

    VWB_BarScaledCoord (0,0,screenWidth,screenHeight-px*STATUSLINES,bordercol);

    const int xl = screenWidth/2-viewwidth/2;
    const int yl = (screenHeight-px*STATUSLINES-viewheight)/2;
    VWB_BarScaledCoord (xl,yl,viewwidth,viewheight,0);

    if(xl != 0)
    {
        // Paint game view border lines
        VWB_BarScaledCoord(xl-px, yl-px, viewwidth+px, px, 0);                      // upper border
        VWB_BarScaledCoord(xl, yl+viewheight, viewwidth+px, px, bordercol-2);       // lower border
        VWB_BarScaledCoord(xl-px, yl-px, px, viewheight+px, 0);                     // left border
        VWB_BarScaledCoord(xl+viewwidth, yl-px, px, viewheight+2*px, bordercol-2);  // right border
        VWB_BarScaledCoord(xl-px, yl+viewheight, px, px, bordercol-3);              // lower left highlight
    }
    else
    {
        // Just paint a lower border line
        VWB_BarScaledCoord(0, yl+viewheight, viewwidth, px, bordercol-2);       // lower border
    }
}


/*
===================
=
= DrawPlayScreen
=
===================
*/

void DrawPlayScreen (void)
{
    VWB_DrawPicScaledCoord ((screenWidth-scaleFactor*320)/2,screenHeight-scaleFactor*STATUSLINES,STATUSBARPIC);
    DrawPlayBorder ();

    DrawFace ();
    DrawHealth ();
    DrawLives ();
    DrawLevel ();
    DrawAmmo ();
    DrawKeys ();
    DrawWeapon ();
    DrawScore ();
}


//==========================================================================

/*
==================
=
= PlayDemo
=
= Fades the screen out, then starts a demo.  Exits with the screen unfaded
=
==================
*/

void PlayDemo (int demonumber)
{
    int length;
    int dems[4]={T_DEMO0,T_DEMO1,T_DEMO2,T_DEMO3};

    CA_CacheGrChunk(dems[demonumber]);
    demoptr = (int8_t *) grsegs[dems[demonumber]];

    NewGame (1,0);
    gamestate.mapon = *demoptr++;
    gamestate.difficulty = gd_hard;
    length = READWORD(*(uint8_t **)&demoptr);
    // TODO: Seems like the original demo format supports 16 MB demos
    //       But T_DEM00 and T_DEM01 of Wolf have a 0xd8 as third length size...
    demoptr++;
    lastdemoptr = demoptr-4+length;

    VW_FadeOut ();

    SETFONTCOLOR(0,15);
    DrawPlayScreen ();

    startgame = false;
    demoplayback = true;

    SetupGameLevel ();
    StartMusic ();

    PlayLoop ();

    UNCACHEGRCHUNK(dems[demonumber]);

    demoplayback = false;

    StopMusic ();
    ClearMemory ();
}

//==========================================================================

/*
==================
=
= Died
=
==================
*/

#define DEATHROTATE 2

void Died (void)
{
    float   fangle;
    int32_t dx,dy;
    int     iangle,curangle,clockwise,counter,change;

    if (screenfaded)
    {
        ThreeDRefresh ();
        VW_FadeIn ();
    }

    gamestate.weapon = (weapontype) -1;                     // take away weapon
    SD_PlaySound (PLAYERDEATHSND);

    //
    // swing around to face attacker
    //
    if(killerobj)
    {
        dx = killerobj->x - player->x;
        dy = player->y - killerobj->y;

        fangle = (float) atan2((float) dy, (float) dx);     // returns -pi to pi
        if (fangle<0)
            fangle = (float) (M_PI*2+fangle);

        iangle = (int) (fangle/(M_PI*2)*ANGLES);
    }
    else
    {
        iangle = player->angle + ANGLES / 2;
        if(iangle >= ANGLES) iangle -= ANGLES;
    }

    if (player->angle > iangle)
    {
        counter = player->angle - iangle;
        clockwise = ANGLES-player->angle + iangle;
    }
    else
    {
        clockwise = iangle - player->angle;
        counter = player->angle + ANGLES-iangle;
    }

    curangle = player->angle;

    if (clockwise<counter)
    {
        //
        // rotate clockwise
        //
        if (curangle>iangle)
            curangle -= ANGLES;
        do
        {
            change = tics*DEATHROTATE;
            if (curangle + change > iangle)
                change = iangle-curangle;

            curangle += change;
            player->angle += change;
            if (player->angle >= ANGLES)
                player->angle -= ANGLES;

            ThreeDRefresh ();
            CalcTics ();
        } while (curangle != iangle);
    }
    else
    {
        //
        // rotate counterclockwise
        //
        if (curangle<iangle)
            curangle += ANGLES;
        do
        {
            change = -(int)tics*DEATHROTATE;
            if (curangle + change < iangle)
                change = iangle-curangle;

            curangle += change;
            player->angle += change;
            if (player->angle < 0)
                player->angle += ANGLES;

            ThreeDRefresh ();
            CalcTics ();
        } while (curangle != iangle);
    }

    //
    // fade to red
    //
    FinishPaletteShifts ();

    if(usedoublebuffering) VH_UpdateScreen();

    VL_BarScaledCoord (viewscreenx,viewscreeny,viewwidth,viewheight,4);

    IN_ClearKeysDown ();

    FizzleFade(screenBuffer,viewscreenx,viewscreeny,viewwidth,viewheight,70,false);

    IN_UserInput(100);
    SD_WaitSoundDone ();
    ClearMemory();

    gamestate.lives--;

    if (gamestate.lives > -1)
    {
        gamestate.health = 100;
        gamestate.weapon = gamestate.bestweapon
            = gamestate.chosenweapon = wp_pistol;
        gamestate.ammo = STARTAMMO;
        gamestate.keys = 0;
        pwallstate = pwallpos = 0;
        attackframe = attackcount = weaponframe = 0;

        DrawKeys ();
        DrawWeapon ();
        DrawAmmo ();
        DrawHealth ();
        DrawFace ();
        DrawLives ();
    }
}

//==========================================================================

/*
===================
=
= GameLoop
=
===================
*/

void GameLoop (void)
{
    boolean died;

restartgame:
    ClearMemory ();
    SETFONTCOLOR(0,15);
    VW_FadeOut();
    DrawPlayScreen ();
    died = false;
    do
    {
        gamestate.score = gamestate.oldscore;

        DrawScore();

        startgame = false;
        SetupGameLevel ();

        DrawLevel ();

        ingame = true;
        StartMusic ();

        if (!died)
            PreloadGraphics ();             // TODO: Let this do something useful!
        else
        {
            died = false;
            fizzlein = true;
        }

        PlayLoop ();

        StopMusic ();
        SD_StopSound ();
        ingame = false;

        if (startgame)
            goto restartgame;

        switch (playstate)
        {
            case ex_completed:
            case ex_secretlevel:
                gamestate.keys = 0;
                DrawKeys ();
                VW_FadeOut ();

                ClearMemory ();

                LevelCompleted ();              // do the intermission

                gamestate.oldscore = gamestate.score;

                //
                // COMING BACK FROM SECRET LEVEL
                //
                if (gamestate.mapon == 9)
                    gamestate.mapon = ElevatorBackTo[gamestate.episode];    // back from secret
                else
                    //
                    // GOING TO SECRET LEVEL
                    //
                    if (playstate == ex_secretlevel)
                        gamestate.mapon = 9;
                    else
                        //
                        // GOING TO NEXT LEVEL
                        //
                        gamestate.mapon++;

                UpdateLevelRestore (true);
                break;

            case ex_died:
                Died ();
                died = true;                    // don't "get psyched!"

                if (gamestate.lives > -1)
                    break;                          // more lives left

                VW_FadeOut ();
                UpdateLevelRestore (false);

                ClearMemory ();

                CheckHighScore (gamestate.score,gamestate.mapon+1);
                strcpy(MainMenu[viewscores].string,STR_VS);
                MainMenu[viewscores].routine = CP_ViewScores;
                return;

            case ex_victorious:
                VW_FadeOut ();

                ClearMemory ();

                Victory ();
                UpdateLevelRestore (false);

                CheckHighScore (gamestate.score,gamestate.mapon+1);
                strcpy(MainMenu[viewscores].string,STR_VS);
                MainMenu[viewscores].routine = CP_ViewScores;
                return;

            default:
                ClearMemory ();
                break;
        }
    } while (1);
}
