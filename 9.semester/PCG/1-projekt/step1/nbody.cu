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

     // Computing gravitation & collision velocity
    for(int p2_index = 0; p2_index < N; p2_index++){
      // Preload to register
      float w = p_in.weight[p2_index];
      // Precompute value to registers
      float Fg_dt_m2_r = -G * dt * w;
      // Get the position differences
      dx = pos_x - p_in.pos_x[p2_index];
      dy = pos_y - p_in.pos_y[p2_index];
      dz = pos_z - p_in.pos_z[p2_index];

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

        tmp_vel_x += ((vel_x * (weight_diff) + weight_double* p_in.vel_x[p2_index]) / (weight_sum)) - vel_x;
        tmp_vel_y += ((vel_y * (weight_diff) + weight_double* p_in.vel_y[p2_index]) / (weight_sum)) - vel_y;
        tmp_vel_z += ((vel_z * (weight_diff) + weight_double* p_in.vel_z[p2_index]) / (weight_sum)) - vel_z;
      }
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
