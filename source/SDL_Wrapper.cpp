#include "SDL_wrapper.h"

//exit indicator
bool g_exit = false; void soft_exit() { g_exit = true; }

//joycon buttons
u64 g_buttons;

//window
SDL_Window*   g_window;
SDL_Renderer* g_renderer;


//initialize sdl
int SDL_initialize() {
    
    //init sdl
    SDL_Init(SDL_INIT_EVERYTHING);
    
    //set linear rendering
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    
    //create window
    g_window = SDL_CreateWindow(NULL,
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);
    
    //create renderer
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_SOFTWARE);
    
    //init sdl-image
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    
    //init sdl-ttf
    TTF_Init();
    
    //init sdl-mixer
    Mix_Init            (MIX_INIT_MP3 | MIX_INIT_OGG);
    Mix_OpenAudio       (44100, MIX_DEFAULT_FORMAT, 2, 1024);
    Mix_AllocateChannels(16);
    
    return 0;
}

//run sdl window
int SDL_loop() {
    
    //run
    SDL_Event e;
    u32 frame_start, frame_time;
    const int FPS = 120;
    const int DELAY = 2.0f / FPS;
    
    //main loop
    while(true) {
        
        frame_start = SDL_GetTicks();
        
        //update input buttons
        hidScanInput();
        g_buttons = hidKeysHeld(CONTROLLER_P1_AUTO);
        
        //sdl events
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_QUIT: goto EXIT;
            }
        }
        
        //clear screen
        clear();
        
        //update
        SDL_update();
        
        //exit flag
        if(g_exit) { goto EXIT; }
        
        //render
        render();
        
        //fps manipulation
        frame_time = SDL_GetTicks() - frame_start;
        if(frame_time < DELAY) {
            SDL_Delay((int)(DELAY - frame_time));
        }
    }
    
EXIT:
    
    return 0;
}

//free memory
void SDL_Destroy() {
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
}



//return button state
bool key_down(int key) {
    return !!(g_buttons & key);
}



//load texture from hard disk
SDL_Texture* load_texture(const char* path) {
    SDL_Surface* s = IMG_Load(path);
    if(s == NULL)  { return NULL; }
    SDL_Texture* t = SDL_CreateTextureFromSurface(g_renderer, s);
    SDL_FreeSurface(s);
    return       t;
}

//deallocate texture
void         close_texture(SDL_Texture* t) {
    if(t != NULL) { SDL_DestroyTexture(t); }
}

//load font from hard disk
TTF_Font*    load_font(const char* path, u32 size) {
    TTF_Font* f = TTF_OpenFont(path, size);
    if(f == NULL) { return NULL; }
    return    f;
}

//deallocate font
void         close_font(TTF_Font* f) {
    if(f != NULL) { TTF_CloseFont(f); }
}

//load music from hard disk
Mix_Music*   load_track(const char* path) {
    Mix_Music* m = Mix_LoadMUS(path);
    if(m == NULL) { return NULL; }
    return     m;
}

//deallocate music
void         close_track(Mix_Music* m) {
    if(m == NULL) { return; }
    stop_track    ();
    Mix_FreeMusic (m);
    m             = NULL;
}

//load sound from hard disk
Mix_Chunk*   load_sound(const char* path) {
    Mix_Chunk* c = Mix_LoadWAV(path);
    if(c == NULL) { return NULL; }
    return     c;
}

//deallocate sound
void         close_sound(Mix_Chunk* c) {
    if(c == NULL) { return; }
    Mix_FreeChunk (c);
    c             = NULL;
}

//new entity
Entity* entity_alloc(int x, int y, int w, int h, const char* texture_path) {
    Entity* e = (Entity*)malloc(sizeof(Entity));
    e->x = x; e->y = y; e->w = w; e->h = h;
    e->r = 0xff; e->g = 0x00; e->b = 0xff;
    e->rot = 0.0; e->sc_x = 1.0; e->sc_y = 1.0;
    if(texture_path != NULL) {
        e->t = load_texture(texture_path);
    } else {
        e->t = NULL;
    }
    return e;
}

//free entity
void   entity_dealloc(Entity* e) {
    if(e->t != NULL) { close_texture(e->t); }
    free(e);
}



//play music
void play_track(Mix_Music* m, int loops) {
    if(m == NULL) { return; }
    Mix_PlayMusic(m, loops);
}

//stop music
void stop_track() {
    Mix_HaltMusic();
}

//pause music
void pause_track() {
    Mix_PauseMusic();
}

//resume music
void resume_track() {
    Mix_ResumeMusic();
}

//set music volume 0 - 128
void volume_track(int v) {
    Mix_VolumeMusic(v);
}

//play sound
void play_sound(Mix_Chunk* c, int loops) {
    if(Mix_PlayChannel(-1, c, loops) == -1) {
        Mix_PlayChannel(0, c, loops);
    }
}

//set sound volume
void volume_sound(Mix_Chunk* c, int v) {
    c->volume = v;
}

//render texture
void render_texture(int x, int y, int w, int h, SDL_Texture* texture)
{
    SDL_Rect rect; rect.x = x; rect.y = y; rect.w = w; rect.h = h;
    
    SDL_RenderCopy(g_renderer, texture, NULL, &rect);
}

//render texture with special arguments
void render_textureEX(int x, int y, int w, int h, double angle, double sc_x, double sc_y, SDL_Texture* texture) {
    
    SDL_RendererFlip flip  = SDL_FLIP_NONE;
    if(sc_x < 0.0) { flip |= SDL_FLIP_HORIZONTAL; }
    if(sc_y < 0.0) { flip |= SDL_FLIP_VERTICAL;   }
    
    
    SDL_Rect rect; rect.x = x; rect.y = y; rect.w = w * abs(sc_x); rect.h = h * abs(sc_y);
    
    SDL_RenderCopyEx(g_renderer, texture, NULL, &rect, angle, NULL, flip);
    
}

//render font
void render_font(int x, int y, char r, char g, char b, TTF_Font* f, const char* text) {
    
    if(f == NULL || text == NULL) { return; }
    
    SDL_Color c = { r, g, b };
    SDL_Surface* s = TTF_RenderText_Blended(f, text, c);
    SDL_Texture* t = SDL_CreateTextureFromSurface(g_renderer, s);
    
    int w, h; SDL_QueryTexture(t, NULL, NULL, &w, &h);
    
    render_texture(x, y, w, h, t);
    
    close_texture(t);
    SDL_FreeSurface(s);
}

//render rectangle
void render_rect(int x, int y, int w, int h, char r, char g, char b) {
    
    SDL_Rect rect; rect.x = x; rect.y = y; rect.w = w; rect.h = h;
    
    SDL_SetRenderDrawColor(g_renderer, r, g, b, 0xFF);
    SDL_RenderFillRect    (g_renderer, &rect);
}

//render entity
void render_entity(Entity* e) {
    if(e    == NULL) { return; }
    if(e->t == NULL) {
        render_rect(e->x, e->y, e->w, e->h, e->r, e->g, e->b);
    } else {
        if(e->rot == 0.0 && e->sc_x == 1.0 && e->sc_y == 1.0) {
            render_texture(e->x, e->y, e->w, e->h, e->t);
        } else {
            render_textureEX(e->x, e->y, e->w, e->h, e->rot, e->sc_x, e->sc_y, e->t);
        }
    }
}

//clear screen
void clear() {
    SDL_SetRenderDrawColor(g_renderer, 0x11, 0x11, 0x11, 0xff);
    SDL_RenderClear(g_renderer);
}

//render
void render() {
    SDL_RenderPresent(g_renderer);
}
