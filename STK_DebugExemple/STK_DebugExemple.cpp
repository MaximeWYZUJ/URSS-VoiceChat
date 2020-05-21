#define __OS_WINDOWS_STK__
#define __WINDOWS_DS_STK__

#include "AudioPlayer.h"
#include "MicrophoneRecorder.h"
#include "SignalData.h"
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

#include "FftRealPair.hpp"

using namespace stk;
using namespace std;

static std::random_device rdev;
static std::mt19937 rgen(rdev());


class MicroListener : public ChunkListener {
    AudioPlayer<>* player;

public:
    MicroListener(AudioPlayer<>* p) : player{ p } {}

    void onBufferFull(SignalData signal) {

        std::thread th([](SignalData signal, AudioPlayer<>* player) {
            // Passer vSer en réseau
            std::uniform_int_distribution<int> distrib(0, 5);
            int n = distrib(rdev);
            this_thread::sleep_for(n * 10ms);


            vector<StkFrames> deserFrames = signal.decode();
            player->ReceiveChunk(deserFrames);
        }, signal, player);
        th.detach();
    }
};

int main(int argc, char* argv[])
{
    const int sampleRate = 44100;
    const int channels = 2;
    const double recordingChunkDuration = 0.1;

    MicrophoneRecorder* micro = new MicrophoneRecorder(channels, sampleRate, recordingChunkDuration);
    AudioPlayer<>* player = new AudioPlayer<>(channels);

    ChunkListener* listener = new MicroListener{ player };
    micro->AddListener(listener);

    micro->StartRecording();

    this_thread::sleep_for(25s);

    micro->StopRecording();
    cout << "stopped recording" << endl;
    //player->StopPlaying();
    //cout << "stopped playing" << endl;
    
    this_thread::sleep_for(7s);

    micro->StartRecording();
    cout << "restarted" << endl;

    this_thread::sleep_for(7s);

    micro->StopRecording();
    cout << "stopped recording" << endl;
    player->StopPlaying();
    cout << "stopped playing" << endl;

    this_thread::sleep_for(7s);
    
    micro->~MicrophoneRecorder();
    cout << "destroyed microphone" << endl;
    player->~AudioPlayer<>();
    cout << "destroyed player" << endl;

    return 0;
}
