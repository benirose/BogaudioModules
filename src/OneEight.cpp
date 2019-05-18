
#include "OneEight.hpp"

#define SELECT_ON_CLOCK "select_on_clock"

void OneEight::onReset() {
	_step = 0;
	_clock.reset();
	_reset.reset();
}

void OneEight::onSampleRateChange() {
	_timer.setParams(engineGetSampleRate(), 0.001f);
}

json_t* OneEight::toJson() {
	json_t* root = json_object();
	json_object_set_new(root, SELECT_ON_CLOCK, json_boolean(_selectOnClock));
	return root;
}

void OneEight::fromJson(json_t* root) {
	json_t* s = json_object_get(root, SELECT_ON_CLOCK);
	if (s) {
		_selectOnClock = json_is_true(s);
	}
}

void OneEight::step() {
	bool reset = _reset.process(inputs[RESET_INPUT].value);
	if (reset) {
		_timer.reset();
	}
	bool timer = _timer.next();
	bool clock = _clock.process(inputs[CLOCK_INPUT].value) && !timer;

	int steps = clamp(params[STEPS_PARAM].value, 1.0f, 8.0f);
	int reverse = 1 - 2 * (params[DIRECTION_PARAM].value == 0.0f);
	_step = (_step + reverse * clock) % steps;
	_step += (_step < 0) * steps;
	_step -= _step * reset;
	int select = params[SELECT_PARAM].value;
	select += clamp(inputs[SELECT_INPUT].value, 0.0f, 10.0f) * 0.1f * 8.0f;
	if (!_selectOnClock || clock) {
		_select = select;
	}
	int step = _step + _select;
	step = step % 8;

	float in = inputs[IN_INPUT].value + !inputs[IN_INPUT].active * 10.0f;
	for (int i = 0; i < 8; ++i) {
		outputs[OUT1_OUTPUT + i].value = (step == i) * in;
		lights[OUT1_LIGHT + i].value = step == i;
	}
}

struct SelectOnClockMenuItem : MenuItem {
	OneEight* _module;

	SelectOnClockMenuItem(OneEight* module, const char* label)
	: _module(module)
	{
		this->text = label;
	}

	void onAction(EventAction &e) override {
		_module->_selectOnClock = !_module->_selectOnClock;
	}

	void step() override {
		rightText = _module->_selectOnClock ? "✔" : "";
	}
};

struct OneEightWidget : ModuleWidget {
	static constexpr int hp = 6;

	OneEightWidget(OneEight* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(pluginInstance, "res/OneEight.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto stepsParamPosition = Vec(15.5, 131.5);
		auto directionParamPosition = Vec(16.0, 167.5);
		auto selectParamPosition = Vec(9.0, 230.0);

		auto clockInputPosition = Vec(11.5, 35.0);
		auto resetInputPosition = Vec(11.5, 72.0);
		auto selectInputPosition = Vec(11.5, 270.0);
		auto inInputPosition = Vec(11.5, 324.0);

		auto out1OutputPosition = Vec(54.5, 35.0);
		auto out2OutputPosition = Vec(54.5, 76.3);
		auto out3OutputPosition = Vec(54.5, 118.6);
		auto out4OutputPosition = Vec(54.5, 158.9);
		auto out5OutputPosition = Vec(54.5, 200.1);
		auto out6OutputPosition = Vec(54.5, 241.4);
		auto out7OutputPosition = Vec(54.5, 282.7);
		auto out8OutputPosition = Vec(54.5, 324.0);

		auto out1LightPosition = Vec(66.5, 61.5);
		auto out2LightPosition = Vec(66.5, 102.8);
		auto out3LightPosition = Vec(66.5, 145.1);
		auto out4LightPosition = Vec(66.5, 185.4);
		auto out5LightPosition = Vec(66.5, 226.6);
		auto out6LightPosition = Vec(66.5, 267.9);
		auto out7LightPosition = Vec(66.5, 309.2);
		auto out8LightPosition = Vec(66.5, 350.5);
		// end generated by svg_widgets.rb

		{
			auto w = createParam<Knob16>(stepsParamPosition, module, OneEight::STEPS_PARAM, 1.0, 8.0, 8.0);
			dynamic_cast<Knob*>(w)->snap = true;
			addParam(w);
		}
		addParam(createParam<SliderSwitch2State14>(directionParamPosition, module, OneEight::DIRECTION_PARAM, 0.0, 1.0, 1.0));
		{
			auto w = createParam<Knob29>(selectParamPosition, module, OneEight::SELECT_PARAM, 0.0, 7.0, 0.0);
			dynamic_cast<Knob*>(w)->snap = true;
			addParam(w);
		}

		addInput(createPort<Port24>(clockInputPosition, PortWidget::INPUT, module, OneEight::CLOCK_INPUT));
		addInput(createPort<Port24>(resetInputPosition, PortWidget::INPUT, module, OneEight::RESET_INPUT));
		addInput(createPort<Port24>(selectInputPosition, PortWidget::INPUT, module, OneEight::SELECT_INPUT));
		addInput(createPort<Port24>(inInputPosition, PortWidget::INPUT, module, OneEight::IN_INPUT));

		addOutput(createPort<Port24>(out1OutputPosition, PortWidget::OUTPUT, module, OneEight::OUT1_OUTPUT));
		addOutput(createPort<Port24>(out2OutputPosition, PortWidget::OUTPUT, module, OneEight::OUT2_OUTPUT));
		addOutput(createPort<Port24>(out3OutputPosition, PortWidget::OUTPUT, module, OneEight::OUT3_OUTPUT));
		addOutput(createPort<Port24>(out4OutputPosition, PortWidget::OUTPUT, module, OneEight::OUT4_OUTPUT));
		addOutput(createPort<Port24>(out5OutputPosition, PortWidget::OUTPUT, module, OneEight::OUT5_OUTPUT));
		addOutput(createPort<Port24>(out6OutputPosition, PortWidget::OUTPUT, module, OneEight::OUT6_OUTPUT));
		addOutput(createPort<Port24>(out7OutputPosition, PortWidget::OUTPUT, module, OneEight::OUT7_OUTPUT));
		addOutput(createPort<Port24>(out8OutputPosition, PortWidget::OUTPUT, module, OneEight::OUT8_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(out1LightPosition, module, OneEight::OUT1_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(out2LightPosition, module, OneEight::OUT2_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(out3LightPosition, module, OneEight::OUT3_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(out4LightPosition, module, OneEight::OUT4_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(out5LightPosition, module, OneEight::OUT5_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(out6LightPosition, module, OneEight::OUT6_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(out7LightPosition, module, OneEight::OUT7_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(out8LightPosition, module, OneEight::OUT8_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
		OneEight* m = dynamic_cast<OneEight*>(module);
		assert(m);
		menu->addChild(new MenuLabel());
		menu->addChild(new SelectOnClockMenuItem(m, "Select on clock"));
	}
};

Model* modelOneEight = bogaudio::createModel<OneEight, OneEightWidget>("Bogaudio-OneEight", "1:8", "mux & sequential switch", SWITCH_TAG);
