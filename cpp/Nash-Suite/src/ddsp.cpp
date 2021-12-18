#include "plugin.hpp"


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

	Ddsp() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(PITCH_INPUT, "");
		configInput(LOUDNESS_INPUT, "");
		configOutput(OUTPUT_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
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

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.809, 10.576)), module, Ddsp::PITCH_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.311, 10.431)), module, Ddsp::LOUDNESS_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.067, 113.581)), module, Ddsp::OUTPUT_OUTPUT));
	}
};


Model* modelDdsp = createModel<Ddsp, DdspWidget>("ddsp");