#include "OZDSP_Overdrive.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 0;

enum EParams
{
	kVolumePid,
	kDrivePid,
	kTonePid,
	kNumParams
};

enum ELayout
{
	kWidth = GUI_WIDTH,
	kHeight = GUI_HEIGHT,
	kKnobFrames = 128,

	kVolumeKnobX = 30,
	kVolumeKnobY = 35,
	kDriveKnobX = 130,
	kDriveKnobY = 35,
	kToneKnobX = 230,
	kToneKnobY = 35
};

OZDSP_Overdrive::OZDSP_Overdrive(IPlugInstanceInfo instanceInfo) :
	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
	mVolume(0.5),
	mThreshold(1.0),
	mTone(0.5)
{
	TRACE;

	//arguments are: name, defaultVal, minVal, maxVal, step, label
	GetParam(kVolumePid)->InitDouble("Volume", 5.0, 0.0, 10.0, 0.01, "");
	GetParam(kDrivePid)->InitDouble("Drive", 0.0, 0.0, 10.0, 0.01, "");
	GetParam(kTonePid)->InitDouble("Tone", 5.0, 0.0, 10.0, 0.01, "");


	IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
	pGraphics->AttachBackground(BACKGROUND_RID, BACKGROUND_FN);

	IBitmap knob80 = pGraphics->LoadIBitmap(KNOB_80_RID, KNOB_80_FN, kKnobFrames);

	pGraphics->AttachControl(new IKnobMultiControl(this, kVolumeKnobX, kVolumeKnobY, kVolumePid, &knob80));
	pGraphics->AttachControl(new IKnobMultiControl(this, kDriveKnobX, kDriveKnobY, kDrivePid, &knob80));
	pGraphics->AttachControl(new IKnobMultiControl(this, kToneKnobX, kToneKnobY, kTonePid, &knob80));

	AttachGraphics(pGraphics);

	CreatePresets();
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
			if (in > 0)
			{
				out = min(in, mThreshold);
			}
			else
			{
				out = max(in, -mThreshold);
			}
			out /= mThreshold;
			out *= mVolume;

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
	case kVolumePid:
		mVolume = pow(GetParam(kVolumePid)->Value() / 10, 2);
	case kDrivePid:
		mThreshold = 1 - (GetParam(kDrivePid)->Value() / 10);
		mThreshold = pow(mThreshold, 2);
		mThreshold = max(mThreshold, MIN_THRESHOLD);
		break;
	case kTonePid:
		mTone = GetParam(kTonePid)->Value() / 10;
		break;
	default:
		break;
	}
}
