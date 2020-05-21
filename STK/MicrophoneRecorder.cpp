#include "MicrophoneRecorder.h"
#include <algorithm>

#include <iostream>

stk::MicrophoneRecorder::MicrophoneRecorder(int nbChannels, int sampleRate, double chunkSecondsDuration)
	: tickSize{ static_cast<int>(sampleRate * chunkSecondsDuration) },
	recordingInput{ new RtWvIn(nbChannels) },
	currentTick{}, recordedFrames(static_cast<int>(sampleRate* chunkSecondsDuration), StkFrames(1, nbChannels)),
	th{}, mustStop{ false }, isRecording_{ false }
{
}

stk::MicrophoneRecorder::~MicrophoneRecorder()
{
	StopRecording();
	delete recordingInput;
}

void stk::MicrophoneRecorder::tick()
{
	// Eventuel reset du buffer
	if (currentTick == tickSize)
	{
		std::cout << "buffer full" << std::endl;
		std::for_each(listeners.begin(), listeners.end(), [this](ChunkListener* l) {
			l->onBufferFull(SignalData(recordedFrames));
			});

		currentTick = 0;
	}

	// Capture de la frame
	recordingInput->tick(recordedFrames[currentTick]);
	currentTick++;
}

void stk::MicrophoneRecorder::StartRecording()
{
	recordingInput->start();
	mustStop.store(false);
	isRecording_ = true;

	th = new std::thread([&] {
		while (!mustStop.load()) {
			tick();
		}
		isRecording_ = false;
	});
}

void stk::MicrophoneRecorder::StopRecording()
{
	if (IsRecording()) {
		mustStop.store(true);
		th->join();
		delete th;

		recordingInput->stop();
	}
}

bool stk::MicrophoneRecorder::IsRecording()
{
	return isRecording_.load();
}
