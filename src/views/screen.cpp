#include "screen.h"


app::app(){
    quit = false;
    _X = 80;
    _Y = 80;
    _map_name = "map1";
    game_run = false;
    show_switch = true;
    map_index = 1;
}

app::~app(){
    TTF_CloseFont(ttf);
    TTF_CloseFont(ttf_small);
    TTF_CloseFont(ttf_large);
    SDL_DestroyTexture(ttf_welcome);
    SDL_DestroyTexture(ttf_welcome1);
    SDL_DestroyTexture(ttf_welcome2);
    SDL_DestroyTexture(ttf_break);
    SDL_DestroyTexture(ttf_congra);
    SDL_DestroyTexture(ttf_hint);
    SDL_DestroyTexture(ttf_all_done);
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

void app::run(){

    while(!quit){
        bool polled = false;
        SDL_Event last_event;
        
        while(SDL_PollEvent(&event)){
            polled = true;
            last_event = event; // Capture the last valid event

            if(event.type == SDL_QUIT){
                quit = true;
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
                        if (map_index > 5) map_index = 1; // Loop back to map1 after 5 maps, or adjust as needed
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
                int w, h;
                // Line 1: CONGRATULATIONS!
                SDL_QueryTexture(ttf_congra, NULL, NULL, &w, &h);
                SDL_Rect ttf_ract_con = { (500 - w) / 2, 380, w, h };
                SDL_RenderCopy(renderer,ttf_congra ,NULL,&ttf_ract_con);

                // Line 2: Message
                int w2, h2;
                if (map_index == 5) {
                    SDL_QueryTexture(ttf_all_done, NULL, NULL, &w2, &h2);
                    SDL_Rect ttf_ract = { (500 - w2) / 2, 410, w2, h2 };
                    SDL_RenderCopy(renderer, ttf_all_done, NULL, &ttf_ract);
                } else {
                    SDL_QueryTexture(ttf_break, NULL, NULL, &w2, &h2);
                    SDL_Rect ttf_ract = { (500 - w2) / 2, 410, w2, h2 };
                    SDL_RenderCopy(renderer, ttf_break, NULL, &ttf_ract);
                }
            }

            // Game Hint: Arrow keys to move, R to reset
            int w, h;
            SDL_QueryTexture(ttf_hint, NULL, NULL, &w, &h);
            SDL_Rect ttf_ract_hint = { (500 - w) / 2, 560, w, h };
            SDL_RenderCopy(renderer, ttf_hint, NULL, &ttf_ract_hint);
        }
        else{
            int w, h;
            // Title: ttf_welcome1 ("PUSH BOX")
            SDL_QueryTexture(ttf_welcome1, NULL, NULL, &w, &h);
            SDL_Rect ttf_ract1 = { (500 - w) / 2, 150, w, h };
            SDL_RenderCopy(renderer,ttf_welcome1 ,NULL,&ttf_ract1);

            // Subtitle Line 1: "PLEASE PRESS SPACE"
            SDL_QueryTexture(ttf_welcome, NULL, NULL, &w, &h);
            SDL_Rect ttf_ract = { (500 - w) / 2, 380, w, h };
            SDL_RenderCopy(renderer,ttf_welcome,NULL,&ttf_ract);

            // Subtitle Line 2: "TO START"
            SDL_QueryTexture(ttf_welcome2, NULL, NULL, &w, &h);
            SDL_Rect ttf_ract2 = { (500 - w) / 2, 410, w, h };
            SDL_RenderCopy(renderer,ttf_welcome2,NULL,&ttf_ract2);
        }
        
        SDL_RenderPresent(renderer);
    }

}

void app::switch_map(std::string _str){
    map_mp = init_map();
    this->_map_name = _str;
    app::set_map_info(&map_mp[this->_map_name]);
    _s._score = 0;
    show_switch = true;
}

void app::ttf_init(){
    const char* font_path = "fonts/ka1.ttf";
    ttf = TTF_OpenFont(font_path, 24);
    ttf_small = TTF_OpenFont(font_path, 18);
    ttf_large = TTF_OpenFont(font_path, 56);
    
    surfaceText = TTF_RenderUTF8_Blended(ttf_large, "PUSH BOX", {0,0,0});
    ttf_welcome1 = SDL_CreateTextureFromSurface(renderer, surfaceText);

    surfaceText = TTF_RenderUTF8_Blended(ttf, "PLEASE PRESS SPACE", {0,0,0});
    ttf_welcome = SDL_CreateTextureFromSurface(renderer, surfaceText);

    surfaceText = TTF_RenderUTF8_Blended(ttf, "TO START", {0,0,0});
    ttf_welcome2 = SDL_CreateTextureFromSurface(renderer, surfaceText);

    surfaceText = TTF_RenderUTF8_Blended(ttf, "CONGRATULATIONS!", {0,0,0});
    ttf_congra = SDL_CreateTextureFromSurface(renderer, surfaceText);

    surfaceText = TTF_RenderUTF8_Blended(ttf, "PRESS SPACE FOR NEXT", {0,0,0});
    ttf_break = SDL_CreateTextureFromSurface(renderer, surfaceText);

    surfaceText = TTF_RenderUTF8_Blended(ttf_small, "ARROW TO MOVE          R TO RESET", {0,0,0});
    ttf_hint = SDL_CreateTextureFromSurface(renderer, surfaceText);

    surfaceText = TTF_RenderUTF8_Blended(ttf, "ALL LEVELS COMPLETED", {0,0,0});
    ttf_all_done = SDL_CreateTextureFromSurface(renderer, surfaceText);
}
