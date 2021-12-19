#include "convolver.h"
#include <osdialog.h>

Convolver::Convolver() : convolver(new dsp::RealTimeConvolver(BLOCK_SIZE))
{
}

void Convolver::loadIR()
{
  	std::string filepathIR;
		filepathIR = osdialog_file(osdialog_file_action::OSDIALOG_OPEN, nullptr, nullptr, nullptr);

		if (filepathIR.c_str()) 
		{
			std::cout << filepathIR << std::endl;
			
			std::string ext = string::lowercase(system::getExtension(filepathIR));
			std::cout << ext << std::endl;
			assert(ext == ".wav");

			drwav wav;

#if defined ARCH_WIN
			if (!drwav_init_file_w(&wav, string::UTF8toUTF16(path).c_str(), NULL))
#else
			if (!drwav_init_file(&wav, filepathIR.c_str(), NULL))
#endif
				return;

			samples.clear();
			size_t len = wav.totalPCMFrameCount * wav.channels;
			samples.resize(len);
			drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, samples.data());
			size_t kernelLen = wav.dataChunkDataSize / sizeof(float);
			const float * kernelPtr = samples.data();
			convolver->setKernel(kernelPtr, kernelLen);
		}
}

void Convolver::convolve(const float* in_buffer, float &output, const int sampleRate)
{
		float in = *in_buffer;

		if (!inputBuffer.full()) {
			dsp::Frame<1> inputFrame;
			inputFrame.samples[0] = in;
			inputBuffer.push(inputFrame);
		}

		if (outputBuffer.empty()) {
			float input[BLOCK_SIZE] = {};
			float output[BLOCK_SIZE];

			// Convert input buffer
			{
				inputSrc.setRates(sampleRate, 48000);
				int inLen = inputBuffer.size();
				int outLen = BLOCK_SIZE;
				inputSrc.process(inputBuffer.startData(), &inLen, (dsp::Frame<1>*) input, &outLen);
				inputBuffer.startIncr(inLen);
			}

			convolver->processBlock(input, output);

			// Convert output buffer
			{
				outputSrc.setRates(48000, sampleRate);
				int inLen = BLOCK_SIZE;
				int outLen = outputBuffer.capacity();
				outputSrc.process((dsp::Frame<1>*) output, &inLen, outputBuffer.endData(), &outLen);
				outputBuffer.endIncr(outLen);
			}
		}

		// Set output
		if (outputBuffer.empty())
			return;

		float wet = outputBuffer.shift().samples[0];
    output = wet;
		// outputs[OUTPUT_OUTPUT].setVoltage(clamp(wet, -10.0f, 10.0f));
}