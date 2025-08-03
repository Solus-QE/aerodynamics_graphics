#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

const char* vertexShaderSource = "#version 330 core\n" // Vertex Shader
"layout (location = 0) in vec3 aPos;\n" // Input vertex attribute at location 0
"void main()\n" // Main function of the vertex shader
	"{\n"
	"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n" // Set the position of the vertex
	"}\0";
const char* fragmentShaderSource = "#version 330 core\n" // Fragment Shader
"out vec4 FragColor;\n" // Output color of the fragment shader
	"void main()\n"
	"{\n"
	"   FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n" // Set the color of the fragment to orange
	"}\0";

int main()
{
	glfwInit();

	// Hints for OpenGL version and profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Package for OpenGL 3.3 Core Profile (package of functions)

	GLfloat vertices[] = { // Vertex data for a triangle
		/* ISOCOLES TRIANGLE
		-0.5f, -0.5f * float(sqrt(3.0f)) / 3, 0.0f, // Bottom left vertex
		 0.5f, -0.5f * float(sqrt(3.0f)) / 3, 0.0f, // Bottom right vertex
		 0.0f,  0.5f * float(sqrt(3.0f)) * 2 / 3, 0.0f // Top vertex
		*/
		// SQUARE (two right triangles)
            // First triangle
            -0.5f, -0.5f, 0.0f, // Bottom left
             0.5f, -0.5f, 0.0f, // Bottom right
             0.5f,  0.5f, 0.0f, // Top right
            // Second triangle
            -0.5f, -0.5f, 0.0f, // Bottom left
             0.5f,  0.5f, 0.0f, // Top right
            -0.5f,  0.5f, 0.0f  // Top left
	};

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


	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); // Create a vertex shader object
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // Attach the vertex shader source code to the shader object
	glCompileShader(vertexShader); // Compile the vertex shader

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Create a fragment shader object
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); // Attach the fragment shader source code to the shader object
	glCompileShader(fragmentShader); // Compile the fragment shader

	GLuint shaderProgram = glCreateProgram(); // Create a shader program object

	glAttachShader(shaderProgram, vertexShader); // Attach the vertex shader to the shader program
	glAttachShader(shaderProgram, fragmentShader); // Attach the fragment shader to the shader program

	glLinkProgram(shaderProgram); // Link the shader program

	glDeleteShader(vertexShader); // Delete the vertex shader object as it is no longer needed
	glDeleteShader(fragmentShader); // Delete the fragment shader object as it is no longer needed



	GLuint VAO, VBO; // Vertex Buffer Object and Vertex Array Object

	glGenVertexArrays(1, &VAO); // Generate a vertex array object
	glGenBuffers(1, &VBO); // Generate a buffer object

	glBindVertexArray(VAO); // Bind the vertex array object

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind the buffer object to the GL_ARRAY_BUFFER target
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Upload the vertex data to the buffer

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Define the vertex attribute pointer
	glEnableVertexAttribArray(0); // Enable the vertex attribute at location 0

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the buffer object
	glBindVertexArray(0); // Unbind the vertex array object

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set the clear color for the window
	glClear(GL_COLOR_BUFFER_BIT); // Clear the window with the clear color
	glfwSwapBuffers(window); // Swap the front and back buffers to display the cleared window

	while (!glfwWindowShouldClose(window)) // Only closes window when close button is pressed and not by itself
	{
		glClearColor(0.07f, 0.13f, 0.f, 1.0f); // Set the clear color for the window
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram); // Use the shader program
		glBindVertexArray(VAO); // Bind the vertex array object
		glDrawArrays(GL_TRIANGLES, 0, 6); // Draw the triangle using the vertex data
		glfwSwapBuffers(window); // Swap the front and back buffers to display the rendered triangle

		glfwPollEvents(); // Processes all the polled events so the window will not be in a state of not responding
	}

	glDeleteVertexArrays(1, &VAO); // Delete the vertex array object
	glDeleteBuffers(1, &VBO); // Delete the buffer object
	glDeleteProgram(shaderProgram); // Delete the shader program

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
