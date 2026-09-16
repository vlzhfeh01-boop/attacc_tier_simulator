#include "tier_runtime/kv_block_shape.h"

#include <limits>
#include <stdexcept>

namespace Ramulator {
namespace TierRuntime {

KVBlockShape::KVBlockShape(
    const KVBlockShapeConfig& config)
    : m_config(config) {
  validate_config();
        // 현재 HBM안의 (layer , kv_head )조합 개수
  m_layer_head_count =
      checked_multiply(
          m_config.layers_on_hbm,
          m_config.kv_heads_per_layer_on_hbm);
        // 한 layer의 한 KV head에 해당하는 K or V Slice의 원소 갯수
  m_elements_per_k_or_v_slice =
      checked_multiply(
          m_config.tokens_per_block,
          m_config.head_dimension);
        // K or V slice의 실제 데이터 크기
  m_payload_bytes_per_k_or_v_slice =
      checked_multiply(
          m_elements_per_k_or_v_slice,
          m_config.bytes_per_element);
        // 하나의 request block에 포함된 모든 K Slice의 합
  m_k_payload_bytes_per_block =
      checked_multiply(
          m_payload_bytes_per_k_or_v_slice,
          m_layer_head_count);
        // K와 V를 모두 합친 logical KV block 크기이다.
  m_kv_payload_bytes_per_block =
      checked_multiply(
          m_k_payload_bytes_per_block,
          2);
        // 한 token이 차지하는 전체 K/V paylod를 의미.
  m_kv_payload_bytes_per_token =
      m_kv_payload_bytes_per_block /
      m_config.tokens_per_block;
}
// getter들은 나중에 KV Layout과 PIMCommand Generator가 사용할 예정이다.
std::uint64_t
KVBlockShape::tokens_per_block() const {
  return m_config.tokens_per_block;
}

std::uint64_t
KVBlockShape::layers_on_hbm() const {
  return m_config.layers_on_hbm;
}

std::uint64_t
KVBlockShape::
kv_heads_per_layer_on_hbm() const {
  return m_config.kv_heads_per_layer_on_hbm;
}

std::uint64_t
KVBlockShape::head_dimension() const {
  return m_config.head_dimension;
}

std::uint64_t
KVBlockShape::bytes_per_element() const {
  return m_config.bytes_per_element;
}

std::uint64_t
KVBlockShape::layer_head_count() const {
  return m_layer_head_count;
}

std::uint64_t
KVBlockShape::
elements_per_k_or_v_slice() const {
  return m_elements_per_k_or_v_slice;
}

std::uint64_t
KVBlockShape::
payload_bytes_per_k_or_v_slice() const {
  return m_payload_bytes_per_k_or_v_slice;
}

std::uint64_t
KVBlockShape::
k_payload_bytes_per_block() const {
  return m_k_payload_bytes_per_block;
}

std::uint64_t
KVBlockShape::
v_payload_bytes_per_block() const {
  return m_k_payload_bytes_per_block;
}

std::uint64_t
KVBlockShape::
kv_payload_bytes_per_block() const {
  return m_kv_payload_bytes_per_block;
}

std::uint64_t
KVBlockShape::
kv_payload_bytes_per_token() const {
  return m_kv_payload_bytes_per_token;
}

void KVBlockShape::validate_config() const {
  if (m_config.tokens_per_block == 0) {
    throw std::invalid_argument(
        "tokens_per_block must be greater than zero");
  }

  if (m_config.layers_on_hbm == 0) {
    throw std::invalid_argument(
        "layers_on_hbm must be greater than zero");
  }

  if (m_config.kv_heads_per_layer_on_hbm == 0) {
    throw std::invalid_argument(
        "kv_heads_per_layer_on_hbm must be greater than zero");
  }

  if (m_config.head_dimension == 0) {
    throw std::invalid_argument(
        "head_dimension must be greater than zero");
  }

  if (m_config.bytes_per_element == 0) {
    throw std::invalid_argument(
        "bytes_per_element must be greater than zero");
  }
}

std::uint64_t
KVBlockShape::checked_multiply(
    std::uint64_t lhs,
    std::uint64_t rhs) {
  const std::uint64_t max_value =
      std::numeric_limits<std::uint64_t>::max();

  if (lhs != 0 &&
      rhs > max_value / lhs) { // uint64가 최댓값을 넘으면 자동으로 작은 숫자로 돌아갈 수 있다. 이를 막기 위해 곱셈전에 검사진행한다.
    throw std::overflow_error(
        "KV block shape calculation overflow");
  }

  return lhs * rhs;
}

}  // namespace TierRuntime
}  // namespace Ramulator