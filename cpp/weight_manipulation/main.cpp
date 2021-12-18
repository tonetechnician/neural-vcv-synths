#include <torch/script.h> // One-stop header.

#include <iostream>
#include <memory>

//void interpolate(torch::jit::script::Module &module1, torch::jit::script::Module &module2, float knob);

void print_weights(torch::jit::script::Module &module)
    {

    /*Disable grad computation - it will allow change weights one by one.*/
    torch::NoGradGuard no_guard; //Will only disable grads in current thread.

    for (const auto& params : module.parameters())
        {
        /*Let's go through all the parameters*/
        const auto ndim = params.sizes().size(); // get number of dimensions.
        std::cout << "Parameter: " << std::endl;
        std::cout << "[ ";
        for(int j=0;j<ndim;j++)
            std::cout << params.size(j) << " ";
        std::cout << " ]" << std::endl;


        /*Lazy way of reading through weights - should be implemented recursively*/

        /*A zero-dimension set of parameters is a scalar.*/
        if(ndim == 0)
            {
            float *data = params.data_ptr<float>();
            std::cout << *data << std::endl;
            }


        if(ndim == 1)
            {
            const auto dlen = params.size(0);
            float *data = params.data_ptr<float>();
            for(int j=0;j<dlen;j++)
                {
                std::cout << data[j] << " ";
                }
            std::cout << std::endl;
            }

        if(ndim == 2)
            {
            const auto dlen = params.size(0);
            for(int j=0;j<dlen;j++)
                {
                const auto dlen = params.size(1);
                float *data = params[j].data_ptr<float>();
                for(int k=0;k<dlen;k++)
                    {
                    std::cout << data[k] << " ";
                    }
                std::cout << std::endl;
                }
            std::cout << std::endl;
            }
        }
    }

void reset_weights(torch::jit::script::Module &module)
    {

    /*Disable grad computation - it will allow change weights one by one.*/
    torch::NoGradGuard no_guard; //Will only disable grads in current thread.
    for (const auto& params : module.parameters())
        {
        const auto ndim = params.sizes().size(); // get number of dimensions.

        /*Lazy way of reading through weights - should be implemented recursively*/

        /*A zero-dimension set of parameters is a scalar.*/
        if(ndim == 0)
            {
            float *data = params.data_ptr<float>();
            *data = 0;
            }


        if(ndim == 1)
            {
            const auto dlen = params.size(0);
            float *data = params.data_ptr<float>();
            for(int j=0;j<dlen;j++)
                {
                data[j] = 0;
                }
            }

        if(ndim == 2)
            {
            const auto dlen = params.size(0);
            for(int j=0;j<dlen;j++)
                {
                const auto dlen = params.size(1);
                float *data = params[j].data_ptr<float>();
                for(int k=0;k<dlen;k++)
                    {
                    data[k] = 0;
                    }
                }
            }        
        }
    }

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
    

    reset_weights(module);
    
    auto t2 = high_resolution_clock::now();

    auto ms_int = duration_cast<milliseconds>(t2 - t1);    

    std::cout << "done in " << ms_int.count() << "ms\n";
    print_weights(module);
    
    return 0;
}