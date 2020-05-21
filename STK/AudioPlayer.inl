#include "AudioPlayer.h"
#include <algorithm>
#include <iostream>

template <int SAMPLE_RATE, int NB_CHUNKS>
stk::AudioPlayer<SAMPLE_RATE, NB_CHUNKS>::AudioPlayer(int nbChannels) : playingOutput{ new RtWvOut(nbChannels, SAMPLE_RATE) }, nbChannels{ nbChannels }, frames{},
mutex_{}, th{}, nbChunkToRead{ 0 }, threadTicking{ false }
{
	playingOutput->stop();
};

template <int SAMPLE_RATE, int NB_CHUNKS>
stk::AudioPlayer<SAMPLE_RATE, NB_CHUNKS>::~AudioPlayer()
{
	std::lock_guard<std::mutex> _{ mutex_ };
	if (IsPlaying()) {
		th->join();
		delete th;
	}
	playingOutput->start();
	delete playingOutput;
}

template <int SAMPLE_RATE, int NB_CHUNKS>
void stk::AudioPlayer<SAMPLE_RATE, NB_CHUNKS>::ReceiveChunk(SignalData signalData)
{
	std::vector<StkFrames> buffer = signalData.decode();

	mutex_.lock();

	std::for_each(buffer.begin(), buffer.end(), [this](StkFrames& f) {
		frames.push(std::move(f));
	});
	nbChunkToRead.fetch_add(1);

	if (frames.size() >= 4 * buffer.size() && !threadTicking.load()) {
		
		th = new std::thread([&] (int bufferSize) {
			threadTicking.store(true);
			int nbTickedFrames = 0;
			while (nbChunkToRead.load() > 0) {
				tick();
				nbTickedFrames++;
				if (nbTickedFrames == bufferSize) {
					nbTickedFrames = 0;
					nbChunkToRead.fetch_sub(1);
				}
			}
			playingOutput->stop();
			threadTicking.store(false);
		}, buffer.size());
	}

	mutex_.unlock();
}

template <int SAMPLE_RATE, int NB_CHUNKS>
void stk::AudioPlayer<SAMPLE_RATE, NB_CHUNKS>::StopPlaying()
{
	std::lock_guard<std::mutex> _{ mutex_ };
	if (IsPlaying()) {
		th->join();
		delete th;
	
		playingOutput->stop();
	}
}

template <int SAMPLE_RATE, int NB_CHUNKS>
bool stk::AudioPlayer<SAMPLE_RATE, NB_CHUNKS>::IsPlaying() const
{
	return threadTicking.load();
}

template <int SAMPLE_RATE, int NB_CHUNKS>
void stk::AudioPlayer<SAMPLE_RATE, NB_CHUNKS>::tick()
{
	playingOutput->tick(frames.pop_element());
}

