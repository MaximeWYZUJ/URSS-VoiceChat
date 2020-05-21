#include "SignalData.h"
#include "FftRealPair.hpp"

const int compression = 10;

stk::SignalData::SignalData(const std::vector<StkFrames>& frames)
    : nFrames_{ frames.size() * frames[0].frames() }, nChannels_{ frames[0].channels() }, frequencies{}
{
    std::vector<double> data_;
    data_.reserve(nFrames_);
    for (auto f : frames)
        for (int i = 0; i < f.frames() * nChannels_; i += nChannels_)
            data_.push_back(static_cast<double>(f.data()[i]));

    // FFT
    //std::vector<double> imag(data_.size(), 0);
    //Fft::transform(data_, imag);

    // Store frequencies
    std::cout << "avant : " << sizeof(data_[0]) * data_.size() << std::endl;
    frequencies.reserve(data_.size());
    for (int i = 0; i < data_.size(); i += compression) {
        frequencies.push_back(data_[i]);
    }
    std::cout << "apres : " << sizeof(frequencies[0]) * frequencies.size() << std::endl;
}

std::vector<stk::StkFrames> stk::SignalData::decode() const
{
    std::vector<double> data;
    data.reserve(frequencies.size() * compression);
    for (int i = 0; i < frequencies.size() - 1; i++) {
        for (int r = 0; r < compression; r++) {
            data.push_back(frequencies[i] + (r * (frequencies[i + 1] - frequencies[i]) / compression));
        }
    }
    data.push_back(*frequencies.end());

    //Fft::inverseTransform(real, imag);

    std::vector<StkFrames> out;
    out.reserve(data.size());
    for (int i = 0; i < data.size(); i++) {
        out.emplace_back(data[i], 1, nChannels_);
    }

    return out;
}


