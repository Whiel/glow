#pragma once

// This parses an obj file.
// It is probably not worth to spend much time analyzing it.
// Here is a description of the format.
//  https://en.wikipedia.org/wiki/Wavefront_.obj_file
//  http://paulbourke.net/dataformats/obj/
// It is plain text, you can open it and try to figure it out


#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <iostream>


namespace obj
{
  static std::string  rtrim(std::string s) {
    auto first = std::find_if(s.rbegin(), s.rend(),
                              [](auto ch){return !std::isspace(ch);});
    s.erase(first.base(), s.end());
    return s;
  }

  static std::string ltrim(std::string s) {
    auto last =  std::find_if(s.begin(), s.end(),
                              [](auto ch){return !std::isspace(ch);});
    s.erase(s.begin(),last);
    return s;
  }

  static std::string trim(std::string s) {
    return ltrim(rtrim(s));
  }


  class obj_file
  {
  public:
    obj_file(const std::string & filename)
      : _in_file(filename)
    {    
      std::string line;
      _good = _in_file.good();
      while(std::getline(_in_file,line)){
        if (line[0] == 'o'){
          _offsets.emplace(trim(line.substr(2)),_in_file.tellg());
        }
      }
    }    
  
    
    void get_object(const std::string & name,
                    std::vector<glm::vec3> & coords,
                    std::vector<glm::vec3> & normals,
                    std::vector<glm::vec2> & tex_coords)
    {
      const auto offset_it = _offsets.find(name);
      if (offset_it != end(_offsets)){
        _in_file.clear();
        _in_file.seekg(offset_it->second);
        read_object(coords,normals,tex_coords);
      }
      else throw std::runtime_error("Object " + name + "not present in .obj file");
    }
    
    
    
    std::vector<std::string> objects() const
    {
      std::vector<std::string> o(_offsets.size());
      std::transform(begin(_offsets),end(_offsets), begin(o),
                     [](auto it){return it.first;});
      return o;
    }
    
    bool good() const
    {
      return _good;
    }

    
  private:
    void read_object(std::vector<glm::vec3> & coords,
                     std::vector<glm::vec3> & normals,
                     std::vector<glm::vec2> & tex_coords) 
    {
      std::vector<glm::vec3> unordered_coords;
      std::vector<glm::vec2> unordered_tex_coords;
      std::vector<glm::vec3> unordered_normals;
      
      std::string tag;
      std::string line = "";
      glm::vec3 v3,center,coord;
      glm::vec2 v2;

      int coord_idx = 0;
      int tex_idx = 0;
      int norm_idx = 0;
      center = glm::vec3(0,0,0);
    
      while (std::getline(_in_file, line)){
        if (line.size() > 3){
          std::istringstream reader(line);
          std::getline(reader,tag,' ');
          if(tag == "v"){
            reader >> v3[0]; reader >> v3[1]; reader >> v3[2];
            unordered_coords.push_back(v3);
          }else if(tag == "vt"){
            reader >> v2[0]; reader >> v2[1];
            unordered_tex_coords.push_back(v2);
          }else if(tag == "vn"){
            reader >> v3[0]; reader >> v3[1]; reader >> v3[2];
            unordered_normals.push_back(v3);          
          }else if(tag == "f"){
            std::string str_indices;
            for(int i = 1; i <= 3; ++i){
              std::getline(reader,str_indices,' ');
              std::replace(begin(str_indices),end(str_indices),'/',' ');
              std::istringstream reader_indices(str_indices);
              if(! (reader_indices >> coord_idx))
                throw std::runtime_error("Expected coordinates");
              tex_idx = norm_idx = 0;
              if(reader_indices >> tex_idx){
                if(!(reader_indices >> norm_idx)){
                  norm_idx = tex_idx;
                  tex_idx = 0;
                }
              }
              if(coord_idx){
                if (coord_idx < 0)
                  coord_idx = unordered_coords.size() + coord_idx;
                else
                  --coord_idx;
                coord = unordered_coords.at(coord_idx);
                coords.push_back(coord);
                center += coord;
              }
              if(norm_idx){
                if (norm_idx < 0)
                  norm_idx = unordered_normals.size() + norm_idx;
                else
                  --norm_idx;
                normals.push_back(unordered_normals.at(norm_idx));
              }
              if(tex_idx){
                if (tex_idx < 0)
                  tex_idx = unordered_tex_coords.size() + tex_idx;
                else
                  --tex_idx;
                tex_coords.push_back(unordered_tex_coords.at(tex_idx));
              }
                                         
            }
          }
        }
      }
      center=center / float(coords.size());
      std::transform(begin(coords),end(coords),begin(coords),
                     [&center](auto v){return v - center;});
    }
        
    bool _good;
    std::ifstream _in_file;
    std::unordered_map<std::string, std::streamoff> _offsets;
  };
}
