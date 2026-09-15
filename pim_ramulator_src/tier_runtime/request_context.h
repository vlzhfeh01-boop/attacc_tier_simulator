/* 
Request Context : 여러 토큰과 수많음 PIM 명령을 포함하는 하나의 LLM 요청
이 요청이 어떤 채널을 사용하는지 기록
*/
#ifndef RAMULATOR_TIER_RUNTIME_REQUEST_CONTEXT_H
#define RAMULATOR_TIER_RUNTIME_REQUEST_CONTEXT_H

#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>

#include "base/type.h"

namespace Ramulator {
namespace TierRuntime{

    enum class RequestState {
        Waiting,
        Running,
        Evicted,
        Completed,
    };

    // Workload 에서 읽어오는 요청의 입력 조건.
    struct RequestSpec {
        std::int64_t request_id = -1;
        Clk_t arrival_cycle = 0; // 요청 도착 시점

        std::uint32_t prompt_tokens = 0; // 이미 존재하는 초기 kv 길이
        std::uint32_t predicted_output_tokens = 0; // Tier 초기 할당 결정
        std::uint32_t actual_output_tokens = 0; // Oracle 정책 및 실제 종료 시점
        std::uint32_t max_output_tokens = 0; // Max Length 정책
    };


    struct RequestContext {
        RequestSpec spec {};
        RequestState state = RequestState::Waiting;

        std::uint32_t generated_tokens = 0;
        std::vector<int> allocated_channels {};

        Clk_t admitted_cycle = -1;
        Clk_t first_token_cycle = -1;
        Clk_t last_token_cycle = -1;
        Clk_t completion_cycle = -1;

        bool token_in_progress = false;
        std::size_t inflight_commands = 0;

        std::size_t allocated_channel_count() const {
            return allocated_channels.size();
        }
        // channel을 해당 request가 소유하고 있는지 아닌지 확인한다.
        bool owns_channel(int channel_id) const {
            return std::find(allocated_channels.begin(),
                            allocated_channels.end(),
                            channel_id) != allocated_channels.end();
        }
        // Eviction 가능 여부 확인
        bool is_evictable() const {
            return state == RequestState::Running && 
                    !token_in_progress && 
                    inflight_commands==0;
        }
        // 종료 조건인지 확인
        bool is_terminal() const {
            return state == RequestState::Evicted ||
                    state == RequestState::Completed;
        }
    };

    
}   // namespace TierRuntime
}   // namespace Ramulator

#endif