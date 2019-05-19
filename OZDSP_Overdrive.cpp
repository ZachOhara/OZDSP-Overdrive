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
		.InitParam("Drive", kDrivePid, DRIVE_CONTROL_ID, 30, 30)
		.InitLabel()
		.MakePercentageParam(50.0),
	ParameterInfo()
		.InitParam("Tone", kTonePid, TONE_CONTROL_ID, 130, 30)
		.InitLabel()
		.MakePercentageParam(50.0),
	ParameterInfo()
		.InitParam("Volume", kVolumePid, VOLUME_CONTROL_ID, 230, 30)
		.InitLabel()
		.MakeVolumeReductionParam(),
};

OZDSP_Overdrive::OZDSP_Overdrive(IPlugInstanceInfo instanceInfo) :
	CorePlugBase(instanceInfo, kNumParams, kNumPrograms,
		MakeGraphics(this, GUI_WIDTH, GUI_HEIGHT),
		COMMONPLUG_CTOR_PARAMS)
{
	SetBackground(BACKGROUND_ID, BACKGROUND_FN);

	RegisterBitmap(DRIVE_CONTROL_ID, DRIVE_CONTROL_FN, DRIVE_CONTROL_FRAMES);
	RegisterBitmap(TONE_CONTROL_ID, TONE_CONTROL_FN, TONE_CONTROL_FRAMES);
	RegisterBitmap(VOLUME_CONTROL_ID, VOLUME_CONTROL_FN, VOLUME_CONTROL_FRAMES);

	AddParameterList(kParameterList);

	RegisterProcessor(&mToneProcessor);
	mToneProcessor.RegisterParameter(kTonePid, ToneMixProcessor::kMixPercentParam);

	RegisterProcessor(&mVolumeProcessor);
	mVolumeProcessor.RegisterParameter(kVolumePid, VolumeProcessor::kDecibelsParam);

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
	CorePlugBase::OnParamChange(paramIndex);

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
