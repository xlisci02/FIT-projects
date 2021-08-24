/**
 * @file      nbody.cpp
 *
 * @author    Matus Liscinsky (xlisci02) \n
 *            Faculty of Information Technology \n
 *            Brno University of Technology \n
 *            xlisci02@stud.fit.vutbr.cz
 *
 * @brief     PCG Assignment 2
 *            N-Body simulation in ACC
 *
 * @version   2021
 *
 * @date      11 November  2020, 11:22 (created) \n
 * @date      11 November  2020, 11:37 (revised) \n
 *
 */

#include <math.h>
#include <cfloat>
#include "nbody.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Declare following structs / classes                                          //
//                                  If necessary, add your own classes / routines                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Compute gravitation velocity
void calculate_gravitation_velocity(const Particles& p,
                                    Velocities&      tmp_vel,
                                    const int        N,
                                    const float      dt)
{
  float r, dx, dy, dz;
  // Variables for partial sums
  float tmp_vel_x = 0.0f, tmp_vel_y = 0.0f, tmp_vel_z = 0.0f;

  #pragma acc parallel loop present(p, tmp_vel) gang worker vector
  for (unsigned i = 0; i < N; i++){
    // Preloading of particle position data to registers
    float pos_x = p.pos_x[i];
    float pos_y = p.pos_y[i];
    float pos_z = p.pos_z[i];

    // Computing gravitation velocity (from CPU, version 2)
    #pragma acc loop seq
    for (unsigned j = 0; j < N; j++){
      // First, we compute what we can
      float Fg_dt_m2_r = -G * dt * p.weight[j];
      // Get the position differences
      dx = pos_x - p.pos_x[j];
      dy = pos_y - p.pos_y[j];
      dz = pos_z - p.pos_z[j];

      // Compute r, based on the position difference
      r = sqrtf(dx*dx + dy*dy + dz*dz);
      // Finish computing Fg_dt_m2_r with r
      Fg_dt_m2_r /= (r * r * r + FLT_MIN);

      // Compute impact of particle on index j
      tmp_vel_x += (r > COLLISION_DISTANCE) ? Fg_dt_m2_r * dx : 0.0f;
      tmp_vel_y += (r > COLLISION_DISTANCE) ? Fg_dt_m2_r * dy : 0.0f;
      tmp_vel_z += (r > COLLISION_DISTANCE) ? Fg_dt_m2_r * dz : 0.0f;
    }
    // Use temp velocity arrays to save the velocity changes
    tmp_vel.x[i] = tmp_vel_x;
    tmp_vel.y[i] = tmp_vel_y;
    tmp_vel.z[i] = tmp_vel_z;
    tmp_vel_x = 0.0f, tmp_vel_y = 0.0f, tmp_vel_z = 0.0f;
  }
}// end of calculate_gravitation_velocity
//----------------------------------------------------------------------------------------------------------------------

void calculate_collision_velocity(const Particles& p,
                                  Velocities&      tmp_vel,
                                  const int        N,
                                  const float      dt)
{
  float r, dx, dy, dz;
  // Variables for partial sums
  float tmp_vel_x = 0.0f, tmp_vel_y = 0.0f, tmp_vel_z = 0.0f;

  #pragma acc parallel loop present(p, tmp_vel) gang worker vector
  for (unsigned i = 0; i < N; i++){
    // Preloading of particle data to registers
    float pos_x = p.pos_x[i];
    float pos_y = p.pos_y[i];
    float pos_z = p.pos_z[i];
    float weight = p.weight[i];
    float vel_x = p.vel_x[i];
    float vel_y = p.vel_y[i];
    float vel_z = p.vel_z[i];

    // Computing gravitation velocity (from CPU, version 2)
    #pragma acc loop seq
    for (unsigned j = 0; j < N; j++){
      // Get the position differences
      dx = pos_x - p.pos_x[j];
      dy = pos_y - p.pos_y[j];
      dz = pos_z - p.pos_z[j];

      // Compute r, based on the position difference
      r = sqrtf(dx*dx + dy*dy + dz*dz);

      // if r is within range
      if (r > 0.0f && r < COLLISION_DISTANCE){
        // Precompute values to registers
        float w = p.weight[j];
        float weight_sum = weight + w;
        float weight_diff = weight - w;
        float weight_double = w * 2;

        // Compute impact of particle on index j
        tmp_vel_x += ((vel_x * (weight_diff) + weight_double* p.vel_x[j]) / (weight_sum)) - vel_x;
        tmp_vel_y += ((vel_y * (weight_diff) + weight_double* p.vel_y[j]) / (weight_sum)) - vel_y;
        tmp_vel_z += ((vel_z * (weight_diff) + weight_double* p.vel_z[j]) / (weight_sum)) - vel_z;
      }
    }
    // Use temp velocity arrays to save the velocity changes
    tmp_vel.x[i] += tmp_vel_x;
    tmp_vel.y[i] += tmp_vel_y;
    tmp_vel.z[i] += tmp_vel_z;
    tmp_vel_x = 0.0f, tmp_vel_y = 0.0f, tmp_vel_z = 0.0f;
  }
}// end of calculate_collision_velocity
//----------------------------------------------------------------------------------------------------------------------

/// Update particle position
void update_particle(const Particles& p,
                     Velocities&      tmp_vel,
                     const int        N,
                     const float      dt)
{
  #pragma acc parallel loop present(p, tmp_vel) gang worker vector
  for (unsigned i = 0; i < N; i++){
      p.vel_x[i] += tmp_vel.x[i];
      p.vel_y[i] += tmp_vel.y[i];
      p.vel_z[i] += tmp_vel.z[i];

      p.pos_x[i] += p.vel_x[i] * dt;
      p.pos_y[i] += p.vel_y[i] * dt;
      p.pos_z[i] += p.vel_z[i] * dt;
  }
}// end of update_particle
//----------------------------------------------------------------------------------------------------------------------



/// Compute center of gravity
float4 centerOfMassGPU(const Particles& p,
                       const int        N)
{

  return {0.0f, 0.0f, 0.0f, 0.0f};
}// end of centerOfMassGPU
//----------------------------------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Compute center of mass on CPU
float4 centerOfMassCPU(MemDesc& memDesc)
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
}// end of centerOfMassCPU
//----------------------------------------------------------------------------------------------------------------------
