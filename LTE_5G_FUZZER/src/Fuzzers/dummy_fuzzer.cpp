#include "Fuzzers/dummy_fuzzer.h"

#include "packet.h"

bool DummyFuzzer::init() {
	return Base_Fuzzer::init();
}

bool DummyFuzzer::fuzz(Packet& packet) {
	return apply_predefined_patches(packet);
}