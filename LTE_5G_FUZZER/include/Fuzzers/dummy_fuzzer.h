#pragma once

#include "Fuzzers/base_fuzzer.h"

#include "packet.h"

class DummyFuzzer : public Base_Fuzzer {
public:
    virtual ~DummyFuzzer() {}
    bool init() override;
    bool fuzz(Packet& packet) override;
};