//
// WL_MENU.H
//
#define BORDCOLOR       0x29
#define BORD2COLOR      0x23
#define DEACTIVE        0x2b
#define BKGDCOLOR       0x2d
#define STRIPE          0x2c

#define MenuFadeOut()   VL_FadeOut(0,255,43,0,0,10)

#define READCOLOR       0x4a
#define READHCOLOR      0x47
#define VIEWCOLOR       0x7f
#define TEXTCOLOR       0x17
#define HIGHLIGHT       0x13
#define MenuFadeIn()    VL_FadeIn(0,255,gamepal,10)


#define MENUSONG        WONDERIN_MUS
#define INTROSONG       NAZI_NOR_MUS

#define SENSITIVE       60
#define CENTERX         ((int) screenWidth / 2)
#define CENTERY         ((int) screenHeight / 2)

#define MENU_X  76
#define MENU_Y  81
#define MENU_W  178
#define MENU_H  13*5+6

#define SM_X    48
#define SM_W    250

#define SM_Y1   20
#define SM_H1   3*13-7
#define SM_Y2   SM_Y1+4*13
#define SM_H2   3*13-7

#define NM_X    50
#define NM_Y    100
#define NM_W    225
#define NM_H    13*4+15

#define NE_X    10
#define NE_Y    23
#define NE_W    320-NE_X*2
#define NE_H    200-NE_Y*2

#define CST_X           8
#define CST_Y           48
#define CST_START       60
#define CST_SPC 60


//
// TYPEDEFS
//
typedef struct {
                short x,y,amount,curpos,indent;
                } CP_iteminfo;

typedef struct {
                short active;
                char string[36];
                int (* routine)(int temp1);
                } CP_itemtype;

typedef struct {
                short allowed[4];
                } CustomCtrls;

extern CP_itemtype MainMenu[];
extern CP_iteminfo MainItems;

//
// FUNCTION PROTOTYPES
//

void US_ControlPanel(ScanCode);

void EnableEndGameMenuItem();

void SetupControlPanel(void);
void CleanupControlPanel(void);

void DrawMenu(CP_iteminfo *item_i,CP_itemtype *items);
int  HandleMenu(CP_iteminfo *item_i,
                CP_itemtype *items,
                void (*routine)(int w));
void ClearMScreen(void);
void DrawWindow(int x,int y,int w,int h,int wcolor);
void DrawOutline(int x,int y,int w,int h,int color1,int color2);
void WaitKeyUp(void);
void ReadAnyControl(ControlInfo *ci);
void TicDelay(int count);
void CacheLump(int lumpstart,int lumpend);
void UnCacheLump(int lumpstart,int lumpend);
void StartCPMusic(int song);
int  Confirm(const char *string);
void Message(const char *string);
void CheckPause(void);
void ShootSnd(void);

void DrawGun(CP_iteminfo *item_i,CP_itemtype *items,int x,int *y,int which,int basey,void (*routine)(int w));
void DrawHalfStep(int x,int y);
void EraseGun(CP_iteminfo *item_i,CP_itemtype *items,int x,int y,int which);
void SetTextColor(CP_itemtype *items,int hlight);
void DrawMenuGun(CP_iteminfo *iteminfo);
void DrawStripes(int y);

void DefineKeyBtns(void);
void DefineKeyMove(void);
void EnterCtrlData(int index,CustomCtrls *cust,void (*DrawRtn)(int),void (*PrintRtn)(int),int type);

void DrawMainMenu(void);
void DrawNewEpisode(void);
void DrawNewGame(void);
void DrawControlScreen(void);
void DrawCustKeybd(int hilight);
void DrawCustKeys(int hilight);
void PrintCustKeybd(int i);
void PrintCustKeys(int i);

void DrawNewGameDiff(int w);
void FixupCustom(int w);

int CP_NewGame(int);
int CP_Quit(int);
int CP_ViewScores(int);
int CP_EndGame(int);
int CP_Control(int);

void CheckForEpisodes(void);

void FreeMusic(void);


enum {KEYBOARDBTNS,KEYBOARDMOVE};        // FOR INPUT TYPES

enum menuitems
{
        newgame,
        control,
        viewscores,
        backtodemo,
        quit
};

//
// WL_INTER
//
typedef struct {
                byte kill,secret,treasure;
                word time;
                } LRstruct;

extern LRstruct LevelRatios[];

void Write (int x,int y,const char *string);
int GetYorN(int x,int y,int pic);
