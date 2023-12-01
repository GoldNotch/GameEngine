#include <cstdlib>
#include <cstdio>
#include <GLFW/glfw3.h>
#include <Engine.h>

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::printf("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	usConstructOptions opts{ usVideoOptions{800, 600} };
	USEnginePtr engine = usEngine_Create(opts);


	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

	}
	usEngine_Destroy(engine);

	glfwTerminate();
	return 0;
}