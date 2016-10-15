#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>

#include "common/shader.hpp"

/* Indices of things passed to the vertex shader.
 * These are the X in 'layout (location=X)' */
static const int POSITION_INDEX = 0;


// Initial window size
const int INITIAL_WIDTH = 800;
const int INITIAL_HEIGHT = 600;


/* Coordinates for a beautiful quad */
static const float quad_positions[] = {
   1.0f, 1.0f, 0.0f,
  -1.0f,-1.0f, 0.0f,
   1.0f,-1.0f, 0.0f,
  
  -1.0f,-1.0f, 0.0f,
   1.0f, 1.0f, 0.0f,
  -1.0f, 1.0f, 0.0f,
};


/* I will use this structure to hold everything related to an object we can
 * draw. It will evolve from example to example.
 * If you are fluent in C++ you will probably want to make this a class, add 
 * constructors, destructors, forbid copy... For sake of example I will try to 
 * keep the code short, even at a cost in functionality. */
struct model
{
  model()
  {
    vertices = 0;
    vertex_array = 0;
    vertex_buffer = 0;
  }

  GLuint vertex_array;
  GLuint vertex_buffer;
  int vertices; // Number of vertices in the array
};

/* Creates the vertex and array buffer for a quad (jargon for 4-sided planar figure) 
 * The only attribute that will be passed to the Vertex shader is in location
 * POSITION_INDEX. POSITION_INDEX is defined as 0 up there ^ */
static model create_quad_model()
{
  model quad;
  
  glGenBuffers(1,&quad.vertex_buffer);
  glGenVertexArrays(1,&quad.vertex_array);
  
  glBindVertexArray(quad.vertex_array);

  glBindBuffer(GL_ARRAY_BUFFER, quad.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_positions),
               quad_positions, GL_STATIC_DRAW);
  glVertexAttribPointer(POSITION_INDEX, 3, GL_FLOAT, GL_FALSE, 0, NULL); // Look at https://www.opengl.org/sdk/docs/man4/html/glVertexAttribPointer.xhtml
  glEnableVertexAttribArray(POSITION_INDEX);
  
  // Here i count the number of vertices.
  // I could have hardcoded the number (6), but this way you are free to modify
  //  quad_positions and still get a sensible result.
  // I just divide the size of the array and the size of a vertex.
  quad.vertices = sizeof(quad_positions) / (sizeof(float)*3);
  
  glBindVertexArray(0);

  return quad;
}

/* This function just renders a model */
static void render_model(const model & m)
{
  if(m.vertex_array and m.vertex_buffer and m.vertices){
    glBindVertexArray(m.vertex_array);
    glDrawArrays(GL_TRIANGLES,0,m.vertices);
    glBindVertexArray(0);
  }else{
    std::cerr << "Attempt to render invalid model" << std::endl;
  }
}

/* This function gets called in the game loop.
 * All the drawing is done here. */
static void render()
{
  static model quad = create_quad_model();
  static GLuint program = shaders::build_program("./identity.vert","./julia.frag");

  glUseProgram(program);
  glClearColor(1.f,0.1f,0.1f,1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  render_model(quad);
}


/* This function is called when the window is resized 
 * ( see glfwSetWindowSizeCallback, called at main() )
 * Its only function is to resize the viewport so that it covers all the window.
 */
static void size_callback(GLFWwindow* window,int,int){
  int width,height;
  /* This gets the size of the part of the window where we can draw.
   * Sometimes the size of the window includes the borders and the top bar,
   *  that is why we dont use glfwGetWindowSize*/
  glfwGetFramebufferSize(window, &width, &height);
  /* Here we set the viewport, the part of the window where we want to draw,
   * to cover the whole window */
  glViewport(0, 0, width, height);

  // The swapping is done in the loop
}



int main()
{  
  if (!glfwInit())
    return 1;
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
  
  GLFWwindow *window =
    glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT, "1. Julia", NULL, NULL);
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

  while(not glfwWindowShouldClose(window)){
    glfwWaitEvents();
    render();
    glfwSwapBuffers(window);
  }
  return 0;
}




