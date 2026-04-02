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
const float  dt = 0.1;

const uint numParticles = 4096;

//  Compute shader
void main()
{
   //  Global Thread ID
   uint  gid = gl_GlobalInvocationID.x;

    if (gid > numParticles)
    {
        return;
    }


    //  Get position and velocity
    vec3 p0 = pos[gid].xyz;
   vec3 v0 = vel[gid].xyz;

    //calculate gravity (vector towards center of sphere(xyz) of magnitude 9.8)
    G = p0 - xyz;
    G = normalize(G);//unit vector of our position from origin

    vec3 v = G;
    //make velocity perpandicular, so that it orbits instead of falls
    uint zeroing = gid % 3;
    uint negation = gid % 2;
    if (zeroing == 0)// (y,x,0)
    {
        v = vec3(v.y, v.x, 0);
        if (negation == 0)
        {
            //-y
            v.y = -v.y;
        }
        else
        {
            //-x
            v.x = -v.x;
        }
    }
    else if (zeroing == 1)// (z, 0, x)
    {
        //
        v = vec3(v.z, 0, v.x);
        if (zeroing == 0)
        {
            //-z
            v.z = -v.z;
        }
        else
        {
            //-x
            v.x = -v.x;
        }
        v = v0;
    }
    else // (0, z, y)
    {
        //
        v = vec3(0, v.z, v.y);
        if (zeroing == 0)
        {
            //-z
            v.z = -v.z;
        }
        else
        {
            //-y
            v.y = -v.y;
        }
    }


    G *= -9.8;//give the unit vector magnitude

    v *= -98;

    //G = vec3(-G.y, G.x, 0);

    //  Compute new position and velocity
    vec3 p = p0 + v*dt + 0.5 * dt * dt * G;
    //vec3 v =  G;

 

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

    //check collisions, assuming mass of all objects is 1
    for (int i = 0; i < numParticles - 1; i++)
    {
        vec3 secondPosition = vec3(pos[i]);
        //check collision, within barrier
        if (abs(length(p - secondPosition)) < 0.01 && i != gid)
        {
            //collision happened
            vec3 secondVelocity = vec3(vel[i]);

            //first object
            //start v1 - (<v1 - v2, x1 - x2> / |x1-x2|^2) (x1 - x2)
            //x1,x2 are positions
            //<a,b> is dot product
            vec3 v1 = v;
            vec3 velocity = v - secondVelocity; //v1 - v2
            vec3 positions = p - secondPosition; // x1 - x2

            float multiplicand = dot(velocity, positions);
            float divisor = length(positions); // |x1-x2|
            divisor *= divisor; // |x1-x2| ^ 2
            //gurantee divisor is non-zero
            divisor = max(divisor, 0.0000001);

            multiplicand /= divisor;// (<v1 - v2, x1 - x2> / |x1-x2|^2)

            v = v1 - multiplicand * positions;

            //second object
            vec3 v2 = secondPosition;
            velocity = secondVelocity - v1; //v2 - v1
            positions = secondPosition - p; // x2 - x1

            multiplicand = dot(velocity, positions);
            divisor = length(positions);
            divisor *= divisor;
            divisor = max(divisor, 0.0000001);

            multiplicand /= divisor;

            vel[i] = vec4(v2 - multiplicand * positions, vel[i].w);


        }
    }


    //  Update position and velocity
    pos[gid].xyz = p;
   vel[gid].xyz = v;
}
