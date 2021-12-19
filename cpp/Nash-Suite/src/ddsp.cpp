#include "plugin.hpp"
#include <osdialog.h>
#include <stdlib.h>
#include "ddsp_model.h"
#include <audio.hpp>
#include <thread>
#include "dr_wav.h"

static const size_t B_SIZE = 1024;

void thread_perform(DDSPModel* model, float* freq_buf, float* loudness_buf, float* out_buffer, int block_size)
{
		model->perform(freq_buf, loudness_buf, out_buffer, block_size);
}
struct Ddsp : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		PITCH_INPUT,
		LOUDNESS_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};



	DDSPModel* ddspModel;
	std::thread* compute_thread = nullptr;
	int head = 0;
	float freq_buffer[2 * B_SIZE];
	float loudness_buffer[2 * B_SIZE];
	float out_buffer[2 * B_SIZE];

	char* modelPath;

	bool linear = false;

	int model_head = 0;
	int sample_counter = 0;

	dsp::RealTimeConvolver* convolver = NULL;
	std::vector<float> samples;
	dsp::DoubleRingBuffer<dsp::Frame<1>, 16 * B_SIZE> convolutionInputBuffer;
	dsp::DoubleRingBuffer<dsp::Frame<1>, 16 * B_SIZE> convolutionOutputBuffer;
	dsp::SampleRateConverter<1> inputSrc;
	dsp::SampleRateConverter<1> outputSrc;

	Ddsp() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(PITCH_INPUT, "Pitch");
		configInput(LOUDNESS_INPUT, "Loudness");
		configOutput(OUTPUT_OUTPUT, "Audio");
		std::cout  << "Hi from ddsp" << std::endl;
		ddspModel = new DDSPModel();
		for (int i = 0; i < 2 * B_SIZE; i++) 
		{
			freq_buffer[i] = 440.0f;
			loudness_buffer[i] = 68;
		}

		convolver = new dsp::RealTimeConvolver(B_SIZE);
	}

	void process(const ProcessArgs& args) override {
		float pitch = 0;
		float freq = 0;
		if (inputs[PITCH_INPUT].isConnected() && ddspModel->modelIsLoaded())
		{
			if (!linear) {
				pitch += inputs[PITCH_INPUT].getVoltage();
				freq = dsp::FREQ_C4 * dsp::approxExp2_taylor5(pitch + 30.f) / std::pow(2.f, 30.f);
			}
			else {
				freq = dsp::FREQ_C4 * dsp::approxExp2_taylor5(pitch + 30.f) / std::pow(2.f, 30.f);
				freq += dsp::FREQ_C4 * inputs[PITCH_INPUT].getVoltage();
			}
			freq_buffer[head % (2 * B_SIZE)] = clamp(freq, 0.f, args.sampleRate / 2.f);



			float modelOut =  out_buffer[(model_head + head) % (2 * B_SIZE)];

			if (!convolutionInputBuffer.full()) {
			dsp::Frame<1> inputFrame;
			inputFrame.samples[0] = modelOut;
			convolutionInputBuffer.push(inputFrame);
			}

			if (convolutionOutputBuffer.empty()) {
				float input[B_SIZE] = {};
				float output[B_SIZE];

				// Convert input buffer
				{
					inputSrc.setRates(args.sampleRate, 48000);
					int inLen = convolutionInputBuffer.size();
					int outLen = B_SIZE;
					inputSrc.process(convolutionInputBuffer.startData(), &inLen, (dsp::Frame<1>*) input, &outLen);
					convolutionInputBuffer.startIncr(inLen);
				}

				convolver->processBlock(input, output);

				// Convert output buffer
				{
					outputSrc.setRates(48000, args.sampleRate);
					int inLen = B_SIZE;
					int outLen = convolutionOutputBuffer.capacity();
					outputSrc.process((dsp::Frame<1>*) output, &inLen, convolutionOutputBuffer.endData(), &outLen);
					convolutionOutputBuffer.endIncr(outLen);
				}

				float wet = convolutionOutputBuffer.shift().samples[0];
				outputs[OUTPUT_OUTPUT].setVoltage(rack::math::clamp(modelOut + wet, -10.0f, 10.0f));
			}

			if (!(head % B_SIZE))
			{
					// Processed the output buffer
					if (compute_thread)
					{
							compute_thread->join();
					}

					// Thread complete calculate another block, place it a varying parts of the buffer
					model_head = ((head + B_SIZE) % (2 * B_SIZE));

					compute_thread = new std::thread(		thread_perform,
																							ddspModel,
																							&freq_buffer[model_head],
																							&loudness_buffer[model_head],
																							&out_buffer[model_head],
																							B_SIZE);

					head = head % (2 * B_SIZE);  	 
			}

			// increment the buffer counter
			head++;
		} 
	}

	void openDialogAndLoadModel() {
		modelPath = osdialog_file(osdialog_file_action::OSDIALOG_OPEN, nullptr, nullptr, nullptr);
		if (modelPath)
		{
			ddspModel->load(at::str(modelPath));
		}
	}

	void openDialogAndLoadIR() {
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
			
			size_t len = wav.totalPCMFrameCount * wav.channels;
			if (len == 0 || len >= (1 << 20))
				return;

			samples.clear();
			samples.resize(len);

			drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, samples.data());

			std::cout << samples.data() << std::endl;

			size_t kernelLen = wav.dataChunkDataSize / wav.bitsPerSample;

			convolver->setKernel(samples.data(), samples.size());
		}
	}
};


struct DdspWidget : ModuleWidget {
	DdspWidget(Ddsp* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ddsp.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.489, 10.576)), module, Ddsp::PITCH_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.706, 10.431)), module, Ddsp::LOUDNESS_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24, 113.581)), module, Ddsp::OUTPUT_OUTPUT));
	}

	void appendContextMenu(Menu* menu) override {
		Ddsp* module = dynamic_cast<Ddsp*>(this->module);
		assert(module);

		menu->addChild(new MenuSeparator);

		menu->addChild(createMenuItem("Load Module", "", [=]() {module->openDialogAndLoadModel();}));

		menu->addChild(new MenuSeparator);
		menu->addChild(createMenuItem("Load IR", "", [=]() {module->openDialogAndLoadIR();}));
	}
};


Model* modelDdsp = createModel<Ddsp, DdspWidget>("ddsp");