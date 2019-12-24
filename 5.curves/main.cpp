#include "common/shader.hpp"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> // vec2

#include <vector>
#include <iostream>

#include "plot.hpp"


// Initial window size
const int INITIAL_WIDTH = 800;
const int INITIAL_HEIGHT = 600;

/* This function gets called in the game loop.
 * All the drawing is done here. */
plot sinc;

static void render(float time)
{
  std::vector<glm::vec2> points(1<<12);
  float anim = sin(time) * 10;
  float t = -1;
  for(auto & v : points) {
    v = glm::vec2{t, sin(t*(20+anim))/(t*(20+anim))};
    t+=40.f/points.size();
  }

  sinc.set_data(points);

  glClearColor(0.f,0.1f,0.1f,1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  sinc.color({1.,0.,0.5*(1. + sin(time)),1.});
  sinc.line_width(3.f + 2*cos(time));    
  sinc.draw();
}


/* This function is called when the window is resized 
 * ( see glfwSetWindowSizeCallback, called at main() )
 * Its only function is to resize the viewport so that it covers all the window.
 */
static void size_callback(GLFWwindow* window,int,int){
  int width,height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
}


static void debugMessage(
  GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
  const GLchar *message, const void *userParam)
{
  std::cerr << "GL debug message:  " << message << std::endl;
  // Print, log, whatever based on the enums and message
}

int main()
{  
  if (!glfwInit())
    return 1;
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
   
  GLFWwindow *window =
    glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT, "5. Plot", NULL, NULL);
  if (!window){
    std::cerr << "glfw: Failed to create the window." << std::endl;
    glfwTerminate();
    return 1;
  }  
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Limit the drawing speed to the screen refresh speed.

  /* Initialize GLEW */
  glewExperimental=true; // GLEW dies in core profile if this is not set!
  GLenum err = glewInit();
  if (GLEW_OK != err){
    std::cerr << "glew error: " << glewGetErrorString(err) << std::endl;
    return 1;
  }

  glDisable(GL_CULL_FACE);
    
  glfwSetWindowSizeCallback(window, size_callback);
  glViewport(0,0,INITIAL_WIDTH,INITIAL_HEIGHT);

  GLint flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
  {
    std::cout << "Debug context" << std::endl;
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugMessage, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);    
  }

  float time = 0.f;
  while(not glfwWindowShouldClose(window)){
    glfwWaitEventsTimeout(0.1);
    render(time);
    time += 0.1f;
    glfwSwapBuffers(window);
  }
  return 0;
}




