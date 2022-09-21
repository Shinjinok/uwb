// -----------------------------------------------------------------------------
// This file was autogenerated by symforce from template:
//     backends/cpp/templates/function/FUNCTION.h.jinja
// Do NOT modify by hand.
// -----------------------------------------------------------------------------

#pragma once

#include <matrix/math.hpp>

namespace sym {

/**
 * This function was autogenerated from a symbolic function. Do not modify by hand.
 *
 * Symbolic function: fuse_beta
 *
 * Args:
 *     v_local: Matrix31
 *     state: Matrix31
 *     P: Matrix33
 *     q_att: Matrix41
 *     R: Scalar
 *     epsilon: Scalar
 *
 * Outputs:
 *     H: Matrix13
 *     K: Matrix31
 *     innov_var: Scalar
 *     innov: Scalar
 */
template <typename Scalar>
void FuseBeta(const matrix::Matrix<Scalar, 3, 1>& v_local, const matrix::Matrix<Scalar, 3, 1>& state,
              const matrix::Matrix<Scalar, 3, 3>& P, const matrix::Matrix<Scalar, 4, 1>& q_att,
              const Scalar R, const Scalar epsilon, matrix::Matrix<Scalar, 1, 3>* const H = nullptr,
              matrix::Matrix<Scalar, 3, 1>* const K = nullptr, Scalar* const innov_var = nullptr,
              Scalar* const innov = nullptr) {
  // Total ops: 75

  // Input arrays

  // Intermediate terms (23)
  const Scalar _tmp0 = 2 * v_local(2, 0);
  const Scalar _tmp1 = std::pow(q_att(1, 0), Scalar(2));
  const Scalar _tmp2 = std::pow(q_att(2, 0), Scalar(2));
  const Scalar _tmp3 = std::pow(q_att(0, 0), Scalar(2)) - std::pow(q_att(3, 0), Scalar(2));
  const Scalar _tmp4 = _tmp1 - _tmp2 + _tmp3;
  const Scalar _tmp5 = -state(0, 0) + v_local(0, 0);
  const Scalar _tmp6 = -state(1, 0) + v_local(1, 0);
  const Scalar _tmp7 = q_att(0, 0) * q_att(3, 0);
  const Scalar _tmp8 = q_att(1, 0) * q_att(2, 0);
  const Scalar _tmp9 = 2 * _tmp7 + 2 * _tmp8;
  const Scalar _tmp10 = _tmp0 * (-q_att(0, 0) * q_att(2, 0) + q_att(1, 0) * q_att(3, 0)) +
                        _tmp4 * _tmp5 + _tmp6 * _tmp9;
  const Scalar _tmp11 =
      _tmp10 + epsilon * (2 * math::min<Scalar>(0, (((_tmp10) > 0) - ((_tmp10) < 0))) + 1);
  const Scalar _tmp12 = Scalar(1.0) / (_tmp11);
  const Scalar _tmp13 = -2 * _tmp7 + 2 * _tmp8;
  const Scalar _tmp14 = -_tmp1 + _tmp2 + _tmp3;
  const Scalar _tmp15 = _tmp0 * (q_att(0, 0) * q_att(1, 0) + q_att(2, 0) * q_att(3, 0)) +
                        _tmp13 * _tmp5 + _tmp14 * _tmp6;
  const Scalar _tmp16 = _tmp15 / std::pow(_tmp11, Scalar(2));
  const Scalar _tmp17 = -_tmp12 * _tmp13 + _tmp16 * _tmp4;
  const Scalar _tmp18 = -_tmp12 * _tmp14 + _tmp16 * _tmp9;
  const Scalar _tmp19 = P(0, 0) * _tmp17;
  const Scalar _tmp20 = P(1, 1) * _tmp18;
  const Scalar _tmp21 =
      R + _tmp17 * (P(1, 0) * _tmp18 + _tmp19) + _tmp18 * (P(0, 1) * _tmp17 + _tmp20);
  const Scalar _tmp22 = Scalar(1.0) / (math::max<Scalar>(_tmp21, epsilon));

  // Output terms (4)
  if (H != nullptr) {
    matrix::Matrix<Scalar, 1, 3>& _H = (*H);

    _H(0, 0) = _tmp17;
    _H(0, 1) = _tmp18;
    _H(0, 2) = 0;
  }

  if (K != nullptr) {
    matrix::Matrix<Scalar, 3, 1>& _K = (*K);

    _K(0, 0) = _tmp22 * (P(0, 1) * _tmp18 + _tmp19);
    _K(1, 0) = _tmp22 * (P(1, 0) * _tmp17 + _tmp20);
    _K(2, 0) = _tmp22 * (P(2, 0) * _tmp17 + P(2, 1) * _tmp18);
  }

  if (innov_var != nullptr) {
    Scalar& _innov_var = (*innov_var);

    _innov_var = _tmp21;
  }

  if (innov != nullptr) {
    Scalar& _innov = (*innov);

    _innov = -_tmp12 * _tmp15;
  }
}  // NOLINT(readability/fn_size)

// NOLINTNEXTLINE(readability/fn_size)
}  // namespace sym
