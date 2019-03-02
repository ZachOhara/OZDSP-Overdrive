#include "OZDSP_Overdrive.h"
#include "IPlug_include_in_plug_src.h"
#include "resource.h"

const int kNumPrograms = 0;

enum EParams
{
	kDrivePid,
	kTonePid,
	kVolumePid,
	kNumParams
};

std::vector<ParameterInfo> kParameterList =
{
	ParameterInfo()
		.InitParam("Drive", kDrivePid, KNOB_80_ID, 30, 35)
		.InitLabel()
		.MakePercentageParam(0.0),
	ParameterInfo()
		.InitParam("Tone", kTonePid, KNOB_80_ID, 130, 35)
		.InitLabel()
		.MakePercentageParam(),
	ParameterInfo()
		.InitParam("Volume", kVolumePid, KNOB_80_ID, 230, 35)
		.InitLabel()
		.MakeVolumeReductionParam(),
};

OZDSP_Overdrive::OZDSP_Overdrive(IPlugInstanceInfo instanceInfo) :
	CommonPlugBase(instanceInfo, kNumParams, kNumPrograms,
		MakeGraphics(this, GUI_WIDTH, GUI_HEIGHT),
		COMMONPLUG_CTOR_PARAMS)
{
	SetBackground(BACKGROUND_ID, BACKGROUND_FN);
	RegisterBitmap(KNOB_80_ID, KNOB_80_FN, KNOB_FRAMES);
	AddParameters(kParameterList);
	AddToneParamBridge(kTonePid, &mToneProcessor);
	AddVolumeParamBridge(kVolumePid, &mVolumeProcessor);
	FinishConstruction();
}

OZDSP_Overdrive::~OZDSP_Overdrive() {}

void OZDSP_Overdrive::CreatePresets()
{
	// No presets
}

void OZDSP_Overdrive::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
	// Mutex is already locked for us.
	const int nChannels = 2;

	for (int i = 0; i < nFrames; i++)
	{
		for (int j = 0; j < nChannels; j++)
		{
			double in = inputs[j][i];
			double out = in;

			// Apply threshold
			if (in > 0)
			{
				out = min(in, mThreshold);
			}
			else
			{
				out = max(in, -mThreshold);
			}
			// Correct for volume decrease
			out /= mThreshold;
			// Apply settings
			out = mToneProcessor.GetAdjustedSample(in, out);
			out = mVolumeProcessor.GetAdjustedSample(out);

			// Output
			outputs[j][i] = out;
		}
	}
}

void OZDSP_Overdrive::OnParamChange(int paramIndex)
{
	CommonPlugBase::OnParamChange(paramIndex);

	switch (paramIndex)
	{
	case kDrivePid:
		mThreshold = 1 - (GetParam(kDrivePid)->Value() / 100.0);
		mThreshold = pow(mThreshold, 2);
		mThreshold = max(mThreshold, kMinThreshold);
		break;
	default:
		break;
	}
}
