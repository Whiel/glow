#version 330

/* This shader implements a simplified version of what 
 * is known as the blinn-phong shading model.
 * It gives each fragment an amount of light depending on their position relative
 * to the light and the camera.
 * 
 * https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model
 *
 * I considered adding this example before loading models because without
 * any shading the objects will look like silhouettes of one single solid color.
 * A simpler shading model (eg: normal·constant vector + bias) could do the trick too.
 *
 * You can disable and enable the different components, or change their coefficients
 * to see how they affect the result.
 *
 * The gamma correction at the end compensates for the nonlinear response of the
 * screen to color values. One may expect that a pixel with color 0.5,0.5,0.5 
 * is half as bright as it can be, but it is about 20%. 
 * That is somehow used to represent the whole range of light intensities the 
 * monitor can display, as perceived by the human eye.
 * https://en.wikipedia.org/wiki/Gamma_correction
 */

// This comes from the vertex shader:
in vec3 v_normal;
in vec3 v_pos;

void main()
{
  /* Parameters, should probably be uniforms or come from the arrays in a more
   * practical case. */
  vec3 object_color = vec3(0.4,0.4,0.9);  
  vec3 light_color = vec3(0.8,0.8,0.8);
  vec3 light_source = vec3(-2,1.5,3);
  vec3 light_direction = normalize(light_source - v_pos);
  vec3 camera = vec3(0,4,4);  
  float specular_power = 64.0;

  vec3 halfway_vector = normalize((light_source - v_pos) + (camera - v_pos));
  vec3 color = object_color * light_color;
  // This term simulates scattered light
  vec3 ambient_color = 0.02 * color;
  // This term adds light depending on the angle of the surface and the light
  vec3 diffuse_color = 0.9 * color*max(dot(v_normal,light_direction),0);
  // This term adds this sharp reflections when the light and the camera are at
  // the same angle
  vec3 specular_color = 1.0 * vec3(1,1,1)*pow(max(dot(v_normal, halfway_vector),0),specular_power);
  vec3 linear_color = ambient_color + diffuse_color + specular_color;
  
  // Gamma correction, 
  vec3 corrected_color = pow(linear_color, vec3(1.0/2.2));
  
  gl_FragColor = vec4(clamp(corrected_color,0,1),1);
}


