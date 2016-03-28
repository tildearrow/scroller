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
#include <stdio.h>
#include <vector>
#include <queue>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

const unsigned char defaultcolors[3]={255,255,255};

struct format {
  unsigned char r, g, b;
  bool bold, italic, underline, stroke, conceal, shake, negative;
  int blink;
};

struct gchar {
  int character, x;
  format f;
};

std::vector<gchar> chars;
std::queue<int> charq;
std::queue<format> formatq;
int gx, gy, gw, gh;
int pc1, pc2;
int fc;
int counter;
int popped;
format poppedformat;
int speed;
int colorsR[256];
int colorsG[256];
int colorsB[256];
format curformat;
SDL_Window* window;
SDL_Renderer* r;
SDL_Surface* texts;
SDL_Texture* textt;
SDL_Thread* thread;
TTF_Font* font;
SDL_Rect reeect;
SDL_Color color={255,255,255,255};

int gputchar(int x, int y, int c, bool actuallyrender) {
    texts=TTF_RenderGlyph_Blended(font,c,color);
    textt=SDL_CreateTextureFromSurface(r,texts);
    reeect.x=x;
    reeect.y=y;
    reeect.w=texts->clip_rect.w;
    reeect.h=texts->clip_rect.h;
    if (actuallyrender) {
      SDL_RenderCopy(r,textt,&texts->clip_rect,&reeect);
    }
    SDL_FreeSurface(texts);
    SDL_DestroyTexture(textt);
    return reeect.w;
}

static int inthread(void* ptr) {
  bool getout;
  getout=false;
  int curindex;
  char chaar;
  char chaaar;
  std::vector<int> formatlist;
  while (true) {
    chaar=getchar();
    if (chaar=='\n') {
      charq.push(' ');
      formatq.push(curformat);
    } else {
      if (chaar==0x1b) {
	chaar=getchar();
	switch (chaar) {
	  case '[':
	    getout=false;
            curindex=0;
            formatlist.resize(1);
            formatlist[0]=0;
            while (!getout) {
              chaaar=getchar();
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
              }
            }
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
                  break;
                case 1:
                  curformat.bold=1;
                  break;
                case 2:
                  curformat.bold=0;
                  break;
                case 3:
                  curformat.italic=1;
                  break;
                case 4:
                  curformat.underline=1;
                  break;
                case 5:
                  curformat.blink=24;
                  break;
                case 6:
                  curformat.blink=12;
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
                case 21:
                  curformat.underline=2;
                  break;
                case 22:
                  curformat.bold=0;
                  break;
                case 23:
                  curformat.italic=0;
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
                    curformat.r=colorsR[8+(formatlist[ok]-30)];
                    curformat.g=colorsG[8+(formatlist[ok]-30)];
                    curformat.b=colorsB[8+(formatlist[ok]-30)];
                  } else {
                    curformat.r=colorsR[formatlist[ok]-30];
                    curformat.g=colorsG[formatlist[ok]-30];
                    curformat.b=colorsB[formatlist[ok]-30];
                  }
                  break;
                default:
                  printf("new format, %d\n",formatlist[ok]);
                  break;
              }
            }
	}
      } else {
	charq.push(chaar);
	formatq.push(curformat);
      }
    }
  }
  return 0;
}

int main(int argc, char** argv) {
  gx=0; gy=0; gw=1920; gh=32; fc=0; counter=4; speed=3;
  printf("usage: %s FONT SIZE\n",argv[0]);
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
  
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  font=TTF_OpenFont(argv[1],atoi(argv[2]));
  window=SDL_CreateWindow("scroller",gx,gy,gw,gh,0);
  r=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
  thread=SDL_CreateThread(inthread,"inthread",NULL);
  while (true) {
    SDL_RenderClear(r);
    for (int i=0; i<fmax(1,speed); i++) {
      if (counter==0) {
	if (charq.size()>0) {
	  speed=3;
	  popped=charq.front();
	  charq.pop();
          poppedformat=formatq.front();
          formatq.pop();
	  chars.resize(chars.size()+1);
	  chars[chars.size()-1].x=gw; chars[chars.size()-1].character=popped;
          chars[chars.size()-1].f.r=poppedformat.r;
          chars[chars.size()-1].f.g=poppedformat.g;
          chars[chars.size()-1].f.b=poppedformat.b;
          // i know, i know...
          color.r=255;
          color.g=255;
          color.b=255;
          color.a=255;
	  counter=gputchar(0,0,popped,false)-1;
	  if (charq.size()==0) {
	    counter=16;
	  }
	} else {
	  speed=0;
	}
      } else {
	counter--;
      }
      if (speed>0) {
	for (int i=0; i<chars.size(); i++) {
	  chars[i].x-=1;
	}
      }
    }
    for (std::vector<gchar>::iterator i=chars.begin(); i!=chars.end(); i++) {
      // i know, i know...
      color.r=i->f.r;
      color.g=i->f.g;
      color.b=i->f.b;
      color.a=255;
      gputchar(i->x,0,i->character,true);
      if (i->x<-128) { // hehe
	chars.erase(i);
      }
    }
    
    
    SDL_RenderPresent(r);
    ++fc;
    pc2=pc1;
    pc1=SDL_GetTicks();
  }
  return 0;
}
