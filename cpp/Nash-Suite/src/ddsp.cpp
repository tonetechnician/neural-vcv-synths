#include "plugin.hpp"
#include <osdialog.h>
#include <stdlib.h>
#include "ddsp_model.h"
#include <audio.hpp>
static const size_t BLOCK_SIZE = 1024;

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

	Ddsp() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(PITCH_INPUT, "Pitch");
		configInput(LOUDNESS_INPUT, "Loudness");
		configOutput(OUTPUT_OUTPUT, "Audio");
		std::cout  << "Hi from ddsp" << std::endl;
		ddspModel = new DDSPModel();
	}

	void process(const ProcessArgs& args) override {
		float out = 0;
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
			freq = clamp(freq, 0.f, args.sampleRate / 2.f);
			float loudness = 0;
			ddspModel->perform(&freq, &loudness, &out, BLOCK_SIZE);
			outputs[OUTPUT_OUTPUT].setVoltage(rack::math::clamp(out, -5.0f, 5.0f));
		}
	}

	void openDialogAndLoadModel() {
		filepath = osdialog_file(osdialog_file_action::OSDIALOG_OPEN, nullptr, nullptr, nullptr);
		if (filepath)
		{
			ddspModel->load(at::str(filepath));
		}
	}
	char* filepath;

	bool linear = false;
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