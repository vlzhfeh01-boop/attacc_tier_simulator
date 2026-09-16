#ifndef RAMULATOR_TIER_RUNTIME_KV_PLACEMENT_H
#define RAMULATOR_TIER_RUNTIME_KV_PLACEMENT_H

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace Ramulator {
    namespace TierRuntime{

        struct KVBlockPlacement{
            std::uint64_t request_block_index = 0; // 한 요청의 전체 token sequence를 block으로 나눴을 때 순서.
            std::uint64_t first_token = 0; // block의 첫번째 토큰 번호
            std::uint64_t token_capacity = 0; // block당 저장 토큰 용량

            int channel_id = -1; // 속한 채널 번호
            std::uint64_t channel_block_index = 0; // 특정 채널안에 이 block이 몇 번째로 배치됐는가.

            bool contains(std::uint64_t token_index) const;
        };
        // 각 KV Block이 어느 채널에 있는지 기록
        class KVPlacementTable {
            public : 
                explicit KVPlacementTable(std::uint64_t tokens_per_block);

                std::uint64_t tokens_per_block() const;
                std::size_t block_count() const;
                std::uint64_t mapped_token_capacity() const;

                bool has_mapping_for_token(std::uint64_t token_index) const;

                KVBlockPlacement append_block(int channel_id);

                KVBlockPlacement placement_for_token(std::uint64_t token_index) const;
            
            private : 
                std::uint64_t m_tokens_per_block; // 블록당 저장가능한 토큰 수
                std::uint64_t m_mapped_token_capacity = 0; // 저장된 토큰 수

                // request block 순서대로 저장. 
                std::vector<KVBlockPlacement> m_blocks; // 하나의 요청에 대한 총 블록수
                // 채널마다 다음 local block 번호가 다르다.
                // 할당된 채널에 대한 local block 할당
                std::unordered_map<int,std::uint64_t> m_next_channel_block_by_channel;
        };

    }
}



#endif