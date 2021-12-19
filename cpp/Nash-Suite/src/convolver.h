#include "plugin.hpp"
#include <iostream>
#include "dr_wav.h"

#define BLOCK_SIZE 2048

class Convolver
{
private:
	dsp::RealTimeConvolver* convolver = NULL;
	dsp::SampleRateConverter<1> inputSrc;
	dsp::SampleRateConverter<1> outputSrc;
	dsp::DoubleRingBuffer<dsp::Frame<1>, 16 * BLOCK_SIZE> inputBuffer;
	dsp::DoubleRingBuffer<dsp::Frame<1>, 16 * BLOCK_SIZE> outputBuffer;
	std::vector<float> samples;

  char* irPath;
  bool irLoaded = false;
public:
    Convolver();
    void loadIR();
    void convolve(const float* in_buffer, float &output, const int sampleRate);
    bool isIRLoaded();
};