/**
 * @file      main.cpp
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

#include <chrono>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <string.h>

#include "nbody.h"
#include "h5Helper.h"

/**
 * Main routine of the project
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv)
{
  // Parse command line parameters
  if (argc != 7)
  {
    printf("Usage: nbody <N> <dt> <steps> <write intesity> <input> <output>\n");
    exit(EXIT_FAILURE);
  }

  const int   N         = std::stoi(argv[1]);
  const float dt        = std::stof(argv[2]);
  const int   steps     = std::stoi(argv[3]);
  const int   writeFreq = (std::stoi(argv[4]) > 0) ? std::stoi(argv[4]) : 0;

  printf("N: %d\n", N);
  printf("dt: %f\n", dt);
  printf("steps: %d\n", steps);

  const size_t recordsNum = (writeFreq > 0) ? (steps + writeFreq - 1) / writeFreq : 0;


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                         Code to be implemented                                                   //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // 1.  Memory allocation on CPU

  // allocation is part of the constructor
  struct Particles particles_in(N);
  struct Particles particles_out(N);

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                            FILL IN: CPU side memory allocation (step 0)                                          //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // 2. Create memory descriptor
  /*
   * Caution! Create only after CPU side allocation
   * parameters:
   *                                    Stride of two               Offset of the first
   *             Data pointer           consecutive elements        element in floats,
   *                                    in floats, not bytes        not bytes
  */
  MemDesc md(
                   particles_in.pos_x,                1,                          0,            // Position in X
                   particles_in.pos_y,                1,                          0,            // Position in Y
                   particles_in.pos_z,                1,                          0,            // Position in Z
                   particles_in.vel_x,                1,                          0,            // Velocity in X
                   particles_in.vel_y,                1,                          0,            // Velocity in Y
                   particles_in.vel_z,                1,                          0,            // Velocity in Z
                   particles_in.weight,               1,                          0,            // Weight
                   N,                                                                // Number of particles
                   recordsNum);                                                      // Number of records in output file



  H5Helper h5Helper(argv[5], argv[6], md);

  // Read data
  try
  {
    h5Helper.init();
    h5Helper.readParticleData();
  } catch (const std::exception& e)
  {
    std::cerr<<e.what()<<std::endl;
    return EXIT_FAILURE;
  }

  // 3. Copy data to GPU

  memcpy((void*)particles_out.weight, (void *)particles_in.weight, N * sizeof(float));
  particles_in.copyToGpu(N);
  particles_out.copyWeightToGpu(N);
  // Start the time
  auto startTime = std::chrono::high_resolution_clock::now();


  struct CenterOfMass centerOfMass; // center of mass structure

  // 4. Run the loop - calculate new Particle positions.
  for (int s = 0; s < steps; s++)
  {
    // calculate center of mass on stream 2
    centerOfMassGPU(((s % 2 == 0)? particles_in : particles_out), N, centerOfMass);

    /// In step 4 - fill in the code to store Particle snapshots.
    if (writeFreq > 0 && (s % writeFreq == 0))
    {
      // copy particles data on CPU on stream 3, but wait until previous velocity computation is done on stream 1
      ((s % 2 == 0)? particles_in : particles_out).copyToCpuAsync(N);
      // copy com data, but wait until previous com computation is done on stream 2
      centerOfMass.copyToCpuAsync();

      // write particle data, wait until particle host data update is done on stream 3
      // notice, that concurent writing to file and com data update is possible
      #pragma acc wait(3)
      h5Helper.writeParticleData(s / writeFreq);
      // write com data, wait until com host data update is done on stream 4
      #pragma acc wait(4)
      centerOfMass.divideByWeight();
      h5Helper.writeCom(centerOfMass.x, centerOfMass.y, centerOfMass.z, centerOfMass.w, s / writeFreq);
    }
    // calculate new positions and velocities on stream 1
    calculate_velocity(
      (s % 2 == 0)? particles_in : particles_out,
      (s % 2 == 0)? particles_out : particles_in, N, dt);
    // insert wait before next calculate_velocity until com computation is done and vice versa
    #pragma acc wait(2) async(1)
    #pragma acc wait(1) async(2)

  }// for s ...
  #pragma acc wait
  // 5. In steps 3 and 4 -  Compute center of gravity
  float4 comOnGPU;
  comOnGPU = centerOfMassGPU_w_red(((steps % 2 == 0)? particles_in : particles_out), N);


  // Stop watchclock
  const auto   endTime = std::chrono::high_resolution_clock::now();
  const double time    = (endTime - startTime) / std::chrono::milliseconds(1);
  printf("Time: %f s\n", time / 1000);


  // 5. Copy data from GPU back to CPU.
  ((steps % 2 == 0)? particles_in : particles_out).copyToCpu(N);
  // copy data from particles_out to particles_in, because memory descriptor is set to particles_in
  if(steps % 2 != 0)
    particles_out.forkCpuData(particles_in, N);
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /// Calculate center of gravity
  float4 comOnCPU = centerOfMassCPU(md);


    std::cout<<"Center of mass on CPU:"<<std::endl
      << comOnCPU.x <<", "
      << comOnCPU.y <<", "
      << comOnCPU.z <<", "
      << comOnCPU.w
      << std::endl;

    std::cout<<"Center of mass on GPU:"<<std::endl
      << comOnGPU.x <<", "
      << comOnGPU.y <<", "
      << comOnGPU.z <<", "
      << comOnGPU.w
      <<std::endl;

  // Store final positions of the particles into a file
  h5Helper.writeComFinal(comOnGPU.x, comOnGPU.y, comOnGPU.z, comOnGPU.w);
  h5Helper.writeParticleDataFinal();

  return EXIT_SUCCESS;
}// end of main
//----------------------------------------------------------------------------------------------------------------------

