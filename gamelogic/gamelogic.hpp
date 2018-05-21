#include "chipmunk.h"
//#include "sdlwrap.hpp"
#include <vector>
#include <stdio.h>
namespace gamelogic {
    enum class COLLISION_TYPE: unsigned int {
        WORM, 
    }
    class Object {
        cpBody *body;
        cpShape *shape;
        //Sprite* sprites;
        /*virtual void render(SDL_Renderer* renderer){
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }*/
    };
    class Worm {
    public:
        static constexpr const float SPEED = 10.0;
        enum class ACTION: int{
            LEFT, RIGHT, JUMP, STOP
        };
        enum class STATE: int{
            STANDING, MOVING_RIGHT, MOVING_LEFT, JUMPING
        };
    private:
        cpBody *body;
        cpShape *shape;
        STATE last_state = STATE::STANDING;
        STATE state = STATE::STANDING;
        bool user_control = false;
    public:
        Worm(float x, float y){
            float mass = 5.0;
            float r = 5, f = 0.7;
            float w = 32, h = 8;
            int verts_num = 4;
            cpVect verts[] = {cpv(-w/2, h/2), cpv(w/2, h/2), cpv(w/2, -h/2), cpv(-w/2, -h/2)};
            body = cpBodyNew(mass, cpMomentForPoly(mass, verts_num, verts, cpvzero, 1.0));
            cpBodySetPosition(body, cpv(x, y));
            shape = cpPolyShapeNewRaw(body, verts_num, verts, 1.0);
            cpShapeSetFriction(shape, f);
            cpShapeSetCollisionType(shape, 0);
        }
        ~Worm(){
            cpBodyFree(body);
        }
        cpBody* getBody(){ return body; }
        cpShape* getShape(){ return shape; }
        void userControlStart(){
            user_control = true;
        }
        void userControlStop(){
            user_control = false;
        }
        void control(ACTION action){
            switch(action){
                case ACTION::LEFT:
                    if(state != STATE::JUMPING)
                        state = STATE::MOVING_LEFT;
                break;
                case ACTION::RIGHT:
                    if(state != STATE::JUMPING){
                        state = STATE::MOVING_RIGHT;
                    }
                break;
                case ACTION::JUMP:
                    if(state != STATE::JUMPING){
                        cpBodySetForce(body, cpv(0, -10000));
                        last_state = state;
                        state = STATE::JUMPING;
                    }
                break;
                case ACTION::STOP:
                    if(state != STATE::JUMPING){
                        state = STATE::STANDING;
                    }
                break;
            }
        }
        void update(){
            last_state = state;
            auto v = cpBodyGetVelocity(body);
            switch(state){
                case STATE::MOVING_LEFT:
                    cpBodySetVelocity(body, cpv(v.x < -SPEED? v.x: -SPEED, v.y));
                    //cpBodySetForce(body, cpv(-400, 0));
                    break;
                case STATE::MOVING_RIGHT:
                    cpBodySetVelocity(body, cpv(v.x > SPEED? v.x: SPEED, v.y));
                    //cpBodySetForce(body, cpv(400, 0));
                    break;
                //case STATE::JUMPING:
            }
        }
    };

    class World {
        std::vector<Worm*> worms;
        cpSpace* space;
        cpFloat time;
        float width = 800., height = 600., wall_margin = 50., wall_friction = 0.9;
        std::vector<cpVect> walls = {
            {0 + wall_margin, wall_margin}, 
            {0 + wall_margin, height-wall_margin}, 
            {width - wall_margin, height-wall_margin},
            {width - wall_margin, wall_margin}, 
        };
    public:
        World(){
            time = 0.;
            space = cpSpaceNew();
            cpSpaceSetGravity(space, cpv(0, 100));
            cpShape* wall_shape;
            for(int i=0, l=walls.size()-1; i<l; ++i){
                wall_shape = cpSegmentShapeNew(cpSpaceGetStaticBody(space), walls[i], walls[i+1], 0);
                cpShapeSetFriction(wall_shape, wall_friction);
                cpSpaceAddShape(space, wall_shape);
            }
        }
        float getHeight(){ return height; }
        float getWidth(){ return width; }
        std::vector<cpVect>& getWalls(){
            return walls;
        }
        int genWorm(){
            Worm* worm = new Worm(height-wall_margin - 10, height-wall_margin - 10);
            worms.push_back(worm);
            cpSpaceAddBody(space, worm->getBody());
            cpSpaceAddShape(space, worm->getShape());
            return worms.size()-1;
        }
        Worm* getWorm(int id){
            return worms[id];
        }
        std::vector<Worm*>& getWorms(){
            return worms;
        }
        cpShape* ground, *left_wall, *right_wall;
        int serialize(char* buf) {
            // How to handle 1k objects on the screen?
            return 1;
        }
        char* deserialize(char* buf) {
            return NULL;
        }
        int serializeDelta(char* buf){
            return 0;
        }
        void update(float time_step){
            time += time_step;
            for(auto&& worm: worms)
                worm->update();
            cpSpaceStep(space, time_step);
            //printf("%d\n", time_step);
        }
    };
};
