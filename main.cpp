#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <vector>
#include <cmath>
#include "FluidSim.h"

const int simSize = 128;
// Increased time step and reduced viscosity for faster flow
FluidSim fluid(simSize, 0.00001f, 0.0000001f, 0.2f);

// Shader sources for obstacle
const char* obstacleVertexShaderSource = R"(#version 330 core
layout (location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    FragColor = vec4(0.2, 0.5, 1.0, 1.0);  // Blue
})";

const char* obstacleFragmentShaderSource = R"(#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
})";

// Brighter fluid shader
const char* pointVertexShaderSource = R"(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aIntensity;
out float Intensity;
void main()
{
    gl_Position = vec4(aPos, 1.0);
    gl_PointSize = 12.0;  // Larger points
    Intensity = aIntensity;
})";

const char* pointFragmentShaderSource = R"(#version 330 core
in float Intensity;
out vec4 FragColor;
void main()
{
    vec3 color = vec3(1.0, 0.0, 0.0);  // Red
    FragColor = vec4(color * (0.5 + Intensity * 2.0), 1.0);  // Brighter
})";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "aerodynamics", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 800);
    glDisable(GL_DEPTH_TEST);

    // Set up wind tunnel boundaries
    for (int i = 0; i < simSize; i++) {
        fluid.setObstacle(i, 0, true);           // Bottom wall
        fluid.setObstacle(i, simSize - 1, true);    // Top wall
    }

    // Set up obstacle - centered in wind tunnel
    int obsSize = simSize / 8;
    int obsStartX = simSize / 2;
    int obsEndX = obsStartX + obsSize;
    int obsStartY = simSize / 2 - obsSize / 2;
    int obsEndY = obsStartY + obsSize;

    for (int i = obsStartX; i < obsEndX; ++i) {
        for (int j = obsStartY; j < obsEndY; ++j) {
            fluid.setObstacle(i, j, true);
        }
    }

    // Create obstacle shaders and VAO
    GLuint obstacleShaderProgram = glCreateProgram();
    {
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &obstacleVertexShaderSource, NULL);
        glCompileShader(vertexShader);

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &obstacleFragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        glAttachShader(obstacleShaderProgram, vertexShader);
        glAttachShader(obstacleShaderProgram, fragmentShader);
        glLinkProgram(obstacleShaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    // Compute obstacle vertices in NDC
    float ndc_x0 = (obsStartX / (float)simSize) * 2.0f - 1.0f;
    float ndc_x1 = (obsEndX / (float)simSize) * 2.0f - 1.0f;
    float ndc_y0 = (obsStartY / (float)simSize) * 2.0f - 1.0f;
    float ndc_y1 = (obsEndY / (float)simSize) * 2.0f - 1.0f;

    GLfloat obstacleVertices[] = {
        ndc_x0, ndc_y0, 0.0f,
        ndc_x1, ndc_y0, 0.0f,
        ndc_x1, ndc_y1, 0.0f,
        ndc_x0, ndc_y0, 0.0f,
        ndc_x1, ndc_y1, 0.0f,
        ndc_x0, ndc_y1, 0.0f
    };

    GLuint obstacleVAO, obstacleVBO;
    glGenVertexArrays(1, &obstacleVAO);
    glGenBuffers(1, &obstacleVBO);

    glBindVertexArray(obstacleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, obstacleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(obstacleVertices), obstacleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Create point shader program
    GLuint pointShaderProgram = glCreateProgram();
    {
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &pointVertexShaderSource, NULL);
        glCompileShader(vertexShader);

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &pointFragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        glAttachShader(pointShaderProgram, vertexShader);
        glAttachShader(pointShaderProgram, fragmentShader);
        glLinkProgram(pointShaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    // Create VAO and VBO for fluid points
    GLuint pointVAO, pointVBO;
    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);

    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    glBufferData(GL_ARRAY_BUFFER, simSize * simSize * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Create point data structure
    struct FluidPoint {
        float x, y, z;
        float intensity;
    };
    std::vector<FluidPoint> pointData(simSize * simSize);

    // Initialize point positions
    for (int i = 0; i < simSize; i++) {
        for (int j = 0; j < simSize; j++) {
            int idx = i * simSize + j;
            pointData[idx].x = (static_cast<float>(i) / simSize) * 2.0f - 1.0f;
            pointData[idx].y = (static_cast<float>(j) / simSize) * 2.0f - 1.0f;
            pointData[idx].z = 0.0f;
            pointData[idx].intensity = 0.0f;
        }
    }

    // Initialize intensity grid
    std::vector<float> intensityGrid(simSize * simSize, 0.0f);

    // Add strong initial fluid
    int injectionStart = simSize / 3;
    int injectionEnd = 2 * simSize / 3;
    for (int j = injectionStart; j < injectionEnd; j++) {
        fluid.addDensity(2, j, 3000.0f);
        fluid.addVelocity(2, j, 100.0f, 0.0f);
        intensityGrid[2 * simSize + j] = 1.0f;
    }

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Add constant strong fluid input
        for (int j = injectionStart; j < injectionEnd; j++) {
            if (!fluid.isObstacle(2, j)) {
                fluid.addDensity(2, j, 500.0f);
                fluid.addVelocity(2, j, 30.0f, 0.0f);
                intensityGrid[2 * simSize + j] = 1.0f;
            }
        }

        fluid.step();

        // Update intensity grid based on density
        for (int i = 0; i < simSize; i++) {
            for (int j = 0; j < simSize; j++) {
                if (fluid.isObstacle(i, j)) {
                    intensityGrid[i * simSize + j] = 0.0f;
                    continue;
                }

                float d;
                fluid.getDensity(i, j, d);
                float targetIntensity = d / 1000.0f; // Fixed scale for consistent brightness
                if (targetIntensity > 1.0f) targetIntensity = 1.0f;

                // Smooth transition with high persistence
                intensityGrid[i * simSize + j] =
                    0.75f * intensityGrid[i * simSize + j] +
                    0.25f * targetIntensity;
            }
        }

        // Update point intensities
        for (int i = 0; i < simSize; i++) {
            for (int j = 0; j < simSize; j++) {
                int idx = i * simSize + j;
                pointData[idx].intensity = intensityGrid[idx];
            }
        }

        // Update VBO
        glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, pointData.size() * sizeof(FluidPoint), pointData.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw fluid points
        glUseProgram(pointShaderProgram);
        glBindVertexArray(pointVAO);
        glDrawArrays(GL_POINTS, 0, pointData.size());
        glBindVertexArray(0);
        glUseProgram(0);

        // Draw obstacle
        glUseProgram(obstacleShaderProgram);
        glBindVertexArray(obstacleVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &pointVAO);
    glDeleteBuffers(1, &pointVBO);
    glDeleteProgram(pointShaderProgram);
    glDeleteVertexArrays(1, &obstacleVAO);
    glDeleteBuffers(1, &obstacleVBO);
    glDeleteProgram(obstacleShaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}