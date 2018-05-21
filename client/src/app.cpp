//#include <functional>
//#include "game_state.hpp"
//#define SDL_HINT_IME_INTERNAL_EDITING "1"
#include <math.h>
#include "websocket_wrapper.hpp"
#include "sdlwrap.hpp"
#include "chipmunk.h"
#include "gamelogic.hpp"
#include "packet.hpp"
#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#else
#include <unistd.h>
#endif

/*
void ugly_handle_socket_success(int fd, void* ud){
    sdlw::Text* state = reinterpret_cast<sdlw::Text*>(ud);
    state->set("server is alive", sdlw::Font::get<12>(), SDL_Color{0, 255, 0, 255});
}
void ugly_handle_socket_fail(int fd, int t, const char* msg, void* ud){
    sdlw::Text* state = reinterpret_cast<sdlw::Text*>(ud);
    state->set((std::string("server may died: ")+ msg).c_str(), sdlw::Font::get<12>(), SDL_Color{255, 0, 0, 255});
}
*/

void drawWorm(SDL_Renderer* renderer, gamelogic::Worm* worm){
    cpBody* body = worm->getBody();
    cpShape* shape = worm->getShape();
    cpVect pos = cpBodyGetPosition(body);
    cpFloat angle = cpBodyGetAngle(body);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, (Uint8)255);
    SDL_RenderDrawLine(renderer, pos.x-8, pos.y, pos.x + 10*cos(angle), pos.y - 10*sin(angle));
    cpVect a = cpPolyShapeGetVert(shape, 0), b = cpPolyShapeGetVert(shape, 1);
} 
void drawWorld(SDL_Renderer* renderer, gamelogic::World* world){
    auto worms = world->getWorms();
    std::vector<cpVect> walls = world->getWalls();
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for(int i=0, l=walls.size()-1; i<l; ++i){
        SDL_RenderDrawLine(renderer, walls[i].x, walls[i].y, walls[i+1].x, walls[i+1].y);
    }
    for(gamelogic::Worm* worm : worms)
        drawWorm(renderer, worm);
    SDL_RenderPresent(renderer);
}

class App{
private:
    enum class State: int {
        NEED_LOGIN, FAIL_CONNECT_SERVER, TRY_LOGIN, RECEIVING_LAST_STATE, RUNNING
    };
    static constexpr const char* APP_NAME = "CosmosWanderer";
    State state = State::NEED_LOGIN;
    //game_state::PartialMap* map = NULL;
    WebsocketWrapper socket;
    const char* title="CosmosWanderer";
    int width=800, height=600;
public:
    static App& getInstance() {
        static App instance;
        return instance;
    }
    void loadConfig(){
    }
    void setState(State s){
        state = s;
    }
    void run(const char* server_ip, int port){
        sdlw::initSDL();
        gamelogic::World world;
        sdlw::FileManager file_manager;
        file_manager.setPrefPath("Eunchul", "HopeForTheFlowers");
        gamelogic::Worm* worm = world.getWorm(world.genWorm());
        if(socket.connect(server_ip, port) != WebsocketWrapper::CODE::SUCCESS)
            state = State::FAIL_CONNECT_SERVER;

        auto big_font = sdlw::Font::get<24>();
        auto normal_font = sdlw::Font::get<16>();
        auto color = SDL_Color{255, 255, 255, 255},
             yellow = SDL_Color{255, 255, 0, 255};

        sdlw::MainLoop mainloop;
        sdlw::Window window(&mainloop, title, width, height);
        sdlw::SpriteSheet sprite_sheet(&window, "asset/spritesheet/atlas.csv"); 
        sdlw::Scene scene(&window);
        sdlw::Text state_label(&scene, 0, 0, "server connecting..", normal_font, yellow, sdlw::Align::TOP, sdlw::Align::LEFT);
        sdlw::Text a(&scene, width/2, height/2, "Hello World!", big_font, color, sdlw::Align::MIDDLE, sdlw::Align::CENTER);
        //sdlw::SpriteImage si(&scene, sprite_sheet.get("asset/sprite/worm_move4.png"), 90, 90);
        sdlw::AnimatedSprite* as = sprite_sheet.createAnimatedSprite("asset/sprite/worm_move");
        sdlw::Sprite* s = sprite_sheet.get("asset/sprite/worm_move4.png");
        sdlw::AnimatedSpriteImage asi(&scene, sprite_sheet.createAnimatedSprite("asset/sprite/worm_move"), 150, 150);
        //asi.animate(-1, 0.25);
        //as->animate(-1, 0.25);
        window.listenKeyDown([&worm](const SDL_KeyboardEvent& e){
            switch(e.keysym.sym){
            case SDLK_LEFT:
                worm->control(gamelogic::Worm::ACTION::LEFT);
                printf("LEFT\n");
            break;
            case SDLK_RIGHT:
                worm->control(gamelogic::Worm::ACTION::RIGHT);
                printf("RIGHT\n");
            break;
            case SDLK_UP:
                worm->control(gamelogic::Worm::ACTION::JUMP);
                printf("UP\n");
            break;
            case SDLK_DOWN:
                //worm.control(gamelogic::Worm::ACTION::JUMP);
            break;
            }
        }); 
        window.listenAfterRender([&window, &world, &worm](){
            //s->render(window.getRenderer(), 150, 150);
            //as->render(window.getRenderer(), 180, 180); 
            drawWorld(window.getRenderer(), &world);
            world.update(2./60.);
        });
        /*
        #if defined(__EMSCRIPTEN__)
        emscripten_set_socket_open_callback((void*)&state_label, ugly_handle_socket_success);
        emscripten_set_socket_error_callback((void*)&state_label, ugly_handle_socket_fail);
        #else
        if(state == State::FAIL_CONNECT_SERVER) ugly_handle_socket_fail(-1, -1, "server may died", &state_label);
        else ugly_handle_socket_success(-1, &state_label);
        #endif

        char session_id[256] = {'\0',};
        int size = file_manager.load_pref("session_id", session_id);
        if(size < 0){
            state_label.set("No session data: try to get new session id from server");
            Packet req(Packet::OP::JOIN);
            socket.send(req.bytes(), req.size());
        }
        else{
            state_label.set((std::string("try to login with session id: ") + std::string(session_id, size)).c_str());
            Packet req(Packet::OP::LOGIN, (char*)session_id, size_t(size));
            socket.send(req.bytes(), req.size());
        }
        mainloop.listenBeforeDraw([this, &window, &world, &file_manager, &state_label](){
            socket.recv([this, &window, &world, &file_manager, &state_label](char* msg, int size){
                Packet res(msg, size);
                printf("op: %d\n", (int)res.getOP());
                switch(res.getOP()){
                case Packet::OP::ECHO:
                    //state_label.set((std::string("echo: ") + std::string(hres.getData(), res.getDataSize())).c_str());
                    state_label.set((std::string("echo: ") + std::string(res.getData(), res.getDataSize())).c_str());
                break;
                case Packet::OP::JOIN:{
                    Packet req(res.bytes(), res.size());
                    req.setOP(Packet::OP::LOGIN);
                    state_label.set("join success: save session id");
                    file_manager.save_pref("session_id", req.getData(), req.getDataSize());
                    state_label.set((std::string("try to login with session id: ") + std::string(req.getData(), res.getDataSize())).c_str());
                    socket.send(req.bytes(), req.size());
                }
                break;
                case Packet::OP::LOGIN:
                    if(res.nextUInt8()){
                        state_label.set("login success!");
                        Packet req(Packet::OP::ECHO, "abcd...", size_t(6));
                        socket.send(req.bytes(), req.size());
                    }
                    else{
                        state_label.set("login fail!");
                    }
                break;
                }
            });
            //drawWorld(window.getRenderer(), &world);
        });
        mainloop.listenAfterDraw([&window, &world](){
            drawWorld(window.getRenderer(), &world);
        });
        */
        mainloop.run(60);
    }
};

// this is real main
extern "C" {void main_() {
    printf("%s\n", "game start");
    App app = App::getInstance();
    app.run("10.20.21.249", 3030);
#if defined(__EMSCRIPTEN__)
    EM_ASM(
        FS.syncfs(function (err) {
            assert(!err);
            //ccall('success', 'v');
        });
    );
#endif
}}

// this is shadow main
// * DO NOT TOUCH *
int main(int argc, char **argv){
    printf("P: %s\n", SDL_GetPrefPath("My Company", "My Awesome SDL 2 Game"));
    printf("P: %s\n", SDL_GetBasePath());
#if defined(__EMSCRIPTEN__)
    EM_ASM(
        FS.mkdir('/IDBFS');
        FS.mount(IDBFS, {}, '/IDBFS');
        FS.syncfs(true, function (err) {
            assert(!err);
            ccall('main_', 'v');
        });
    );
    emscripten_exit_with_live_runtime();
#else
    main_();
#endif
    return 0;
}
