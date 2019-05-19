#ifndef __OZDSP_OVERDRIVE__
#define __OZDSP_OVERDRIVE__

#include "IPlug_include_in_plug_hdr.h"

#include "../OZDSP_Core/CorePlugBase.h"
#include "../OZDSP_Core/parameter/ParameterInfo.h"
#include "../OZDSP_Core/audio/ToneMixProcessor.h"
#include "../OZDSP_Core/audio/VolumeProcessor.h"

#include <minmax.h>

class OZDSP_Overdrive : public CorePlugBase
{
public:
	OZDSP_Overdrive(IPlugInstanceInfo instanceInfo);
	~OZDSP_Overdrive();

	void OnParamChange(int paramIndex) override;
	void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames) override;

protected:
	void CreatePresets() override;

private:
	ToneMixProcessor mToneProcessor;
	VolumeProcessor mVolumeProcessor;

	const double kMinThreshold = 0.01;
	double mThreshold = 1.0;
};

#endif
