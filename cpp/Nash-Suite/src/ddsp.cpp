#include "plugin.hpp"
#include <osdialog.h>
#include <stdlib.h>
#include "ddsp_model.h"
#include <audio.hpp>
#include <thread>

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
	}

	void process(const ProcessArgs& args) override {
		float pitch = 0;
		float freq = 0;
		if (inputs[PITCH_INPUT].isConnected())
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

			outputs[OUTPUT_OUTPUT].setVoltage(rack::math::clamp(out_buffer[(model_head + head) % (2 * B_SIZE)], -10.0f, 10.0f));

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
	}
};


Model* modelDdsp = createModel<Ddsp, DdspWidget>("ddsp");