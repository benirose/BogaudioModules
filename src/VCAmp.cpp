
#include "VCAmp.hpp"
#include "mixer.hpp"

void VCAmp::sampleRateChange() {
	float sampleRate = APP->engine->getSampleRate();
	for (int c = 0; c < maxChannels; ++c) {
		_levelSL[c].setParams(sampleRate, MixerChannel::levelSlewTimeMS, maxDecibels - minDecibels);
		_rms[c].setSampleRate(sampleRate);
	}
}

void VCAmp::processAll(const ProcessArgs& args) {
	if (inputs[IN_INPUT].isConnected()) {
		int n = inputs[IN_INPUT].getChannels();
		outputs[OUT_OUTPUT].setChannels(n);
		float rmsSum = 0.0f;
		for (int c = 0; c < n; ++c) {
			float level = params[LEVEL_PARAM].getValue();
			if (inputs[CV_INPUT].isConnected()) {
				level *= clamp(inputs[CV_INPUT].getPolyVoltage(c), 0.0f, 10.0f) / 10.0f;
			}
			level *= maxDecibels - minDecibels;
			level += minDecibels;
			_amplifier[c].setLevel(_levelSL[c].next(level));
			float out = _saturator[c].next(_amplifier[c].next(inputs[IN_INPUT].getVoltage(c)));
			outputs[OUT_OUTPUT].setVoltage(out, c);
			rmsSum += _rms[c].next(out / 5.0f);
		}
		_rmsLevel = rmsSum / (float)n;
	}
	else {
		_rmsLevel = 0.0f;
	}
}

struct VCAmpWidget : ModuleWidget {
	static constexpr int hp = 3;

	VCAmpWidget(VCAmp* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/VCAmp.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto levelParamPosition = Vec(13.5, 18.0);

		auto cvInputPosition = Vec(10.5, 213.0);
		auto inInputPosition = Vec(10.5, 248.0);

		auto outOutputPosition = Vec(10.5, 286.0);
		// end generated by svg_widgets.rb

		auto slider = createParam<VUSlider>(levelParamPosition, module, VCAmp::LEVEL_PARAM);
		if (module) {
			dynamic_cast<VUSlider*>(slider)->setVULevel(&(module->_rmsLevel));
		}
		addParam(slider);

		addInput(createInput<Port24>(cvInputPosition, module, VCAmp::CV_INPUT));
		addInput(createInput<Port24>(inInputPosition, module, VCAmp::IN_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, VCAmp::OUT_OUTPUT));
	}
};

Model* modelVCAmp = bogaudio::createModel<VCAmp, VCAmpWidget>("Bogaudio-VCAmp", "VCAMP", "Voltage controlled amplifier with 12dB gain", "VCA", "Polyphonic");
