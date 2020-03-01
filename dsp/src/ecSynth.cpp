/**
 * ecSynth.cpp
 *
 * AUTHOR: Jack Kilgore
 */

/**** Emission Control LIB ****/
#include "ecSynth.h"
#include "utility.h"

/**** ALLOLIB ****/
#include "al/io/al_File.hpp"

using namespace al;

/**** ecSynth Implementation ****/

void ecSynth::setIO(al::AudioIOData *io) {
  mPrevSR = mGlobalSamplingRate;
  grainScheduler.setSamplingRate(io->fps());
  mGlobalSamplingRate = io->fps();
}

void ecSynth::init(al::AudioIOData *io) {
  int index;

  for (index = 0; index < NUM_MODULATORS; index++) {
    Modulators.push_back(std::make_shared<ecModulator>());
  }

  for (index = 0; index < NUM_MODULATORS; index++) {
    LFOparameters.push_back(new LFOstruct{index});
  }

  mGlobalSamplingRate = io->fps();
  mPrevSR = io->fps();

  mPActiveVoices = &mActiveVoices;

  // MUST USE THIS ORDER
  grainRateLFO.setElements({"One", "Two", "Thee", "Four"});
  grainRate.setModulationSource(Modulators[0]); // Default
  grainRateLFO.registerChangeCallback(
      [&](int value) { grainRate.setModulationSource(Modulators[value]); });
  asyncLFO.setElements({"One", "Two", "Three", "Four"});
  asynchronicity.setModulationSource(Modulators[0]);
  asyncLFO.registerChangeCallback([&](int value) {
    asynchronicity.setModulationSource(Modulators[value]);
  });
  intermittencyLFO.setElements({"One", "Two", "Three", "Four"});
  intermittency.setModulationSource(Modulators[0]);
  intermittencyLFO.registerChangeCallback(
      [&](int value) { intermittency.setModulationSource(Modulators[value]); });
  streamsLFO.setElements({"One", "Two", "Three", "Four"});
  streams.setModulationSource(Modulators[0]);
  streamsLFO.registerChangeCallback(
      [&](int value) { streams.setModulationSource(Modulators[value]); });
  grainDurationLFO.setElements({"One", "Two", "Three", "Four"});
  grainDurationMs.setModulationSource(Modulators[0]);
  grainDurationLFO.registerChangeCallback([&](int value) {
    grainDurationMs.setModulationSource(Modulators[value]);
  });
  envelopeLFO.setElements({"One", "Two", "Three", "Four"});
  envelope.setModulationSource(Modulators[0]);
  envelopeLFO.registerChangeCallback(
      [&](int value) { envelope.setModulationSource(Modulators[value]); });
  tapeHeadLFO.setElements({"One", "Two", "Three", "Four"});
  tapeHead.setModulationSource(Modulators[0]);
  tapeHeadLFO.registerChangeCallback(
      [&](int value) { tapeHead.setModulationSource(Modulators[value]); });
  transpositionLFO.setElements({"One", "Two", "Three", "Four"});
  transposition.setModulationSource(Modulators[0]);
  transpositionLFO.registerChangeCallback(
      [&](int value) { transposition.setModulationSource(Modulators[value]); });

  filterLFO.setElements({"One", "Two", "Three", "Four"});
  filter.setModulationSource(Modulators[0]);
  filterLFO.registerChangeCallback(
      [&](int value) { filter.setModulationSource(Modulators[value]); });

  resonanceLFO.setElements({"One", "Two", "Three", "Four"});
  resonance.setModulationSource(Modulators[0]);
  resonanceLFO.registerChangeCallback(
      [&](int value) { resonance.setModulationSource(Modulators[value]); });

  volumeLFO.setElements({"One", "Two", "Three", "Four"});
  volumeDB.setModulationSource(Modulators[0]);
  volumeLFO.registerChangeCallback(
      [&](int value) { volumeDB.setModulationSource(Modulators[value]); });
  panLFO.setElements({"One", "Two", "Three", "Four"});
  pan.setModulationSource(Modulators[0]);
  panLFO.registerChangeCallback(
      [&](int value) { pan.setModulationSource(Modulators[value]); });
  soundFileLFO.setElements({"One", "Two", "Three", "Four"});
  soundFile.setModulationSource(Modulators[0]);
  soundFileLFO.registerChangeCallback(
      [&](int value) { soundFile.setModulationSource(Modulators[value]); });

  grainScheduler.configure(grainRate.getParam(), 0.0, 0.0);

  LFOparameters[0]->shape->registerChangeCallback(
      [&](int value) { Modulators[0]->setWaveform(value); });

  LFOparameters[0]->frequency->registerChangeCallback(
      [&](float value) { Modulators[0]->setFrequency(value); });

  LFOparameters[0]->duty->registerChangeCallback(
      [&](float value) { Modulators[0]->setWidth(value); });

  // for (int index = 0; index < NUM_MODULATORS; ++index) {
  // LFOparameters[index]->shape->registerChangeCallback(
  //     [&](int value) { Modulators[index]->setWaveform(value); });

  // LFOparameters[index]->frequency->registerChangeCallback(
  //     [&](float value) { Modulators[index]->setFrequency(value); });

  // LFOparameters[index]->duty->registerChangeCallback(
  //     [&](float value) { Modulators[index]->setWidth(value); });
  // }

  // modSineFrequency.mParameter->registerChangeCallback([&](float value) {
  // 	modSine.setFrequency(value);
  // });

  modSinePhase.mParameter->registerChangeCallback(
      [&](float value) { modSine.setPhase(value); });

  modSquareFrequency.mParameter->registerChangeCallback(
      [&](float value) { modSquare.setFrequency(value); });

  modSquareWidth.mParameter->registerChangeCallback(
      [&](float value) { modSquare.setWidth(value); });

  modSawFrequency.mParameter->registerChangeCallback(
      [&](float value) { modSaw.setFrequency(value); });
  modSawWidth.mParameter->registerChangeCallback(
      [&](float value) { modSaw.setWidth(value); });

  grainSynth.allocatePolyphony<Grain>(1024);
  grainSynth.setDefaultUserData(this);

  /**
   * Input correct number of files into parameters.
   */
  soundFile.mParameterInt->max(mClipNum);
  soundFile.mLowRange->max(mClipNum);
  soundFile.mHighRange->max(mClipNum);
  soundFile.mHighRange->set(mClipNum);
}

void ecSynth::onProcess(AudioIOData &io) {
  //        updateFromParameters();
  while (io()) {

    for (int index = 0; index < NUM_MODULATORS; ++index)
      Modulators[index]->sampleAndStore();

    modSineValue = modSine(); // construct modulation value
    modSquareValue = modSquare();
    modSawValue = modSaw();
    modNoiseValue = modNoise();

    // THIS IS WHERE WE WILL MODULATE THE GRAIN SCHEDULER

    // NOTE grainRate noise isnt very perceptible
    if (modGrainRateDepth.getParam() > 0) // modulate the grain rate
      grainScheduler.setFrequency(
          grainRate.getModParam(modGrainRateDepth.getParam()));
    // grainScheduler.setFrequency(grainRate.getModParam(modSineValue,
    // modSquareValue, modSawValue, modNoiseValue,
    // modGrainRateDepth.getParam()));
    else
      grainScheduler.setFrequency(grainRate.getParam());

    if (modAsynchronicityWidth.getParam() > 0) // modulate the asynchronicity
      grainScheduler.setAsynchronicity(asynchronicity.getModParam(
          modSineValue, modSquareValue, modSawValue, modNoiseValue,
          modAsynchronicityWidth.getParam()));
    else
      grainScheduler.setAsynchronicity(asynchronicity.getParam());

    if (modIntermittencyWidth.getParam() > 0) // modulate the intermittency
      grainScheduler.setIntermittence(intermittency.getModParam(
          modSineValue, modSquareValue, modSawValue, modNoiseValue,
          modIntermittencyWidth.getParam()));
    else
      grainScheduler.setIntermittence(intermittency.getParam());

    if (modStreamsWidth.getParam() >
        0) // Modulate the amount of streams playing.
      grainScheduler.setPolyStream(
          consts::synchronous,
          streams.getModParam(modSineValue, modSquareValue, modSawValue,
                              modNoiseValue, modStreamsWidth.getParam()));
    else
      grainScheduler.setPolyStream(consts::synchronous, streams.getParam());

    // if(modSoundFileWidth.getParam() > 0)
    //  soundFile.getModParam(modSineValue, modSquareValue, modSawValue,
    //  modNoiseValue,modSoundFileWidth.getParam());

    // CONTROL RATE LOOP (Executes every 4th sample)
    if (controlRateCounter == 4) {
      controlRateCounter = 0;
      mModClip =
          soundFile.getModParam(modSineValue, modSquareValue, modSawValue,
                                modNoiseValue, modSoundFileWidth.getParam()) -
          1;
    }
    controlRateCounter++;
    /////

    // Grain by Grain Initilization
    if (grainScheduler.trigger()) {
      auto *voice = static_cast<Grain *>(grainSynth.getFreeVoice());
      if (voice) {
        grainParameters list = {grainDurationMs,
                                modGrainDurationWidth.getParam(),
                                envelope,
                                modEnvelopeWidth.getParam(),
                                tapeHead,
                                modTapeHeadWidth.getParam(),
                                transposition,
                                modTranspositionWidth.getParam(),
                                filter,
                                modFilterDepth.getParam(),
                                resonance,
                                modResonanceDepth.getParam(),
                                volumeDB,
                                modVolumeWidth.getParam(),
                                pan,
                                modPanWidth.getParam(),
                                soundClip[mModClip],
                                modSineValue,
                                modSquareValue,
                                modSawValue,
                                modNoiseValue,
                                mPActiveVoices};

        voice->configureGrain(list, mGlobalSamplingRate);

        mActiveVoices++;
        grainSynth.triggerOn(voice, io.frame());

      } else {
        std::cout << "out of voices!" << std::endl;
      }
    }
  }

  grainSynth.render(io);

  io.frame(0);
  while (io()) {
  }
}

void ecSynth::onTriggerOn() {}

void ecSynth::onTriggerOff() {}

void ecSynth::loadSoundFile(std::string fileName) {
  bool temp = util::load(fileName, soundClip, mGlobalSamplingRate, true);
  if (temp) {
    mClipNum++;
    soundFile.mParameterInt->max(mClipNum);
    soundFile.mLowRange->max(mClipNum);
    soundFile.mHighRange->max(mClipNum);
    soundFile.mHighRange->set(mClipNum); // stylistic choice, might take out
  }
}

std::string ecSynth::loadInitSoundFiles() {
  std::string execPath = util::getExecutablePath();
  File f(execPath);
  std::string initDir = f.directory(execPath) + "samples/";
  FileList initAudioFiles = fileListFromDir(initDir);
  initAudioFiles.sort(util::compareFileNoCase);
  for (auto i = initAudioFiles.begin(); i != initAudioFiles.end(); i++) {
    if (i->file().substr(i->file().length() - 4) == ".wav" ||
        i->file().substr(i->file().length() - 4) == ".aif") {
      loadSoundFile(i->filepath());
    } else if (i->file().substr(i->file().length() - 5) == ".aiff") {
      loadSoundFile(i->filepath());
    }
  }
  return initDir;
}

void ecSynth::clearSoundFiles() {
  for (auto i = soundClip.begin(); i != soundClip.end(); i++) {
    (*i)->deleteBuffer();
  }
  soundClip.clear();

  mClipNum = 0;
  soundFile.mParameterInt->max(mClipNum);
  soundFile.mLowRange->max(mClipNum);
  soundFile.mHighRange->max(mClipNum);
  soundFile.mHighRange->set(mClipNum); // stylistic choice, might take out
}

void ecSynth::resampleSoundFiles() {
  // If sampling rate is the same as before, no need for resampling.
  if (static_cast<int>(mPrevSR) == static_cast<int>(mGlobalSamplingRate))
    return;

  int i;
  std::vector<std::string> filePaths;
  // Collect filepaths of audio buffers.
  for (i = 0; i < soundClip.size(); i++) {
    filePaths.push_back(soundClip[i]->filePath);
  }

  clearSoundFiles();
  for (i = 0; i < filePaths.size(); i++)
    loadSoundFile(filePaths[i]);
}

/**** TO DO TO DO TO DO ****/
void ecSynth::throttle(float time, float ratio) {
  if (mCounter < time * consts::SAMPLE_RATE) {
    mCounter++;
    mAvgActiveVoices += mActiveVoices;
    return;
  } else {
    mCounter++;
    mAvgActiveVoices /= mCounter;
    mCounter = 0;
  }

  float adaptThresh;

  if (mPeakCPU > adaptThresh) {
  }
  if (mAvgCPU > adaptThresh) {

  } else {
  }
}