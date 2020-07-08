
#include "PEQ6XF.hpp"

void PEQ6XF::sampleRateChange() {
	float sr = APP->engine->getSampleRate();
	for (int c = 0; c < _channels; ++c) {
		for (int i = 0; i < 6; ++i) {
			_engines[c]->efs[i].setSampleRate(sr);
		}
	}
}

void PEQ6XF::addChannel(int c) {
	_engines[c] = new Engine();
	float sr = APP->engine->getSampleRate();
	for (int i = 0; i < 6; ++i) {
		_engines[c]->efs[i].setSampleRate(sr);
	}
}

void PEQ6XF::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void PEQ6XF::modulate() {
	float response = params[DAMP_PARAM].getValue();
	if (_response != response) {
		_response = response;
		for (int c = 0; c < _channels; ++c) {
			for (int i = 0; i < 6; ++i) {
				_engines[c]->efs[i].setSensitivity(_response);
			}
		}
	}

	float db = clamp(params[GAIN_PARAM].getValue(), -1.0f, 1.0f);
	if (db < 0.0f) {
		db = -db * efGainMinDecibels;
	}
	else {
		db *= std::min(12.0f, efGainMaxDecibels);
	}
	_gain.setLevel(db);
}

void PEQ6XF::processAll(const ProcessArgs& args) {
	for (int i = 0; i < 6; ++i) {
		outputs[EF1_OUTPUT + i].setChannels(_channels);
	}

	_baseMessage = NULL;
	if (baseConnected()) {
		_baseMessage = fromBase();
	}
}

void PEQ6XF::processChannel(const ProcessArgs& args, int c) {
	if (_baseMessage) {
		Engine& e = *_engines[c];
		for (int i = 0; i < 6; ++i) {
			float out = e.efs[i].next(_baseMessage->outs[c][i]);
			out = scaleEF(out, _baseMessage->frequencies[c][i], _baseMessage->bandwidths[c]);
			out = _gain.next(out);
			out = _saturator.next(out);
			outputs[EF1_OUTPUT + i].setVoltage(out, c);
		}
	}
	else {
		for (int i = 0; i < 6; ++i) {
			outputs[EF1_OUTPUT + i].setVoltage(0.0f, c);
		}
	}
}

struct PEQ6XFWidget : ModuleWidget {
	static constexpr int hp = 3;

	PEQ6XFWidget(PEQ6XF* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PEQ6XF.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto dampParamPosition = Vec(14.5, 32.0);
		auto gainParamPosition = Vec(14.5, 71.0);

		auto ef1OutputPosition = Vec(10.5, 107.0);
		auto ef2OutputPosition = Vec(10.5, 142.0);
		auto ef3OutputPosition = Vec(10.5, 177.0);
		auto ef4OutputPosition = Vec(10.5, 212.0);
		auto ef5OutputPosition = Vec(10.5, 247.0);
		auto ef6OutputPosition = Vec(10.5, 282.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob16>(dampParamPosition, module, PEQ6XF::DAMP_PARAM));
		addParam(createParam<Knob16>(gainParamPosition, module, PEQ6XF::GAIN_PARAM));

		addOutput(createOutput<Port24>(ef1OutputPosition, module, PEQ6XF::EF1_OUTPUT));
		addOutput(createOutput<Port24>(ef2OutputPosition, module, PEQ6XF::EF2_OUTPUT));
		addOutput(createOutput<Port24>(ef3OutputPosition, module, PEQ6XF::EF3_OUTPUT));
		addOutput(createOutput<Port24>(ef4OutputPosition, module, PEQ6XF::EF4_OUTPUT));
		addOutput(createOutput<Port24>(ef5OutputPosition, module, PEQ6XF::EF5_OUTPUT));
		addOutput(createOutput<Port24>(ef6OutputPosition, module, PEQ6XF::EF6_OUTPUT));
	}
};

Model* modelPEQ6XF = createModel<PEQ6XF, PEQ6XFWidget>("Bogaudio-PEQ6XF", "PEQ6XF", "PEQ6 envelope followers expander", "Filter", "Expander", "Polyphonic");