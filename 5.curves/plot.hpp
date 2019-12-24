#pragma once

#include <algorithm>

class plot {
public: // Construct - copy - destroy
  plot(){}

  template <typename Collection>
  plot(const Collection & x){
    set_data(x);
  }

  // The trivial copy-constructor does not work, since we own a resource
  // that would be shared by copies, you can implement your own.
  plot(const plot &) = delete; 
  plot& operator=(const plot &) = delete;
  
  virtual ~plot() {
    release_buffer();
    release_shader();
  }

public: // interface
  template <typename Collection> 
  void set_data(const Collection & points){
    int length = std::distance(begin(points),end(points));
    if (_buffer_length < length){
      release_buffer();
      allocate_buffer(length);
      std::cout << "Allocate " << length << std::endl;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    float * mapped = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    std::for_each(begin(points),end(points), [mapped] (auto & v) mutable{
                                               *(mapped++) = v.x;
                                               *(mapped++) = v.y;
                                             });
    glUnmapBuffer(GL_ARRAY_BUFFER);
  }

  void draw(){
    if(_vao and _buffer){
      // For smooth lines
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable( GL_LINE_SMOOTH );
      glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

      // Line width
      glLineWidth(_line_width);
      
      glUseProgram(shader());
      glBindVertexArray(_vao);
      glDrawArrays(GL_LINE_STRIP,0,_buffer_length);
      glBindVertexArray(0);
    }
  }
  
  void color(const glm::vec4 & color) {
    shader(); // Ensure the shader exists
    glUniform4f(_ul_color, color.x, color.y, color.z, color.w);
  }

  void line_width(float pixels){
    _line_width = pixels;
  }
  
private: // private methods

  void allocate_buffer(int length) {
    glGenVertexArrays(1,&_vao);
    glGenBuffers(1,&_buffer);

    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vao);
    glBufferData(GL_ARRAY_BUFFER, 2*length*sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(POSITION_INDEX, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(POSITION_INDEX);

    glBindVertexArray(0);
    _buffer_length = length;
  }

  GLuint shader() {
    if(_shader == 0){
      _shader = shaders::build_program("./plot.vert","./plot.frag");
      _ul_color = glGetUniformLocation(_shader, "u_color");
    }
    return _shader;
  }
  
  void release_buffer() {
    if(_vao) glDeleteVertexArrays(1,&_vao);
    _vao = 0;
    if(_buffer) glDeleteBuffers(1,&_buffer);
    _buffer = 0;
  }

  void release_shader() {
    if(_shader) glDeleteShader(_shader);
    _shader = 0;
  }
  
private: // attribs
  int _buffer_length = 0;  
  GLuint _vao = 0;
  GLuint _buffer = 0;
  GLuint _shader = 0;
  float _line_width = 1.f;
  
  GLuint _ul_color = 0; // color uniform location
  
  static constexpr GLint POSITION_INDEX = 0;
};
