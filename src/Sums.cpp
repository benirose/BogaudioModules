
#include "Sums.hpp"

void Sums::processAll(const ProcessArgs& args) {
	for (int i = 0, cn = std::max(1, inputs[A_INPUT].getChannels()); i < cn; ++i) {
		float a = inputs[A_INPUT].getPolyVoltage(i);
		float b = inputs[B_INPUT].getPolyVoltage(i);
		outputs[SUM_OUTPUT].setChannels(cn);
		outputs[DIFFERENCE_OUTPUT].setChannels(cn);
		outputs[MAX_OUTPUT].setChannels(cn);
		outputs[MIN_OUTPUT].setChannels(cn);
		if (_disableOutputLimit) {
			outputs[SUM_OUTPUT].setVoltage(a + b, i);
			outputs[DIFFERENCE_OUTPUT].setVoltage(a - b, i);
			outputs[MAX_OUTPUT].setVoltage(std::max(a, b), i);
			outputs[MIN_OUTPUT].setVoltage(std::min(a, b), i);
		}
		else {
			outputs[SUM_OUTPUT].setVoltage(clamp(a + b, -12.0f, 12.0f), i);
			outputs[DIFFERENCE_OUTPUT].setVoltage(clamp(a - b, -12.0f, 12.0f), i);
			outputs[MAX_OUTPUT].setVoltage(clamp(std::max(a, b), -12.0f, 12.0f), i);
			outputs[MIN_OUTPUT].setVoltage(clamp(std::min(a, b), -12.0f, 12.0f), i);
		}
	}

	int cn = inputs[NEGATE_INPUT].getChannels();
	outputs[NEGATE_OUTPUT].setChannels(cn);
	for (int i = 0; i < cn; ++i) {
		if (_disableOutputLimit) {
			outputs[NEGATE_OUTPUT].setVoltage(-inputs[NEGATE_INPUT].getPolyVoltage(i), i);
		}
		else {
			outputs[NEGATE_OUTPUT].setVoltage(clamp(-inputs[NEGATE_INPUT].getPolyVoltage(i), -12.0f, 12.0f), i);
		}
	}
}

struct SumsWidget : DisableOutputLimitModuleWidget {
	static constexpr int hp = 3;

	SumsWidget(Sums* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Sums.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto aInputPosition = Vec(10.5, 23.0);
		auto bInputPosition = Vec(10.5, 53.0);
		auto negateInputPosition = Vec(10.5, 262.0);

		auto sumOutputPosition = Vec(10.5, 86.0);
		auto differenceOutputPosition = Vec(10.5, 126.0);
		auto maxOutputPosition = Vec(10.5, 166.0);
		auto minOutputPosition = Vec(10.5, 206.0);
		auto negateOutputPosition = Vec(10.5, 295.0);
		// end generated by svg_widgets.rb

		addInput(createInput<Port24>(aInputPosition, module, Sums::A_INPUT));
		addInput(createInput<Port24>(bInputPosition, module, Sums::B_INPUT));
		addInput(createInput<Port24>(negateInputPosition, module, Sums::NEGATE_INPUT));

		addOutput(createOutput<Port24>(sumOutputPosition, module, Sums::SUM_OUTPUT));
		addOutput(createOutput<Port24>(differenceOutputPosition, module, Sums::DIFFERENCE_OUTPUT));
		addOutput(createOutput<Port24>(maxOutputPosition, module, Sums::MAX_OUTPUT));
		addOutput(createOutput<Port24>(minOutputPosition, module, Sums::MIN_OUTPUT));
		addOutput(createOutput<Port24>(negateOutputPosition, module, Sums::NEGATE_OUTPUT));
	}
};

Model* modelSums = bogaudio::createModel<Sums, SumsWidget>("Bogaudio-Sums", "SUMS", "Arithmetic logic utility", "Logic", "Polyphonic");
