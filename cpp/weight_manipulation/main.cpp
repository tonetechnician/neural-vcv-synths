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
    
    std::vector<torch::Tensor> test_vct;
    copy_params_to_vector(module,test_vct);
    print_weights_vector(test_vct);

    reset_weights(module);
    
    auto t2 = high_resolution_clock::now();

    auto ms_int = duration_cast<milliseconds>(t2 - t1);    

    std::cout << "done in " << ms_int.count() << "ms\n";
    print_weights(module);

    return 0;
}