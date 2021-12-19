#include "weight_tools.h"

void print_weights_vector(std::vector<torch::Tensor> &vct_dest)
    {
    for (const torch::Tensor& params : vct_dest)
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

void print_weights(torch::jit::script::Module &module)
    {
    /*Disable grad computation - it will allow change weights one by one.*/
    torch::NoGradGuard no_guard; //Will only disable grads in current thread.

    for (const torch::Tensor& params : module.parameters())
        {
        /*Let's go through all the parameters*/
        const auto ndim = params.sizes().size(); // get number of dimensions.
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


void copy_params_to_vector(torch::jit::script::Module module, std::vector<torch::Tensor> &vct_dest)
    {
    for (const torch::Tensor& params : module.parameters())
        {
        /*Generate a new tensor with clone() method and store to vector.*/
        vct_dest.push_back( params.detach().clone() );
        
        }    
    }



void interpolate_weights(torch::jit::script::Module &module,std::vector<torch::Tensor> &state1, std::vector<torch::Tensor> &state2, float knob)
    {

    /*Disable grad computation - it will allow change weights one by one.*/
    torch::NoGradGuard no_guard; //Will only disable grads in current thread.
    
    std::vector<torch::Tensor>::iterator st1 = state1.begin();
    std::vector<torch::Tensor>::iterator st2 = state2.begin();
    
    for (const torch::Tensor& params : module.parameters())
    {
        const auto ndim = params.sizes().size(); // get number of dimensions.
        /*std::cout << "Parameter: " << std::endl;
        std::cout << "module params ndim " << ndim << " - state1 ndim " << (*st1).sizes().size() << " - state2 ndim " << (*st2).sizes().size() << std::endl ;
        */
        /*Lazy way of reading through weights - should be implemented recursively but it could affect performance.*/

        /*A zero-dimension set of parameters is a scalar.*/
        if(ndim == 0)
        {
            float *s1 = (*st1).data_ptr<float>();
            float *s2 = (*st2).data_ptr<float>();
            
            float *module_param = params.data_ptr<float>();
            *module_param= (1-knob)*(*s1) + knob*(*s2);
        }


        if(ndim == 1)
        {
            const auto dlen = params.size(0);
            float *s1 = (*st1).data_ptr<float>();
            float *s2 = (*st2).data_ptr<float>();
            
            float *module_param = params.data_ptr<float>();
            
            for(int j=0;j<dlen;j++)
            {
                module_param[j] = (1-knob)*s1[j] + knob*s2[j];
            }
        }

        if(ndim == 2)
        {
            const auto dlen = params.size(0);
            for(int j=0;j<dlen;j++)
            {
                const auto dlen = params.size(1);
                //(*st1)[j].print();
                float *s1 = (*st1)[j].data_ptr<float>();
                float *s2 = (*st2)[j].data_ptr<float>();
                
                float *module_param = params[j].data_ptr<float>();
                
                for(int k=0;k<dlen;k++)
                    {
                    module_param[k] = (1-knob)*s1[k] + knob*s2[k];
                    }
                
                }

            }        
            ++st1;
            ++st2; 
        }

    }


void reset_weights(torch::jit::script::Module &module)
{

    /*Disable grad computation - it will allow change weights one by one.*/
    torch::NoGradGuard no_guard; //Will only disable grads in current thread.
    for (const torch::Tensor& params : module.parameters())
        {
        const auto ndim = params.sizes().size(); // get number of dimensions.

        /*Lazy way of reading through weights - should be implemented recursively but it could affect performance.*/

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
    
