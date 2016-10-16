#include "common/shader.hpp"
#include "common/trackball.hpp"
#include "common/obj.hpp"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // For lookAt, perspective
#include <glm/gtx/transform.hpp> // For rotate
#include <glm/gtc/type_ptr.hpp> // For value_ptr

#include <vector>
#include <iostream>

const float Pi = 3.141592653589793;

/* Indices of things passed to the vertex shader.*/
static const int POSITION_INDEX = 0;
static const int NORMAL_INDEX   = 1;

// Initial window size
const int INITIAL_WIDTH  = 800;
const int INITIAL_HEIGHT = 600;
const float INITIAL_FOV  = Pi/4.f;



struct scene_state
{
  scene_state()
  {
    // Set initial values
    displacing = false;
    mouse_pos = glm::vec2(0,0);
    camera_position = glm::vec3(0,0,4);
    width = INITIAL_WIDTH;
    height = INITIAL_HEIGHT; 
    fov = INITIAL_FOV;
  }

  glm::mat4 view() const
  {
    // The trackball provides a rotation matrix, the translation comes from
    // the camera position.
    return glm::translate(glm::mat4(),-camera_position) * trackball.matrix();
  }
  
  util::trackball trackball;
  glm::vec3 camera_position;

  bool displacing;
  glm::vec2 mouse_pos;
  
  void update_projection()
  {
    const float near = 0.1f;
    const float far  = 100.f;
    float aspect_ratio = float(width)/height;
    projection = glm::perspective(fov, aspect_ratio, near, far);
  }

  void update_trackball()
  {
    trackball.radius(std::max(width,height)/2.f);
    trackball.center(glm::vec2(width/2.f,height/2.f));    
  }
  
  glm::mat4 projection;
  float fov;
  
  int width;
  int height;
};

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

  glm::mat4 transform; 
  GLuint vertex_array;
  GLuint position_buffer;
  GLuint normal_buffer;
  int vertices;
};


/* Creates a model with given coordinates and normals. 
 * Very similar to the old create_cube_model
 */
static model model_from_data(std::vector<glm::vec3> & coords,
                             std::vector<glm::vec3> & normals)
{
  model m;

  glGenBuffers(1,&m.position_buffer);
  glGenBuffers(1,&m.normal_buffer);
  glGenVertexArrays(1,&m.vertex_array);
  
  glBindVertexArray(m.vertex_array);

  // Positions
  glBindBuffer(GL_ARRAY_BUFFER, m.position_buffer);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(glm::vec3) * coords.size(),
               coords.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(POSITION_INDEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(POSITION_INDEX);

  // normals
  glBindBuffer(GL_ARRAY_BUFFER, m.normal_buffer);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(glm::vec3) * normals.size(),
               normals.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(NORMAL_INDEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(NORMAL_INDEX);

  m.vertices = std::min(coords.size(),normals.size());
  
  glBindVertexArray(0);
  return m;
}

// Loads the coordinates and positions from a file using some old code
// I wrote somwhen. Then uses model_from_data to make a model with it.
static model load_model(const std::string & filename)
{
  
  std::vector<glm::vec3> coords;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> tex_coords; // Texture coordinates are ignored in this example
  
  obj::obj_file f (filename);
  std::cout << "File '" << filename << "' contains objects:" << std::endl;
  for (std::string object : f.objects())
    std::cout << "   " << object << std::endl;
  std::cout << "Only the first is loaded" << std::endl;
  f.get_object(f.objects().front(), coords, normals, tex_coords);

  return model_from_data(coords,normals);
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
  static model cube = load_model("../models/teapot.obj");
  static GLuint program = shaders::build_program("./shade.vert","./shade.frag");
  static GLint u_mvp_loc = glGetUniformLocation(program, "u_mvp");
  static GLint u_normal_mat_loc = glGetUniformLocation(program, "u_normal_mat");

  glUseProgram(program);
  glClearColor(0.2f,0.2f,0.25f,1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 mvp = projection*view*cube.transform;
  glUniformMatrix4fv(u_mvp_loc,1,GL_FALSE,glm::value_ptr(mvp));

  glm::mat3 nm = glm::transpose(glm::inverse(glm::mat3(view*cube.transform)));
  glUniformMatrix3fv(u_normal_mat_loc,1,GL_FALSE,glm::value_ptr(nm));
  
  render_model(cube);   
}


static void size_callback(GLFWwindow* window,int,int){
  scene_state & state = *(scene_state*)glfwGetWindowUserPointer(window);
  glfwGetFramebufferSize(window, &state.width, &state.height);
  glViewport(0, 0, state.width, state.height);
  
  state.update_trackball();
  state.update_projection();
}



static void mouse_button_callback(GLFWwindow* window,
                                  int button, int action, int mods)
{
  scene_state & state = *(scene_state*)glfwGetWindowUserPointer(window);
  
  if (button == GLFW_MOUSE_BUTTON_LEFT){
    if(action == GLFW_PRESS)
      state.trackball.start_tracking(state.mouse_pos);
    else
      state.trackball.stop_tracking();
  }
  else if (button == GLFW_MOUSE_BUTTON_RIGHT){
    state.displacing = (action == GLFW_PRESS);
  }
  
}

static void mouse_move_callback(GLFWwindow* window,
                                double x, double y){
  float translation_speed = 0.002;
  glm::vec2 mouse_pos(x,y);
  scene_state & state = *(scene_state*)glfwGetWindowUserPointer(window);
  
  if(state.trackball.tracking()){
    state.trackball.move(glm::vec2(x,y));
  }
  if(state.displacing){
    glm::vec2 delta = mouse_pos - state.mouse_pos;
    glm::vec3 up =  glm::vec3(0,1,0);
    glm::vec3 right =  glm::vec3(-1,0,0);
    state.camera_position += translation_speed * (up*delta.y + right*delta.x);
  }
  state.mouse_pos = mouse_pos;
}

static void scroll_callback(GLFWwindow* window,double dx, double dy){
  scene_state & state = *(scene_state*)glfwGetWindowUserPointer(window);
  float step = 1/100.f;

  state.fov = glm::clamp(state.fov+step*dy, 0.001, Pi - 0.1);
  state.update_projection();
}



int main()
{
  scene_state state;
  if (!glfwInit())
    return 1;
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
  
  GLFWwindow *window =
    glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT, "4. model", NULL, NULL);
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

  glfwSetWindowUserPointer(window, &state);
  
  glfwSetWindowSizeCallback(window, size_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, mouse_move_callback);
  glfwSetScrollCallback(window, scroll_callback);

  size_callback(window,INITIAL_WIDTH,INITIAL_HEIGHT);  
  
  while(not glfwWindowShouldClose(window)){
    glfwWaitEvents();
    render(state.projection,state.view());
    glfwSwapBuffers(window);
  }
  return 0;
}




