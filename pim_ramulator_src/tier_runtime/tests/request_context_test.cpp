#include <cassert>
#include <iostream>

#include "tier_runtime/request_context.h"

using namespace Ramulator::TierRuntime;

int main() {
    RequestContext request;

    request.spec.request_id = 7;

    request.allocated_channels.push_back(2);
    request.allocated_channels.push_back(5);

    assert(request.allocated_channel_count() == 2);
    
    assert(request.owns_channel(2));
    assert(request.owns_channel(5));
    assert(!request.owns_channel(3));

    // Growth 상황도 확인
    request.allocated_channels.push_back(7);

    assert(request.allocated_channel_count() == 3);
    assert(request.allocated_channels[0]==2);
    assert(request.allocated_channels[1]==5);
    assert(request.allocated_channels[2]==7);


    assert(!request.is_evictable());

    request.state = RequestState::Running;
    assert(request.is_evictable());

    request.token_in_progress = false;
    request.inflight_commands = 1;
    assert(!request.is_evictable());

    request.inflight_commands = 0;
    request.state = RequestState::Completed;

    assert(request.is_terminal());
    assert(!request.is_evictable());

    std::cout << "request_context_test_passed\n";
    
    return 0;
}