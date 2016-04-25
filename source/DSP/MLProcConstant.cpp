
// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#include "MLProc.h"
#include "MLChangeList.h"

// ----------------------------------------------------------------
// class definition


class MLProcConstant : public MLProc
{
public:
	MLProcConstant();
	~MLProcConstant();

	err prepareToProcess();
	void process(const int n);		
	MLProcInfoBase& procInfo() { return mInfo; }

private:
	MLProcInfo<MLProcConstant> mInfo;
	float mVal;	
};

// ----------------------------------------------------------------
// registry section

namespace
{
	MLProcRegistryEntry<MLProcConstant> classReg("constant");
	ML_UNUSED MLProcParam<MLProcConstant> params[] = {"in"};
	ML_UNUSED MLProcOutput<MLProcConstant> outputs[] = {"out"};
}

// ----------------------------------------------------------------
// implementation

MLProcConstant::MLProcConstant()
{
}

MLProcConstant::~MLProcConstant()
{
}

MLProc::err MLProcConstant::prepareToProcess()
{
	mVal = getParam("in");
	return OK;
}

void MLProcConstant::process(const int frames)
{
	MLSignal& y = getOutput();
	y.setToConstant(mVal);
	// y.setVecToConstant(mVal); // TODO when copmiler has const vec size
}


