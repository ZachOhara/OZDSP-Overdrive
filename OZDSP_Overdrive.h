#ifndef __OZDSP_OVERDRIVE__
#define __OZDSP_OVERDRIVE__

#include "IPlug_include_in_plug_hdr.h"
#include <iostream>
#include <math.h>
#include <minmax.h>

static const double MIN_THRESHOLD = 0.01;

class OZDSP_Overdrive : public IPlug
{
public:
	OZDSP_Overdrive(IPlugInstanceInfo instanceInfo);
	~OZDSP_Overdrive();

	void Reset();
	void OnParamChange(int paramIdx);
	void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
	double mVolume;
	double mThreshold;
	double mTone;

	void CreatePresets();
};

#endif
