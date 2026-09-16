#ifndef RAMULATOR_TIER_RUNTIME_KV_BLOCK_SHAPE_H
#define RAMULATOR_TIER_RUNTIME_KV_BLOCK_SHAPE_H

#include <cstdint>

namespace Ramulator {
namespace TierRuntime {

struct KVBlockShapeConfig {
  std::uint64_t tokens_per_block = 0;
  std::uint64_t layers_on_hbm = 0;
  std::uint64_t kv_heads_per_layer_on_hbm = 0;
  std::uint64_t head_dimension = 0;
  std::uint64_t bytes_per_element = 0;
};
// K/V 데이터와 논리적 크기인 PayLoad까지 계산한다.
class KVBlockShape {
 public:
  explicit KVBlockShape(
      const KVBlockShapeConfig& config);

  std::uint64_t tokens_per_block() const;

  std::uint64_t layers_on_hbm() const;

  std::uint64_t
  kv_heads_per_layer_on_hbm() const;

  std::uint64_t head_dimension() const;

  std::uint64_t bytes_per_element() const;

  std::uint64_t layer_head_count() const;

  std::uint64_t
  elements_per_k_or_v_slice() const;

  std::uint64_t
  payload_bytes_per_k_or_v_slice() const;

  std::uint64_t
  k_payload_bytes_per_block() const;

  std::uint64_t
  v_payload_bytes_per_block() const;

  std::uint64_t
  kv_payload_bytes_per_block() const;

  std::uint64_t
  kv_payload_bytes_per_token() const;

 private:
  KVBlockShapeConfig m_config;

  std::uint64_t m_layer_head_count = 0;

  std::uint64_t
      m_elements_per_k_or_v_slice = 0;

  std::uint64_t
      m_payload_bytes_per_k_or_v_slice = 0;

  std::uint64_t
      m_k_payload_bytes_per_block = 0;

  std::uint64_t
      m_kv_payload_bytes_per_block = 0;

  std::uint64_t
      m_kv_payload_bytes_per_token = 0;

  void validate_config() const;

  static std::uint64_t checked_multiply(
      std::uint64_t lhs,
      std::uint64_t rhs);
};

}  // namespace TierRuntime
}  // namespace Ramulator

#endif