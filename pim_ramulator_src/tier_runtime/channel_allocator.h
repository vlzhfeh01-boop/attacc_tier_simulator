/* 
각 채널을 어느 요청이 소유하는지 관리
요청에 필요한 개수만큼 비어 있는 채널을 할당, 반납
*/
#ifndef RAMULATOR_TIER_RUNTIME_CHANNEL_ALLOCATOR_H
#define RAMULATOR_TIER_RUNTIME_CHANNEL_ALLOCATOR_H

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace Ramulator {
namespace TierRuntime {

    class ChannelAllocator {
        public:
            explicit ChannelAllocator(std::size_t total_channels);

            std::size_t total_channel_count() const;
            std::size_t free_channel_count() const;
            bool can_allocate(std::size_t required_channels) const;

            // 할당된 채널 목록 반환 or std::nullopt 반환
            std::optional<std::vector<int>> allocate(
                std::int64_t request_id,
                std::size_t num_required_channels);
            
            std::vector<int> release(std::int64_t request_id);

            bool is_free(int channel_id) const;
            std::optional<std::int64_t> owner_of(int channel_id) const;

        private:
            static constexpr std::int64_t kFreeOwner = -1; // -1이면 해당 채널의 소유자가 없다 의미.

            std::vector<std::int64_t> m_owner_by_channel; // 소유권 배열

            void validate_channel_id(int channel_id) const;
    };
}
}

#endif