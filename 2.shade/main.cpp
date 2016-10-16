#include "common/shapes.hpp"
#include "common/shader.hpp"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // For lookAt, perspective
#include <glm/gtx/transform.hpp> // For rotate
#include <glm/gtc/type_ptr.hpp> // For value_ptr

#include <vector>
#include <iostream>



/* Indices of things passed to the vertex shader.
 * These are the X in 'layout (location=X)' 
 * As we have two arguments for the vertex shader (normals and positions)
 *  we need two indices.*/
static const int POSITION_INDEX = 0;
static const int NORMAL_INDEX   = 1;

// Initial window size
const int INITIAL_WIDTH = 800;
const int INITIAL_HEIGHT = 600;

/* Represents an oject in the scene */ 
struct model
{
  model()
  {
    vertices        = 0;
    vertex_array    = 0;
    position_buffer = 0;
    normal_buffer   = 0;
  }

  glm::mat4 transform; // default-constructed as an identity matrix
  GLuint vertex_array;
  GLuint position_buffer;
  GLuint normal_buffer; // < new!
  int vertices;
};

/* Creates a model with the coordinates and normals from shapes::cube. 
 * Two buffers are used, one for each parameter. 
 */
static model create_cube_model()
{
  model cube;
  
  glGenBuffers(1,&cube.position_buffer);
  glGenBuffers(1,&cube.normal_buffer);
  glGenVertexArrays(1,&cube.vertex_array);
  
  glBindVertexArray(cube.vertex_array);

  // see https://www.opengl.org/sdk/docs/man4/html/glBindBuffer.xhtml
  //  OpenGL semantics, 'target' is the name of the first argument of this
  //  function and the next one, refers to those 'attachments' in the Context.
  glBindBuffer(GL_ARRAY_BUFFER, cube.position_buffer);
  // see https://www.opengl.org/sdk/docs/man4/html/glBufferData.xhtml
  //  for the meaning of GL_STATIC_DRAW
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(glm::vec3) * shapes::cube::positions.size(),
               shapes::cube::positions.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(POSITION_INDEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(POSITION_INDEX);

  /* New */
  // Same as before with the new buffer
  glBindBuffer(GL_ARRAY_BUFFER, cube.normal_buffer);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(glm::vec3) * shapes::cube::normals.size(),
               shapes::cube::normals.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(NORMAL_INDEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(NORMAL_INDEX);

  // better trick:
  cube.vertices = shapes::cube::positions.size();
  
  glBindVertexArray(0);

  return cube;
}

/* This function just renders a model */
static void render_model(const model & m)
{
  if(m.vertex_array and m.position_buffer and m.vertices){
    glBindVertexArray(m.vertex_array);
    glDrawArrays(GL_TRIANGLES,0,m.vertices);
    glBindVertexArray(0);
  }else{
    std::cerr << "Attempt to render an invalid model" << std::endl;
  }
}

/* This function gets called in the game loop.
 * All the drawing is done here. */
static void render(const glm::mat4 & projection, const glm::mat4 & view)
{
  static model cube = create_cube_model();
  static GLuint program = shaders::build_program("./shade.vert","./shade.frag");
  static GLint u_mvp_loc = glGetUniformLocation(program, "u_mvp");
  static GLint u_normal_mat_loc = glGetUniformLocation(program, "u_normal_mat");

  glUseProgram(program);
  glClearColor(0.2f,0.2f,0.25f,1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  
  /* We calculate our transformation matrix mvp = projection * view * model
   * The model matrix belongs to the model. If there were several models
   * (two cubes, for example), we'd want one model per cube so that they
   * are transformed differently.
   * I call the product mvp (as in model-view-projection).
   * The projection-view product should be done in the CPU if possible.
   * The CPU is very fast and the calculation is done only once regardless of 
   * the number of vertices. 
   * If done in the Shader, the same matrix product would be repeated once per
   * vertex. 
   * The product by the model matrix generally follows the same rule.
   * The matrix-position product is done in the GPU because it is
   * different for every vertex.
   */
  glm::mat4 mvp = projection*view*cube.transform;
  // See https://www.opengl.org/sdk/docs/man/html/glUniform.xhtml
  //  Its all about suffixes in the function's name.
  // glm::value_ptr converts glm objects to void pointers as opengl wants them.
  //  (this is, presumably, at zero cost)
  glUniformMatrix4fv(u_mvp_loc,1,GL_FALSE,glm::value_ptr(mvp));

  /* Normals also need to be transformed,
   * The transform must keep the orthogonality of the normal regardless of the
   * scale, but otherwise is the same as the view*model
   * If mvp is orthogonal, and usually is, view*model does the job, but 
   * if you scale things non uniformly it stops working.
   * I don't know the math behind that transpose of inverse, tho.
   */
  glm::mat3 nm = glm::transpose(glm::inverse(glm::mat3(view*cube.transform)));
  glUniformMatrix3fv(u_normal_mat_loc,1,GL_FALSE,glm::value_ptr(nm));
  
  render_model(cube);  
  // Spin it right round round, like a record.
  cube.transform = glm::rotate(0.01f, glm::vec3(0.4,0.2,1)) * cube.transform;
  
  
}


static void size_callback(GLFWwindow* window,int,int){
  int width,height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
}

int main()
{  
  if (!glfwInit())
    return 1;
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
  
  GLFWwindow *window =
    glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT, "2. Shading", NULL, NULL);
  if (!window){
    std::cerr << "glfw: Failed to create the window." << std::endl;
    glfwTerminate();
    return 1;
  }  
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); 

  glewExperimental=true;
  GLenum err = glewInit();
  if (GLEW_OK != err){
    std::cerr << "glew error: " << glewGetErrorString(err) << std::endl;
    return 1;
  }

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
    
  glfwSetWindowSizeCallback(window, size_callback);
  glViewport(0,0,INITIAL_WIDTH,INITIAL_HEIGHT);

  
  /* new */
  float fov = 3.14159f/4.f;
  // Initialize the projection matrix, I will clean this a bit in the next example
  glm::mat4 projection
    = glm::perspective(fov, float(INITIAL_WIDTH)/INITIAL_HEIGHT, 0.1f, 100.f);
  glm::mat4 view
    = glm::lookAt(glm::vec3(0,4,4),  // eye: where the camera is
                  glm::vec3(0,0,0),  // centre: where the camera points
                  glm::vec3(0,1,0)); // up (+y)  
  
  while(not glfwWindowShouldClose(window)){
    glfwPollEvents();
    render(projection,view);
    glfwSwapBuffers(window);
  }
  return 0;
}




