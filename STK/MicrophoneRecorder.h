#pragma once
#include "RtWvIn.h"
#include "SignalData.h"
#include <vector>
#include <thread>
#include <atomic>

namespace stk {

class ChunkListener {
public:
	ChunkListener() = default;
	virtual ~ChunkListener() = default;

	virtual void onBufferFull(SignalData buffer) = 0;
};

class MicrophoneRecorder
{
	mutable std::thread* th;
	mutable std::atomic<bool> mustStop;
	mutable std::atomic<bool> isRecording_;

	RtWvIn* recordingInput;
	int tickSize;
	int currentTick;
	std::vector<StkFrames> recordedFrames;
	std::vector<ChunkListener*> listeners;

	// Capture du son
	void tick();

public:
	MicrophoneRecorder(int nbChannels, int sampleRate, double chunkSecondsDuration);
	~MicrophoneRecorder();

	// Incopiable
	MicrophoneRecorder(const MicrophoneRecorder&) = delete;
	MicrophoneRecorder& operator=(const MicrophoneRecorder&) = delete;

	// Demarrage/Arret de la capture
	void StartRecording();
	void StopRecording();
	bool IsRecording();

	void AddListener(ChunkListener* listener) { listeners.push_back(listener); }
};

};