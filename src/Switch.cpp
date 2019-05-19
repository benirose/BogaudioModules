
#include "Switch.hpp"

void bogaudio::Switch::onReset() {
	_trigger.reset();
}

void bogaudio::Switch::process(const ProcessArgs& args) {
	bool latched = params[LATCH_PARAM].getValue() > 0.5f;
	lights[LATCH_LIGHT].value = latched;

	bool triggered = _trigger.process(params[GATE_PARAM].getValue() + inputs[GATE_INPUT].getVoltage());
	if (latched) {
		if (triggered) {
			_latchedHigh = !_latchedHigh;
		}
	}
	else {
		_latchedHigh = false;
	}

	if (_latchedHigh || _trigger.isHigh()) {
		outputs[OUT1_OUTPUT].setVoltage(inputs[HIGH1_INPUT].getVoltage());
		outputs[OUT2_OUTPUT].setVoltage(inputs[HIGH2_INPUT].getVoltage());
	}
	else {
		outputs[OUT1_OUTPUT].setVoltage(inputs[LOW1_INPUT].getVoltage());
		outputs[OUT2_OUTPUT].setVoltage(inputs[LOW2_INPUT].getVoltage());
	}
}

struct SwitchWidget : ModuleWidget {
	static constexpr int hp = 3;

	SwitchWidget(bogaudio::Switch* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Switch.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto gateParamPosition = Vec(13.5, 22.0);
		auto latchParamPosition = Vec(32.9, 82.9);

		auto gateInputPosition = Vec(10.5, 44.0);
		auto high1InputPosition = Vec(10.5, 100.0);
		auto low1InputPosition = Vec(10.5, 136.0);
		auto high2InputPosition = Vec(10.5, 217.0);
		auto low2InputPosition = Vec(10.5, 253.0);

		auto out1OutputPosition = Vec(10.5, 174.0);
		auto out2OutputPosition = Vec(10.5, 291.0);

		auto latchLightPosition = Vec(4.0, 84.5);
		// end generated by svg_widgets.rb

		addParam(createParam<Button18>(gateParamPosition, module, bogaudio::Switch::GATE_PARAM));
		addParam(createParam<StatefulButton9>(latchParamPosition, module, bogaudio::Switch::LATCH_PARAM));

		addInput(createInput<Port24>(gateInputPosition, module, bogaudio::Switch::GATE_INPUT));
		addInput(createInput<Port24>(high1InputPosition, module, bogaudio::Switch::HIGH1_INPUT));
		addInput(createInput<Port24>(low1InputPosition, module, bogaudio::Switch::LOW1_INPUT));
		addInput(createInput<Port24>(high2InputPosition, module, bogaudio::Switch::HIGH2_INPUT));
		addInput(createInput<Port24>(low2InputPosition, module, bogaudio::Switch::LOW2_INPUT));

		addOutput(createOutput<Port24>(out1OutputPosition, module, bogaudio::Switch::OUT1_OUTPUT));
		addOutput(createOutput<Port24>(out2OutputPosition, module, bogaudio::Switch::OUT2_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(latchLightPosition, module, bogaudio::Switch::LATCH_LIGHT));
	}
};

Model* modelSwitch = bogaudio::createModel<bogaudio::Switch, SwitchWidget>("Bogaudio-Switch", "Switch",  "signal router", SWITCH_TAG);
