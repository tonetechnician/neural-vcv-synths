#include <torch/script.h> // One-stop header.

#include <iostream>
#include <memory>
#include "weight_tools.h"
//void interpolate(torch::jit::script::Module &module1, torch::jit::script::Module &module2, float knob);

int main(int argc, const char* argv[]) 
{
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    /*Disable grad computation - it will allow change weights one by one.*/
    torch::NoGradGuard no_guard; //Will only disable grads in current thread.

    if (argc != 2)
    {
    std::cerr << "usage: example-app <path-to-exported-script-module>\n";
    return -1;
    }

    torch::jit::script::Module module;

    try
        {
        // Deserialize the ScriptModule from a file using torch::jit::load().
        module = torch::jit::load(argv[1]);
        }
    catch (const c10::Error& e)
        {
        std::cerr << "error loading the model\n";
        return -1;
        }

    std::cout << "module loaded.\n";
    
    auto t1 = high_resolution_clock::now();
    
    std::vector<torch::Tensor> state1,state2;
    /*Copy model parameters to vector.*/
    copy_params_to_vector(module,state1);
    /*Set all model weights to zero.*/
    reset_weights(module);    
    /*Copy all zeros to vector*/
    copy_params_to_vector(module,state2);
    
    /*
    std::cout << "STATE 1 \n========" << std::endl;
    print_weights_vector(state1);
    std::cout << "STATE 2 \n========" << std::endl;
    print_weights_vector(state2);
    */
    
    
    /*state 1 has original module weights, - state 2 has all zeros. 
      The model should have all its original weights halved. */
    interpolate_weights(module,state1,state2,0.5);
    
    /*std::cout << "INTERPOLATED\n========" << std::endl;
    print_weights(module);
    */
    
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    
    std::cout << "done in " << ms_int.count() << "ms\n";
    
    return 0;
}