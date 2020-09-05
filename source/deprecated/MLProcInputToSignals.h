
// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#ifndef ML_PROC_MIDI_TO_SIGNALS_H
#define ML_PROC_MIDI_TO_SIGNALS_H

#include "AppConfig.h"

#include "MLDSP.h"
#include "MLProc.h"
#include "MLScale.h"
#include "MLChangeList.h"
#include "MLInputProtocols.h"
#include "MLControlEvent.h"
#include "MLT3DHub.h"
#include "MLDefaultFileLocations.h"

// a voice that can play.
//
class MLVoice
{
public:
  
  enum State
  {
    kOff,
    kOn,
    kSustain
  };
  
  MLVoice();
  ~MLVoice() {} ;
  void clearState();
  void clearChanges();
  void setSampleRate(float sr);
  void resize(int size);
  void zero();
  void zeroExceptPitch();
  void zeroPressure();
  
  // send a note on, off or sustain event to the voice.
  void addNoteEvent(const MLControlEvent& e, const MLScale& scale);
  void stealNoteEvent(const MLControlEvent& e, const MLScale& scale, bool retrig);
  
  int mState;
  int mInstigatorID; // for matching event sources, could be MIDI key, or touch number.
  int mNote;
  int mAge;  // time active, measured to the end of the current process buffer
  int mChannel{0}; // channel that activated this voice -- for MPE
  
  // for continuous touch inputs (OSC)
  float mStartX;
  float mStartY;
  float mStartVel;
  float mPitch;
  float mX1;
  float mY1;
  float mZ1;
  
  MLChangeList mdPitch;
  MLChangeList mdPitchBend;
  MLChangeList mdGate;
  MLChangeList mdAmp;
  MLChangeList mdVel;
  MLChangeList mdNotePressure;
  MLChangeList mdChannelPressure;
  MLChangeList mdMod;
  MLChangeList mdMod2;
  MLChangeList mdMod3;
  MLChangeList mdDrift;
  
  MLControlEvent mCurrentUnisonNoteEvent;
};

extern const ml::Symbol voiceSignalNames[];

static constexpr int kMPEInputChannels{16};

class MLProcInputToSignals : public MLProc
{
public:
  
  static const float kControllerScale;
  static const float kDriftConstantsAmount;
  static const float kDriftRandomAmount;
  
  MLProcInputToSignals();
  ~MLProcInputToSignals();
  
  MLProcInfoBase& procInfo() override { return mInfo; }
  int getOutputIndex(const ml::Symbol name) override;
  
  void setInputFrameBuffer(Queue<TouchFrame>* pBuf);
  
  // TODO we can't possibly need all of these methods. redo and document clearly the function of each.
  void clear() override;
  void setup() override;
  err resize() override;
  MLProc::err prepareToProcess() override;
	
	// TEMP MLTEST
	// TODO make this not a Proc use processEvent() API in parent
	Queue<MLControlEvent>* mEventQueue{nullptr};
	void setQueue(Queue<MLControlEvent>* q)
	{
		mEventQueue = q;
	}
	
  void process() override;
	
  void clearChangeLists();
  void doParams();
  
  void setVectorStartTime(uint64_t t);
  
private:
  
  //void processOSC(const int n);
  void processEvents();
  void writeOutputSignals(const int n);
  
  void processEvent(const MLControlEvent& event);
  void doNoteOn(const MLControlEvent& event);
  void doNoteOff(const MLControlEvent& event);
  void doNoteUpdate(const MLControlEvent& event);
  void doSustain(const MLControlEvent& event);
  void doController(const MLControlEvent& event);
  void doPitchWheel(const MLControlEvent& event);
  void doNotePressure(const MLControlEvent& event);
  void doChannelPressure(const MLControlEvent& event);
  
  void dumpEvents();
  void dumpVoices();
  void dumpSignals();
  void dumpTouchFrame();

  int findFreeVoice(size_t start, size_t len);
  int findOldestSustainedVoice();
  int findNearestVoice(int note);
  int findOldestVoice();
  
  int MPEChannelToVoiceIDX(int i);
  
  // OSC, MIDI, MIDI_MPE or nothing
  // MIDI_MPE enables MPE (Multidimensional Polyphonic Expression) mode via MIDI
  int mProtocol;
  
  MLProcInfo<MLProcInputToSignals> mInfo;
  Queue<TouchFrame>* mpFrameBuf{nullptr};
  TouchFrame mPrevTouchFrame;
  TouchFrame mLatestTouchFrame;
  TouchFrame mLatestTouchFrameSorted;

  // TODO remove these custom container types
  MLControlEventVector mNoteEventsPlaying;    // notes with keys held down and sounding
  
  // notes stolen that may play again when voices are freed. in unison mode only.
  MLControlEventStack mNoteEventsPending;
  
  // the usual voices for each channel
  std::vector<MLVoice> mVoices;
  
  std::array<MLChangeList, kMPEInputChannels> mPitchBendChangesByChannel;
  std::array<MLSignal, kMPEInputChannels> mPitchBendSignals;
  
  // a special voice for the MPE "Main Channel"
  // stores main pitch bend and controller inputs, which are added to other voices.
  MLVoice mMPEMainVoice;
  
  int mNextEventIdx;
  int mVoiceRotateOffset;
  
  int mEventTimeOffset;
  
  int mControllerNumber;
  int mControllerMPEXNumber; 
  
  int mCurrentVoices;
  int mDriftCounter;
  int mEventCounter;
  int mFrameCounter;
  
  MLRange mPitchRange;
  MLRange mAmpRange;
  bool mGlissando;
  bool mUnisonMode;
  bool mRotateMode;
  int mUnisonInputTouch;
  float mUnisonVel;
  float mGlide;
  
  int mOSCDataRate;
  
  float mUnisonPitch1;
  
  MLSignal mTempSignal;
  MLSignal mMainPitchSignal;
  MLSignal mMainChannelPressureSignal;
  MLSignal mMainModSignal;
  MLSignal mMainMod2Signal;
  MLSignal mMainMod3Signal;
  
  float mPitchWheelSemitones;
  MLScale mScale;
	float mMasterTune {440.f};
	float mMasterPitchOffset {0.f};
  
  int temp;
  bool mSustainPedal;
  std::string mScalePath;
  
  uint64_t mVectorStartTime;
  ml::NoiseGen mRand;

  int mNullFrameCounter{};
  float mPreviousMaxZ{};
};


#endif // ML_PROC_MIDI_TO_SIGNALS_H

