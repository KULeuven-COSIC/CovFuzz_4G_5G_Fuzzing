#pragma once

#include "Fuzzers/base_fuzzer.h"

#include "packet.h"
#include "patch.h"

#include <string>
#include <memory>
#include <vector>

class RandomFuzzer : public Base_Fuzzer {
public:
    virtual ~RandomFuzzer() {}
    bool init() override;
    bool fuzz(Packet& packet) override;
};