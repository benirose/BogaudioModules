
#include "Noise.hpp"

#define NOISE_CHANNELS "noise_channels"

json_t* Noise::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, NOISE_CHANNELS, json_integer(_noiseChannels));
	return root;
}

void Noise::dataFromJson(json_t* root) {
	json_t* nc = json_object_get(root, NOISE_CHANNELS);
	if (nc) {
		_noiseChannels = json_integer_value(nc);
	}
}

void Noise::processChannel(const ProcessArgs& args, int c) {
	assert(c == 0);

	for (int i = 0; i < _noiseChannels; ++i) {
		if (outputs[BLUE_OUTPUT].isConnected()) {
			outputs[BLUE_OUTPUT].setChannels(_noiseChannels);
			outputs[BLUE_OUTPUT].setVoltage(clamp(_blue.next() * 20.0f, -10.0f, 10.f), i);
		}
		if (outputs[WHITE_OUTPUT].isConnected()) {
			outputs[WHITE_OUTPUT].setChannels(_noiseChannels);
			outputs[WHITE_OUTPUT].setVoltage(clamp(_white.next() * 10.0f, -10.0f, 10.f), i);
		}
		if (outputs[PINK_OUTPUT].isConnected()) {
			outputs[PINK_OUTPUT].setChannels(_noiseChannels);
			outputs[PINK_OUTPUT].setVoltage(clamp(_pink.next() * 15.0f, -10.0f, 10.f), i);
		}
		if (outputs[RED_OUTPUT].isConnected()) {
			outputs[RED_OUTPUT].setChannels(_noiseChannels);
			outputs[RED_OUTPUT].setVoltage(clamp(_red.next() * 20.0f, -10.0f, 10.f), i);
		}
		if (outputs[GAUSS_OUTPUT].isConnected()) {
			outputs[GAUSS_OUTPUT].setChannels(_noiseChannels);
			outputs[GAUSS_OUTPUT].setVoltage(clamp(_gauss.next(), -10.0f, 10.f), i);
		}
	}

	int n = inputs[ABS_INPUT].getChannels();
	outputs[ABS_OUTPUT].setChannels(n);
	for (int i = 0; i < n; ++i) {
		float in = inputs[ABS_INPUT].getPolyVoltage(i);
		if (in < 0.0) {
			in = -in;
		}
		outputs[ABS_OUTPUT].setVoltage(in, i);
	}
}

struct NoiseWidget : ModuleWidget {
	static constexpr int hp = 3;

	NoiseWidget(Noise* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Noise.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto absInputPosition = Vec(10.5, 253.0);

		auto blueOutputPosition = Vec(10.5, 24.0);
		auto whiteOutputPosition = Vec(10.5, 65.0);
		auto pinkOutputPosition = Vec(10.5, 106.0);
		auto redOutputPosition = Vec(10.5, 147.0);
		auto gaussOutputPosition = Vec(10.5, 188.0);
		auto absOutputPosition = Vec(10.5, 291.0);
		// end generated by svg_widgets.rb

		addInput(createInput<Port24>(absInputPosition, module, Noise::ABS_INPUT));

		addOutput(createOutput<Port24>(blueOutputPosition, module, Noise::BLUE_OUTPUT));
		addOutput(createOutput<Port24>(whiteOutputPosition, module, Noise::WHITE_OUTPUT));
		addOutput(createOutput<Port24>(pinkOutputPosition, module, Noise::PINK_OUTPUT));
		addOutput(createOutput<Port24>(redOutputPosition, module, Noise::RED_OUTPUT));
		addOutput(createOutput<Port24>(gaussOutputPosition, module, Noise::GAUSS_OUTPUT));
		addOutput(createOutput<Port24>(absOutputPosition, module, Noise::ABS_OUTPUT));
	}

	struct ChannelsMenuItem : MenuItem {
		Noise* _module;

		ChannelsMenuItem(Noise* module, const char* label) : _module(module) {
			this->text = label;
		}

		Menu* createChildMenu() override {
			Menu* menu = new Menu;
			Noise* m = _module;
			menu->addChild(new OptionMenuItem("Monophonic", [m]() { return m->_noiseChannels == 1; }, [m]() { m->_noiseChannels = 1; }));
			for (int i = 2; i <= BGModule::maxChannels; i++) {
				char s[10];
				snprintf(s, 10, "%d", i);
				menu->addChild(new OptionMenuItem(s, [m, i]() { return m->_noiseChannels == i; }, [m, i]() { m->_noiseChannels = i; }));
			}
			return menu;
		}

		void step() override {
			MenuItem::step();
			char s[10];
			snprintf(s, 10, "%d ▸", _module->_noiseChannels);
			this->rightText = s;
		}
	};

	void appendContextMenu(Menu* menu) override {
		Noise* m = dynamic_cast<Noise*>(module);
		assert(m);

		menu->addChild(new MenuLabel());
		menu->addChild(new ChannelsMenuItem(m, "Polyphony channels"));
	}
};

Model* modelNoise = bogaudio::createModel<Noise, NoiseWidget>("Bogaudio-Noise", "NOISE", "Noise source with multiple flavors & absolute value", "Noise", "Random", "Polyphonic");
