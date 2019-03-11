#ifndef __OZDSP_OVERDRIVE__
#define __OZDSP_OVERDRIVE__

#include "IPlug_include_in_plug_hdr.h"

#include "../OZDSP_Common/CommonPlugBase.h"
#include "../OZDSP_Common/parameter/ParameterInfo.h"
#include "../OZDSP_Common/processing/ToneProcessor.h"
#include "../OZDSP_Common/processing/VolumeProcessor.h"

#include <minmax.h>

class OZDSP_Overdrive : public CommonPlugBase
{
public:
	OZDSP_Overdrive(IPlugInstanceInfo instanceInfo);
	~OZDSP_Overdrive();

	void OnParamChange(int paramIndex) override;
	void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames) override;

protected:
	void CreatePresets() override;

private:
	const double kMinThreshold = 0.01;
	double mThreshold = 1.0;

	ToneProcessor mToneProcessor;
	VolumeProcessor mVolumeProcessor;
};

#endif
