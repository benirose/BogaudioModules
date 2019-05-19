
#include "AMRM.hpp"

void AMRM::process(const ProcessArgs& args) {
	if (!(outputs[OUT_OUTPUT].isConnected() || outputs[RECTIFY_OUTPUT].isConnected())) {
		return;
	}

	float rectify = params[RECTIFY_PARAM].getValue();
	if (inputs[RECTIFY_INPUT].isConnected()) {
		rectify = clamp(rectify + inputs[RECTIFY_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
	}
	rectify = 1.0f - rectify;

	float depth = params[DRYWET_PARAM].getValue();
	if (inputs[DRYWET_INPUT].isConnected()) {
		depth = clamp(depth + inputs[DRYWET_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
	}

	float modulator = inputs[MODULATOR_INPUT].getVoltage();
	if (rectify < 1.0f) {
		rectify *= -5.0f;
		if (modulator < rectify) {
			modulator = rectify - (modulator - rectify);
		}
	}
	outputs[RECTIFY_OUTPUT].setVoltage(modulator);

	modulator *= depth;
	modulator += (1.0f - depth) * 5.0f;
	outputs[OUT_OUTPUT].setVoltage(_saturator.next(modulator * inputs[CARRIER_INPUT].getVoltage() * 0.2f));
}

struct AMRMWidget : ModuleWidget {
	static constexpr int hp = 6;

	AMRMWidget(AMRM* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/AMRM.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto rectifyParamPosition = Vec(22.5, 66.5);
		auto drywetParamPosition = Vec(22.5, 165.5);

		auto modulatorInputPosition = Vec(16.0, 244.0);
		auto carrierInputPosition = Vec(50.0, 244.0);
		auto rectifyInputPosition = Vec(16.0, 280.0);
		auto drywetInputPosition = Vec(50.0, 280.0);

		auto rectifyOutputPosition = Vec(16.0, 320.0);
		auto outOutputPosition = Vec(50.0, 320.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob45>(rectifyParamPosition, module, AMRM::RECTIFY_PARAM));
		addParam(createParam<Knob45>(drywetParamPosition, module, AMRM::DRYWET_PARAM));

		addInput(createInput<Port24>(modulatorInputPosition, module, AMRM::MODULATOR_INPUT));
		addInput(createInput<Port24>(carrierInputPosition, module, AMRM::CARRIER_INPUT));
		addInput(createInput<Port24>(rectifyInputPosition, module, AMRM::RECTIFY_INPUT));
		addInput(createInput<Port24>(drywetInputPosition, module, AMRM::DRYWET_INPUT));

		addOutput(createOutput<Port24>(rectifyOutputPosition, module, AMRM::RECTIFY_OUTPUT));
		addOutput(createOutput<Port24>(outOutputPosition, module, AMRM::OUT_OUTPUT));
	}
};

Model* modelAMRM = bogaudio::createModel<AMRM, AMRMWidget>("Bogaudio-AMRM", "AM/RM", "ring modulator", RING_MODULATOR_TAG, EFFECT_TAG);
