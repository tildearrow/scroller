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
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct gchar {
  int character, x;
};

std::vector<gchar> chars;
int gx, gy, gw, gh;
int pc1, pc2;
int fc;
SDL_Window* window;
SDL_Renderer* r;
SDL_Surface* texts;
SDL_Texture* textt;
TTF_Font* font;
SDL_Rect reeect;

int gputchar(int x, int y, int c) {
    texts=TTF_RenderGlyph_Blended(font,c,{255,255,255,255});
    textt=SDL_CreateTextureFromSurface(r,texts);
    reeect.x=x;
    reeect.y=y;
    reeect.w=texts->clip_rect.w;
    reeect.h=texts->clip_rect.h;
    SDL_RenderCopy(r,textt,&texts->clip_rect,&reeect);
    SDL_FreeSurface(texts);
    SDL_DestroyTexture(textt);
}

int main(int argc, char** argv) {
  gx=0; gy=0; gw=1920; gh=32; fc=0;
  printf("usage: %s FONT SIZE\n",argv[0]);
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  font=TTF_OpenFont(argv[1],atoi(argv[2]));
  window=SDL_CreateWindow("scroller",gx,gy,gw,gh,0);
  r=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
  while (true) {
    SDL_RenderClear(r);
    gputchar(fc,0,'a');
    SDL_RenderPresent(r);
    ++fc;
    pc2=pc1;
    pc1=SDL_GetTicks();
    printf("%d\n",pc1-pc2);
  }
  return 0;
}