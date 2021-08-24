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

/// Compute velocity
void calculate_velocity(const Particles& p_in,
                        const Particles& p_out,
                        const int        N,
                        const float      dt)
{
  float r, dx, dy, dz;
  // Variables for partial sums
  float tmp_vel_x = 0.0f, tmp_vel_y = 0.0f, tmp_vel_z = 0.0f;

  // launch async computation on stream 1
  #pragma acc parallel loop present(p_in, p_out) gang worker vector async(1)
  for (unsigned i = 0; i < N; i++){
    // Preloading of particle position data to registers
    float pos_x = p_in.pos_x[i];
    float pos_y = p_in.pos_y[i];
    float pos_z = p_in.pos_z[i];
    float weight = p_in.weight[i];
    float vel_x = p_in.vel_x[i];
    float vel_y = p_in.vel_y[i];
    float vel_z = p_in.vel_z[i];

    #pragma acc loop seq
    for (unsigned j = 0; j < N; j++){
      // Preload to register
      float w = p_in.weight[j];
      // First, we compute what we can
      float Fg_dt_m2_r = -G * dt * w;
      // Get the position differences
      dx = pos_x - p_in.pos_x[j];
      dy = pos_y - p_in.pos_y[j];
      dz = pos_z - p_in.pos_z[j];

      // Compute r, based on the position difference
      r = sqrt(dx*dx + dy*dy + dz*dz);

      // Gravitation
      // Finish computing Fg_dt_m2_r with r
      Fg_dt_m2_r /= (r * r * r + FLT_MIN);

      // Compute impact of particle on index j
      tmp_vel_x += (r > COLLISION_DISTANCE) ? Fg_dt_m2_r * dx : 0.0f;
      tmp_vel_y += (r > COLLISION_DISTANCE) ? Fg_dt_m2_r * dy : 0.0f;
      tmp_vel_z += (r > COLLISION_DISTANCE) ? Fg_dt_m2_r * dz : 0.0f;

      // Collision
      // if r is within range
      if (r > 0.0f && r < COLLISION_DISTANCE){
        // Precompute values to registers
        float weight_sum = weight + w;
        float weight_diff = weight - w;
        float weight_double = w * 2;

        // Compute impact of particle on index j
        tmp_vel_x += ((vel_x * (weight_diff) + weight_double* p_in.vel_x[j]) / (weight_sum)) - vel_x;
        tmp_vel_y += ((vel_y * (weight_diff) + weight_double* p_in.vel_y[j]) / (weight_sum)) - vel_y;
        tmp_vel_z += ((vel_z * (weight_diff) + weight_double* p_in.vel_z[j]) / (weight_sum)) - vel_z;
      }
    }
    // Update velocities and positions
    p_out.vel_x[i] = vel_x + tmp_vel_x;
    p_out.vel_y[i] = vel_y + tmp_vel_y;
    p_out.vel_z[i] = vel_z + tmp_vel_z;

    p_out.pos_x[i] = pos_x + ((vel_x + tmp_vel_x) * dt);
    p_out.pos_y[i] = pos_y + ((vel_y + tmp_vel_y) * dt);
    p_out.pos_z[i] = pos_z + ((vel_z + tmp_vel_z) * dt);

    tmp_vel_x = 0.0f, tmp_vel_y = 0.0f, tmp_vel_z = 0.0f;
  }
}// end of calculate_gravitation_velocity
//----------------------------------------------------------------------------------------------------------------------

/// Compute center of gravity using atomic update (this is modified CenterOfMassGPU_ver1 function from step3)
/// This function is used in the simulation loop for asynchronous calculating of Center Of Mass
/// In contrast to step3, we had to zero the CoM on CPU and update the values on GPU before calculating
/// Notice that function does not return anything, it calculates product of positions and weight
/// and leave the results on the GPU; in the simulation loop we update host values and divide x,y,z by total weight w
void centerOfMassGPU(const Particles& p,
                     const int        N,
                     CenterOfMass&    CoM)
{
  // We use the same principle as above, but now we work with the whole
  // structure CoM not just with their items, so we can't use reduction,
  // but we may use atomic operations to update every structure item (x,y,z,w)
  CoM.x = 0.0f; CoM.y = 0.0f; CoM.z = 0.0f; CoM.w = 0.0f; // zero CoM values
  CoM.copyToGpuAsync(); // async data update to GPU on stream 5

  // add to stream 2 but wait until data update on (5) is done
  #pragma acc parallel loop present(p,CoM) async(2) wait(5)
  for (unsigned i = 0; i < N; i++){
    // Sum only product of position and weight
    #pragma acc atomic update
    CoM.x += p.pos_x[i] * p.weight[i];
    #pragma acc atomic update
    CoM.y += p.pos_y[i] * p.weight[i];
    #pragma acc atomic update
    CoM.z += p.pos_z[i] * p.weight[i];
    #pragma acc atomic update
    CoM.w += p.weight[i];
  }
  // after sum, we divide CoM.x, CoM.y, CoM.z by total weight (on CPU in the simulation loop, before writing to file)
}// end of centerOfMassGPU
//----------------------------------------------------------------------------------------------------------------------

/// THIS IS NOT ASYNC VERSION OF COM COMPUTATION, ASYNC VERSION BEGINS ABOVE
/// Compute center of gravity using reduction (this is modified CenterOfMassGPU function from step3)
/// Note that this function doesn't use CenterOfMass structure,
/// because the structure allocates memory on CPU/GPU in this step, which is not neccesary for this function
/// This function is only used for calculating final COM, so the COM at the end of simulation
float4 centerOfMassGPU_w_red(const Particles& p,
                             const int        N)
{
  struct float4 CoM; // Center of Mass
  float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f; // variables for reduction
  // Center of Mass is simlpy weighted average, so firstly we only sum
  // product of particles positions and their weight, and at the end we divide
  // it by total weight of all particles
  #pragma acc parallel loop present(p) reduction(+:x,y,z,w)
  for (unsigned i = 0; i < N; i++){
    // Sum only product of position and weight
    x += p.pos_x[i] * p.weight[i];
    y += p.pos_y[i] * p.weight[i];
    z += p.pos_z[i] * p.weight[i];
    w += p.weight[i];
  }
  // after sum, we divide CoM.x, CoM.y, CoM.z by total weight (on CPU)
  // note: float4 structure does not have divideByWeight function like CenterOfMass structure does
  CoM.x = x/w;
  CoM.y = y/w;
  CoM.z = z/w;
  CoM.w = w;
  return CoM;
}// end of centerOfMassGPU
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------------------------------------

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
