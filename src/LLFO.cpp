
#include "LLFO.hpp"

void LLFO::reset() {
	for (int c = 0; c < maxChannels; ++c) {
		_resetTrigger[c].reset();
	}
}

void LLFO::sampleRateChange() {
	for (int c = 0; c < maxChannels; ++c) {
		_phasor[c].setSampleRate(APP->engine->getSampleRate());
	}
}

bool LLFO::active() {
	return outputs[OUT_OUTPUT].isConnected();
}

int LLFO::channels() {
	return inputs[PITCH_INPUT].getChannels();
}

void LLFO::addChannel(int c) {
	if (c > 0) {
		_phasor[c].syncPhase(_phasor[0]);
	}
}

void LLFO::modulate() {
	_slowMode = params[SLOW_PARAM].getValue() > 0.5f;

	_invert = false;
	Wave wave = (Wave)params[WAVE_PARAM].getValue();
	switch (wave) {
		case SINE_WAVE: {
			_oscillator = &_sine;
			break;
		}
		case TRIANGLE_WAVE: {
			_oscillator = &_triangle;
			break;
		}
		case RAMP_UP_WAVE: {
			_oscillator = &_ramp;
			break;
		}
		case RAMP_DOWN_WAVE: {
			_oscillator = &_ramp;
			_invert = true;
			break;
		}
		case SQUARE_WAVE: {
			_oscillator = &_square;
			_square.setPulseWidth(0.5f);
			break;
		}
		case PULSE_WAVE: {
			_oscillator = &_square;
			_square.setPulseWidth(0.1f);
			break;
		}
	}

	_offset = params[OFFSET_PARAM].getValue() * 5.0f;
	_scale = params[SCALE_PARAM].getValue();
}

void LLFO::modulateChannel(int c) {
	setFrequency(params[FREQUENCY_PARAM], inputs[PITCH_INPUT], _phasor[c], c);
}

void LLFO::processAlways(const ProcessArgs& args) {
	Wave wave = (Wave)params[WAVE_PARAM].getValue();
	lights[SINE_LIGHT].value = wave == SINE_WAVE;
	lights[TRIANGLE_LIGHT].value = wave == TRIANGLE_WAVE;
	lights[RAMP_UP_LIGHT].value = wave == RAMP_UP_WAVE;
	lights[RAMP_DOWN_LIGHT].value = wave == RAMP_DOWN_WAVE;
	lights[SQUARE_LIGHT].value = wave == SQUARE_WAVE;
	lights[PULSE_LIGHT].value = wave == PULSE_WAVE;
}

void LLFO::processChannel(const ProcessArgs& args, int c) {
	if (_resetTrigger[c].next(inputs[RESET_INPUT].getPolyVoltage(c))) {
		_phasor[c].resetPhase();
	}
	_phasor[c].advancePhase();
	float sample = _oscillator->nextFromPhasor(_phasor[c]) * amplitude * _scale;
	if (_invert) {
		sample = -sample;
	}
	sample += _offset;
	outputs[OUT_OUTPUT].setChannels(_channels);
	outputs[OUT_OUTPUT].setVoltage(sample, c);
}

struct LLFOWidget : ModuleWidget {
	static constexpr int hp = 3;

	LLFOWidget(LLFO* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LLFO.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto frequencyParamPosition = Vec(9.5, 27.0);
		auto slowParamPosition = Vec(31.0, 71.0);
		auto waveParamPosition = Vec(18.0, 126.0);
		auto offsetParamPosition = Vec(14.5, 158.5);
		auto scaleParamPosition = Vec(14.5, 199.5);

		auto pitchInputPosition = Vec(10.5, 231.0);
		auto resetInputPosition = Vec(10.5, 266.0);

		auto outOutputPosition = Vec(10.5, 304.0);

		auto sineLightPosition = Vec(2.0, 89.0);
		auto rampUpLightPosition = Vec(2.0, 102.0);
		auto squareLightPosition = Vec(2.0, 115.0);
		auto triangleLightPosition = Vec(24.0, 89.0);
		auto rampDownLightPosition = Vec(24.0, 102.0);
		auto pulseLightPosition = Vec(24.0, 115.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob26>(frequencyParamPosition, module, LLFO::FREQUENCY_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(slowParamPosition, module, LLFO::SLOW_PARAM));
		addParam(createParam<StatefulButton9>(waveParamPosition, module, LLFO::WAVE_PARAM));
		addParam(createParam<Knob16>(offsetParamPosition, module, LLFO::OFFSET_PARAM));
		addParam(createParam<Knob16>(scaleParamPosition, module, LLFO::SCALE_PARAM));

		addInput(createInput<Port24>(pitchInputPosition, module, LLFO::PITCH_INPUT));
		addInput(createInput<Port24>(resetInputPosition, module, LLFO::RESET_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, LLFO::OUT_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(sineLightPosition, module, LLFO::SINE_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(rampUpLightPosition, module, LLFO::RAMP_UP_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(squareLightPosition, module, LLFO::SQUARE_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(triangleLightPosition, module, LLFO::TRIANGLE_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(rampDownLightPosition, module, LLFO::RAMP_DOWN_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(pulseLightPosition, module, LLFO::PULSE_LIGHT));
	}
};

Model* modelLLFO = bogaudio::createModel<LLFO, LLFOWidget>("Bogaudio-LLFO", "LLFO", "Compact low frequency oscillator", "LFO", "Polyphonic");
