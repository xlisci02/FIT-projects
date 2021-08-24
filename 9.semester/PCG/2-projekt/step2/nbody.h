/**
 * @file      nbody.h
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

#ifndef __NBODY_H__
#define __NBODY_H__

#include <cstdlib>
#include <cstdio>
#include  <cmath>
#include <cstring>
#include "h5Helper.h"

/// Gravity constant
constexpr float G = 6.67384e-11f;

/// Collision distance threshold
constexpr float COLLISION_DISTANCE = 0.01f;

/**
 * @struct float4
 * Structure that mimics CUDA float4
 */
struct float4
{
  float x;
  float y;
  float z;
  float w;
};

/// Define sqrtf from CUDA libm library
#pragma acc routine(sqrtf) seq

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Declare following structs / classes                                          //
//                                  If necessary, add your own classes / routines                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Structure with particle data
 */
struct Particles
{
  // Fill the structure holding the particle/s data
  // It is recommended to implement constructor / destructor and copyToGPU and copyToCPU routines

    float * pos_x;
    float * pos_y;
    float * pos_z;
    float * vel_x;
    float * vel_y;
    float * vel_z;
    float * weight;

    Particles(const int N){
      // Memory allocation on CPU
      pos_x = (float *) malloc(N * sizeof(float));
      pos_y = (float *) malloc(N * sizeof(float));
      pos_z = (float *) malloc(N * sizeof(float));
      vel_x = (float *) malloc(N * sizeof(float));
      vel_y = (float *) malloc(N * sizeof(float));
      vel_z = (float *) malloc(N * sizeof(float));
      weight = (float *) malloc(N * sizeof(float));

      // Memory allocation on GPU
      #pragma acc enter data copyin(this)
      #pragma acc enter data create(pos_x[:N])
      #pragma acc enter data create(pos_y[:N])
      #pragma acc enter data create(pos_z[:N])
      #pragma acc enter data create(vel_x[:N])
      #pragma acc enter data create(vel_y[:N])
      #pragma acc enter data create(vel_z[:N])
      #pragma acc enter data create(weight[:N])
    }

    void copyToCpu(const int N){
      // CPU updates itself
      #pragma acc update self(pos_x[:N])
      #pragma acc update self(pos_y[:N])
      #pragma acc update self(pos_z[:N])
      #pragma acc update self(vel_x[:N])
      #pragma acc update self(vel_y[:N])
      #pragma acc update self(vel_z[:N])
      // #pragma acc update self(weight[:N])
    }

    void copyToGpu(const int N){
      // GPU update
      #pragma acc update device(pos_x[:N])
      #pragma acc update device(pos_y[:N])
      #pragma acc update device(pos_z[:N])
      #pragma acc update device(vel_x[:N])
      #pragma acc update device(vel_y[:N])
      #pragma acc update device(vel_z[:N])
      #pragma acc update device(weight[:N])
    }

    void copyWeightToGpu(const int N){
      // GPU weight update
      #pragma acc update device(weight[:N])
    }

    void forkCpuData(Particles& particles, const int N){
      memcpy(particles.pos_x, pos_x, N * sizeof(float));
      memcpy(particles.pos_y, pos_y, N * sizeof(float));
      memcpy(particles.pos_z, pos_z, N * sizeof(float));
      memcpy(particles.vel_x, vel_x, N * sizeof(float));
      memcpy(particles.vel_y, vel_y, N * sizeof(float));
      memcpy(particles.vel_z, vel_z, N * sizeof(float));
      memcpy(particles.weight, weight, N * sizeof(float));
    }

    ~Particles(){
      #pragma acc exit data delete(pos_x)
      #pragma acc exit data delete(pos_y)
      #pragma acc exit data delete(pos_z)
      #pragma acc exit data delete(vel_x)
      #pragma acc exit data delete(vel_y)
      #pragma acc exit data delete(vel_z)
      #pragma acc exit data delete(weight)
      #pragma acc exit data delete(this)
      free(pos_x);
      free(pos_y);
      free(pos_z);
      free(vel_x);
      free(vel_y);
      free(vel_z);
      free(weight);
    }
};// end of Particles
//----------------------------------------------------------------------------------------------------------------------

/**
 * Compute gravitation velocity
 * @param [in]  p        - Particles
 * @param [out] tmp_vel  - Temporal velocity
 * @param [in ] N        - Number of particles
 * @param [in]  dt       - Time step size
 */
void calculate_velocity(const Particles& p,
                        const Particles& tmp_vel,
                        const int        N,
                        const float      dt);


/**
 * Compute center of gravity - implement in steps 3 and 4.
 * @param [in] p - Particles
 * @param [in] N - Number of particles
 * @return Center of Mass [x, y, z] and total weight[w]
 */
float4 centerOfMassGPU(const Particles& p,
                       const int        N);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Compute center of mass on CPU
 * @param memDesc
 * @return centre of gravity
 */
float4 centerOfMassCPU(MemDesc& memDesc);

#endif /* __NBODY_H__ */
