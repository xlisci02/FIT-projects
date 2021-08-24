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
 * @param p       - particles
 * @param tmp_vel - temp array for velocities
 * @param N       - Number of particles
 * @param dt      - Size of the time step
 */
__global__ void calculate_gravitation_velocity(t_particles p, t_velocities tmp_vel, int N, float dt)
{

  // Get global tread id
  int p1_index = threadIdx.x + blockIdx.x * blockDim.x;

  // If thread index within range
  if (p1_index < N){
    float r, dx, dy, dz;

    // Preloading of particle position data to registers
    float pos_x = p.pos_x[p1_index];
    float pos_y = p.pos_y[p1_index];
    float pos_z = p.pos_z[p1_index];

    // Temporary variables for partial sums
    float tmp_vel_x = 0.0f, tmp_vel_y = 0.0f, tmp_vel_z = 0.0f;

    // Computing gravitation velocity (from CPU, version 2)
    for(int p2_index = 0; p2_index < N; p2_index++){
      // First, we compute what we can
      float Fg_dt_m2_r = -G * dt * p.weight[p2_index];
      // Get the position differences
      dx = pos_x - p.pos_x[p2_index];
      dy = pos_y - p.pos_y[p2_index];
      dz = pos_z - p.pos_z[p2_index];

      // Compute r, based on the position differencies
      r = sqrt(dx*dx + dy*dy + dz*dz);
      // Finish computing Fg_dt_m2_r with r
      Fg_dt_m2_r /= (r * r * r + FLT_MIN);

      // Compute impact of particle on index p2_index
      tmp_vel_x += (r > COLLISION_DISTANCE) ? Fg_dt_m2_r * dx : 0.0f;
      tmp_vel_y += (r > COLLISION_DISTANCE) ? Fg_dt_m2_r * dy : 0.0f;
      tmp_vel_z += (r > COLLISION_DISTANCE) ? Fg_dt_m2_r * dz : 0.0f;
    }
    // Use temp velocity arrays to save the velocity changes
    tmp_vel.x[p1_index] = tmp_vel_x;
    tmp_vel.y[p1_index] = tmp_vel_y;
    tmp_vel.z[p1_index] = tmp_vel_z;
  }
}// end of calculate_gravitation_velocity
//----------------------------------------------------------------------------------------------------------------------

/**
 * CUDA kernel to calculate collision velocity
 * @param p       - particles
 * @param tmp_vel - temp array for velocities
 * @param N       - Number of particles
 * @param dt      - Size of the time step
 */
__global__ void calculate_collision_velocity(t_particles p, t_velocities tmp_vel, int N, float dt)
{
  // Get global tread id.
  int p1_index = threadIdx.x + blockIdx.x * blockDim.x;

  // If thread index within range
  if (p1_index < N){
    float r, dx, dy, dz;

    // Preloading of particle data to registers
    float pos_x = p.pos_x[p1_index];
    float pos_y = p.pos_y[p1_index];
    float pos_z = p.pos_z[p1_index];
    float weight = p.weight[p1_index];
    float vel_x = p.vel_x[p1_index];
    float vel_y = p.vel_y[p1_index];
    float vel_z = p.vel_z[p1_index];

    // Temporary variables for partial sums
    float tmp_vel_x = 0.0f, tmp_vel_y = 0.0f, tmp_vel_z = 0.0f;

    // Computing gravitation velocity (from CPU, version 2)
    for(int p2_index = 0; p2_index < N; p2_index++){
      // Get the position differences
      dx = pos_x - p.pos_x[p2_index];
      dy = pos_y - p.pos_y[p2_index];
      dz = pos_z - p.pos_z[p2_index];

      // Compute r, based on the position differencies
      r = sqrtf(dx*dx + dy*dy + dz*dz);

      // if r is within range
      if (r > 0.0f && r < COLLISION_DISTANCE){
        // Precompute values to registers
        float w = p.weight[p2_index];
        float weight_sum = weight + w;
        float weight_diff = weight - w;
        float weight_double = w * 2;

        // Compute impact of particle on index p2_index
        tmp_vel_x += ((vel_x * (weight_diff) + weight_double* p.vel_x[p2_index]) / (weight_sum)) - vel_x;
        tmp_vel_y += ((vel_y * (weight_diff) + weight_double* p.vel_y[p2_index]) / (weight_sum)) - vel_y;
        tmp_vel_z += ((vel_z * (weight_diff) + weight_double* p.vel_z[p2_index]) / (weight_sum)) - vel_z;
      }
    }
    // Use temp velocity arrays to save the velocity changes
    tmp_vel.x[p1_index] += tmp_vel_x;
    tmp_vel.y[p1_index] += tmp_vel_y;
    tmp_vel.z[p1_index] += tmp_vel_z;
  }
}// end of calculate_collision_velocity
//----------------------------------------------------------------------------------------------------------------------

/**
 * CUDA kernel to update particles
 * @param p       - particles
 * @param tmp_vel - temp array for velocities
 * @param N       - Number of particles
 * @param dt      - Size of the time step
 */
__global__ void update_particle(t_particles p, t_velocities tmp_vel, int N, float dt)
{
  int p1_index = threadIdx.x + blockIdx.x * blockDim.x;
  if (p1_index < N){
    p.vel_x[p1_index] += tmp_vel.x[p1_index];
    p.vel_y[p1_index] += tmp_vel.y[p1_index];
    p.vel_z[p1_index] += tmp_vel.z[p1_index];

    p.pos_x[p1_index] += p.vel_x[p1_index] * dt;
    p.pos_y[p1_index] += p.vel_y[p1_index] * dt;
    p.pos_z[p1_index] += p.vel_z[p1_index] * dt;
  }
}// end of update_particle
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
