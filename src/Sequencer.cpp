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
        START_STEP_PARAM,
        END_STEP_PARAM,
        STEP_PARAM,
        DIVIDER_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        CLOCK_INPUT,
        START_STEP_INPUT,
        END_STEP_INPUT,
        STEP_INPUT,
        DIVIDER_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        CV_OUTPUT,
        GATE_OUTPUT,
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
    int startStep;
    int endStep;
    int step;
    int division;
    int divisionCounter;

    dsp::SchmittTrigger clockIn;
    dsp::PulseGenerator gatePulse;
    
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
        configParam(START_STEP_PARAM, 0.f, 7.f, 7.f, "Start Step");
        configParam(END_STEP_PARAM, 0.f, 7.f, 7.f, "End Step");
        configParam(STEP_PARAM, 1.f, 7.f, 1.f, "Step");
        configParam(DIVIDER_PARAM, 1.f, 8.f, 1.f, "Clock divider");

        currentStep = 0;
        startStep = 0;
        endStep = 7;
        step = 1;
        divisionCounter = 0;
    }

    void process(const ProcessArgs &args) override {
        setGate(gatePulse.process(args.sampleTime) != 0.f);
        
        if (clockIn.process(rescale(inputs[CLOCK_INPUT].getVoltage(), 0.1f, 2.f, 0.f, 1.f))) {
            updateDivision();
            
            divisionCounter++;
            if (divisionCounter >= division) divisionCounter = 0;
            
            if (divisionCounter != 0) return;
                
            updateSteps();
            turnOffStep();
            
            if (params[DIRECTION_PARAM].getValue() == 1.f) { // forward
                if (currentStep == endStep) {
                    currentStep = startStep;
                } else {
                    currentStep += step;
                    if (currentStep > 7) currentStep = 0;
                }
            } else {
                if (currentStep == startStep) {
                    currentStep = endStep;
                } else {
                    currentStep -= step;
                    if (currentStep < 0) currentStep = 7;
                }
            }
            
            outputCV();
            gatePulse.trigger();
            turnOnStep();
        }
    }
    
    void outputCV() {
        outputs[CV_OUTPUT].setVoltage(params[STEP0_PARAM + currentStep].getValue());
    }
    
    void setGate(bool high) {
        outputs[GATE_OUTPUT].setVoltage(high ? 10.f : 0.f);
    }
    
    void turnOffStep() {
        lights[STEP0_LIGHT + currentStep].setBrightness(0.f);
    }
    
    void turnOnStep() {
        lights[STEP0_LIGHT + currentStep].setBrightness(1.f);
    }
    
    void updateSteps() {
        startStep = (int) clamp(std::round(params[START_STEP_PARAM].getValue() + inputs[START_STEP_INPUT].getVoltage()), 0.f, 7.f);
        endStep = (int) clamp(std::round(params[END_STEP_PARAM].getValue() + inputs[END_STEP_INPUT].getVoltage()), 0.f, 7.f);
        step = (int) clamp(std::round(params[STEP_PARAM].getValue() + inputs[STEP_INPUT].getVoltage()), 1.f, 7.f);
    }

    void updateDivision() {
        division = (int) clamp(std::round(params[DIVIDER_PARAM].getValue() + inputs[DIVIDER_INPUT].getVoltage()), 1.f, 8.f);
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
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(50, 110)), module, Sequencer::DIVIDER_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(90, 110)), module, Sequencer::START_STEP_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(110, 110)), module, Sequencer::END_STEP_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(130, 110)), module, Sequencer::STEP_INPUT));
        
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
        
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(170, 95)), module, Sequencer::GATE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(170, 110)), module, Sequencer::CV_OUTPUT));
        
        addParam(createParamCentered<CKSS>(mm2px(Vec(30, 95)), module, Sequencer::DIRECTION_PARAM));
        addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(50, 95)), module, Sequencer::DIVIDER_PARAM));
        addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(90, 95)), module, Sequencer::START_STEP_PARAM));
        addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(110, 95)), module, Sequencer::END_STEP_PARAM));
        addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(130, 95)), module, Sequencer::STEP_PARAM));
    }
};


Model *modelSequencer = createModel<Sequencer, SequencerWidget>("Sequencer");