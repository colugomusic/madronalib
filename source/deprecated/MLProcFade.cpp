
// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#include "MLProc.h"

using namespace ml;

// ----------------------------------------------------------------
// class definition

class MLProcFade : public MLProc
{
public:
	void process() override;		
	MLProcInfoBase& procInfo() override { return mInfo; }
	
private:
	MLProcInfo<MLProcFade> mInfo;
};

// ----------------------------------------------------------------
// registry section

namespace
{
	MLProcRegistryEntry<MLProcFade> classReg("fade");
	// no parameters.  ML_UNUSED MLProcParam<MLProcFade> params[1] = { "" };
	ML_UNUSED MLProcInput<MLProcFade> inputs[] = {"in1", "in2", "mix"}; 
	ML_UNUSED MLProcOutput<MLProcFade> outputs[] = {"out"};
}

// ----------------------------------------------------------------
// implementation

void MLProcFade::process()
{	
	const MLSignal& in1 = getInput(1);
	const MLSignal& in2 = getInput(2);
	const MLSignal& mix = getInput(3);
	MLSignal& out = getOutput();
	
	for (int n=0; n<kFloatsPerDSPVector; ++n)
	{
		const MLSample a = in1[n];
		const MLSample b = in2[n];
		out[n] = a + (b - a)*mix[n];
	}
}
