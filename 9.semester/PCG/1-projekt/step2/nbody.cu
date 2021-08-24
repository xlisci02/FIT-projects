/**
 * @File nbody.cu
 *
 * Implementation of the N-Body problem
 *
 * Paralelní programování na GPU (PCG 2020)
 * Projekt c. 1 (cuda)
 * Login: xlisci02
 */

 #include <cmath>
 #include <cfloat>
 #include "nbody.h"

 /**
  * CUDA kernel to calculate gravitation velocity
  * @param p_in       - input particles data
  * @param p_out      - output particles data
  * @param N       - Number of particles
  * @param dt      - Size of the time step
  */
 __global__ void calculate_velocity(t_particles p_in, t_particles p_out, int N, float dt)
 {
  extern __shared__ float shared_particles_data[];

   // Get global tread id.
  int p1_index = threadIdx.x + blockIdx.x * blockDim.x;

  // If thread index within range
  if (p1_index < N){
    float r, dx, dy, dz;

    // Preloading of particle data to registers
    float pos_x = p_in.pos_x[p1_index];
    float pos_y = p_in.pos_y[p1_index];
    float pos_z = p_in.pos_z[p1_index];
    float weight = p_in.weight[p1_index];
    float vel_x = p_in.vel_x[p1_index];
    float vel_y = p_in.vel_y[p1_index];
    float vel_z = p_in.vel_z[p1_index];

    // Temporary variables for partial sums
    float tmp_vel_x = 0.0f, tmp_vel_y = 0.0f, tmp_vel_z = 0.0f;

    // Save pointers to shared memory
    float *sh_pos_x = &shared_particles_data[0];
    float *sh_pos_y = &shared_particles_data[blockDim.x];
    float *sh_pos_z = &shared_particles_data[2 * blockDim.x];
    float *sh_weight = &shared_particles_data[3 * blockDim.x];
    float *sh_vel_x = &shared_particles_data[4 * blockDim.x];
    float *sh_vel_y = &shared_particles_data[5 * blockDim.x];
    float *sh_vel_z = &shared_particles_data[6 * blockDim.x];

    // Loop, moving with the size of the tile (threads/block)
    for(int i = 0, tile = 0; i < N; i += blockDim.x, tile++){

      // Index with the respect to tile
      int idx = tile * blockDim.x + threadIdx.x;

      // Load current tile data from global memory to shared memory
      sh_pos_x[threadIdx.x] =  p_in.pos_x[idx];
      sh_pos_y[threadIdx.x] =  p_in.pos_y[idx];
      sh_pos_z[threadIdx.x] =  p_in.pos_z[idx];
      sh_weight[threadIdx.x] =  p_in.weight[idx];
      sh_vel_x[threadIdx.x] =  p_in.vel_x[idx];
      sh_vel_y[threadIdx.x] =  p_in.vel_y[idx];
      sh_vel_z[threadIdx.x] =  p_in.vel_z[idx];

      // Synchronization on block level
      // Every thread should have the same data in shared memory before continuing
      __syncthreads();

      // Computing gravitation & collision velocity using shared memory
      for (int j = 0; j < blockDim.x; j++){
        float w = sh_weight[j];
        // Precompute value to registers
        float Fg_dt_m2_r = -G * dt * w;
        // Get the position differences
        dx = pos_x - sh_pos_x[j];
        dy = pos_y - sh_pos_y[j];
        dz = pos_z - sh_pos_z[j];

        r = sqrt(dx*dx + dy*dy + dz*dz);

        // Gravitation
        Fg_dt_m2_r /= (r * r * r + FLT_MIN);

        tmp_vel_x += (r > COLLISION_DISTANCE) ? Fg_dt_m2_r * dx : 0.0f;
        tmp_vel_y += (r > COLLISION_DISTANCE) ? Fg_dt_m2_r * dy : 0.0f;
        tmp_vel_z += (r > COLLISION_DISTANCE) ? Fg_dt_m2_r * dz : 0.0f;

        // Collision
        if (r > 0.0f && r < COLLISION_DISTANCE){
          // Precompute values to registers
          float weight_sum = weight + w;
          float weight_diff = weight - w;
          float weight_double = w * 2;

          tmp_vel_x += ((vel_x * (weight_diff) + weight_double* sh_vel_x[j]) / (weight_sum)) - vel_x;
          tmp_vel_y += ((vel_y * (weight_diff) + weight_double* sh_vel_y[j]) / (weight_sum)) - vel_y;
          tmp_vel_z += ((vel_z * (weight_diff) + weight_double* sh_vel_z[j]) / (weight_sum)) - vel_z;

        }
      }
      // Thread synchronization before writing to shared memory in next iteration
      __syncthreads();
    }
    // Update velocities and positions
    p_out.vel_x[p1_index] = vel_x + tmp_vel_x;
    p_out.vel_y[p1_index] = vel_y + tmp_vel_y;
    p_out.vel_z[p1_index] = vel_z + tmp_vel_z;

    p_out.pos_x[p1_index] = pos_x + ((vel_x + tmp_vel_x) * dt);
    p_out.pos_y[p1_index] = pos_y + ((vel_y + tmp_vel_y) * dt);
    p_out.pos_z[p1_index] = pos_z + ((vel_z + tmp_vel_z) * dt);
   }
 }// end of calculate_velocity
 //----------------------------------------------------------------------------------------------------------------------

 /**
  * CPU implementation of the Center of Mass calculation
  * @param particles - All particles in the system
  * @param N         - Number of particles
  */
 __host__ float4 centerOfMassCPU(MemDesc& memDesc)
 {
   float4 com = {0 ,0, 0, 0};

   for(int i = 0; i < memDesc.getDataSize(); i++)
   {
     // Calculate the vector on the line connecting points and most recent position of center-of-mass
     const float dx = memDesc.getPosX(i) - com.x;
     const float dy = memDesc.getPosY(i) - com.y;
     const float dz = memDesc.getPosZ(i) - com.z;

     // Calculate weight ratio only if at least one particle isn't massless
     const float dw = ((memDesc.getWeight(i) + com.w) > 0.0f)
                           ? ( memDesc.getWeight(i) / (memDesc.getWeight(i) + com.w)) : 0.0f;

     // Update position and weight of the center-of-mass according to the weight ration and vector
     com.x += dx * dw;
     com.y += dy * dw;
     com.z += dz * dw;
     com.w += memDesc.getWeight(i);
   }
   return com;
 }// enf of centerOfMassCPU
 //----------------------------------------------------------------------------------------------------------------------
