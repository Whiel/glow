#version 330

// This comes from the vertex shader.
// Contains the coordinates (in the range -1,1) of the fragment we are drawing.
in vec2 v_quad_position;

void main()
{
  const float iter = 200.0;
  const float threshold = 4.0;

  vec2 z = v_quad_position * normalize(vec2(16,9)) * 2;
  vec2 c = vec2(-0.4,0.6); //vec2(phi-2,phi-1);

  // f(z) = z^2 + c
  // Compute z_i = f(z_(i-1))
  // Until the norm of z grows over a threshold
  float i;
  for(i = 0.0; i<iter && dot(z,z) < threshold; ++i){
    float x = z.x*z.x - z.y*z.y;
    float y = 2*z.x*z.y;
    z = vec2(x,y)+c;
  }

  // Play around with the number of iterations until a nice coloring appears
  i = pow(i,0.5);
  // I have no good idea of how to set the color to this
  float r = 0.5*(1+cos(i+0.4)); // make sure the result range is [0,1]
  float g = 0.5*(1+cos(i+1));
  float b = 0.5*(1+cos(i+2.5));

  gl_FragColor = vec4(r,g,b*b,1.0);
  
}


