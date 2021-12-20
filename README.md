# NASH Suite 

This repo holds code for a realtime implementations of neural network modules designed for VCV which was started at the [neural audio synthesis hackathon (NASH) 2021](https://signas-qmul.github.io/nash/). 

Over the hackathon weekend we were able to implement DDSP, but we'd like to implement a few other implementation that we've seen about, namely RAVE + performance RNN and other latent space sampler searchers.

[Explainer video](https://youtu.be/xz7XeO1rsUM "DDSP in VCVRack")

## Project Description and Background

We want to open up neural audio synthesis to everyone! We decided that VCVRack2 is a perfect platform to do so, thanks to it's very open format and the ability to interact with hardware synthesizers. 

After researching a bit into [differential digital signal processing](https://magenta.tensorflow.org/ddsp) (a.k.a DDSP), it seemed like a perfect fit, and a great project to implement this in VCVRack2 for a short weekend hackathon. 

Thanks to the help of NASH rep, Ben Hayes, we were given a fantastic reference to go from - a [DDSP implementation in PureData](https://github.com/acids-ircam/ddsp_pytorch) by Antoine Callion. This really gave us the inspiration to get the fundamentals setup in VCV. 

We hope that this module will provide inspiration for more realtime implementations of neural audio nets, and we look forward to a contributing more modules like this to the community. 

## Getting started 

There are two directories. 

1. cpp - Holds the code for the 
2. python - Holds the python code for neural model generation (currently not used, refer to references python scripts).
3. references - Some useful examples to base our implementation from.

The repo holds some sub-repos. Please clone them with:

1. `git clone https://gitlab.com/tonetechnician/nash-suite.git --recurse-submodules`

## Folder structure

Currently the guts of this code is in the cpp/Nash-Suite folder. It includes the port of the [ddsp_pytorch](https://github.com/acids-ircam/ddsp_pytorch). Currently all our model data is trained using the [train.py](https://github.com/acids-ircam/ddsp_pytorch/blob/master/train.py) script housed within the ddsp_pytorch repo. 

## Next steps 

Now that the hackathon is complete we are planning to get the module into a suitable state for distribution to the [VCVRack library](https://library.vcvrack.com/). This will include the following:

1. Embedding some models in to the module.
2. Smoothing out the interpolation function 
3. Building with static libs for MacOS, Windows and Linux for distribution to VCVRack users.
