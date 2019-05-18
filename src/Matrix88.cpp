
#include "Matrix88.hpp"

void Matrix88::step() {
	for (int i = 0; i < 8; ++i) {
		int paramOffset = MIX11_PARAM + i * 8;
		float out = 0.0f;
		for (int j = 0; j < 8; ++j) {
			out += inputs[IN1_INPUT + j].value * params[paramOffset + j].value;
		}
		outputs[OUT1_OUTPUT + i].value = _saturators[i].next(params[LEVEL_PARAM].value * out);
	}
}

struct Matrix88Widget : ModuleWidget {
	static constexpr int hp = 22;

	Matrix88Widget(Matrix88* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(pluginInstance, "res/Matrix88.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(createWidget<ScrewSilver>(Vec(15, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));

		// generated by svg_widgets.rb
		auto mix11ParamPosition = Vec(53.7, 32.2);
		auto mix21ParamPosition = Vec(53.7, 69.2);
		auto mix31ParamPosition = Vec(53.7, 106.2);
		auto mix41ParamPosition = Vec(53.7, 143.2);
		auto mix51ParamPosition = Vec(53.7, 180.2);
		auto mix61ParamPosition = Vec(53.7, 217.2);
		auto mix71ParamPosition = Vec(53.7, 254.2);
		auto mix81ParamPosition = Vec(53.7, 291.2);
		auto mix12ParamPosition = Vec(88.7, 32.2);
		auto mix22ParamPosition = Vec(88.7, 69.2);
		auto mix32ParamPosition = Vec(88.7, 106.2);
		auto mix42ParamPosition = Vec(88.7, 143.2);
		auto mix52ParamPosition = Vec(88.7, 180.2);
		auto mix62ParamPosition = Vec(88.7, 217.2);
		auto mix72ParamPosition = Vec(88.7, 254.2);
		auto mix82ParamPosition = Vec(88.7, 291.2);
		auto mix13ParamPosition = Vec(123.7, 32.2);
		auto mix23ParamPosition = Vec(123.7, 69.2);
		auto mix33ParamPosition = Vec(123.7, 106.2);
		auto mix43ParamPosition = Vec(123.7, 143.2);
		auto mix53ParamPosition = Vec(123.7, 180.2);
		auto mix63ParamPosition = Vec(123.7, 217.2);
		auto mix73ParamPosition = Vec(123.7, 254.2);
		auto mix83ParamPosition = Vec(123.7, 291.2);
		auto mix14ParamPosition = Vec(158.7, 32.2);
		auto mix24ParamPosition = Vec(158.7, 69.2);
		auto mix34ParamPosition = Vec(158.7, 106.2);
		auto mix44ParamPosition = Vec(158.7, 143.2);
		auto mix54ParamPosition = Vec(158.7, 180.2);
		auto mix64ParamPosition = Vec(158.7, 217.2);
		auto mix74ParamPosition = Vec(158.7, 254.2);
		auto mix84ParamPosition = Vec(158.7, 291.2);
		auto mix15ParamPosition = Vec(193.7, 32.2);
		auto mix25ParamPosition = Vec(193.7, 69.2);
		auto mix35ParamPosition = Vec(193.7, 106.2);
		auto mix45ParamPosition = Vec(193.7, 143.2);
		auto mix55ParamPosition = Vec(193.7, 180.2);
		auto mix65ParamPosition = Vec(193.7, 217.2);
		auto mix75ParamPosition = Vec(193.7, 254.2);
		auto mix85ParamPosition = Vec(193.7, 291.2);
		auto mix16ParamPosition = Vec(228.7, 32.2);
		auto mix26ParamPosition = Vec(228.7, 69.2);
		auto mix36ParamPosition = Vec(228.7, 106.2);
		auto mix46ParamPosition = Vec(228.7, 143.2);
		auto mix56ParamPosition = Vec(228.7, 180.2);
		auto mix66ParamPosition = Vec(228.7, 217.2);
		auto mix76ParamPosition = Vec(228.7, 254.2);
		auto mix86ParamPosition = Vec(228.7, 291.2);
		auto mix17ParamPosition = Vec(263.7, 32.2);
		auto mix27ParamPosition = Vec(263.7, 69.2);
		auto mix37ParamPosition = Vec(263.7, 106.2);
		auto mix47ParamPosition = Vec(263.7, 143.2);
		auto mix57ParamPosition = Vec(263.7, 180.2);
		auto mix67ParamPosition = Vec(263.7, 217.2);
		auto mix77ParamPosition = Vec(263.7, 254.2);
		auto mix87ParamPosition = Vec(263.7, 291.2);
		auto mix18ParamPosition = Vec(298.7, 32.2);
		auto mix28ParamPosition = Vec(298.7, 69.2);
		auto mix38ParamPosition = Vec(298.7, 106.2);
		auto mix48ParamPosition = Vec(298.7, 143.2);
		auto mix58ParamPosition = Vec(298.7, 180.2);
		auto mix68ParamPosition = Vec(298.7, 217.2);
		auto mix78ParamPosition = Vec(298.7, 254.2);
		auto mix88ParamPosition = Vec(298.7, 291.2);
		auto levelParamPosition = Vec(14.5, 339.5);

		auto in1InputPosition = Vec(10.5, 30.0);
		auto in2InputPosition = Vec(10.5, 67.0);
		auto in3InputPosition = Vec(10.5, 104.0);
		auto in4InputPosition = Vec(10.5, 141.0);
		auto in5InputPosition = Vec(10.5, 178.0);
		auto in6InputPosition = Vec(10.5, 215.0);
		auto in7InputPosition = Vec(10.5, 252.0);
		auto in8InputPosition = Vec(10.5, 289.0);

		auto out1OutputPosition = Vec(51.5, 328.0);
		auto out2OutputPosition = Vec(86.5, 328.0);
		auto out3OutputPosition = Vec(121.5, 328.0);
		auto out4OutputPosition = Vec(156.5, 328.0);
		auto out5OutputPosition = Vec(191.5, 328.0);
		auto out6OutputPosition = Vec(226.5, 328.0);
		auto out7OutputPosition = Vec(261.5, 328.0);
		auto out8OutputPosition = Vec(296.5, 328.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob19>(mix11ParamPosition, module, Matrix88::MIX11_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix21ParamPosition, module, Matrix88::MIX21_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix31ParamPosition, module, Matrix88::MIX31_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix41ParamPosition, module, Matrix88::MIX41_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix51ParamPosition, module, Matrix88::MIX51_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix61ParamPosition, module, Matrix88::MIX61_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix71ParamPosition, module, Matrix88::MIX71_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix81ParamPosition, module, Matrix88::MIX81_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix12ParamPosition, module, Matrix88::MIX12_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix22ParamPosition, module, Matrix88::MIX22_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix32ParamPosition, module, Matrix88::MIX32_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix42ParamPosition, module, Matrix88::MIX42_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix52ParamPosition, module, Matrix88::MIX52_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix62ParamPosition, module, Matrix88::MIX62_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix72ParamPosition, module, Matrix88::MIX72_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix82ParamPosition, module, Matrix88::MIX82_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix13ParamPosition, module, Matrix88::MIX13_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix23ParamPosition, module, Matrix88::MIX23_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix33ParamPosition, module, Matrix88::MIX33_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix43ParamPosition, module, Matrix88::MIX43_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix53ParamPosition, module, Matrix88::MIX53_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix63ParamPosition, module, Matrix88::MIX63_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix73ParamPosition, module, Matrix88::MIX73_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix83ParamPosition, module, Matrix88::MIX83_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix14ParamPosition, module, Matrix88::MIX14_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix24ParamPosition, module, Matrix88::MIX24_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix34ParamPosition, module, Matrix88::MIX34_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix44ParamPosition, module, Matrix88::MIX44_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix54ParamPosition, module, Matrix88::MIX54_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix64ParamPosition, module, Matrix88::MIX64_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix74ParamPosition, module, Matrix88::MIX74_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix84ParamPosition, module, Matrix88::MIX84_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix15ParamPosition, module, Matrix88::MIX15_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix25ParamPosition, module, Matrix88::MIX25_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix35ParamPosition, module, Matrix88::MIX35_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix45ParamPosition, module, Matrix88::MIX45_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix55ParamPosition, module, Matrix88::MIX55_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix65ParamPosition, module, Matrix88::MIX65_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix75ParamPosition, module, Matrix88::MIX75_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix85ParamPosition, module, Matrix88::MIX85_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix16ParamPosition, module, Matrix88::MIX16_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix26ParamPosition, module, Matrix88::MIX26_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix36ParamPosition, module, Matrix88::MIX36_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix46ParamPosition, module, Matrix88::MIX46_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix56ParamPosition, module, Matrix88::MIX56_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix66ParamPosition, module, Matrix88::MIX66_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix76ParamPosition, module, Matrix88::MIX76_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix86ParamPosition, module, Matrix88::MIX86_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix17ParamPosition, module, Matrix88::MIX17_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix27ParamPosition, module, Matrix88::MIX27_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix37ParamPosition, module, Matrix88::MIX37_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix47ParamPosition, module, Matrix88::MIX47_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix57ParamPosition, module, Matrix88::MIX57_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix67ParamPosition, module, Matrix88::MIX67_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix77ParamPosition, module, Matrix88::MIX77_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix87ParamPosition, module, Matrix88::MIX87_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix18ParamPosition, module, Matrix88::MIX18_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix28ParamPosition, module, Matrix88::MIX28_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix38ParamPosition, module, Matrix88::MIX38_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix48ParamPosition, module, Matrix88::MIX48_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix58ParamPosition, module, Matrix88::MIX58_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix68ParamPosition, module, Matrix88::MIX68_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix78ParamPosition, module, Matrix88::MIX78_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob19>(mix88ParamPosition, module, Matrix88::MIX88_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob16>(levelParamPosition, module, Matrix88::LEVEL_PARAM, 0.0, 1.0, 1.0));

		addInput(createPort<Port24>(in1InputPosition, PortWidget::INPUT, module, Matrix88::IN1_INPUT));
		addInput(createPort<Port24>(in2InputPosition, PortWidget::INPUT, module, Matrix88::IN2_INPUT));
		addInput(createPort<Port24>(in3InputPosition, PortWidget::INPUT, module, Matrix88::IN3_INPUT));
		addInput(createPort<Port24>(in4InputPosition, PortWidget::INPUT, module, Matrix88::IN4_INPUT));
		addInput(createPort<Port24>(in5InputPosition, PortWidget::INPUT, module, Matrix88::IN5_INPUT));
		addInput(createPort<Port24>(in6InputPosition, PortWidget::INPUT, module, Matrix88::IN6_INPUT));
		addInput(createPort<Port24>(in7InputPosition, PortWidget::INPUT, module, Matrix88::IN7_INPUT));
		addInput(createPort<Port24>(in8InputPosition, PortWidget::INPUT, module, Matrix88::IN8_INPUT));

		addOutput(createPort<Port24>(out1OutputPosition, PortWidget::OUTPUT, module, Matrix88::OUT1_OUTPUT));
		addOutput(createPort<Port24>(out2OutputPosition, PortWidget::OUTPUT, module, Matrix88::OUT2_OUTPUT));
		addOutput(createPort<Port24>(out3OutputPosition, PortWidget::OUTPUT, module, Matrix88::OUT3_OUTPUT));
		addOutput(createPort<Port24>(out4OutputPosition, PortWidget::OUTPUT, module, Matrix88::OUT4_OUTPUT));
		addOutput(createPort<Port24>(out5OutputPosition, PortWidget::OUTPUT, module, Matrix88::OUT5_OUTPUT));
		addOutput(createPort<Port24>(out6OutputPosition, PortWidget::OUTPUT, module, Matrix88::OUT6_OUTPUT));
		addOutput(createPort<Port24>(out7OutputPosition, PortWidget::OUTPUT, module, Matrix88::OUT7_OUTPUT));
		addOutput(createPort<Port24>(out8OutputPosition, PortWidget::OUTPUT, module, Matrix88::OUT8_OUTPUT));
	}
};

Model* modelMatrix88 = bogaudio::createModel<Matrix88, Matrix88Widget>("Bogaudio-Matrix88", "Matrix88", "8x8 matrix mixer", MIXER_TAG);
