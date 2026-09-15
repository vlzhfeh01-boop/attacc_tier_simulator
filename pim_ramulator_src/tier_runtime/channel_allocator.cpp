#include "tier_runtime/channel_allocator.h"

#include <stdexcept>

namespace Ramulator {
namespace TierRuntime {

    ChannelAllocator::ChannelAllocator(std::size_t total_channels)
        : m_owner_by_channel(total_channels, kFreeOwner) { //vector(벡터 사이즈, 초기화할 값)
            if(total_channels == 0) {
                throw std::invalid_argument(
                    "ChannelAllocator requires at least one channel.");
            }
        }
    std::size_t ChannelAllocator::total_channel_count() const {
        return m_owner_by_channel.size();
    }
    // admission, growth 가능 여부 판단.
    std::size_t ChannelAllocator::free_channel_count() const {
        std::size_t free_count = 0;

        for(std::int64_t owner : m_owner_by_channel) {
            if(owner == kFreeOwner) {
                ++free_count;
            }
        }
        return free_count;
    }
    // 실제 할당 전 가능 여부를 묻는다.
    bool ChannelAllocator::can_allocate(std::size_t required_channels) const {
        return required_channels > 0 && required_channels <= free_channel_count();
    }
    // 입력값 검증, 충분한 free channel 탐색, 성공시에만 소유권 변경
    std::optional<std::vector<int>> ChannelAllocator::allocate(
        std::int64_t request_id, std::size_t num_required_channels) {
            if(request_id<0) {
                throw std::invalid_argument(
                    "request_id must be non-negative");
            }

            if(num_required_channels ==0) {
                throw std::invalid_argument(
                    "allocation count must be greater than zero");
            }
            if(!can_allocate(num_required_channels)) {
                return std::nullopt;
            }

            std::vector<int> selected_channels;
            selected_channels.reserve(num_required_channels);

            for(std::size_t channel_id=0; channel_id< m_owner_by_channel.size(); channel_id++) {
                if(m_owner_by_channel[channel_id]==kFreeOwner) {
                    selected_channels.push_back(static_cast<int>(channel_id));
                }

                if(selected_channels.size() == num_required_channels) {
                    break;
                }
            }

            for(int channel_id : selected_channels) {
                m_owner_by_channel[channel_id] = request_id;
            }

            return selected_channels;
            
    }
    // 전체 요청 단위 release
    std::vector<int> ChannelAllocator::release(std::int64_t request_id) {
        if(request_id < 0) {
            throw std::invalid_argument("request_id must be non-negative");
        }

        std::vector<int> released_channels;

        for(std::size_t channel_id=0; channel_id<m_owner_by_channel.size(); channel_id++) {
            if(m_owner_by_channel[channel_id] == request_id) {
                m_owner_by_channel[channel_id] = kFreeOwner;

                released_channels.push_back(static_cast<int>(channel_id));
            }
        }
        return released_channels;
    }
    
    //해당 채널이 free인지 아닌지.
    bool ChannelAllocator::is_free(int channel_id) const {
        return !owner_of(channel_id).has_value();
    }
    // 해당 채널의 request id 반환.
    std::optional<std::int64_t> ChannelAllocator::owner_of(int channel_id) const {
        validate_channel_id(channel_id);

        std::int64_t owner = m_owner_by_channel[channel_id];
        if(owner == kFreeOwner){
            return std::nullopt;
        }
        return owner;
    }

    void ChannelAllocator::validate_channel_id(int channel_id) const {
        if(channel_id < 0 || static_cast<std::size_t>(channel_id) >= m_owner_by_channel.size()) {
            throw std::out_of_range("channel_id is outside the allocator range");
        }
    }

}
}