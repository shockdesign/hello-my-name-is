// "hello. my name is DOS!"
// Code by shock and easy.
// Graphics by darkowl
// Music by ctrix

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <conio.h>

#include <wildfile.h>
#include <mikmod.h>

#define M_PI 3.141592654

static const char *modfile = "hello.mod";

void vmode(unsigned short);
#pragma aux vmode =\
        "int 10h",\
        parm [ax] modify [ax];

#if 0
void waitvr (void) {
  /* source: http://www.delorie.com/djgpp/doc/ug/graphics/vga.html */
  /* wait until any previous retrace has ended */
  while (inp(0x3da) & 8) {};

  /* wait until a new retrace has just begun */
  while (!(inp(0x3da) & 8)) {};
}
#else
void waitvr(void);
#pragma aux waitvr =\
        "mov dx,3dah",\
        "wait1:",\
        "in al,dx",\
        "test al,8",\
        "jz wait1",\
        modify [al dx];
#endif

char scrolltext[] = "IN:TYPICAL:LAST:MINUTE:STYLE:A:PARTY:SCROLLER:IS:BORN:FOR:SYNTAX2014:BY:DISASTER:AREA:AND:EXTRA:FRIENDS::::TO:KEEP:THIS:SHORT:GREETS:OUT:TO:THE:FOLLOWING::::::DEFAME::::::ONSLAUGHT::::::A:LIFE:IN:HELL::::::RESET64::::::HELLFIRE64::::::MUDLORD::::::SYNTAX:PARTYGOERS::::::RETROGAMERS:EVERYWHERE::::::ALL:THOSE:WE:FORGOT::::::CODE::SH0CK::EASY::::GFX::DARKOWL::::MUSIC::CTRIX::::::PEACE:OUT:::AND:REPEATING:NOW";

extern unsigned char landscape_map[];
extern unsigned char diagonal_image[];
extern unsigned char title_palette[];
extern unsigned char title[];
extern unsigned char rotozoomer_palette[];
extern unsigned char rotozoomer[];
extern unsigned char letters_palette[];
extern unsigned char letters[];

void draw_wavey(int t, char *vidmem, int *sineLookup, char *image,
                unsigned char *map) {
  int v, l, offsetX, y, x, i, colour;

  v = l = offsetX = y = x = colour = 0;

  for (y = 0; y < 200; y ++) {
    for (x = 96; x < 128; x ++) {
      offsetX = (x + t) % 0xff;
      v = map[offsetX + (y << 8)];
      l = sineLookup[x + (v << 8)];
      colour = image[offsetX + (y << 8)];
      for (i = (160 - l); i < 160; i ++) {
        vidmem[i + (y << 8) + (y << 6)] = colour;
      }
    }
    for (x = 163; x >= 128; x --) {
      offsetX = (x + t) % 0xff;
      v = map[offsetX + (y << 8)];
      l = sineLookup[x + (v << 8)];
      colour = image[offsetX + (y << 8)];
      for (i = 160; i < (160 + l + 1); i ++) {
        vidmem[i + (y << 8) + (y << 6)] = colour;
      }
    }
  }
}

void draw_zoomer(int t, char *vidmem, unsigned char *image, int *sintab) {
  int m, dy, dx, sv, cv, y, x, ddx, ddy;

  m = (int)t;
  dy = sintab[(m/3)&0xFF] << 8;
  dx = sintab[(m/2)&0xFF] << 8;
  sv = sintab[m&0xFF];
  cv = sintab[(m+64)&0xFF];
  for (y=0; y<200; y++) {
    ddx = dx;
    ddy = dy;
    for (x=0; x<320; x++) {
      vidmem[x + (y << 8) + (y << 6)] = image[(ddx >> 8 & 0xff) + ((ddy >> 8 & 0xff) << 8)];
      ddx += cv;
      ddy += sv;
    }
    dx -= sv;
    dy += cv;
  }
}

void set129pal(unsigned char colour) {
  outp (0x3c8, 129);
  outp (0x3c9, colour);
  outp (0x3c9, colour);
  outp (0x3c9, colour);
}

void setpal(unsigned char palp[]) {
  int pcount;

  outp (0x3c8, 0);

  for (pcount=0; pcount<768; pcount+=3)
  {
    outp (0x3c9, palp[pcount]);
    outp (0x3c9, palp[pcount+1]);
    outp (0x3c9, palp[pcount+2]);
  }
}

void drawchar(char letter, int x, int y, unsigned char *letters,
              unsigned char *buffer) {
  // 32x, 50y
  int start_x, len_x, start_y, len_y, offset, offset_x, offset_y, i, j;
  unsigned char colour;

  if (x + 32 < 0) return;
  if (y + 50 < 0) return;
  if (x > 320) return;
  if (y > 200) return;

  start_x = ((x < 0)?0:x);
  start_y = ((y < 0)?0:y);
  offset_x = ((x < 0)?abs(x):0);
  offset_y = ((y < 0)?abs(y):0);
  len_x = ((x+32) > 319?32 - ((x+32) - 319):32 - offset_x);
  len_y = ((y+50) > 199?50 - ((y+50) - 199):50 - offset_y);

  offset = (letter - 48) * 32;
  for (i = offset_x; i < offset_x + len_x; i ++) {
    for (j = offset_y; j < len_y; j ++) {
      colour = letters[offset + i + ((49 - j) * 1376)];
      if (colour != 255) {
        buffer[start_x + i - offset_x + ((start_y + j - offset_y) << 8) +
               ((start_y + j - offset_y) << 6)] = colour;
      }
    }
  }
}

void drawstring(char *words, int x, int y, int c, unsigned char *letters,
                unsigned char *buffer, int *sineRoto) {
  int i;
  for(i = 0; i < strlen(words); i ++) {
    drawchar(words[i], x + (i * 32) + (i * 2),
             y + (sineRoto[(c + (i * 10)) % 0xff] >> 4), letters, buffer);
  }
}

void tickhandler(void)
{
        MP_HandleTick();    /* play 1 tick of the module */
        MD_SetBPM(mp_bpm);
}

int main ()
{
  UNIMOD *mf;
  static char pal[768];
  static int sineLookup[256*256];
  static int sineRoto[256];
  static unsigned char bitmap[256*256];
  static unsigned char map[256*256];
  unsigned char framebuffer[320*200];

  int  v, x, y, counter, slide, bounce;

  char z;
  char *vidmem = (char *)0xA0000;

  int textlen = strlen(scrolltext);

        /*
                Initialize soundcard parameters.. you _have_ to do this
                before calling MD_Init(), and it's illegal to change them
                after you've called MD_Init()
        */

        md_mixfreq      =44100;                     /* standard mixing freq */
        md_dmabufsize   =10000;                     /* standard dma buf size */
        md_mode         =DMODE_16BITS|DMODE_STEREO; /* standard mixing mode */
        md_device       =0;                                                     /* standard device: autodetect */

        /*
                Register the loaders we want to use..
        */

        //ML_RegisterLoader(&load_m15);    /* if you use m15load, register it as first! */
        ML_RegisterLoader(&load_mod);

        MD_RegisterDriver(&drv_nos);
MD_RegisterDriver(&drv_sb);
MD_RegisterPlayer(MP_HandleTick);
        if(!MD_Init()){
                printf("MikMod driver error: %s\n",myerr);
                return 0;
        }
/*
        printf("Using %s for %d bit %s %s sound at %u Hz\n",
                        md_driver->Name,
                        (md_mode&DMODE_16BITS) ? 16:8,
                        (md_mode&DMODE_INTERP) ? "interpolated":"normal",
                        (md_mode&DMODE_STEREO) ? "stereo":"mono",
                        md_mixfreq);
  */              /* load the module */

                mf=ML_LoadFN(modfile);

                /* didn't work -> exit with errormsg. */

                if(mf==NULL){
                        printf("MikMod Error: %s\n",myerr);
                        return 0;
                }
                MP_Init(mf);
/*
                printf( "Songname: %s\n"
                                "Modtype : %s\n"
                                "Periods : %s,%s\n",
                                mf->songname,
                                mf->modtype,
                                (mf->flags&UF_XMPERIODS) ? "XM type" : "mod type",
                                (mf->flags&UF_LINEAR) ? "Linear" : "Log");
  */
  md_numchn=mf->numchn;
  MD_PlayStart();

  for (x=0, y=0; x<64*3; x+=3, y++)
    pal[x] = y;

  for (x=64*3, y=63; x<128*3; x+=3, y--)
    pal[x] = y;

  for (x=128*3, y=0; x<192*3; x+=3, y++)
    pal[x+2] = y;

  for (x=192*3, y=63; x<256*3; x+=3, y--)
    pal[x+2] = y;

  // Landscape palette
  pal[(255 * 3) + 2] = 54;
  pal[(255 * 3) + 1] = 67;
  pal[(255 * 3) + 0] = 244;
  pal[(254 * 3) + 2] = 243;
  pal[(254 * 3) + 1] = 149;
  pal[(254 * 3) + 0] = 32;
  pal[(253 * 3) + 2] = 181;
  pal[(253 * 3) + 1] = 81;
  pal[(253 * 3) + 0] = 62;
  pal[(252 * 3) + 2] = 183;
  pal[(252 * 3) + 1] = 57;
  pal[(252 * 3) + 0] = 103;
  pal[(251 * 3) + 2] = 175;
  pal[(251 * 3) + 1] = 39;
  pal[(251 * 3) + 0] = 156;
  pal[(250 * 3) + 2] = 98;
  pal[(250 * 3) + 1] = 30;
  pal[(250 * 3) + 0] = 233;

  // Rotozoomer palette is first 128 colours
  for (x = 0; x < 128 * 3; x += 3) {
    pal[x + 0] = rotozoomer_palette[x + 2] >> 2;
    pal[x + 1] = rotozoomer_palette[x + 1] >> 2;
    pal[x + 2] = rotozoomer_palette[x + 0] >> 2;
  }

  // Title palette is next 2 colours
  for (x = 0; x < 2 * 3; x += 3) {
    pal[x + (128 * 3) + 0] = title_palette[x + 2] >> 2;
    pal[x + (128 * 3) + 1] = title_palette[x + 1] >> 2;
    pal[x + (128 * 3) + 2] = title_palette[x + 0] >> 2;
  }

  // Letters palette is the last 32 colours
  for (x = 0; x < 32 * 3; x += 3) {
    pal[x + (224 * 3) + 0] = letters_palette[x + 2] >> 2;
    pal[x + (224 * 3) + 1] = letters_palette[x + 1] >> 2;
    pal[x + (224 * 3) + 2] = letters_palette[x + 0] >> 2;
  }

  for (x = 0; x < 256; x ++) {
    for (y = 0; y < 256; y ++) {
      if (x >= 128) {
        sineLookup[x + (y << 8)] = floor(y * (-sin(x / 128.0f * M_PI)) / 4);
      } else {
        sineLookup[x + (y << 8)] = floor(y * (sin(x / 128.0f * M_PI)) / 4);
      }
    }
  }

  for (x = 0; x < 256; x ++) {
    sineRoto[x] = (int)(sin(x/256.0*M_PI*2)*127+127)*2;
  }

  for (x = 0; x < 256*256; x ++) {
    bitmap[x] = rand() % 256;
  }

  for (x = 0; x < 256*256; x ++) {
    map[x] = 128;
  }

  vmode (0x13);
  setpal (pal);

  slide = 320;
  counter = 0;

  while (!kbhit ())
  {
    memset(framebuffer, 0, 320*200);

    if (counter >= 0 && counter < 64) {
      set129pal(counter);
      memcpy(framebuffer, title, 320*200);
    } else if (counter >= 64 && counter < 256) {
      memcpy(framebuffer, title, 320*200);
    } else if (counter >= 256 && counter < 320) {
      set129pal(63 - (counter - 256));
      memcpy(framebuffer, title, 320*200);
    } else {
      draw_zoomer(counter, framebuffer, rotozoomer, sineRoto);
      draw_wavey(counter, framebuffer, sineLookup, rotozoomer, landscape_map);
      drawstring(scrolltext, slide, 100, counter, letters, framebuffer, sineRoto);
      slide -= 2;
      if (slide < -(textlen*34)) {
        slide = 320;
      }
    }

    counter ++;

    waitvr ();
    memcpy(vidmem, framebuffer, 320*200);
    MD_Update();
  }

  getch ();
  vmode (3);

                MD_PlayStop();          /* stop playing */
                ML_Free(mf);            /* and free the module */
        MD_Exit();
  printf("Hello, my name was DOS!\n");
  return 0;
}
