#include<cassert>
#include<cstdint>
#include<iostream>
#include<limits>
#include<stdexcept>

#include "tier_runtime/kv_capacity_model.h"

using Ramulator::TierRuntime::KVCapacityModel;

int main() {
    KVCapacityModel model(256,1024); // token의 byte랑 채널당 사용가능 byte를 넘긴다.

    assert(model.bytes_for_tokens(0) == 0);
    assert(model.bytes_for_tokens(4) == 1024);
    assert(model.bytes_for_tokens(5) == 1280);

    assert(model.required_channels_for_tokens(0)==0);
    assert(model.required_channels_for_tokens(1) == 1);
    assert(model.required_channels_for_tokens(4) == 1);
    assert(model.required_channels_for_tokens(5) == 2);
    assert(model.required_channels_for_tokens(8) == 2);

    bool invalid_config_detected = false;

    try {
        KVCapacityModel invalid_model(0, 1024);
    } catch (const std::invalid_argument&) {
        invalid_config_detected = true;
    }

    assert(invalid_config_detected);
    KVCapacityModel overflow_model(
        std::numeric_limits<std::uint64_t>::max(),
        1024);

    bool overflow_detected = false;

    try {
        overflow_model.bytes_for_tokens(2);
    } catch (const std::overflow_error&) {
        overflow_detected = true;
    }

    assert(overflow_detected);

    std::cout << "kv_capacity_model_test passed\n";

    return 0;
}