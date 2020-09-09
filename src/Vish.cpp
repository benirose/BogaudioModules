
#include "Vish.hpp"

void Vish::Engine::reset() {
	trigger.reset();
	gatePulseGen.process(10.0);
}

void Vish::Engine::setSampleRate(float sr) {
	velocitySL.setParams(sr, 5.0f, 1.0f);
}

void Vish::reset() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->reset();
	}
}

void Vish::sampleRateChange() {
	_sampleRate = APP->engine->getSampleRate();
	_sampleTime = APP->engine->getSampleTime();
	for (int i = 0; i < _channels; ++i) {
		_engines[i]->setSampleRate(_sampleRate);
	}
}

bool Vish::active() {
	return outputs[OUT_OUTPUT].isConnected();
}

int Vish::channels() {
	return inputs[GATE_INPUT].getChannels();
}

void Vish::addChannel(int c) {
	_engines[c] = new Engine();
	_engines[c]->reset();
	_engines[c]->setSampleRate(_sampleRate);
}

void Vish::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void Vish::modulateChannel(int c) {
	_engines[c]->slew.modulate(
		_sampleRate,
		params[RISE_PARAM],
		&inputs[RISE_INPUT],
		300.0f * _timeScale,
		params[RISE_SHAPE_PARAM],
		params[FALL_PARAM],
		&inputs[FALL_INPUT],
		1000.0f * _timeScale,
		params[FALL_SHAPE_PARAM],
		c
	);
}

void Vish::processChannel(const ProcessArgs& args, int c) {
	Engine& e = *_engines[c];

	float in = inputs[GATE_INPUT].getPolyVoltage(c);
	if (e.trigger.process(in)) {
		float time = clamp(params[MINIMUM_GATE_PARAM].getValue(), 0.0f, 1.0f);
		if (inputs[MINIMUM_GATE_INPUT].isConnected()) {
			time *= clamp(inputs[MINIMUM_GATE_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
		}
		time *= time;
		time *= _timeScale;
		e.gateSeconds = time;

		e.gateElapsedSeconds = 0.0f;
		if (_gateToTrigger) {
			e.gateSeconds = std::max(0.01f, time);
		}
		else {
			e.gateSeconds = time;
		}
	}
	else {
		e.gateElapsedSeconds += _sampleTime;
	}

	float gate = 0.0f;
	if (e.gateElapsedSeconds < e.gateSeconds) {
		gate = 10.0f;
	}
	else if (!_gateToTrigger) {
		gate = in;
	}

	float velocity = 1.0f;
	if (inputs[VELOCITY_INPUT].isConnected()) {
		velocity = clamp(inputs[VELOCITY_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	velocity = e.velocitySL.next(velocity);
	e.velocityAmp.setLevel(_minVelocityDb + velocity * (_maxVelocityDb - _minVelocityDb));

	outputs[OUT_OUTPUT].setChannels(_channels);
	outputs[OUT_OUTPUT].setVoltage(e.velocityAmp.next(e.slew.next(gate)), c);
}

struct VishWidget : LPGEnvBaseWidget {
	static constexpr int hp = 5;

	VishWidget(Vish* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Vish");
		createScrews();

		// generated by svg_widgets.rb
		auto riseParamPosition = Vec(12.5, 47.0);
		auto riseShapeParamPosition = Vec(52.0, 52.0);
		auto fallParamPosition = Vec(12.5, 111.0);
		auto fallShapeParamPosition = Vec(52.0, 116.0);
		auto minimumGateParamPosition = Vec(23.0, 175.0);
		auto gateToTriggerParamPosition = Vec(57.0, 220.0);
		auto times10xParamPosition = Vec(55.0, 234.0);

		auto minimumGateInputPosition = Vec(10.5, 251.0);
		auto riseInputPosition = Vec(40.5, 251.0);
		auto velocityInputPosition = Vec(10.5, 288.0);
		auto fallInputPosition = Vec(40.5, 288.0);
		auto gateInputPosition = Vec(10.5, 325.0);

		auto outOutputPosition = Vec(40.5, 325.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob26>(riseParamPosition, module, Vish::RISE_PARAM));
		addParam(createParam<Knob16>(riseShapeParamPosition, module, Vish::RISE_SHAPE_PARAM));
		addParam(createParam<Knob26>(fallParamPosition, module, Vish::FALL_PARAM));
		addParam(createParam<Knob16>(fallShapeParamPosition, module, Vish::FALL_SHAPE_PARAM));
		addParam(createParam<Knob29>(minimumGateParamPosition, module, Vish::MINIMUM_GATE_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(gateToTriggerParamPosition, module, Vish::GATE_TO_TRIGGER_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(times10xParamPosition, module, Vish::TIMES_10X_PARAM));

		addInput(createInput<Port24>(minimumGateInputPosition, module, Vish::MINIMUM_GATE_INPUT));
		addInput(createInput<Port24>(riseInputPosition, module, Vish::RISE_INPUT));
		addInput(createInput<Port24>(velocityInputPosition, module, Vish::VELOCITY_INPUT));
		addInput(createInput<Port24>(fallInputPosition, module, Vish::FALL_INPUT));
		addInput(createInput<Port24>(gateInputPosition, module, Vish::GATE_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, Vish::OUT_OUTPUT));
	}
};

Model* modelVish = createModel<Vish, VishWidget>("Bogaudio-Vish", "VISH", "Vactrol-ish envelope generator", "Envelope generator", "Polyphonic");
