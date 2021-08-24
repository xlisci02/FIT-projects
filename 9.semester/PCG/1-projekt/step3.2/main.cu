/**
 * @File main.cu
 *
 * The main file of the project
 *
 * Paralelní programování na GPU (PCG 2020)
 * Projekt c. 1 (cuda)
 * Login: xlisci02
 */

#include <sys/time.h>
#include <cstdio>
#include <cmath>

#include "nbody.h"
#include "h5Helper.h"

/**
 * Main rotine
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv)
{
  // Time measurement
  struct timeval t1, t2;

  if (argc != 10)
  {
    printf("Usage: nbody <N> <dt> <steps> <threads/block> <write intesity> <reduction threads> <reduction threads/block> <input> <output>\n");
    exit(1);
  }

  // Number of particles
  const int N           = std::stoi(argv[1]);
  // Length of time step
  const float dt        = std::stof(argv[2]);
  // Number of steps
  const int steps       = std::stoi(argv[3]);
  // Number of thread blocks
  const int thr_blc     = std::stoi(argv[4]);
  // Write frequency
  int writeFreq         = std::stoi(argv[5]);
  // number of reduction threads
  const int red_thr     = std::stoi(argv[6]);
  // Number of reduction threads/blocks
  const int red_thr_blc = std::stoi(argv[7]);

  // Size of the simulation CUDA gird - number of blocks
  const size_t simulationGrid = (N + thr_blc - 1) / thr_blc;
  // Size of the reduction CUDA grid - number of blocks
  const size_t reductionGrid  = (red_thr + red_thr_blc - 1) / red_thr_blc;

  // Log benchmark setup
  printf("N: %d\n", N);
  printf("dt: %f\n", dt);
  printf("steps: %d\n", steps);
  printf("threads/block: %d\n", thr_blc);
  printf("blocks/grid: %lu\n", simulationGrid);
  printf("reduction threads/block: %d\n", red_thr_blc);
  printf("reduction blocks/grid: %lu\n", reductionGrid);

  const size_t recordsNum = (writeFreq > 0) ? (steps + writeFreq - 1) / writeFreq : 0;
  writeFreq = (writeFreq > 0) ?  writeFreq : 0;


  t_particles particles_cpu;
  t_com com_cpu;
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                            FILL IN: CPU side memory allocation (step 0)                                          //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  particles_cpu.pos_x = (float *) malloc(N * sizeof(float));
  particles_cpu.pos_y = (float *) malloc(N * sizeof(float));
  particles_cpu.pos_z = (float *) malloc(N * sizeof(float));
  particles_cpu.vel_x = (float *) malloc(N * sizeof(float));
  particles_cpu.vel_y = (float *) malloc(N * sizeof(float));
  particles_cpu.vel_z = (float *) malloc(N * sizeof(float));
  particles_cpu.weight = (float *) malloc(N * sizeof(float));

  // Center of mass structure allocation
  com_cpu.x = (float *) malloc(sizeof(float));
  com_cpu.y = (float *) malloc(sizeof(float));
  com_cpu.z = (float *) malloc(sizeof(float));
  com_cpu.w = (float *) malloc(sizeof(float));

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                              FILL IN: memory layout descriptor (step 0)                                          //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /*
   * Caution! Create only after CPU side allocation
   * parameters:
   *                      Stride of two               Offset of the first
   *  Data pointer        consecutive elements        element in floats,
   *                      in floats, not bytes        not bytes
  */
  MemDesc md(
        particles_cpu.pos_x,                1,                          0,              // Postition in X
        particles_cpu.pos_y,                1,                          0,              // Postition in Y
        particles_cpu.pos_z,                1,                          0,              // Postition in Z
        particles_cpu.vel_x,                1,                          0,              // Velocity in X
        particles_cpu.vel_y,                1,                          0,              // Velocity in Y
        particles_cpu.vel_z,                1,                          0,              // Velocity in Z
        particles_cpu.weight,               1,                          0,              // Weight
        N,                                                                  // Number of particles
        recordsNum);                                                        // Number of records in output file

  // Initialisation of helper class and loading of input data
  H5Helper h5Helper(argv[8], argv[9], md);

  try
  {
    h5Helper.init();
    h5Helper.readParticleData();
  }
  catch (const std::exception& e)
  {
    std::cerr<<e.what()<<std::endl;
    return -1;
  }

  // GPU structures and variable
  t_particles p_in, p_out;
  t_com com_gpu;
  int *lock;
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                  FILL IN: GPU side memory allocation (step 0)                                    //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  cudaMalloc<float>(&(p_in.pos_x), (size_t) (N * sizeof(float)));
  cudaMalloc<float>(&(p_in.pos_y), (size_t) (N * sizeof(float)));
  cudaMalloc<float>(&(p_in.pos_z), (size_t) (N * sizeof(float)));
  cudaMalloc<float>(&(p_in.vel_x), (size_t) (N * sizeof(float)));
  cudaMalloc<float>(&(p_in.vel_y), (size_t) (N * sizeof(float)));
  cudaMalloc<float>(&(p_in.vel_z), (size_t) (N * sizeof(float)));
  cudaMalloc<float>(&(p_in.weight), (size_t) (N * sizeof(float)));

  cudaMalloc<float>(&(p_out.pos_x), (size_t) (N * sizeof(float)));
  cudaMalloc<float>(&(p_out.pos_y), (size_t) (N * sizeof(float)));
  cudaMalloc<float>(&(p_out.pos_z), (size_t) (N * sizeof(float)));
  cudaMalloc<float>(&(p_out.vel_x), (size_t) (N * sizeof(float)));
  cudaMalloc<float>(&(p_out.vel_y), (size_t) (N * sizeof(float)));
  cudaMalloc<float>(&(p_out.vel_z), (size_t) (N * sizeof(float)));
  cudaMalloc<float>(&(p_out.weight), (size_t) (N * sizeof(float)));

  // COM -- Center Of Mass allocation in global memory
  cudaMalloc<float>(&(com_gpu.x), (size_t)(sizeof(float)));
  cudaMalloc<float>(&(com_gpu.y), (size_t)(sizeof(float)));
  cudaMalloc<float>(&(com_gpu.z), (size_t)(sizeof(float)));
  cudaMalloc<float>(&(com_gpu.w), (size_t)(sizeof(float)));
  cudaMalloc<int>(&(lock), (size_t)(sizeof(int)));


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                       FILL IN: memory transfers (step 0)                                         //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  cudaMemcpy(p_in.pos_x, particles_cpu.pos_x, N * sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(p_in.pos_y, particles_cpu.pos_y, N * sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(p_in.pos_z, particles_cpu.pos_z, N * sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(p_in.vel_x, particles_cpu.vel_x, N * sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(p_in.vel_y, particles_cpu.vel_y, N * sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(p_in.vel_z, particles_cpu.vel_z, N * sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(p_in.weight, particles_cpu.weight, N * sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(p_out.weight, particles_cpu.weight, N * sizeof(float), cudaMemcpyHostToDevice);

  // Reset COM values and initialize lock
  cudaMemset(com_gpu.x, 0, 1);
  cudaMemset(com_gpu.y, 0, 1);
  cudaMemset(com_gpu.z, 0, 1);
  cudaMemset(com_gpu.w, 0, 1);
  cudaMemset(lock, 0, 1);

  gettimeofday(&t1, 0);

  // Helper structure for exchange inside loop
  t_particles p_in_copy;
  for(int s = 0; s < steps; s++)
  {

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                       FILL IN: kernels invocation (step 0)                                     //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    calculate_velocity<<<simulationGrid, thr_blc, thr_blc*sizeof(float)*7>>>(p_in, p_out, N, dt);
    // particle data arrays exchange
    p_in_copy = p_in;
    p_in = p_out;
    p_out = p_in_copy;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                          FILL IN: synchronization  (step 4)                                    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (writeFreq > 0 && (s % writeFreq == 0))
    {
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      //                          FILL IN: synchronization and file access logic (step 4)                             //
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }
  }


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //              FILL IN: invocation of center-of-mass kernel (step 3.1, step 3.2, step 4)                           //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  cudaDeviceSynchronize();
  // calling kernel with the size of shared memory
  centerOfMass<<<simulationGrid, thr_blc, thr_blc * sizeof(float) * 5>>>(p_in, com_gpu.x, com_gpu.y, com_gpu.z, com_gpu.w, lock, N);
  gettimeofday(&t2, 0);

  // Approximate simulation wall time
  double t = (1000000.0 * (t2.tv_sec - t1.tv_sec) + t2.tv_usec - t1.tv_usec) / 1000000.0;
  printf("Time: %f s\n", t);


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                             FILL IN: memory transfers for particle data (step 0)                                 //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  float4 comOnGPU;

  cudaMemcpy(particles_cpu.pos_x, p_in.pos_x, N * sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(particles_cpu.pos_y, p_in.pos_y, N * sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(particles_cpu.pos_z, p_in.pos_z, N * sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(particles_cpu.vel_x, p_in.vel_x, N * sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(particles_cpu.vel_y, p_in.vel_y, N * sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(particles_cpu.vel_z, p_in.vel_z, N * sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(particles_cpu.weight, p_in.weight, N * sizeof(float), cudaMemcpyDeviceToHost);


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                        FILL IN: memory transfers for center-of-mass (step 3.1, step 3.2)                         //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // Copy computed COM from GPU on CPU
  cudaMemcpy(com_cpu.x , com_gpu.x, sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(com_cpu.y , com_gpu.y, sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(com_cpu.z , com_gpu.z, sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(com_cpu.w , com_gpu.w, sizeof(float), cudaMemcpyDeviceToHost);

  // Division by total weight
  comOnGPU.x = *com_cpu.x / (*com_cpu.w);
  comOnGPU.y = *com_cpu.y / (*com_cpu.w);
  comOnGPU.z = *com_cpu.z / (*com_cpu.w);
  comOnGPU.w = *com_cpu.w;


  float4 comOnCPU = centerOfMassCPU(md);

  std::cout << "Center of mass on CPU:" << std::endl
            << comOnCPU.x <<", "
            << comOnCPU.y <<", "
            << comOnCPU.z <<", "
            << comOnCPU.w
            << std::endl;

  std::cout << "Center of mass on GPU:" << std::endl
            << comOnGPU.x<<", "
            << comOnGPU.y<<", "
            << comOnGPU.z<<", "
            << comOnGPU.w
            << std::endl;

  // Writing final values to the file
  h5Helper.writeComFinal(comOnGPU.x, comOnGPU.y, comOnGPU.z, comOnGPU.w);
  h5Helper.writeParticleDataFinal();

  return 0;
}// end of main
//----------------------------------------------------------------------------------------------------------------------
