
// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#include "MLProcHostPhasor.h"

// ----------------------------------------------------------------
// registry section

namespace
{
	MLProcRegistryEntry<MLProcHostPhasor> classReg("host_phasor");
	ML_UNUSED MLProcOutput<MLProcHostPhasor> outputs[] = { "out" };
}			

// ----------------------------------------------------------------
// implementation

MLProcHostPhasor::MLProcHostPhasor() :
mDpDt(0),
mPhase1(0.),
mDt(0)
{
	clear();
//	//debug() << "MLProcHostPhasor constructor\n";
}

MLProcHostPhasor::~MLProcHostPhasor()
{
//	//debug() << "MLProcHostPhasor destructor\n";
}

void MLProcHostPhasor::doParams(void) 
{
	//static const float bpmToHz = 1.f / 240.f * 16.f; // 16th notes
	// mSr = getContextSampleRate();
	
	// new phase
	//float sixteenths = mTime * 4.f;
	//int ks = (int)sixteenths;
	mParamsChanged = false;
}

// set input parameters from host info
void MLProcHostPhasor::setTimeAndRate(const double secs, const double ppqPos, const double bpm, bool isPlaying)
{
	// working around a bug I can't reproduce, so I'm covering all the bases.
	if ( ((ml::isNaN(ppqPos)) || (ml::isInfinite(ppqPos)))
		|| ((ml::isNaN(bpm)) || (ml::isInfinite(bpm)))
		|| ((ml::isNaN(secs)) || (ml::isInfinite(secs))) ) 
	{
		//debug() << "MLProcHostPhasor::setTimeAndRate: bad input! \n";
		return;
	}
	
	// debug() << "setTimeAndRate: secs " << secs << " ppq: " << ppqPos << " playing: " << isPlaying << " phase: " << mPhase1 << " dp: " << mDpDt << "\n" ;
	
	double phase = 0.;
	double newTime = ml::clamp(ppqPos, 0., 100000.);
	mActive = (mTime != newTime) && (secs >= 0.) && isPlaying;
	if (mActive)
	{
		mTime = newTime;
		mParamsChanged = true;

		phase = newTime - int(newTime);
		mOmega = (float)phase;
		double newRate = ml::clamp(bpm, 0., 1000.);
		if (mRate != newRate)
		{
			mRate = newRate;				
			mParamsChanged = true;
		}
		
		double dPhase = phase - mPhase1;
		if(dPhase < 0.)
		{
			dPhase += 1.;
		}
		mDpDt = ml::clamp(dPhase/static_cast<double>(mDt), 0., 1.);	
	}
	else
	{
		mOmega = -0.0001f;
		mDpDt = 0.;	
	}
	
	mPhase1 = phase;
	mDt = 0;
}

void MLProcHostPhasor::clear()
{	
	mTime = 0.f;
	mRate = 0.f;
	mOmega = 0.f;
	mActive = 0;
	mPlaying = 0;
}

// generate a quarter-note phasor from the input parameters
void MLProcHostPhasor::process()
{	
	if (mParamsChanged) 
	{
		doParams();
	}

	MLSignal& y = getOutput();
	for (int n=0; n<kFloatsPerDSPVector; ++n)
	{
		mOmega += mDpDt;
		if(mOmega > 1.f) 
		{
			mOmega -= 1.f;
		}
		y[n] = mOmega;
	}
	mDt += kFloatsPerDSPVector;
	////debug() << y[0] << " -- " << y[samples - 1] << "\n";
}
  
