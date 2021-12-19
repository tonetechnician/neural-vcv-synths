#include <torch/script.h> // One-stop header.

#include <iostream>
#include <memory>
void print_weights(torch::jit::script::Module &module);


void reset_weights(torch::jit::script::Module &module);

void copy_params_to_vector(torch::jit::script::Module module, std::vector<torch::Tensor> &vct_dest);

void print_weights_vector(std::vector<torch::Tensor> &vct_dest);

void interpolate_weights(torch::jit::script::Module &module,std::vector<torch::Tensor> &state1, std::vector<torch::Tensor> &state2, float knob);