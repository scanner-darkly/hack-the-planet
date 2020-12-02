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
        DIRECTION_PARAM,
        LENGTH_PARAM,
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
        STEP0_LIGHT,
        STEP1_LIGHT,
        STEP2_LIGHT,
        STEP3_LIGHT,
        STEP4_LIGHT,
        STEP5_LIGHT,
        STEP6_LIGHT,
        STEP7_LIGHT,
        NUM_LIGHTS
    };

    int currentStep;
    int sequenceLength;
    dsp::SchmittTrigger clockIn;
    
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

        configParam(DIRECTION_PARAM, 0.f, 1.f, 1.f, "Direction");
        configParam(LENGTH_PARAM, 0.f, 8.f, 8.f, "Length");

        currentStep = 0;
        sequenceLength = 8;
    }

    void process(const ProcessArgs &args) override {

        if (clockIn.process(rescale(inputs[CLOCK_INPUT].getVoltage(), 0.1f, 2.f, 0.f, 1.f))) {
            
            updateLength();
            turnOffStep();
            
            if (params[DIRECTION_PARAM].getValue() == 1.f) { // forward
                currentStep++;
                if (currentStep > (sequenceLength - 1)) currentStep = 0;
            } else {
                currentStep--;
                if (currentStep < 0) currentStep = sequenceLength - 1;
                
            }
            
            outputCV();
            
            turnOnStep();
        }
    }
    
    void outputCV() {
        outputs[CV_OUTPUT].setVoltage(params[STEP0_PARAM + currentStep].getValue());
    }
    
    void turnOffStep() {
        lights[STEP0_LIGHT + currentStep].setBrightness(0.f);
    }
    
    void turnOnStep() {
        lights[STEP0_LIGHT + currentStep].setBrightness(1.f);
    }
    
    void updateLength() {
        sequenceLength = (int) clamp(std::round(params[LENGTH_PARAM].getValue()), 1.f, 8.f);
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
        
        addChild(createLight<SmallLight<RedLight>>(mm2px(Vec(29, 68)), module, Sequencer::STEP0_LIGHT));
        addChild(createLight<SmallLight<RedLight>>(mm2px(Vec(49, 68)), module, Sequencer::STEP1_LIGHT));
        addChild(createLight<SmallLight<RedLight>>(mm2px(Vec(69, 68)), module, Sequencer::STEP2_LIGHT));
        addChild(createLight<SmallLight<RedLight>>(mm2px(Vec(89, 68)), module, Sequencer::STEP3_LIGHT));
        addChild(createLight<SmallLight<RedLight>>(mm2px(Vec(109, 68)), module, Sequencer::STEP4_LIGHT));
        addChild(createLight<SmallLight<RedLight>>(mm2px(Vec(129, 68)), module, Sequencer::STEP5_LIGHT));
        addChild(createLight<SmallLight<RedLight>>(mm2px(Vec(149, 68)), module, Sequencer::STEP6_LIGHT));
        addChild(createLight<SmallLight<RedLight>>(mm2px(Vec(169, 68)), module, Sequencer::STEP7_LIGHT));
        
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(170, 110)), module, Sequencer::CV_OUTPUT));
        
        addParam(createParam<CKSS>(Vec(50, 110), module, Sequencer::DIRECTION_PARAM));
        addParam(createParam<RoundBlackSnapKnob>(Vec(80, 105), module, Sequencer::LENGTH_PARAM));
    }
};


Model *modelSequencer = createModel<Sequencer, SequencerWidget>("Sequencer");