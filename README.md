# NASH Suite 

This repo holds code for a realtime implementations of neural network modules designed for VCV. Currently only DDSP is implemented, but we'd like to implement a few other implementation that we've seen about, namely RAVE + performance RNN and other latent space sampler searchers.

[![Youtube Link](http://img.youtube.com/vi/Yxz7XeO1rsUM/0.jpg)](https://youtu.be/xz7XeO1rsUM "DDSP in VCVRack")

## Getting started 

There are two directories. 

1. cpp - Holds the code for the 
2. python - Holds the python code for neural model generation (currently not used, refer to references python scripts).
3. references - Some useful examples to base our implementation from.

The repo holds some sub-repos. Please clone them with:

1. `git clone https://gitlab.com/tonetechnician/nash-suite.git --recurse-submodules`

## Folder structure

Currently the guts of this code is in the cpp/Nash-Suite folder. It includes the port of the [ddsp_pytorch](https://github.com/acids-ircam/ddsp_pytorch). Currently all our model data is trained using the [train.py](https://github.com/acids-ircam/ddsp_pytorch/blob/master/train.py) script housed within the ddsp_pytorch. 

## Next steps 

Now that the hackathon is complete we are planning to get the module into a suitable state for distribution to the [VCVRack library](https://library.vcvrack.com/). This will include the following:

1. Embedding some models in to the module.
2. Smoothing out the interpolation function 
3. Building with static libs for MacOS, Windows and Linux.
