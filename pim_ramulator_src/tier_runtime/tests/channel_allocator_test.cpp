#include <cassert>
#include <iostream>

#include "tier_runtime/channel_allocator.h"
#include "tier_runtime/request_context.h"

using Ramulator::TierRuntime::ChannelAllocator;
using Ramulator::TierRuntime::RequestContext;
using Ramulator::TierRuntime::RequestState;

int main() {
    ChannelAllocator allocator(6);

    assert(allocator.total_channel_count() == 6);
    assert(allocator.free_channel_count() ==6 );

    for(int index = 0; index<6 ; ++index) {
        auto allocation = allocator.allocate(100+index, 1);

        assert(allocation.has_value());
        assert(allocation->size() == 1);
        assert(allocation->front() == index);
    }

    assert(allocator.free_channel_count() == 0);

    allocator.release(102);
    allocator.release(105);

    assert(allocator.is_free(2));
    assert(allocator.is_free(5));

    auto allocation = allocator.allocate(7,2);
    assert(allocation.has_value());
    assert(allocation->size() == 2);
    assert((*allocation)[0] == 2); // 딱 2랑 5만 비어있을테니깐.
    assert((*allocation)[1] == 5);

    RequestContext request;

    request.spec.request_id = 7;
    request.state = RequestState::Running;
    request.allocated_channels = allocation.value(); // 여기서 연결

    assert(request.allocated_channel_count() == 2);
    assert(request.owns_channel(2));
    assert(request.owns_channel(5));
    assert(!request.owns_channel(3));

    allocator.release(103);

    auto growth = allocator.allocate(7, 1);

    assert(growth.has_value());
    assert(growth->size() == 1);
    assert(growth->front() == 3);

    request.allocated_channels.push_back(
        growth->front());

    assert(request.allocated_channel_count() == 3);
    assert(request.allocated_channels[0] == 2);
    assert(request.allocated_channels[1] == 5);
    assert(request.allocated_channels[2] == 3);


    allocator.release(104);

    std::size_t free_before_failure =
        allocator.free_channel_count();

    auto failed = allocator.allocate(20, 2);

    assert(!failed.has_value());
    assert(
        allocator.free_channel_count() ==
        free_before_failure);

    auto released = allocator.release(7);

    assert(released.size() == 3);
    assert(allocator.is_free(2));
    assert(allocator.is_free(3));
    assert(allocator.is_free(5));

    request.allocated_channels.clear();
    request.state = RequestState::Evicted;

    assert(request.allocated_channel_count() == 0);
    assert(request.is_terminal());

    std::cout << "channel_allocator_test passed\n";

    return 0;
}