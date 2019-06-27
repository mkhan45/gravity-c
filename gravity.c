#include <stdio.h>
#include "./deps/vec/vec.h"
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

const float G = 1.0;

const int SCREEN_WIDTH = 800; 
const int SCREEN_HEIGHT = 800;

struct vec{
    float x;
    float y;
};

float distance(struct vec pos1, struct vec pos2){
    float dx_sqr = pow(pos2.x - pos1.x, 2);
    float dy_sqr = pow(pos2.y - pos1.y, 2);
    return sqrt(dx_sqr + dy_sqr); //c = sqrt(a^2 + b^2)
}

struct Body{
    float radius;
    float mass;
    struct vec pos;
    struct vec vel;
};

struct Body new_body(float r, float m, float x, float y){
    struct Body body;

    body.radius = r;
    body.mass = m;

    struct vec position;

    position.x = x;
    position.y = y;

    body.pos = position;


    struct vec velocity;

    velocity.x = 0.0;
    velocity.y = 0.0;

    body.vel = velocity;

    return body;
}

float accel_mag(float mass, float radius){
    return (G * mass) / pow(radius, 2); //a_g = G * m / r^2
}

//acceleration of pos2 towards pos1
struct vec accel_vector(float mass, struct vec pos1, struct vec pos2){
    float radius = distance(pos1, pos2);
    float mag = accel_mag(mass, radius);
    float x_comp = (pos2.x - pos1.x)/radius * mag;
    float y_comp = (pos2.y - pos1.y)/radius * mag;

    struct vec ret_vec;
    ret_vec.x = x_comp;
    ret_vec.y = y_comp;

    return ret_vec;
}

typedef vec_t(struct Body) body_vec_t; //defines the struct body_vec_t as a vector of bodies

int main(){
    body_vec_t bodies;
    vec_init(&bodies); //all vector functions use a reference to the vector

    //new_body(radius, mass, x, y)
    vec_push(&bodies, new_body(50.0, 800.0, 400.0, 400.0)); 

    //initializes body with upward velocity
    struct Body satellite = new_body(5.0, 1.0, 750.0, 400.0);
    struct vec new_vel;
    new_vel.x = 0.0; new_vel.y = -1.5;
    satellite.vel = new_vel;

    vec_push(&bodies, satellite);

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;


    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() ); 
    else{
        window = SDL_CreateWindow("SDL Window",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH, SCREEN_HEIGHT,
                SDL_WINDOW_SHOWN);
        if (window == NULL){
            printf("Error creating window: %s\n", SDL_GetError());
        }else{//SDL initializes correctly
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


            SDL_Event e;

            while(1){

                SDL_PollEvent(&e);
                if(e.type == SDL_QUIT){
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    return 0;
                }

                //for every body, iterate through every other body
                for(int i = 0; i < bodies.length; i++){
                    float accel_x = 0.0;
                    float accel_y = 0.0;

                    struct Body current = bodies.data[i];

                    for(int j = 0; j < bodies.length; j++){
                        struct Body other = bodies.data[j];

                        //if the bodies are not intersecting, apply gravitational acceleration
                        if(distance(current.pos, other.pos) >= current.radius + other.radius){
                            struct vec accel = accel_vector(other.mass, current.pos, other.pos);
                            accel_x += accel.x;
                            accel_y += accel.y;
                        }
                    }

                    //integrate
                    bodies.data[i].vel.x += accel_x;
                    bodies.data[i].vel.y += accel_y;

                    bodies.data[i].pos.x += bodies.data[i].vel.x + (accel_x/2);
                    bodies.data[i].pos.y += bodies.data[i].vel.y + (accel_y/2);

                    //draw body
                    filledCircleRGBA(renderer, current.pos.x, current.pos.y, current.radius, 255, 255, 255, 255);
                }


                SDL_RenderPresent(renderer);
                SDL_UpdateWindowSurface(window);

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);

                //remaining time goes into time2 but doesn't get used
                struct timespec time;
                struct timespec time2;
                time.tv_sec = 0;
                time.tv_nsec = 16666670;
                nanosleep(&time, &time2);
            }
        }
    }



    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
