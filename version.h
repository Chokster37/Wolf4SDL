#ifndef _VERSION_H_
#define _VERSION_H_

#ifndef VERSIONALREADYCHOSEN              // used for batch compiling

/* Defines used for different versions */

#define GOODTIMES
#define CARMACIZED
//#define APOGEE_1_1
//#define JAPAN

/*
    Wolf3d Full v1.1 Apogee (with ReadThis)   - define CARMACIZED and APOGEE_1_1
    Wolf3d Full v1.4 Apogee (with ReadThis)   - define CARMACIZED
    Wolf3d Full v1.4 GT/ID/Activision         - define CARMACIZED and GOODTIMES
    Wolf3d Full v1.4 Imagineer (Japanese)     - define CARMACIZED and JAPAN
*/

#endif

#define PLAYDEMOLIKEORIGINAL  // When playing or recording demos, several bug fixes do not take
                              // effect to let the original demos work as in the original Wolf3D v1.4
                              // (actually better, as the second demo rarely worked)
//#define USE_GPL             // Replaces the MAME OPL emulator by the DosBox one, which is under a GPL license
#define REFRESHCORNER         // To fix the "see-through gap in wall corner" problem

#endif
