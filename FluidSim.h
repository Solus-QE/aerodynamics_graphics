#pragma once
#ifndef FLUIDSIM_H
#define FLUIDSIM_H

class FluidSim
{
public:
    FluidSim(int size, float diffusion, float viscosity, float dt);
    ~FluidSim();

    void step();
    void addDensity(int x, int y, float amount);
    void addVelocity(int x, int y, float amountX, float amountY);
    void getDensity(int x, int y, float& density) const;
    void getVelocity(int x, int y, float& velX, float& velY) const;
    int getSize() const;
    float getDiffusion() const;
    float getViscosity() const;
    float getDT() const;

    // Obstacle support
    void setObstacle(int x, int y, bool solid);
    void clearObstacles();
    bool isObstacle(int x, int y) const;

private:
    int size;       // grid size
    float dt;       // timestep
    float diffusion;
    float viscosity;

    float* s;       // temp density
    float* density;

    float* Vx;      // velocity x
    float* Vy;      // velocity y
    float* Vx0;     // temp velocity x
    float* Vy0;     // temp velocity y

    bool* obstacles; // obstacle grid

    int IX(int x, int y) const;

    void diffuse(int b, float* x, float* x0, float diff);
    void advect(int b, float* d, float* d0, float* velocX, float* velocY);
    void project(float* velocX, float* velocY, float* p, float* div);
    void setBoundary(int b, float* x);
};

#endif
