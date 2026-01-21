#include "screen.h"


app::app(){
    quit = false;
    _X = 80;
    _Y = 80;
    _map_name = "map1";
    game_run = false;
    show_switch = true;
    map_index = 1;
    surfaceText = nullptr;
}

app::~app(){
    TTF_CloseFont(ttf);
    TTF_CloseFont(ttf_small);
    TTF_CloseFont(ttf_large);
    ttf_welcome.destroy();
    ttf_welcome1.destroy();
    ttf_welcome2.destroy();
    ttf_break.destroy();
    ttf_congra.destroy();
    ttf_hint.destroy();
    ttf_all_done.destroy();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void app::init(){

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    window = SDL_CreateWindow("C++ pushbox", 
                                SDL_WINDOWPOS_UNDEFINED, 
                                SDL_WINDOWPOS_UNDEFINED, 
                                500, 600, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    app::ttf_init();
    pic_mp = init_picture(renderer);
    master_map_mp = init_map();
}

void app::set_map_info(map *_map){
    _player.set_coord(_coord_map.get_player_coord(*_map).x, _coord_map.get_player_coord(*_map).y);
    _map->map_edit(_player._x, _player._y, '.');
    _chest = _coord_map.init_chest(*_map);
    _dest = _coord_map.init_dest(*_map);
}
void app::set_map_render(map* _map){

    for(int i = 0; i < _chest.size(); i++){
            if(_chest[i].is_pushed(_player._x, _player._y)){
                _chest[i].move(_player._dire, _map);
            }
        }

    _map->map_render(pic_mp["gress"].tex(), renderer, '.');
    _map->map_render(pic_mp["wall"].tex(), renderer, '#');
    for(int i = 0; i < _dest.size(); i++){
        _dest[i].render(_dest[i]._rect,renderer, pic_mp["gray"].tex() );     
    }
    
    _map->map_render(pic_mp["chest"].tex(), renderer, 'C');
    
    _player.render(_player.player_walk(event, _map),renderer, pic_mp["player"].tex() );
    
    _s.detect_dest(*_map,_dest);
}

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
void main_loop(void* arg) {
    app* game = (app*)arg;
    game->loop_step();
}
#endif

void app::loop_step() {
    bool polled = false;
    SDL_Event last_event;
    
    while(SDL_PollEvent(&event)){
        polled = true;
        last_event = event;

        if(event.type == SDL_QUIT){
            quit = true;
            #ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop();
            #endif
            break;
        }
        
        if(event.type == SDL_KEYDOWN){
            if(event.key.keysym.sym == SDLK_SPACE){
                if(!game_run){
                    map_index = 1;
                    switch_map("map1");
                    game_run = true;   
                }
                else if(_s._score == _dest.size()) {
                    map_index++;
                    if (map_index > 5) map_index = 1;
                    std::string next_map = "map" + std::to_string(map_index);
                    switch_map(next_map);
                }
            }
            else if(event.key.keysym.sym == SDLK_r){
                if(game_run){
                    switch_map(this->_map_name);
                }
            }
        }
    }

    if (polled) {
        event = last_event;
    } else {
        event.type = 0; 
    }

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 255);
    SDL_RenderClear(renderer);
    if(game_run){
        app::set_map_render(&map_mp[this->_map_name]);
        
        if(_s._score == _dest.size()){
            // Line 1: CONGRATULATIONS!
            SDL_Rect ttf_ract_con = { (500 - ttf_congra.w) / 2, 380, ttf_congra.w, ttf_congra.h };
            SDL_RenderCopy(renderer,ttf_congra.tex ,NULL,&ttf_ract_con);

            // Line 2: Message
            if (map_index == 5) {
                SDL_Rect ttf_ract = { (500 - ttf_all_done.w) / 2, 410, ttf_all_done.w, ttf_all_done.h };
                SDL_RenderCopy(renderer, ttf_all_done.tex, NULL, &ttf_ract);
            } else {
                SDL_Rect ttf_ract = { (500 - ttf_break.w) / 2, 410, ttf_break.w, ttf_break.h };
                SDL_RenderCopy(renderer, ttf_break.tex, NULL, &ttf_ract);
            }
        }

        // Game Hint: Arrow keys to move, R to reset
        SDL_Rect ttf_ract_hint = { (500 - ttf_hint.w) / 2, 560, ttf_hint.w, ttf_hint.h };
        SDL_RenderCopy(renderer, ttf_hint.tex, NULL, &ttf_ract_hint);
    }
    else{
        // Title: ttf_welcome1 ("PUSH BOX")
        SDL_Rect ttf_ract1 = { (500 - ttf_welcome1.w) / 2, 150, ttf_welcome1.w, ttf_welcome1.h };
        SDL_RenderCopy(renderer,ttf_welcome1.tex ,NULL,&ttf_ract1);

        // Subtitle Line 1: "PLEASE PRESS SPACE"
        SDL_Rect ttf_ract = { (500 - ttf_welcome.w) / 2, 380, ttf_welcome.w, ttf_welcome.h };
        SDL_RenderCopy(renderer,ttf_welcome.tex,NULL,&ttf_ract);

        // Subtitle Line 2: "TO START"
        SDL_Rect ttf_ract2 = { (500 - ttf_welcome2.w) / 2, 410, ttf_welcome2.w, ttf_welcome2.h };
        SDL_RenderCopy(renderer,ttf_welcome2.tex,NULL,&ttf_ract2);
    }
    
    SDL_RenderPresent(renderer);
}

void app::run(){
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(main_loop, this, 0, 1);
#else
    while(!quit){
        loop_step();
    }
#endif
}

void app::switch_map(std::string _str){
    this->_map_name = _str;
    map_mp[_str] = master_map_mp[_str];
    app::set_map_info(&map_mp[this->_map_name]);
    _s._score = 0;
    show_switch = true;
}

void app::ttf_init(){
    const char* font_path = "fonts/ka1.ttf";
    ttf = TTF_OpenFont(font_path, 24);
    ttf_small = TTF_OpenFont(font_path, 18);
    ttf_large = TTF_OpenFont(font_path, 56);
    
    int h; // local temp for QueryTexture
    auto createText = [&](TTF_Font* f, const char* text, TextTexture& out) {
        if (surfaceText) SDL_FreeSurface(surfaceText);
        surfaceText = TTF_RenderUTF8_Blended(f, text, {0,0,0});
        out.tex = SDL_CreateTextureFromSurface(renderer, surfaceText);
        SDL_QueryTexture(out.tex, NULL, NULL, &out.w, &h); 
        out.h = h;
    };

    createText(ttf_large, "PUSH BOX", ttf_welcome1);
    createText(ttf, "PLEASE PRESS SPACE", ttf_welcome);
    createText(ttf, "TO START", ttf_welcome2);
    createText(ttf, "CONGRATULATIONS!", ttf_congra);
    createText(ttf, "PRESS SPACE FOR NEXT", ttf_break);
    createText(ttf_small, "ARROW TO MOVE          R TO RESET", ttf_hint);
    createText(ttf, "ALL LEVELS COMPLETED", ttf_all_done);
}
