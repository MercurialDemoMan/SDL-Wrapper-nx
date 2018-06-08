#include "SDL_Wrapper.h"

//font and music
TTF_Font*  g_font;
Mix_Music* g_track;
Mix_Chunk* g_step;

//player
Entity* g_player;
u32  g_walk_timer  = 0;
u8   g_walk_switch = 0;

//player sprite sheet
enum {
    PLAYER_IDLE,
    PLAYER_WALK_0,
    PLAYER_WALK_1,
    PLAYER_SPRITE_SHEET_SIZE
};
SDL_Texture* g_player_sprite_sheet[PLAYER_SPRITE_SHEET_SIZE];

//main loop
void SDL_update() {
    
    //exit the application
    if(key_down(KEY_PLUS)) { soft_exit(); }
    
    //player movement
    int old_x = g_player->x;
    int old_y = g_player->y;
    
    if(key_down(KEY_DLEFT))  { g_player->x -= 5; }
    if(key_down(KEY_DRIGHT)) { g_player->x += 5; }
    if(key_down(KEY_DDOWN))  { g_player->y += 5; }
    if(key_down(KEY_DUP))    { g_player->y -= 5; }
    
    //player animation
    if(old_x == g_player->x && old_y == g_player->y) {

        //idling
        g_player->t = g_player_sprite_sheet[PLAYER_IDLE];
        
    } else {
        
        //walking
        int dx = g_player->x - old_x;
        
        //flip player texture left or right
        if(dx != 0) { g_player->sc_x = (dx) / abs(dx); }
        
        //change walking texture every 8 frames and play step sound
        g_walk_timer  = ++g_walk_timer % 8;
        
        if(g_walk_timer == 0) {
            g_walk_switch = g_walk_switch ? 0 : 1;
            play_sound(g_step, 0);
        }
        
        //set texture
        g_player->t = g_walk_switch ? g_player_sprite_sheet[PLAYER_WALK_0] : g_player_sprite_sheet[PLAYER_WALK_1];
        
    }
    
    //render entity
    render_entity(g_player);
    
    //render text
    render_font(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0xff, 0xff, 0xff, g_font, "Hello World!");
}

//main
int main(int argc, char **argv) {
    
    //initialization
    SDL_initialize();
    
    //allocate player       X                 Y                  WIDTH  HEIGHT   (OPTIONAL)TEXTURE
    g_player = entity_alloc(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 53,    64,      "/switch/game-resources/idle.png");
    
    //create player sprite sheet
    g_player_sprite_sheet[PLAYER_IDLE]   = g_player->t;
    g_player_sprite_sheet[PLAYER_WALK_0] = load_texture("/switch/game-resources/walk0.png");
    g_player_sprite_sheet[PLAYER_WALK_1] = load_texture("/switch/game-resources/walk1.png");
    
    //load font
    g_font  = load_font ("/switch/game-resources/KidPixies.ttf", 25);
    //load ogg track
    g_track = load_track("/switch/game-resources/bensound-buddy.mp3");
    //load wav sound effect
    g_step  = load_sound("/switch/game-resources/step.wav");
    
    //play music        LOOPS (-1 = loop forever)
    play_track(g_track, -1);
    
    //main loop
    SDL_loop();
    
    //deallocation
    for(u32 i = 0; i < PLAYER_SPRITE_SHEET_SIZE; i++) {
        if(g_player_sprite_sheet[i] != g_player->t) { close_texture(g_player_sprite_sheet[i]); }
    }
    
    entity_dealloc(g_player);
    close_font(g_font);
    close_track(g_track);
    close_sound(g_step);
    
    SDL_Destroy();
    
    return 0;
}

