#include "plugin.hpp"


struct Sequencer : Module {
    enum ParamIds {
        STEP0_PARAM,
        STEP1_PARAM,
        STEP2_PARAM,
        STEP3_PARAM,
        STEP4_PARAM,
        STEP5_PARAM,
        STEP6_PARAM,
        STEP7_PARAM,
        STEP8_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        CLOCK_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        CV_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    Sequencer() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        
        configParam(STEP0_PARAM, 0.f, 10.f, 0.f, "Step 0");
        configParam(STEP1_PARAM, 0.f, 10.f, 0.f, "Step 1");
        configParam(STEP2_PARAM, 0.f, 10.f, 0.f, "Step 2");
        configParam(STEP3_PARAM, 0.f, 10.f, 0.f, "Step 3");
        configParam(STEP4_PARAM, 0.f, 10.f, 0.f, "Step 4");
        configParam(STEP5_PARAM, 0.f, 10.f, 0.f, "Step 5");
        configParam(STEP6_PARAM, 0.f, 10.f, 0.f, "Step 6");
        configParam(STEP7_PARAM, 0.f, 10.f, 0.f, "Step 7");
    }

    void process(const ProcessArgs &args) override {
    }
};


struct SequencerWidget : ModuleWidget {
    SequencerWidget(Sequencer *module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Sequencer.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30, 110)), module, Sequencer::CLOCK_INPUT));
        
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(30, 60)), module, Sequencer::STEP0_PARAM));
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(50, 60)), module, Sequencer::STEP1_PARAM));
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(70, 60)), module, Sequencer::STEP2_PARAM));
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(90, 60)), module, Sequencer::STEP3_PARAM));
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(110, 60)), module, Sequencer::STEP4_PARAM));
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(130, 60)), module, Sequencer::STEP5_PARAM));
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(150, 60)), module, Sequencer::STEP6_PARAM));
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(170, 60)), module, Sequencer::STEP7_PARAM));
        
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(170, 110)), module, Sequencer::CV_OUTPUT));
    }
};


Model *modelSequencer = createModel<Sequencer, SequencerWidget>("Sequencer");