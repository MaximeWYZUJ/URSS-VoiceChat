#pragma once
#include "Stk.h"
#include <vector>


namespace stk
{

class SignalData
{
    std::vector<double> frequencies;

public:
    unsigned int nFrames_;
    unsigned int nChannels_;

    SignalData(const std::vector<StkFrames>& frames);

    std::vector<StkFrames> decode() const;
};

}