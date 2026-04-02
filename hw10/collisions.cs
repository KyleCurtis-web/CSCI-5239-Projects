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

//  Compute shader
void main()
{
   //  Global Thread ID
   uint  gid = gl_GlobalInvocationID.x;

    //  Get position and velocity
    vec3 p = pos[gid].xyz;
   vec3 v = vel[gid].xyz;

   //check collisions, assuming mass of all objects is 1
   for(int i = 0; i < pos.length() - 1; i++)
    {
        vec3 secondPosition = vec3(pos[i]);
        //check collision, within barrier
        if( abs(length(p - secondPosition)) < 0.01 && i != gid)
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
