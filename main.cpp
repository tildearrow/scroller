/*
 * Copyright (c) 2016 tildearrow
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#define VERSION "1.2"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "utf8_decode.c"

#define min(x,y) (((x)<(y))?(x):(y))
#define max(x,y) (((x)>(y))?(x):(y))

unsigned char defaultcolors[3]={255,255,255};
unsigned char utf8seq[8];

struct format {
  unsigned char r, g, b;
  bool bold, italic, stroke, conceal, shake, negative;
  int blink, ci, cf;
  bool underline;
  int c;
  int command, value;
};

enum COMMANDS {
  COMMAND_TEXT=0,
  COMMAND_IMAGE
};

SDL_Texture*** texcache;
bool** texcached;
SDL_Rect** texcacher;

#ifdef _WIN32
#define SWITCH_CHAR '/'
#define helparg "/?"
#else
#define SWITCH_CHAR '-'
#define helparg "--help"
#endif

#define EXTREME_QUALITY_OUTLINE

struct gchar {
  int character, x;
  format f;
};

struct gline {
  int x, y, size;
};

std::vector<gchar> chars;
std::vector<gline> lines;
//std::queue<int> charq;
std::queue<format> formatq;
int gx, gy, gw, gh;
int fontsize;
int pc1, pc2;
int fc;
int fcdegrees;
int fi;
int br, fbr;
int nlsep;
int counter;
int popped;
int catmode;
std::queue<int> fontarg;
std::queue<int> imagearg;
format poppedformat;
int speed, minspeed, minspeedchange, speedchange, maxspeed;
int colorsR[256];
int colorsG[256];
int colorsB[256];
format curformat;
SDL_Window* window;
SDL_Renderer* r;
SDL_Surface* texts;
SDL_Texture* textt;
SDL_Texture* textfinal;
SDL_Texture** image;
SDL_Rect* irect;
SDL_Rect temprect1;
SDL_Thread* thread;
TTF_Font** font;
int* underliney;
SDL_Rect reeect;
SDL_Color color={255,255,255,255};
bool willquit;
bool nostop;
char* geometryinfo;
char* geomX, *geomY, *geomW, *geomH;

int gputchar(int x, int y, format fff, bool actuallyrender) {
  if (fff.command==COMMAND_IMAGE) {
    reeect.x=x;
    reeect.y=y;
    reeect.w=irect[fff.value].w;
    reeect.h=irect[fff.value].h;
    if (actuallyrender) {
      SDL_RenderCopy(r,image[fff.value],NULL,&reeect);
    }
    return irect[fff.value].w;
  } else {
  int minx, maxx, advance;
  if (!texcached[fff.cf][fff.c]) {
    texcached[fff.cf][fff.c]=true;
    texts=TTF_RenderGlyph_Blended(font[fff.cf],fff.c,color);
    if (texts==NULL) {
      fprintf(stderr,"error while rendering character 0x%x\n",fff.c);
      texcached[fff.cf][fff.c]=false;
    } else {
      texcache[fff.cf][fff.c]=SDL_CreateTextureFromSurface(r,texts);
      textfinal=SDL_CreateTexture(r,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_TARGET,texts->clip_rect.w+2,texts->clip_rect.h+2);
      SDL_SetRenderTarget(r,textfinal);
      SDL_SetTextureBlendMode(textfinal,SDL_BLENDMODE_BLEND);
      SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
      reeect.x=x+1;
      reeect.y=y+1;
      reeect.w=texts->clip_rect.w;
      reeect.h=texts->clip_rect.h;
      SDL_RenderClear(r);
      SDL_SetTextureColorMod(texcache[fff.cf][fff.c],0,0,0);
    reeect.y--;
    reeect.x++;
    SDL_RenderCopy(r,texcache[fff.cf][fff.c],&texts->clip_rect,&reeect);
    reeect.x-=2;
    SDL_RenderCopy(r,texcache[fff.cf][fff.c],&texts->clip_rect,&reeect);
    reeect.y+=2;
    SDL_RenderCopy(r,texcache[fff.cf][fff.c],&texts->clip_rect,&reeect);
    reeect.x+=2;
    SDL_RenderCopy(r,texcache[fff.cf][fff.c],&texts->clip_rect,&reeect);
    reeect.x--;
    reeect.y--;
#ifdef EXTREME_QUALITY_OUTLINE
    reeect.x--;
    SDL_RenderCopy(r,texcache[fff.cf][fff.c],&texts->clip_rect,&reeect);
    reeect.x+=2;
    SDL_RenderCopy(r,texcache[fff.cf][fff.c],&texts->clip_rect,&reeect);
    reeect.x--;
    reeect.y--;
    SDL_RenderCopy(r,texcache[fff.cf][fff.c],&texts->clip_rect,&reeect);
    reeect.y+=2;
    SDL_RenderCopy(r,texcache[fff.cf][fff.c],&texts->clip_rect,&reeect);
    reeect.y--;
#endif
    SDL_SetTextureColorMod(texcache[fff.cf][fff.c],255,255,255);
    SDL_RenderCopy(r,texcache[fff.cf][fff.c],&texts->clip_rect,&reeect);
    SDL_SetTextureColorMod(texcache[fff.cf][fff.c],255,255,255);
      SDL_SetRenderTarget(r,NULL);
      SDL_DestroyTexture(texcache[fff.cf][fff.c]);
      texcache[fff.cf][fff.c]=textfinal;
      texcacher[fff.cf][fff.c]=texts->clip_rect;
      texcacher[fff.cf][fff.c].w+=2;
      texcacher[fff.cf][fff.c].h+=2;
    }
    SDL_FreeSurface(texts);
  }
  reeect.x=x-1;
  reeect.y=y-1;
  reeect.w=texcacher[fff.cf][fff.c].w;
  reeect.h=texcacher[fff.cf][fff.c].h;
  if (actuallyrender && !fff.conceal) {
    SDL_SetTextureColorMod(texcache[fff.cf][fff.c],color.r,color.g,color.b);
    SDL_SetTextureAlphaMod(texcache[fff.cf][fff.c],(fff.blink)?((int)(127+(sin(((double)fcdegrees*(double)fff.blink*3.14159265358979323846264338327950)/180.0)*127))):(255));
    SDL_RenderCopy(r,texcache[fff.cf][fff.c],&texcacher[fff.cf][fff.c],&reeect);
    SDL_SetTextureAlphaMod(texcache[fff.cf][fff.c],255);
    SDL_SetTextureColorMod(texcache[fff.cf][fff.c],255,255,255);
  }
  TTF_GlyphMetrics(font[fff.cf],fff.c,&minx,&maxx,NULL,NULL,&advance);
  return (short)(advance<<1)/2;
  }
}

static int inthread(void* ptr) {
  bool getout;
  getout=false;
  int curindex;
  int chaar;
  int mode;
  unsigned char chaaar;
  std::vector<int> formatlist;
  while (true) {
    chaar=getchar();
    if (catmode) {putchar(chaar);}
    if (chaar=='\n') {
      //charq.push(' ');
      curformat.c=' ';
      formatq.push(curformat);
    } else {
      if (chaar==0x1b) {
	chaar=getchar();
	if (catmode) {putchar(chaar);}
	switch (chaar) {
	  case '[':
	    getout=false;
            curindex=0;
            formatlist.resize(1);
            formatlist[0]=0;
            while (!getout) {
              chaaar=getchar();
	      if (catmode) {putchar(chaaar);}
              switch (chaaar) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                  formatlist[curindex]*=10;
                  formatlist[curindex]+=chaaar-0x30;
                  break;
                case ';':
                  curindex++;
                  formatlist.resize(curindex+1);
                  formatlist[curindex]=0;
                  break;
                default:
                  getout=true;
		  mode=chaaar;
              }
            }
            if (mode=='m') {
	      for (int ok=0; ok<=curindex; ok++) {
		switch (formatlist[ok]) {
		  case 0:
		    curformat.r=defaultcolors[0];
		    curformat.g=defaultcolors[1];
		    curformat.b=defaultcolors[2];
		    curformat.bold=0; curformat.underline=0;
		    curformat.stroke=0; curformat.conceal=0;
		    curformat.shake=0; curformat.italic=0;
		    curformat.blink=0; curformat.negative=0;
		    curformat.ci=15; curformat.cf=0;
		    break;
		  case 1:
		    curformat.bold=1;
		    if (curformat.ci<8) {
		      curformat.ci+=8;
		    }
		    curformat.r=colorsR[curformat.ci];
		    curformat.g=colorsG[curformat.ci];
		    curformat.b=colorsB[curformat.ci];
		    break;
		  case 2:
		    if (curformat.ci>=8 && curformat.ci<16) {
		      curformat.ci-=8;
		    }
		    curformat.r=colorsR[curformat.ci];
		    curformat.g=colorsG[curformat.ci];
		    curformat.b=colorsB[curformat.ci];
		    curformat.bold=0;
		    break;
		  case 3:
		    curformat.italic=1;
		    break;
		  case 4:
		    curformat.underline=1;
		    break;
		  case 5:
		    curformat.blink=br;
		    break;
		  case 6:
		    curformat.blink=fbr;
		    break;
		  case 7:
		    curformat.negative=1;
		    break;
		  case 8:
		    curformat.conceal=1;
		    break;
		  case 9:
		    curformat.stroke=1;
		    break;
		  case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19:
		    curformat.cf=formatlist[ok]-10;
		    break;
		  case 20:
		    curformat.shake=1;
		  case 21:
		    curformat.bold=0;
		    break;
		  case 22:
		    /*
		    curformat.r=defaultcolors[0];
		    curformat.g=defaultcolors[1];
		    curformat.b=defaultcolors[2];*/
		    curformat.bold=0;
		    break;
		  case 23:
		    curformat.italic=0;
		    curformat.shake=0;
		    break;
		  case 24:
		    curformat.underline=0;
		    break;
		  case 25:
		    curformat.blink=0;
		    break;
		  case 26:
		    curformat.shake=1;
		    break;
		  case 27:
		    curformat.negative=0;
		    break;
		  case 28:
		    curformat.conceal=0;
		    break;
		  case 29:
		    curformat.stroke=0;
		    break;
		  case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37:
		    if (curformat.bold) {
		      curformat.ci=8+(formatlist[ok]-30);
		    } else {
		      curformat.ci=(formatlist[ok]-30);
		    }
		    curformat.r=colorsR[curformat.ci];
		    curformat.g=colorsG[curformat.ci];
		    curformat.b=colorsB[curformat.ci];
		    break;
		  case 38:
		    ok++;
		    switch (formatlist[ok]) {
		      case 2: // RGB
			curformat.r=formatlist[++ok];
			curformat.g=formatlist[++ok];
			curformat.b=formatlist[++ok];
			break;
		      case 5: // 256 colors
			curformat.r=colorsR[formatlist[++ok]];
			curformat.g=colorsG[formatlist[ok]];
			curformat.b=colorsB[formatlist[ok]];
			break;
		      default:
			ok--;
			break;
		    }
		    break;
		  case 39:
		    curformat.r=defaultcolors[0];
		    curformat.g=defaultcolors[1];
		    curformat.b=defaultcolors[2];
		    break;
		  default:
		    printf("new format, %d\n",formatlist[ok]);
		    break;
	      }
	    }
          } else if (mode=='w') {
	    for (int ok=0; ok<=curindex; ok++) {
	      curformat.command=COMMAND_IMAGE;
	      curformat.value=formatlist[ok];
	      formatq.push(curformat);
	      curformat.command=COMMAND_TEXT;
	      curformat.value=0;
	    }
	  }
        }
      } else {
        if (chaar>0x7f) {
          if (chaar<0xe0) {
            // UTF-8
            utf8seq[0]=chaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[1]=chaaar;
          } else if (chaar<0xf0) {
            utf8seq[0]=chaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[1]=chaaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[2]=chaaar;
          } else if (chaar<0xf8) {
            utf8seq[0]=chaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[1]=chaaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[2]=chaaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[3]=chaaar;
          } else if (chaar<0xfc) {
            utf8seq[0]=chaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[1]=chaaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[2]=chaaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[3]=chaaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[4]=chaaar;
          } else if (chaar<0xfd) {
            utf8seq[0]=chaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[1]=chaaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[2]=chaaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[3]=chaaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[4]=chaaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[5]=chaaar;
          } else if (chaar<0xff) {
            utf8seq[0]=chaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[1]=chaaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[2]=chaaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[3]=chaaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[4]=chaaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[5]=chaaar;
            chaaar=getchar();
	    if (catmode) {putchar(chaaar);}
            utf8seq[6]=chaaar;
          } else {
            printf("what are you doing?\n");
          }
          utf8_decode_init(utf8seq,8);
	  curformat.c=utf8_decode_next();
          formatq.push(curformat);
        } else if (chaar==EOF) {
          break;
        } else {
	  curformat.c=chaar;
          formatq.push(curformat);
        }
      }
    }
  }
  return 0;
}

void failusage(const char* programname) {
  printf("\
usage: %s [OPTIONS]... FONTFILE [FONT2FILE]...\n\
type '%s %s' for more information.\n\
",programname,programname,helparg);
}

void usage(const char* programname) {
  printf("\
usage: %s [OPTIONS]... FONTFILE [FONT2FILE]...\n\
creates a window which scrolls text being fed to stdin.\n\
best used with a pipe.\n\
\n\
FONT OPTIONS:\n\
  %cfs FONTSIZE            defines a custom font size.\n\
                          if this is not defined, a value of 20 will be used\n\
			  as default.\n\
  %cindex INDEX,[INDEX]... selects font indexes on font collections (.ttc,\n\
	                  .fon, etc.).\n\
  %cdefcol R,G,B           default color\n\
\n\
WINDOW OPTIONS:\n\
  %cgeometry GEOMETRY      defines window size and placement.\n\
                          it follows the same format as used in X base\n\
                          applications:\n\
                             WIDTHxHEIGHT+X+Y\n\
  %csolid  [R,G,B]     nyi no transparency\n\
\n\
SCROLL CONTROL:\n\
  %cms SPEED               minimum speed\n\
                          default is 3\n\
  %cmsc SPEED              minimum amount of characters in queue in order to\n\
                          begin speeding up\n\
                          default is 20\n\
  %csc SPEED               amount of characters in queue to speed up by +1\n\
                          default is 20\n\
  %cMs SPEED               maximum speed\n\
                          default is infinity (0)\n\
  %cns SIZE                defines a custom line separator size.\n\
                          if this is not defined, a value of 16 will be used.\n\
  %cnostop                 continue scrolling even if there is no text left\n\
\n\
OUTPUT CONTROL:\n\
  %ccat                    write stdin to stdout\n\
\n\
IMAGE OPTIONS:\n\
  %cimage FILE             load an image for usage with escape code ^[[Nw\n\
\n\
ESCAPE SEQUENCE CONTROL:\n\
  %cbr RATE                set blink rate (in blinks per 360 frames)\n\
  %cfbr RATE               set fast blink rate (in blinks per 360 frames)\n\
\n\
MISC.:\n\
  %cv, %c-version, %cver     show version\n\
  %c?, %c-help, %chelp       show this help\n\
\n\
written by tildearrow, licensed under MIT License.\
\n",programname,SWITCH_CHAR,SWITCH_CHAR,SWITCH_CHAR,SWITCH_CHAR,SWITCH_CHAR,
SWITCH_CHAR,SWITCH_CHAR,SWITCH_CHAR,SWITCH_CHAR,SWITCH_CHAR,SWITCH_CHAR,
SWITCH_CHAR,SWITCH_CHAR,SWITCH_CHAR,SWITCH_CHAR,SWITCH_CHAR,SWITCH_CHAR,
SWITCH_CHAR,SWITCH_CHAR);
}

void about() {
  printf("scroller v%s, copyright 2016 tildearrow.\n\
using SDL v%d.%d.%d.\n\
\n\
Permission is hereby granted, free of charge, to any person obtaining a copy\n\
of this software and associated documentation files (the \"Software\"), to deal\n\
in the Software without restriction, including without limitation the rights\n\
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n\
copies of the Software, and to permit persons to whom the Software is\n\
furnished to do so, subject to the following conditions:\n\
\n\
The above copyright notice and this permission notice shall be included in all\n\
copies or substantial portions of the Software.\n\
\n\
THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n\
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n\
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n\
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n\
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n\
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n\
SOFTWARE.\n\
\n\
portions are copyright 2005 JSON.org, licensed under a modified MIT license:\n\
\n\
The Software shall be used for Good, not Evil.\n\
",VERSION,SDL_MAJOR_VERSION,SDL_MINOR_VERSION,SDL_PATCHLEVEL);
}

int main(int argc, char** argv) {
  bool geometryspecified, fsspecified;
  geometryspecified=false;
  fsspecified=false;
  
  speed=3;
  minspeed=3; minspeedchange=20; speedchange=20; maxspeed=0;
  nlsep=16;
  nostop=false;
  catmode=false;
  br=4; fbr=20;
  
  gx=0; gy=0;
  
  // parse arguments
  for (int curarg=1; curarg<argc; curarg++) {
    if (argv[curarg][0]==SWITCH_CHAR) {
      // switch, so read it
      if (strcmp((argv[curarg])+1,"ns")==0) { // new line separator
	curarg++;
	if (curarg<argc) {
	nlsep=atoi(argv[curarg]);
	} else {printf("%s requires an argument\n",argv[curarg-1]); usage(argv[0]);}
      } else
      if (strcmp((argv[curarg])+1,"fs")==0) {
	curarg++;
	if (curarg<argc) {
	fontsize=atoi(argv[curarg]);
	fsspecified=true;
	} else {printf("%s requires an argument\n",argv[curarg-1]); usage(argv[0]);}
      } else
      if (strcmp((argv[curarg])+1,"ms")==0) {
	curarg++;
	if (curarg<argc) {
	minspeed=atoi(argv[curarg]);
	} else {printf("%s requires an argument\n",argv[curarg-1]); usage(argv[0]);}
      } else
      if (strcmp((argv[curarg])+1,"msc")==0) {
	curarg++;
	if (curarg<argc) {
	minspeedchange=atoi(argv[curarg]);
	} else {printf("%s requires an argument\n",argv[curarg-1]); usage(argv[0]);}
      } else
      if (strcmp((argv[curarg])+1,"sc")==0) {
	curarg++;
	if (curarg<argc) {
	speedchange=atoi(argv[curarg]);
	} else {printf("%s requires an argument\n",argv[curarg-1]); usage(argv[0]);}
      } else
      if (strcmp((argv[curarg])+1,"Ms")==0) {
	curarg++;
	if (curarg<argc) {
	maxspeed=atoi(argv[curarg]);
	} else {printf("%s requires an argument\n",argv[curarg-1]); usage(argv[0]);}
      } else
      if (strcmp((argv[curarg])+1,"defcol")==0) {
	curarg++;
	if (curarg<argc) {
	sscanf(argv[curarg],"%d,%d,%d",&defaultcolors[0],&defaultcolors[1],&defaultcolors[2]);
	} else {printf("%s requires an argument\n",argv[curarg-1]); usage(argv[0]);}
      } else
      if (strcmp((argv[curarg])+1,"geometry")==0) {
	curarg++;
	if (curarg<argc) {
	  if (strstr(argv[curarg],"+")!=NULL) {
	    sscanf(argv[curarg],"%dx%d+%d+%d",&gw,&gh,&gx,&gy);
	  } else {
	    sscanf(argv[curarg],"%dx%d",&gw,&gh);
	  }
	  geometryspecified=true;
	} else {printf("%s requires an argument\n",argv[curarg-1]); usage(argv[0]);}
      } else
      if (strcmp((argv[curarg])+1,"index")==0) {
	curarg++;
	if (curarg<argc) {
	fi=atoi(argv[curarg]);
	} else {printf("%s requires an argument\n",argv[curarg-1]); usage(argv[0]);}
      } else
      if (strcmp((argv[curarg])+1,"br")==0) {
	curarg++;
	if (curarg<argc) {
	br=atoi(argv[curarg]);
	} else {printf("%s requires an argument\n",argv[curarg-1]); usage(argv[0]);}
      } else
      if (strcmp((argv[curarg])+1,"fbr")==0) {
	curarg++;
	if (curarg<argc) {
	fbr=atoi(argv[curarg]);
	} else {printf("%s requires an argument\n",argv[curarg-1]); usage(argv[0]);}
      } else
      if (strcmp((argv[curarg])+1,"nostop")==0) {
	nostop=true;
      } else
      if (strcmp((argv[curarg])+1,"cat")==0) {
	catmode=true;
      } else
      if (strcmp((argv[curarg])+1,"image")==0) {
	curarg++;
	imagearg.push(curarg);
      } else
      if (strcmp((argv[curarg])+1,"v")==0 || strcmp((argv[curarg])+1,"-version")==0 || strcmp((argv[curarg])+1,"ver")==0) {
	about();
	return 0;
      } else
      if (strcmp((argv[curarg])+1,"?")==0 || strcmp((argv[curarg])+1,"-help")==0 || strcmp((argv[curarg])+1,"help")==0) {
	usage(argv[0]);
	return 0;
      }
    } else {
      fontarg.push(curarg);
    }
  }
  
  if (fontarg.size()==0) {
    failusage(argv[0]);
    return 1;
  }
  
  if (!fsspecified) {
    fontsize=20;
  }
  
  SDL_Init(SDL_INIT_VIDEO);
  
  if (!geometryspecified) {
    SDL_Rect temprect;
    int displays;
    displays=SDL_GetNumVideoDisplays();
    if (displays!=1) {
      printf("%d displays detected, using first one\n",displays);
    }
    int dbresult;
    dbresult=SDL_GetDisplayBounds(0,&temprect);
    if (dbresult==-1) {
      printf("i'm sorry, but something happened getting display bounds.\n");
    }
    gw=temprect.w;
    gh=(fontsize*3)/2;
  } else {
    //gw=atoi(geomW); gh=atoi(geomH);
  }
  
  willquit=false; fc=0; counter=4; fcdegrees=0;
  
  // width check
  if (gw<1) {
    if (geometryspecified) {
      printf("i'm sorry, but invalid width.\n");
      return 1;
    } else {
      printf("i'm sorry, but your screen is way too small for this program (%d width).\n",gw);
      return 1;
    }
  }
  
  // prepare colors
  for (int I=0; I<256; I++) {
    colorsR[I]=(I>231)?((I-232)*11): // gray-scale
            (I>15)?(50+(((I-16)/36)*41)): // color cube
            (I>8)?((I&1)?(255):(85)): // 16-color palette
            (I>7)?(128): // color 8
            (I)?(((I&1)*192)): // 16-color palette, dark tones
            0; // zero

    colorsG[I]=(I>231)?((I-232)*11): // gray-scale
               (I>15)?(50+((((I-16)/6)%6)*41)): // color cube
               (I>8)?((I&2)?(255):(85)): // 16-color palette
               (I>7)?(128): // color 8
               (I)?(((I&2)*96)): // 16-color palette, dark tones
               0; // zero
                
    colorsB[I]=(I>231)?((I-232)*11): // gray-scale
               (I>15)?(50+((((I-16))%6)*41)): // color cube
               (I>8)?((I&4)?(255):(85)): // 16-color palette
               (I>7)?(128): // color 8
               (I)?(((I&4)*48)): // 16-color palette, dark tones
               0; // zero
  }
  // some color changes, to make things look better
  colorsB[11]=0; // yellow is a pure yellow
  colorsR[3]=253; colorsG[3]=196; // dark yellow is gold
  colorsR[5]=136; colorsB[5]=155; // dark magenta is purple
  colorsR[1]=230; // dark red is way more red
  colorsG[2]=187; // dark green is more neutral
  colorsB[4]=187; // blue too
  colorsB[6]=202; // dark cyan has higher blue

  curformat.r=defaultcolors[0];
  curformat.g=defaultcolors[1];
  curformat.b=defaultcolors[2];
  curformat.cf=0;
  curformat.command=COMMAND_TEXT;
  curformat.underline=0;

  // font init

  TTF_Init();
  
  font=new TTF_Font*[fontarg.size()];
  int it;
  int itera;
  int fontargsize;
  fontargsize=fontarg.size();
  underliney=new int[fontargsize];
  for (it=0; it<fontargsize; it++) {
    int maxy;
    int truemaxy;
    truemaxy=0;
    font[it]=TTF_OpenFontIndex(argv[fontarg.front()],fontsize,fi);
    fontarg.pop();
    if (!font[it]) {
      printf("i'm sorry but this happened while loading font: %s\n",TTF_GetError());
      return 1;
    }
    // this is for the underline
    underliney[it]=TTF_FontHeight(font[it])+TTF_FontDescent(font[it]);
    printf("font %d's max y: %d\n",it,underliney[it]);
  }
  window=SDL_CreateWindow("scroller",gx,gy,gw,gh,SDL_WINDOW_OPENGL|SDL_WINDOW_BORDERLESS);
  r=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
  
  // image init
  IMG_Init(0xffffffff); // for all future formats
  
  // load all images now
  int imageargsize=imagearg.size();
  image=new SDL_Texture*[imageargsize];
  irect=new SDL_Rect[imageargsize];
  SDL_Surface* tempsurface;
  for (int iter=0; iter<imageargsize; iter++) {
    tempsurface=IMG_Load(argv[imagearg.front()]);
    imagearg.pop();
    if (tempsurface==NULL) {
      printf("error while reading image: %s\n",IMG_GetError());
      return 1;
    }
    irect[iter]=tempsurface->clip_rect;
    image[iter]=SDL_CreateTextureFromSurface(r,tempsurface);
    SDL_FreeSurface(tempsurface);
  }
  
  thread=SDL_CreateThread(inthread,"inthread",NULL);
  // and create the character cache
  texcache=new SDL_Texture**[it+1];
  texcached=new bool*[it+1];
  texcacher=new SDL_Rect*[it+1];
  for (int ite=0; ite<it+1; ite++) {
    texcache[ite]=new SDL_Texture*[1048576]; // i'm sorry, this is needed
    texcached[ite]=new bool[1048576];
    texcacher[ite]=new SDL_Rect[1048576];
    for (int i=0; i<1048576; i++) {
      texcached[ite][i]=false;
    }
  }
  while (true) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      if (ev.type==SDL_QUIT) {
        willquit=true;
      }
    }
    /*SDL_SetRenderDrawColor(r,255,255,255,128);
    SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_ADD);
    SDL_RenderClear(r);*/
    SDL_SetRenderDrawColor(r,0,0,0,0);
    SDL_RenderClear(r);
    
    speed=(counter==0 && formatq.size()<1 && !nostop)?(0):(minspeed+max(0,(speedchange==0)?(0):((formatq.size())/speedchange)));
    for (int i=0; i<fmax(1,speed); i++) {
      if (counter<=0) {
	if (formatq.size()>0) {
          poppedformat=formatq.front();
	  popped=poppedformat.c;
          formatq.pop();
	  chars.resize(chars.size()+1);
	  chars[chars.size()-1].x=gw;
          chars[chars.size()-1].f=poppedformat;
          // i know, i know...
          color.r=255;
          color.g=255;
          color.b=255;
          color.a=255;
	  counter=gputchar(0,0,poppedformat,false)-1;
	  if (formatq.size()==0) {
	    counter=nlsep;
	  }
	  if (chars.size()!=0) {
	    if (chars[chars.size()-1].f.underline) {
	      if (chars.size()==1) {
		lines.resize(lines.size()+1);
		lines[lines.size()-1].x=gw;
		lines[lines.size()-1].y=underliney[0];
		lines[lines.size()-1].size=0;
	      } else if (!chars[chars.size()-2].f.underline) {
		lines.resize(lines.size()+1);
		lines[lines.size()-1].x=gw;
		lines[lines.size()-1].y=underliney[0];
		lines[lines.size()-1].size=0;
	      }
	    }
	  }
	  // is the counter negative?
	  if (counter<0) {
	    chars[chars.size()-1].x+=counter;
	    counter=0;
	  }
	}
      } else {
	counter--;
      }
      if (speed>0) {
	for (int i=0; i<chars.size(); i++) {
	  chars[i].x-=1;
          //lines[i].x-=1;
          /*if (chars.end()->f.underline) {
            lines.end()->size+=1;
          }*/
	}
        for (int i=0; i<lines.size(); i++) {
          lines[i].x-=1;
        }
        if (chars.size()!=0) {
	  if (chars[chars.size()-1].f.underline) {
	    lines[lines.size()-1].size++;
	  }
	}
      }
    }
    for (std::vector<gchar>::iterator i=chars.begin(); i!=chars.end(); i++) {
      // i know, i know...
      color.r=i->f.r;
      color.g=i->f.g;
      color.b=i->f.b;
      color.a=255;
      gputchar(i->x+((i->f.shake)?((rand()%5)-2):(0)),((i->f.shake)?((rand()%5)-2):(0)),i->f,true);
      if (i->x<-128) { // hehe
	chars.erase(i);
      }
    }
    
    for (std::vector<gline>::iterator i=lines.begin(); i!=lines.end(); i++) {
      SDL_SetRenderDrawColor(r,0,0,0,255);
      SDL_RenderDrawLine(r,i->x-1,(i->y)-1,i->x+i->size+1,i->y-1);
      SDL_RenderDrawLine(r,i->x-1,(i->y)+1,i->x+i->size+1,i->y+1);
      SDL_RenderDrawPoint(r,i->x-1,i->y);
      SDL_RenderDrawPoint(r,i->x+i->size+1,i->y);
      SDL_SetRenderDrawColor(r,255,255,255,255);
      SDL_RenderDrawLine(r,i->x,i->y,i->x+i->size,i->y);
    }
    SDL_RenderPresent(r);
    ++fc; fcdegrees=fc%360;
    pc2=pc1;
    pc1=SDL_GetTicks();
    if (willquit) {break;}
  }
  return 0;
}
