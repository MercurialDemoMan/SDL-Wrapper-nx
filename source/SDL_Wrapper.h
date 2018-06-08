#ifndef SDL_WRAPPER_H
#define SDL_WRAPPER_H

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <switch.h>

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

//entity
typedef struct {
    int  x, y, w, h;   //position and dimensions
    char r, g, b;      //shape color
    double rot;        //rotation angle
    double sc_x, sc_y; //scale
    SDL_Texture* t;    //texture
} Entity;

//exit indicator
//should only be used in SDL_update()
void soft_exit();

//initialize sdl
int  SDL_initialize();

//run sdl window
int  SDL_loop();

//update screen
//needs user definition
void SDL_update();

//deallocate sdl
void SDL_Destroy();




//checks key press
bool key_down(int key);




//load texture from hard disk
SDL_Texture* load_texture(const char* path);

//deallocate texture
void         close_texture(SDL_Texture* t);

//load font from hard disk
TTF_Font*    load_font(const char* path, u32 size);

//deallocate font
void         close_font(TTF_Font* f);

//load music from hard disk
//loads only OGG and MP3 files
Mix_Music*   load_track(const char* path);

//deallocate music
void         close_track(Mix_Music* m);

//load sound from hard disk
//loads only WAV files
Mix_Chunk*   load_sound(const char* path);

//deallocate sound
void         close_sound(Mix_Chunk* c);



//new entity
Entity* entity_alloc(int x, int y, int w, int h, const char* texture_path);

//free entity
void    entity_dealloc(Entity* e);



//play music
void play_track(Mix_Music* m, int loops);

//stop music
void stop_track();

//pause music
void pause_track();

//resume music
void resume_track();

//set music volume
void volume_track(int v);

//play sound
void play_sound(Mix_Chunk* c, int loops);

//set sound volume
void volume_sound(Mix_Chunk* c, int v);

//render texture
void render_texture(int x, int y, int w, int h, SDL_Texture* texture);

//render texture with special arguments
void render_textureEX(int x, int y, int w, int h, double angle, double sc_x, double sc_y, SDL_Texture* texture);

//render rectangle
void render_rect(int x, int y, int w, int h, char r, char g, char b);

//render font
void render_font(int x, int y, char r, char g, char b, TTF_Font* f, const char* text);

//render entity
void render_entity(Entity* e);

//clear screen
void clear();

//render
void render();

#endif
