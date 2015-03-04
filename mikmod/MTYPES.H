#ifndef MTYPES_H
#define MTYPES_H

/*
	MikMod atomic types:
	====================
*/

typedef char            SBYTE;          /* has to be 1 byte signed */
typedef unsigned char   UBYTE;          /* has to be 1 byte unsigned */
typedef short           SWORD;          /* has to be 2 bytes signed */
typedef unsigned short  UWORD;          /* has to be 2 bytes unsigned */
typedef long            SLONG;          /* has to be 4 bytes signed */
typedef unsigned long   ULONG;			/* has to be 4 bytes unsigned */
typedef int             BOOL;           /* doesn't matter.. 0=FALSE, <>0 true */


#ifdef __WATCOMC__
#define inportb(x) inp(x)
#define outportb(x,y) outp(x,y)
#define inport(x) inpw(x)
#define outport(x,y) outpw(x,y)
#define disable() _disable()
#define enable() _enable()
#endif

#endif
