#ifndef __OZDSP_OVERDRIVE__
#define __OZDSP_OVERDRIVE__

#include "IPlug_include_in_plug_hdr.h"

#include <minmax.h>

#include "../OZDSP_Common/CommonParameters.h"
#include "../OZDSP_Common/ParamValueLabel.h"
#include "../OZDSP_Common/ToneControl.h"
#include "../OZDSP_Common/VolumeControl.h"

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
	ToneControl mToneControl;
	VolumeControl mVolumeControl;

	ParamValueLabel* mpDriveLabel;
	ParamValueLabel* mpToneLabel;
	ParamValueLabel* mpVolumeLabel;

	double mThreshold;

	void CreatePresets();
};

#endif
