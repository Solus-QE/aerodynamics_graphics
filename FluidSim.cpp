#include "FluidSim.h"
#include <algorithm>
#include <cmath>
#include <cstring>

inline int FluidSim::IX(int x, int y) const {
    x = std::max(0, std::min(x, size - 1));
    y = std::max(0, std::min(y, size - 1));
    return x + y * size;
}

FluidSim::FluidSim(int size, float diffusion, float viscosity, float dt)
    : size(size), diffusion(diffusion), viscosity(viscosity), dt(dt)
{
    int totalCells = size * size;
    s = new float[totalCells]();
    density = new float[totalCells]();
    Vx = new float[totalCells]();
    Vy = new float[totalCells]();
    Vx0 = new float[totalCells]();
    Vy0 = new float[totalCells]();
    obstacles = new bool[totalCells]();
}

FluidSim::~FluidSim() {
    delete[] s;
    delete[] density;
    delete[] Vx;
    delete[] Vy;
    delete[] Vx0;
    delete[] Vy0;
    delete[] obstacles;
}

void FluidSim::setObstacle(int x, int y, bool solid) {
    obstacles[IX(x, y)] = solid;
}

void FluidSim::clearObstacles() {
    int totalCells = size * size;
    memset(obstacles, 0, totalCells * sizeof(bool));
}

bool FluidSim::isObstacle(int x, int y) const {
    return obstacles[IX(x, y)];
}

void FluidSim::addDensity(int x, int y, float amount) {
    if (!isObstacle(x, y)) {
        density[IX(x, y)] += amount;
    }
}

void FluidSim::addVelocity(int x, int y, float amountX, float amountY) {
    if (!isObstacle(x, y)) {
        int idx = IX(x, y);
        Vx[idx] += amountX;
        Vy[idx] += amountY;
    }
}

void FluidSim::getDensity(int x, int y, float& outDensity) const {
    outDensity = density[IX(x, y)];
}

void FluidSim::getVelocity(int x, int y, float& velX, float& velY) const {
    int idx = IX(x, y);
    velX = Vx[idx];
    velY = Vy[idx];
}

int FluidSim::getSize() const { return size; }
float FluidSim::getDiffusion() const { return diffusion; }
float FluidSim::getViscosity() const { return viscosity; }
float FluidSim::getDT() const { return dt; }

void FluidSim::setBoundary(int b, float* x) {
    for (int i = 1; i < size - 1; i++) {
        x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
        x[IX(i, size - 1)] = b == 2 ? -x[IX(i, size - 2)] : x[IX(i, size - 2)];
        x[IX(0, i)] = b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
        x[IX(size - 1, i)] = b == 1 ? -x[IX(size - 2, i)] : x[IX(size - 2, i)];
    }

    x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
    x[IX(0, size - 1)] = 0.5f * (x[IX(1, size - 1)] + x[IX(0, size - 2)]);
    x[IX(size - 1, 0)] = 0.5f * (x[IX(size - 2, 0)] + x[IX(size - 1, 1)]);
    x[IX(size - 1, size - 1)] = 0.5f * (x[IX(size - 2, size - 1)] + x[IX(size - 1, size - 2)]);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (obstacles[IX(i, j)]) x[IX(i, j)] = 0.0f;
        }
    }
}

void FluidSim::diffuse(int b, float* x, float* x0, float diff) {
    float a = dt * diff * (size - 2) * (size - 2);
    for (int k = 0; k < 20; k++) {
        for (int i = 1; i < size - 1; i++) {
            for (int j = 1; j < size - 1; j++) {
                if (!obstacles[IX(i, j)]) {
                    x[IX(i, j)] = (x0[IX(i, j)] + a * (
                        x[IX(i - 1, j)] + x[IX(i + 1, j)] +
                        x[IX(i, j - 1)] + x[IX(i, j + 1)]
                    )) / (1 + 4 * a);
                }
            }
        }
        setBoundary(b, x);
    }
}

void FluidSim::advect(int b, float* d, float* d0, float* u, float* v) {
    float dt0 = dt * size;
    for (int i = 1; i < size - 1; i++) {
        for (int j = 1; j < size - 1; j++) {
            if (obstacles[IX(i, j)]) {
                d[IX(i, j)] = 0.0f;
                continue;
            }

            float x = i - dt0 * u[IX(i, j)];
            float y = j - dt0 * v[IX(i, j)];

            if (x < 0.5f) x = 0.5f;
            if (x > size - 1.5f) x = size - 1.5f;
            if (y < 0.5f) y = 0.5f;
            if (y > size - 1.5f) y = size - 1.5f;

            int i0 = static_cast<int>(x);
            int i1 = i0 + 1;
            int j0 = static_cast<int>(y);
            int j1 = j0 + 1;

            float s1 = x - i0;
            float s0 = 1 - s1;
            float t1 = y - j0;
            float t0 = 1 - t1;

            if (obstacles[IX(i0, j0)] || obstacles[IX(i0, j1)] ||
                obstacles[IX(i1, j0)] || obstacles[IX(i1, j1)]) {
                d[IX(i, j)] = 0.0f;
            }
            else {
                d[IX(i, j)] = s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
                    s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
            }
        }
    }
    setBoundary(b, d);
}

void FluidSim::project(float* u, float* v, float* p, float* div) {
    for (int i = 1; i < size - 1; i++) {
        for (int j = 1; j < size - 1; j++) {
            if (obstacles[IX(i, j)]) {
                div[IX(i, j)] = 0;
                p[IX(i, j)] = 0;
                continue;
            }
            div[IX(i, j)] = -0.5f * (
                u[IX(i + 1, j)] - u[IX(i - 1, j)] +
                v[IX(i, j + 1)] - v[IX(i, j - 1)]
            ) / size;
            p[IX(i, j)] = 0;
        }
    }
    setBoundary(0, div);
    setBoundary(0, p);

    for (int k = 0; k < 20; k++) {
        for (int i = 1; i < size - 1; i++) {
            for (int j = 1; j < size - 1; j++) {
                if (!obstacles[IX(i, j)]) {
                    p[IX(i, j)] = (div[IX(i, j)] +
                        p[IX(i - 1, j)] + p[IX(i + 1, j)] +
                        p[IX(i, j - 1)] + p[IX(i, j + 1)]) / 4;
                }
            }
        }
        setBoundary(0, p);
    }

    for (int i = 1; i < size - 1; i++) {
        for (int j = 1; j < size - 1; j++) {
            if (!obstacles[IX(i, j)]) {
                u[IX(i, j)] -= 0.5f * size * (p[IX(i + 1, j)] - p[IX(i - 1, j)]);
                v[IX(i, j)] -= 0.5f * size * (p[IX(i, j + 1)] - p[IX(i, j - 1)]);
            }
        }
    }
    setBoundary(1, u);
    setBoundary(2, v);
}

void FluidSim::step() {
    // Diffuse velocity
    diffuse(1, Vx0, Vx, viscosity);
    std::swap(Vx, Vx0);

    diffuse(2, Vy0, Vy, viscosity);
    std::swap(Vy, Vy0);

    // Project velocity
    project(Vx, Vy, Vx0, Vy0);

    // Advect velocity
    advect(1, Vx0, Vx, Vx, Vy);
    advect(2, Vy0, Vy, Vx, Vy);
    std::swap(Vx, Vx0);
    std::swap(Vy, Vy0);

    // Project again
    project(Vx, Vy, Vx0, Vy0);

    // Diffuse density
    diffuse(0, s, density, diffusion);
    std::swap(s, density);

    // Advect density
    advect(0, s, density, Vx, Vy);
    std::swap(s, density);
}