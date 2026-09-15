#include "tier_runtime/kv_capacity_model.h"

#include <limits>
#include <stdexcept>

namespace Ramulator{
namespace TierRuntime{

    KVCapacityModel::KVCapacityModel(
        std::uint64_t kv_bytes_per_token,
        std::uint64_t usable_bytes_per_channel)
        : m_kv_bytes_per_token(kv_bytes_per_token),
        m_usable_bytes_per_channel(usable_bytes_per_channel) {
            if(kv_bytes_per_token == 0) {
                throw std::invalid_argument("kv_bytes_per_token must be greated than zero.");
            }

            if(usable_bytes_per_channel ==0) {
                throw std::invalid_argument("usable_bytes_per_channel must be greater than zero.");
            }
        }
    
        std::uint64_t KVCapacityModel::bytes_for_tokens(
            std::uint64_t token_count) const {
                const std::uint64_t max_value = std::numeric_limits<std::uint64_t>::max();
                // uint64 type의 범위를 넘어가지 않는 한도라는 의미를 담는다.
                if(token_count> (max_value/m_kv_bytes_per_token)) {
                    throw std::overflow_error(
                        "KV capacity calculation overflow"
                    );
                }
                return token_count * m_kv_bytes_per_token;
            }

        std::size_t KVCapacityModel::required_channels_for_tokens(
            std::uint64_t token_count) const {
                const std::uint64_t required_bytes = bytes_for_tokens(token_count);

                const std::uint64_t required_channels = ceil_div(required_bytes, m_usable_bytes_per_channel);

                if(required_channels > std::numeric_limits<std::size_t>::max()) {
                    throw std::overflow_error(
                        "required channel count overflow"
                    );
                }
                return static_cast<std::size_t> (required_channels);
            }

        std::uint64_t KVCapacityModel::ceil_div(std::uint64_t numerator, std::uint64_t denominator) {
            return numerator / denominator + (numerator % denominator !=0); // 나머지가 있으면 참인 1로 바뀌어 더해진다.
        }
        
}
}