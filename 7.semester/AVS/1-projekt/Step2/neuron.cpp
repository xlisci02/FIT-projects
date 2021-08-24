/*
 * Architektury výpočetních systémů (AVS 2019)
 * Projekt c. 1 (ANN)
 * Login: xlisci02
 */

#include <cstdlib>
#include "neuron.h"

float evalNeuron(
  size_t inputSize,
  const float* input,
  const float* weights,
  float bias
)
{
  float x = 0.0f;
  for (size_t i = 0; i < inputSize; i++)
    x += input[i] * weights[i];
  x += bias;
  return (x < 0.0f)? 0.0f : x;
}
