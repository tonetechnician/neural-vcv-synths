#pragma once
#include <rack.hpp>
#include <torch/script.h>
#include <torch/torch.h>

using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;
extern Model* modelDdsp;
// Declare each Model, defined in each module source file
// extern Model* modelMyModule;
