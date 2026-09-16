#include "tier_runtime/kv_placement.h"

#include <limits>
#include <stdexcept>

namespace Ramulator{
    namespace TierRuntime{

        bool KVBlockPlacement::contains(std::uint64_t token_index) const {
            return first_token<= token_index && token_index < first_token + token_capacity;
        }

        KVPlacementTable::KVPlacementTable(std::uint64_t tokens_per_block)
        : m_tokens_per_block(tokens_per_block) {
            if(tokens_per_block == 0) {
                throw std::invalid_argument(
                    "tokens_per_block must be greater than zero"
                );
            }
        }

        std::uint64_t KVPlacementTable::tokens_per_block() const {
            return m_tokens_per_block;
        }

        std::size_t KVPlacementTable::block_count() const {
            return m_blocks.size();
        }

        std::uint64_t KVPlacementTable::mapped_token_capacity() const {
            return m_mapped_token_capacity;
        }

        bool KVPlacementTable::has_mapping_for_token(std::uint64_t token_index) const {
            return token_index < m_mapped_token_capacity;
        }

        KVBlockPlacement KVPlacementTable::append_block(int channel_id) {
            if(channel_id < 0) {
                throw std::invalid_argument(
                    "channel_id must be non-negative"
                );
            }

            const std::uint64_t max_value = std::numeric_limits<std::uint64_t>::max();
            if(m_mapped_token_capacity > max_value - m_tokens_per_block) {
                throw std::overflow_error(
                    "mapped token capacity overflow"
                );
            }
            // map 의 채널 객체를 할당받는다.
            std::uint64_t &next_local_block = m_next_channel_block_by_channel[channel_id];

            KVBlockPlacement placement;
            placement.request_block_index = static_cast<std::uint64_t>(m_blocks.size());

            placement.first_token = m_mapped_token_capacity;

            placement.token_capacity = m_tokens_per_block;

            placement.channel_id = channel_id;

            placement.channel_block_index = next_local_block;

            ++next_local_block; // 레퍼런스 참조이기 때문에, map 내부 값도 바뀐다.

            m_blocks.push_back(placement); // 하나의 블록을 또 추가한다.

            m_mapped_token_capacity += m_tokens_per_block;

            return placement;
        }

        KVBlockPlacement KVPlacementTable::placement_for_token(std::uint64_t token_index) const {
            if(!has_mapping_for_token(token_index)){
                throw std::out_of_range(
                    "token does not have a KV placement"
                );
            }

            const std::uint64_t block_index = token_index / m_tokens_per_block;

            return m_blocks[static_cast<std::size_t>(block_index)];
        }
    }
}