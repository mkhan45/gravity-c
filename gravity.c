#include <stdio.h>
#include "./deps/vec/vec.h"
#include <math.h>
#include <time.h>

const float G = 1.0;

struct vec{
    float x;
    float y;
};

float distance(struct vec pos1, struct vec pos2){
    float dx_sqr = pow(pos2.x - pos1.x, 2);
    float dy_sqr = pow(pos2.y - pos1.y, 2);
    return sqrt(dx_sqr + dy_sqr);
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
    return (G * mass) / pow(radius, 2);
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

typedef vec_t(struct Body) body_vec_t;

int main(){
    body_vec_t bodies;
    vec_init(&bodies);

    vec_push(&bodies, new_body(5.0, 5.0, 10.0, 10.0));
    vec_push(&bodies, new_body(5.0, 25.0, 100.0, 100.0));
    vec_push(&bodies, new_body(15.0, 125.0, 600.0, 600.0));

    while(1){
        for(int i = 0; i < bodies.length; i++){
            float accel_x = 0.0;
            float accel_y = 0.0;

            struct Body current = bodies.data[i];

            printf("Body # %i -> x: %f, y: %f\n", i, current.pos.x, current.pos.y);

            for(int j = 0; j < bodies.length; j++){
                struct Body other = bodies.data[j];
                if(distance(current.pos, other.pos) >= current.radius + other.radius){
                    struct vec accel = accel_vector(other.mass, current.pos, other.pos);
                    accel_x += accel.x;
                    accel_y += accel.y;
                }
            }

            bodies.data[i].vel.x += accel_x;
            bodies.data[i].vel.y += accel_y;

            bodies.data[i].pos.x += bodies.data[i].vel.x + (accel_x/2);
            bodies.data[i].pos.y += bodies.data[i].vel.y + (accel_y/2);
        }

        struct timespec time;
        struct timespec time2;
        time.tv_sec = 0;
        time.tv_nsec = 16666670;
        nanosleep(&time, &time2);
    }
}
