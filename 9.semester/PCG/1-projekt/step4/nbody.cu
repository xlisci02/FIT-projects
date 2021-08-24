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

  if (p1_index < N){
    float r, dx, dy, dz;

    float pos_x = p_in.pos_x[p1_index];
    float pos_y = p_in.pos_y[p1_index];
    float pos_z = p_in.pos_z[p1_index];
    float weight = p_in.weight[p1_index];
    float vel_x = p_in.vel_x[p1_index];
    float vel_y = p_in.vel_y[p1_index];
    float vel_z = p_in.vel_z[p1_index];

    float tmp_vel_x = 0.0f, tmp_vel_y = 0.0f, tmp_vel_z = 0.0f;

    float *sh_pos_x = &shared_particles_data[0];
    float *sh_pos_y = &shared_particles_data[blockDim.x];
    float *sh_pos_z = &shared_particles_data[2 * blockDim.x];
    float *sh_weight = &shared_particles_data[3 * blockDim.x];
    float *sh_vel_x = &shared_particles_data[4 * blockDim.x];
    float *sh_vel_y = &shared_particles_data[5 * blockDim.x];
    float *sh_vel_z = &shared_particles_data[6 * blockDim.x];


    for(int i = 0, tile = 0; i < N; i += blockDim.x, tile++){

      int idx = tile * blockDim.x + threadIdx.x;

      sh_pos_x[threadIdx.x] =  p_in.pos_x[idx];
      sh_pos_y[threadIdx.x] =  p_in.pos_y[idx];
      sh_pos_z[threadIdx.x] =  p_in.pos_z[idx];
      sh_weight[threadIdx.x] =  p_in.weight[idx];
      sh_vel_x[threadIdx.x] =  p_in.vel_x[idx];
      sh_vel_y[threadIdx.x] =  p_in.vel_y[idx];
      sh_vel_z[threadIdx.x] =  p_in.vel_z[idx];
      __syncthreads();

      for (int j = 0; j < blockDim.x; j++){
        float w = sh_weight[j];
        float Fg_dt_m2_r = -G * dt * w;
        dx = pos_x - sh_pos_x[j];
        dy = pos_y - sh_pos_y[j];
        dz = pos_z - sh_pos_z[j];

        r = sqrt(dx*dx + dy*dy + dz*dz);

        // gravitation
        Fg_dt_m2_r /= (r * r * r + FLT_MIN);

        tmp_vel_x += (r > COLLISION_DISTANCE) ? Fg_dt_m2_r * dx : 0.0f;
        tmp_vel_y += (r > COLLISION_DISTANCE) ? Fg_dt_m2_r * dy : 0.0f;
        tmp_vel_z += (r > COLLISION_DISTANCE) ? Fg_dt_m2_r * dz : 0.0f;

        // collision
        if (r > 0.0f && r < COLLISION_DISTANCE){
          float weight_sum = weight + w;
          float weight_diff = weight - w;
          float weight_double = w * 2;

          tmp_vel_x += ((vel_x * (weight_diff) + weight_double* sh_vel_x[j]) / (weight_sum)) - vel_x;
          tmp_vel_y += ((vel_y * (weight_diff) + weight_double* sh_vel_y[j]) / (weight_sum)) - vel_y;
          tmp_vel_z += ((vel_z * (weight_diff) + weight_double* sh_vel_z[j]) / (weight_sum)) - vel_z;

        }
      }
      __syncthreads();
    }
    // update
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
 * CUDA kernel to update particles
 * @param p       - particles
 * @param comX    - pointer to a center of mass position in X
 * @param comY    - pointer to a center of mass position in Y
 * @param comZ    - pointer to a center of mass position in Z
 * @param comW    - pointer to a center of mass weight
 * @param lock    - pointer to a user-implemented lock
 * @param N       - Number of particles
 */
 __global__ void centerOfMass(t_particles p, float* comX, float* comY, float* comZ, float* comW, int* lock, const int N)
 {
  // Shared memory
  extern __shared__ float center_of_mass_data[];
  // Get pointers to shared memory.
  float * sh_com_dataX = &center_of_mass_data[0];
  float * sh_com_dataY = &center_of_mass_data[blockDim.x];
  float * sh_com_dataZ = &center_of_mass_data[2 * blockDim.x];
  float * sh_com_dataW = &center_of_mass_data[3 * blockDim.x];

  // Thread id
  unsigned int tid = threadIdx.x;

  // Zero shared memory
  float myX = 0.0f;
  float myY = 0.0f;
  float myZ = 0.0f;
  float myW = 0.0f;

  // In case that we have more particles than threads
  for (unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < N; idx += gridDim.x  * blockDim.x){
    // Sum only product of position and weight,
    // but in registers, not in shared memory
    myX += p.pos_x[idx] * p.weight[idx];
    myY += p.pos_y[idx] * p.weight[idx];
    myZ += p.pos_z[idx] * p.weight[idx];
    myW += p.weight[idx];
  }

  // Warp-synchronous reduction, same as in the PCG lecture/lab
  for (unsigned int stride = 16; stride > 0; stride >>= 1)
    myX += __shfl_down_sync(0xffffffff, myX, stride);
  for (unsigned int stride = 16; stride > 0; stride >>= 1)
    myY += __shfl_down_sync(0xffffffff, myY, stride);
  for (unsigned int stride = 16; stride > 0; stride >>= 1)
    myZ += __shfl_down_sync(0xffffffff, myZ, stride);
  for (unsigned int stride = 16; stride > 0; stride >>= 1)
    myW += __shfl_down_sync(0xffffffff, myW, stride);

  // Stacking results together, prevent warp divergence
  if(tid % 32 == 0){
    sh_com_dataX[tid/32] = myX;
    sh_com_dataY[tid/32] = myY;
    sh_com_dataZ[tid/32] = myZ;
    sh_com_dataW[tid/32] = myW;
  }
  // Synchronize, all threads in a block should have same shared data
  __syncthreads();

  // Reduction using shared memory as in step3.1
  // but now we have 32 times smaller stride at the begining
  // because of warp-synchronous reduction used above
  for (unsigned int stride = blockDim.x / 64; stride > 0; stride >>= 1){
    if (tid < stride){
      unsigned int neigh_tid = tid + stride;
      sh_com_dataX[tid] += sh_com_dataX[neigh_tid];
      sh_com_dataY[tid] += sh_com_dataY[neigh_tid];
      sh_com_dataZ[tid] += sh_com_dataZ[neigh_tid];
      sh_com_dataW[tid] += sh_com_dataW[neigh_tid];
    }
    // Synchronize, before writing to shared memory in next iteration
    __syncthreads();
  }

  // Sum all partial results into global memory,
  // after sum we have to divide comX, comY, comZ by total weight (on CPU)
  if(!tid ){ // only one thread from a block writes to global memory
    // Trying to lock using atomic change and swap operation
    while(atomicCAS(lock, 0, 1));
    // Critical section
    *comX += sh_com_dataX[0];
    *comY += sh_com_dataY[0];
    *comZ += sh_com_dataZ[0];
    *comW += sh_com_dataW[0];
    // Unlock the lock using atomic exchange operation
    atomicExch(lock, 0);
  }


 }// end of centerOfMass
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
