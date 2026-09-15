#ifndef RAMULATOR_TIER_RUNTIME_KV_CAPACITY_MODEL_H
#define RAMULATOR_TIER_RUNTIME_KV_CAPACITY_MODEL_H

#include <cstddef>
#include <cstdint>

namespace Ramulator {
namespace TierRuntime {

    class KVCapacityModel {
        public:
            // 용량 계산에 필요한 두 가지 조건을 객체 생성시 확정한다.
            KVCapacityModel(
                std::uint64_t kv_bytes_per_token,
                std::uint64_t usable_bytes_per_channel);
            // request별 KV 사용량, 전체 KV 사용량 등에서 통계 데이터로 출력할 수 있다.
            std::uint64_t bytes_for_tokens(std::uint64_t token_count) const;
            
            // 정책은 이 함수만 호출하면 된다. HBM용량 계산을 알 필요가 없음. 
            std::size_t required_channels_for_tokens(std::uint64_t token_count) const;

        private:
            std::uint64_t m_kv_bytes_per_token;
            std::uint64_t m_usable_bytes_per_channel;

            static std::uint64_t ceil_div(
                std::uint64_t numerator, std::uint64_t denominator);
    };
}
}

#endif