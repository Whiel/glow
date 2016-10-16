#pragma once

#include <glm/glm.hpp>
#include <vector>


namespace shapes
{
  namespace cube
  {
    /* Coordinates for a rather beautiful cube */
    const std::vector<glm::vec3> positions = {
      // front face
      {-1.0f, -1.0f, 1.0f}, // first triangle starts here
      { 1.0f, -1.0f, 1.0f},
      { 1.0f,  1.0f, 1.0f}, // first triangle ends here
      {-1.0f, -1.0f, 1.0f},
      { 1.0f,  1.0f, 1.0f},
      {-1.0f,  1.0f, 1.0f},
      // back face
      {-1.0f, -1.0f, -1.0f},
      { 1.0f,  1.0f, -1.0f},
      { 1.0f, -1.0f, -1.0f},  
      {-1.0f, -1.0f, -1.0f},
      {-1.0f,  1.0f, -1.0f},
      { 1.0f,  1.0f, -1.0f},  
      // left face
      {-1.0f, -1.0f, -1.0f},  
      {-1.0f, -1.0f,  1.0f},
      {-1.0f,  1.0f, -1.0f},
      {-1.0f, -1.0f,  1.0f},
      {-1.0f,  1.0f,  1.0f},
      {-1.0f,  1.0f, -1.0f},
      // right face
      {1.0f, -1.0f,  1.0f},
      {1.0f, -1.0f, -1.0f},
      {1.0f,  1.0f, -1.0f},       
      {1.0f, -1.0f,  1.0f},
      {1.0f,  1.0f, -1.0f},
      {1.0f,  1.0f,  1.0f}, 	
      // top face
      {-1.0f, 1.0f, -1.0f},
      {-1.0f, 1.0f,  1.0f}, 	
      { 1.0f, 1.0f, -1.0f},
      { 1.0f, 1.0f, -1.0f},
      {-1.0f, 1.0f,  1.0f},       
      { 1.0f, 1.0f,  1.0f},
      // bottom face
      {-1.0f, -1.0f, -1.0f} ,
      { 1.0f, -1.0f, -1.0f} , 	
      {-1.0f, -1.0f,  1.0f} ,
      { 1.0f, -1.0f, -1.0f} ,
      { 1.0f, -1.0f,  1.0f} ,
      {-1.0f, -1.0f,  1.0f} ,
    };

    /* Normals for the cube */
    const std::vector<glm::vec3> normals = {
      // front face
      {0.f, 0.f, 1.f},      
      {0.f, 0.f, 1.f},      
      {0.f, 0.f, 1.f},      
      {0.f, 0.f, 1.f},      
      {0.f, 0.f, 1.f},      
      {0.f, 0.f, 1.f},      
      // back face
      {0.f, 0.f, -1.f},      
      {0.f, 0.f, -1.f},      
      {0.f, 0.f, -1.f},      
      {0.f, 0.f, -1.f},      
      {0.f, 0.f, -1.f},      
      {0.f, 0.f, -1.f},      
      // left face
      {-1.f, 0.f, 0.f},      
      {-1.f, 0.f, 0.f},      
      {-1.f, 0.f, 0.f},      
      {-1.f, 0.f, 0.f},      
      {-1.f, 0.f, 0.f},      
      {-1.f, 0.f, 0.f},      
      // right face
      {1.f, 0.f, 0.f},      
      {1.f, 0.f, 0.f},      
      {1.f, 0.f, 0.f},      
      {1.f, 0.f, 0.f},      
      {1.f, 0.f, 0.f},      
      {1.f, 0.f, 0.f},      
      // top face
      {0.f, 1.f, 0.f},      
      {0.f, 1.f, 0.f},      
      {0.f, 1.f, 0.f},      
      {0.f, 1.f, 0.f},      
      {0.f, 1.f, 0.f},      
      {0.f, 1.f, 0.f},      
      // bottom face
      {0.f, -1.f, 0.f},      
      {0.f, -1.f, 0.f},      
      {0.f, -1.f, 0.f},      
      {0.f, -1.f, 0.f},      
      {0.f, -1.f, 0.f},      
      {0.f, -1.f, 0.f},      
    };
 
  }
}
