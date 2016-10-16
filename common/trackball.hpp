#pragma once

#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

#include <cmath>

namespace util
{
  namespace detail
  {
    static glm::vec3 point_to_sphere(glm::vec2 point, float radius,
                                     glm::vec2 center)
    {
      float x = point[0] - center[0];
      float y = -point[1] + center[1];
      float z = 0.0f; 
    
      float rad2 = radius*radius;
      float screen_rad2 = x*x + y*y;
    
      if (screen_rad2 < rad2){
        z = rad2/std::sqrt(rad2)+std::sqrt(rad2 - screen_rad2);
      }
      else{
        z = rad2 / std::sqrt(screen_rad2);
      }
      return glm::normalize(glm::vec3(x, y, z));
    }
  }
  

  class trackball
  {
  public:
    trackball() :
      _speed(2.f),
      _radius(1.f),
      _center(glm::vec2(0.0f, 0.0f)),
      _tracking(false),
      _previous_point(glm::vec3(0.0f, 0.0f, 1.0f)),
      _current(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
    {}
  
    void radius(double radius)
    {
      _radius = radius;
    }

    double radius() const
    {
      return _radius;
    }

    void center(glm::vec2 center)
    {
      _center = center;
    }

    glm::vec2 center() const
    {
      return _center;
    }

    void start_tracking(glm::vec2 point)
    {
      _previous_point = detail::point_to_sphere(point, _radius, _center);
      _tracking = true;
    }

    void move(glm::vec2 point)
    {
      glm::vec3 current_point = detail::point_to_sphere(point, _radius, _center);
      glm::vec3 rotation_axis = glm::cross(_previous_point, current_point);
      float angle = _speed*asin(glm::clamp(glm::length(rotation_axis),-1.f,1.f));
      glm::quat q = glm::angleAxis(angle, glm::normalize(rotation_axis));
      q = glm::normalize(q);
      _current = glm::normalize(glm::cross(q, _current));
      _previous_point = current_point;
    
    }

    void stop_tracking()
    {
      _tracking = false;
    }
  
    bool tracking() const
    {
      return _tracking;
    }
  
    glm::mat4 matrix() const
    {
      return glm::mat4_cast(_current);
    }

    void speed(float s)
    {
      _speed = s;
    }

    float speed() const
    {
      return _speed;
    }
    
  private:
    float _speed;
    float _radius;
    glm::vec2 _center;
    bool _tracking;
    glm::vec3 _previous_point; 
    glm::quat _current;
  };
}
