#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main()
{
	glfwInit();

	// Hints for OpenGL version and profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Package for OpenGL 3.3 Core Profile (package of functions)

	GLFWwindow* window = glfwCreateWindow(800, 800, "Aerodynamics Graphics", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // Make the window part of the current context

	gladLoadGL(); // Load OpenGL functions using glad


	glViewport(0, 0, 800, 800); // Set the viewport to the size of the window; the viewport goes from (0, 0) to (800, 800)

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set the clear color for the window
	glClear(GL_COLOR_BUFFER_BIT); // Clear the window with the clear color
	glfwSwapBuffers(window); // Swap the front and back buffers to display the cleared window

	while (!glfwWindowShouldClose(window)) // Only closes window when close button is pressed and not by itself
	{
		glfwPollEvents(); // Processes all the polled events so the window will not be in a state of not responding
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
