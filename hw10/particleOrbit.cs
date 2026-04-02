//
//  Particle compute shader
//
#version 440 compatibility
#extension GL_ARB_compute_variable_group_size : enable

//  Array positions
layout(binding=4) buffer posbuf {vec4 pos[];};
layout(binding=5) buffer velbuf {vec4 vel[];};
//  Work group size
layout(local_size_variable) in;

//  Sphere
uniform vec3 xyz;
uniform float dim;
//  Gravity
vec3   G = vec3(0.0,-9.8,0.0);//default gravity
//  Time step
const float  dt = 1.1;

//  Compute shader
void main()
{
   //  Global Thread ID
   uint  gid = gl_GlobalInvocationID.x;

   //  Get position and velocity
   vec3 p0 = pos[gid].xyz;
   vec3 v0 = vel[gid].xyz;

    float radius = length(p0 - xyz);

    //calculate gravity (vector towards center of sphere(xyz) of magnitude 9.8)
    G = p0 - xyz;
    G = normalize(G);//unit vector of our position from origin
    vec3 v = vec3(-G.y, G.x, 0);//make perpandicular so it orbits


    G *= -9.8;//give the unit vector magnitude


    v *= -98;

    //  Compute new position and velocity
    //v = v0 + G * dt;
    vec3 p = p0 + v*dt + 0.5 * dt * dt * G;
    //p = normalize(p) * radius;


   //  Test if inside sphere
   if (length(p-xyz) < dim)
   {
      //  Compute Normal
      vec3 N = normalize(p - xyz);
      //  Compute reflected velocity with damping
      v = 0.9*reflect(v0,N);
      //  Set p0 on the sphere
      p0 = xyz + dim*N;
      //  Compute reflected position
      p = p0 + v*dt + 0.5*dt*dt*G;
   }

   //  Update position and velocity
   pos[gid].xyz = p;
   vel[gid].xyz = v;
}
