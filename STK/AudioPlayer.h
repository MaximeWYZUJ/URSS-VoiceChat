#pragma once
#include "RtWvOut.h"
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include "file_circulaire_concurrente.h"
#include "SignalData.h"

namespace stk {

template <int SAMPLE_RATE = 44100, int NB_CHUNKS = 20>
class AudioPlayer
{
	mutable std::thread* th;
	mutable std::mutex mutex_;
	mutable std::atomic<int> nbChunkToRead;
	mutable std::atomic<bool> threadTicking;

	RtWvOut* playingOutput;
	int nbChannels;
	file_circulaire_concurrente<StkFrames, SAMPLE_RATE * NB_CHUNKS> frames;

	// Jouer une frame
	void tick();

public:
	AudioPlayer(int nbChannels);
	~AudioPlayer();

	// Incopiable
	AudioPlayer(const AudioPlayer&) = delete;
	AudioPlayer& operator=(const AudioPlayer&) = delete;

	// Ajouter des frames a jouer
	void ReceiveChunk(SignalData buffer);

	void StopPlaying();

	bool IsPlaying() const;
};

}

#include "AudioPlayer.inl"