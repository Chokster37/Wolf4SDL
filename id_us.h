//
//	ID Engine
//	ID_US.h - Header file for the User Manager
//	v1.0d1
//	By Jason Blochowiak
//

#ifndef	__ID_US__
#define	__ID_US__

#ifdef	__DEBUG__
#define	__DEBUG_UserMgr__
#endif

//#define	HELPTEXTLINKED

#define	MaxHighName	57
#define	MaxScores	7
typedef	struct
{
    char	name[MaxHighName + 1];
    int32_t	score;
    word	completed,episode;
} HighScore;

#define	MaxString	128	// Maximum input string size

extern	boolean		ingame;		// Set by game code if a game is in progress
extern	word		PrintX,PrintY;	// Current printing location in the window
extern	word		WindowX,WindowY,// Current location of window
					WindowW,WindowH;// Current size of window

extern	void		(*USL_MeasureString)(const char *,word *,word *);
extern void			(*USL_DrawString)(const char *);

extern	HighScore	Scores[];

void            US_Startup(void);
void            US_Shutdown(void);
void			US_CPrint(const char *s),
				US_CPrintLine(const char *s),
				US_Print(const char *s);

boolean         US_LineInput(int x,int y,char *buf,const char *def,boolean escok,
                             int maxchars,int maxwidth);

void            US_InitRndT(int randomize);
int             US_RndT();

#endif
