#include "OZDSP_Overdrive.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 0;

enum EParams
{
	kDrivePid,
	kTonePid,
	kVolumePid,
	kNumParams
};

enum ELayout
{
	kWidth = GUI_WIDTH,
	kHeight = GUI_HEIGHT,
	kKnobFrames = 128,

	kDriveKnobX = 30,
	kDriveKnobY = 35,
	kDriveLabelX = 30,
	kDriveLabelY = 105,
	kDriveLabelWidth = 80,

	kToneKnobX = 130,
	kToneKnobY = 35,
	kToneLabelX = 130,
	kToneLabelY = 105,
	kToneLabelWidth = 80,

	kVolumeKnobX = 230,
	kVolumeKnobY = 35,
	kVolumeLabelX = 230,
	kVolumeLabelY = 105,
	kVolumeLabelWidth = 80
};

OZDSP_Overdrive::OZDSP_Overdrive(IPlugInstanceInfo instanceInfo) :
	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
	mThreshold(1.0)
{
	TRACE;

	//arguments are: name, defaultVal, minVal, maxVal, step, label
	InitPercentParameter(GetParam(kDrivePid), "Drive", 0.0);
	InitPercentParameter(GetParam(kTonePid), "Tone", 100.0);
	InitVolumeParameter(GetParam(kVolumePid));

	IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
	pGraphics->AttachBackground(BACKGROUND_RID, BACKGROUND_FN);

	IBitmap knob80 = pGraphics->LoadIBitmap(KNOB_80_RID, KNOB_80_FN, kKnobFrames);

	pGraphics->AttachControl(new IKnobMultiControl(this, kVolumeKnobX, kVolumeKnobY, kVolumePid, &knob80));
	pGraphics->AttachControl(new IKnobMultiControl(this, kDriveKnobX, kDriveKnobY, kDrivePid, &knob80));
	pGraphics->AttachControl(new IKnobMultiControl(this, kToneKnobX, kToneKnobY, kTonePid, &knob80));

	mpDriveLabel = new ParamValueLabel(this, kDrivePid, kDriveLabelX, kDriveLabelY, kDriveLabelWidth);
	mpToneLabel = new ParamValueLabel(this, kTonePid, kToneLabelX, kToneLabelY, kToneLabelWidth);
	mpVolumeLabel = new ParamValueLabel(this, kVolumePid, kVolumeLabelX, kVolumeLabelY, kVolumeLabelWidth);

	pGraphics->AttachControl(mpDriveLabel);
	pGraphics->AttachControl(mpToneLabel);
	pGraphics->AttachControl(mpVolumeLabel);

	AttachGraphics(pGraphics);

	CreatePresets();
	ForceUpdateParams(this);
}

OZDSP_Overdrive::~OZDSP_Overdrive() {}

void OZDSP_Overdrive::CreatePresets()
{
	// TODO
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
			// Correct for volume decrease from clipping
			out /= mThreshold;

			// Apply the mix setting
			//out = (mTone * out) + ((1 - mTone) * in);
			out = mToneControl.GetAdjustedSample(in, out);

			// Apply the volume setting
			//out *= mVolume;
			out = mVolumeControl.GetAdjustedSample(out);

			// Output
			outputs[j][i] = out;
		}
	}
}

void OZDSP_Overdrive::Reset()
{
	TRACE;
	IMutexLock lock(this);
}

void OZDSP_Overdrive::OnParamChange(int paramIdx)
{
	IMutexLock lock(this);

	switch (paramIdx)
	{
	case kDrivePid:
		mThreshold = 1 - (GetParam(kDrivePid)->Value() / 100.0);
		mThreshold = pow(mThreshold, 2);
		mThreshold = max(mThreshold, MIN_THRESHOLD);
		mpDriveLabel->UpdateDisplay();
		break;
	case kTonePid:
		mToneControl.SetMixPercent(GetParam(kTonePid)->Value());
		mpToneLabel->UpdateDisplay();
		break;
	case kVolumePid:
		HandleVolumeParamChange(GetParam(kVolumePid), &mVolumeControl);
		mpVolumeLabel->UpdateDisplay();
		break;
	default:
		break;
	}
}
