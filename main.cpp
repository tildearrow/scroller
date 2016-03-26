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

struct gchar {
  int character, x;
  unsigned char r, g, b;
  unsigned char bold, underline, stroke, conceal;
};

std::vector<gchar> chars;
std::queue<int> charq;
int gx, gy, gw, gh;
int pc1, pc2;
int fc;
int counter;
int popped;
int speed;
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
  char chaar;
  while (true) {
    chaar=getchar();
    printf("%d\n",chaar);
    if (chaar!='\n') {
      charq.push(chaar);
    }
  }
  return 0;
}

int main(int argc, char** argv) {
  gx=0; gy=0; gw=1920; gh=32; fc=0; counter=4; speed=3;
  printf("usage: %s FONT SIZE\n",argv[0]);
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
	  chars.resize(chars.size()+1);
	  chars[chars.size()-1].x=gw; chars[chars.size()-1].character=popped;
	  counter=gputchar(0,0,popped,false);
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
      gputchar(i->x,0,i->character,true);
      if (i->x<-128) { // hehe
	chars.erase(i);
	printf("char removed!\n");
      }
    }
    
    
    SDL_RenderPresent(r);
    ++fc;
    pc2=pc1;
    pc1=SDL_GetTicks();
  }
  return 0;
}