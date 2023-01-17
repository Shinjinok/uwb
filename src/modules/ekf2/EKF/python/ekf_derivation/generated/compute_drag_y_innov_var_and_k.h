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
 * Symbolic function: compute_drag_y_innov_var_and_k
 *
 * Args:
 *     state: Matrix24_1
 *     P: Matrix24_24
 *     rho: Scalar
 *     cd: Scalar
 *     cm: Scalar
 *     R: Scalar
 *     epsilon: Scalar
 *
 * Outputs:
 *     innov_var: Scalar
 *     K: Matrix24_1
 */
template <typename Scalar>
void ComputeDragYInnovVarAndK(const matrix::Matrix<Scalar, 24, 1>& state,
                              const matrix::Matrix<Scalar, 24, 24>& P, const Scalar rho,
                              const Scalar cd, const Scalar cm, const Scalar R,
                              const Scalar epsilon, Scalar* const innov_var = nullptr,
                              matrix::Matrix<Scalar, 24, 1>* const K = nullptr) {
  // Total ops: 407

  // Input arrays

  // Intermediate terms (76)
  const Scalar _tmp0 = -state(23, 0) + state(5, 0);
  const Scalar _tmp1 = 2 * _tmp0;
  const Scalar _tmp2 = _tmp1 * state(1, 0);
  const Scalar _tmp3 = -state(22, 0) + state(4, 0);
  const Scalar _tmp4 = 2 * _tmp3;
  const Scalar _tmp5 = _tmp4 * state(2, 0);
  const Scalar _tmp6 = 2 * state(6, 0);
  const Scalar _tmp7 = _tmp6 * state(0, 0);
  const Scalar _tmp8 = -_tmp2 + _tmp5 + _tmp7;
  const Scalar _tmp9 = std::pow(state(3, 0), Scalar(2));
  const Scalar _tmp10 = -_tmp9;
  const Scalar _tmp11 = std::pow(state(0, 0), Scalar(2));
  const Scalar _tmp12 = std::pow(state(2, 0), Scalar(2));
  const Scalar _tmp13 = -_tmp12;
  const Scalar _tmp14 = std::pow(state(1, 0), Scalar(2));
  const Scalar _tmp15 = _tmp10 + _tmp11 + _tmp13 + _tmp14;
  const Scalar _tmp16 = state(0, 0) * state(3, 0);
  const Scalar _tmp17 = state(1, 0) * state(2, 0);
  const Scalar _tmp18 = _tmp16 + _tmp17;
  const Scalar _tmp19 = state(0, 0) * state(2, 0);
  const Scalar _tmp20 = state(1, 0) * state(3, 0);
  const Scalar _tmp21 = _tmp1 * _tmp18 + _tmp15 * _tmp3 + _tmp6 * (-_tmp19 + _tmp20);
  const Scalar _tmp22 = state(2, 0) * state(3, 0);
  const Scalar _tmp23 = state(0, 0) * state(1, 0);
  const Scalar _tmp24 = _tmp22 - _tmp23;
  const Scalar _tmp25 = _tmp19 + _tmp20;
  const Scalar _tmp26 = _tmp11 - _tmp14;
  const Scalar _tmp27 = _tmp13 + _tmp26 + _tmp9;
  const Scalar _tmp28 = _tmp1 * _tmp24 + _tmp25 * _tmp4 + _tmp27 * state(6, 0);
  const Scalar _tmp29 = _tmp10 + _tmp12 + _tmp26;
  const Scalar _tmp30 = -_tmp16 + _tmp17;
  const Scalar _tmp31 = _tmp0 * _tmp29 + _tmp30 * _tmp4 + _tmp6 * (_tmp22 + _tmp23);
  const Scalar _tmp32 = std::sqrt(Scalar(std::pow(_tmp21, Scalar(2)) + std::pow(_tmp28, Scalar(2)) +
                                         std::pow(_tmp31, Scalar(2)) + epsilon));
  const Scalar _tmp33 = cd * rho;
  const Scalar _tmp34 = _tmp32 * _tmp33;
  const Scalar _tmp35 = Scalar(0.5) * _tmp34;
  const Scalar _tmp36 = _tmp4 * state(3, 0);
  const Scalar _tmp37 = _tmp1 * state(0, 0);
  const Scalar _tmp38 = _tmp6 * state(1, 0);
  const Scalar _tmp39 = 2 * _tmp28;
  const Scalar _tmp40 = 2 * _tmp31;
  const Scalar _tmp41 = _tmp1 * state(2, 0) + _tmp4 * state(1, 0) + _tmp6 * state(3, 0);
  const Scalar _tmp42 = 2 * _tmp21;
  const Scalar _tmp43 = Scalar(0.25) * _tmp31 * _tmp33 / _tmp32;
  const Scalar _tmp44 =
      -_tmp35 * _tmp8 -
      _tmp43 * (_tmp39 * (_tmp36 - _tmp37 - _tmp38) + _tmp40 * _tmp8 + _tmp41 * _tmp42) -
      _tmp8 * cm;
  const Scalar _tmp45 = -_tmp36 + _tmp37 + _tmp38;
  const Scalar _tmp46 = _tmp4 * state(0, 0);
  const Scalar _tmp47 = _tmp1 * state(3, 0);
  const Scalar _tmp48 = _tmp6 * state(2, 0);
  const Scalar _tmp49 = _tmp46 + _tmp47 - _tmp48;
  const Scalar _tmp50 = -_tmp35 * _tmp45 -
                        _tmp43 * (_tmp39 * _tmp8 + _tmp40 * _tmp45 + _tmp42 * _tmp49) - _tmp45 * cm;
  const Scalar _tmp51 =
      -_tmp35 * _tmp41 - _tmp41 * cm -
      _tmp43 * (_tmp39 * _tmp49 + _tmp40 * _tmp41 + _tmp42 * (_tmp2 - _tmp5 - _tmp7));
  const Scalar _tmp52 = -_tmp46 - _tmp47 + _tmp48;
  const Scalar _tmp53 = -_tmp35 * _tmp52 -
                        _tmp43 * (_tmp39 * _tmp41 + _tmp40 * _tmp52 + _tmp42 * _tmp45) -
                        _tmp52 * cm;
  const Scalar _tmp54 = _tmp29 * cm;
  const Scalar _tmp55 = _tmp29 * _tmp35;
  const Scalar _tmp56 = 4 * _tmp28;
  const Scalar _tmp57 = _tmp29 * _tmp40;
  const Scalar _tmp58 =
      -_tmp43 * (-4 * _tmp18 * _tmp21 - _tmp24 * _tmp56 - _tmp57) + _tmp54 + _tmp55;
  const Scalar _tmp59 = 2 * _tmp16;
  const Scalar _tmp60 = 2 * _tmp17;
  const Scalar _tmp61 = -_tmp59 + _tmp60;
  const Scalar _tmp62 = _tmp15 * _tmp42;
  const Scalar _tmp63 = 2 * _tmp19;
  const Scalar _tmp64 = 2 * _tmp20;
  const Scalar _tmp65 = -_tmp35 * _tmp61 -
                        _tmp43 * (_tmp39 * (_tmp63 + _tmp64) + _tmp40 * _tmp61 + _tmp62) -
                        _tmp61 * cm;
  const Scalar _tmp66 = Scalar(1.0) * _tmp30 * _tmp34 + 2 * _tmp30 * cm -
                        _tmp43 * (-_tmp25 * _tmp56 - 4 * _tmp30 * _tmp31 - _tmp62);
  const Scalar _tmp67 = 2 * _tmp22;
  const Scalar _tmp68 = 2 * _tmp23;
  const Scalar _tmp69 = _tmp67 + _tmp68;
  const Scalar _tmp70 = -_tmp35 * _tmp69 -
                        _tmp43 * (_tmp27 * _tmp39 + _tmp40 * _tmp69 + _tmp42 * (-_tmp63 + _tmp64)) -
                        _tmp69 * cm;
  const Scalar _tmp71 =
      -_tmp43 * (_tmp39 * (_tmp67 - _tmp68) + _tmp42 * (_tmp59 + _tmp60) + _tmp57) - _tmp54 -
      _tmp55;
  const Scalar _tmp72 = P(22, 22) * _tmp66;
  const Scalar _tmp73 = P(23, 23) * _tmp58;
  const Scalar _tmp74 = R +
                        _tmp44 * (P(0, 1) * _tmp50 + P(1, 1) * _tmp44 + P(2, 1) * _tmp51 +
                                  P(22, 1) * _tmp66 + P(23, 1) * _tmp58 + P(3, 1) * _tmp53 +
                                  P(4, 1) * _tmp65 + P(5, 1) * _tmp71 + P(6, 1) * _tmp70) +
                        _tmp50 * (P(0, 0) * _tmp50 + P(1, 0) * _tmp44 + P(2, 0) * _tmp51 +
                                  P(22, 0) * _tmp66 + P(23, 0) * _tmp58 + P(3, 0) * _tmp53 +
                                  P(4, 0) * _tmp65 + P(5, 0) * _tmp71 + P(6, 0) * _tmp70) +
                        _tmp51 * (P(0, 2) * _tmp50 + P(1, 2) * _tmp44 + P(2, 2) * _tmp51 +
                                  P(22, 2) * _tmp66 + P(23, 2) * _tmp58 + P(3, 2) * _tmp53 +
                                  P(4, 2) * _tmp65 + P(5, 2) * _tmp71 + P(6, 2) * _tmp70) +
                        _tmp53 * (P(0, 3) * _tmp50 + P(1, 3) * _tmp44 + P(2, 3) * _tmp51 +
                                  P(22, 3) * _tmp66 + P(23, 3) * _tmp58 + P(3, 3) * _tmp53 +
                                  P(4, 3) * _tmp65 + P(5, 3) * _tmp71 + P(6, 3) * _tmp70) +
                        _tmp58 * (P(0, 23) * _tmp50 + P(1, 23) * _tmp44 + P(2, 23) * _tmp51 +
                                  P(22, 23) * _tmp66 + P(3, 23) * _tmp53 + P(4, 23) * _tmp65 +
                                  P(5, 23) * _tmp71 + P(6, 23) * _tmp70 + _tmp73) +
                        _tmp65 * (P(0, 4) * _tmp50 + P(1, 4) * _tmp44 + P(2, 4) * _tmp51 +
                                  P(22, 4) * _tmp66 + P(23, 4) * _tmp58 + P(3, 4) * _tmp53 +
                                  P(4, 4) * _tmp65 + P(5, 4) * _tmp71 + P(6, 4) * _tmp70) +
                        _tmp66 * (P(0, 22) * _tmp50 + P(1, 22) * _tmp44 + P(2, 22) * _tmp51 +
                                  P(23, 22) * _tmp58 + P(3, 22) * _tmp53 + P(4, 22) * _tmp65 +
                                  P(5, 22) * _tmp71 + P(6, 22) * _tmp70 + _tmp72) +
                        _tmp70 * (P(0, 6) * _tmp50 + P(1, 6) * _tmp44 + P(2, 6) * _tmp51 +
                                  P(22, 6) * _tmp66 + P(23, 6) * _tmp58 + P(3, 6) * _tmp53 +
                                  P(4, 6) * _tmp65 + P(5, 6) * _tmp71 + P(6, 6) * _tmp70) +
                        _tmp71 * (P(0, 5) * _tmp50 + P(1, 5) * _tmp44 + P(2, 5) * _tmp51 +
                                  P(22, 5) * _tmp66 + P(23, 5) * _tmp58 + P(3, 5) * _tmp53 +
                                  P(4, 5) * _tmp65 + P(5, 5) * _tmp71 + P(6, 5) * _tmp70);
  const Scalar _tmp75 = Scalar(1.0) / (math::max<Scalar>(_tmp74, epsilon));

  // Output terms (2)
  if (innov_var != nullptr) {
    Scalar& _innov_var = (*innov_var);

    _innov_var = _tmp74;
  }

  if (K != nullptr) {
    matrix::Matrix<Scalar, 24, 1>& _K = (*K);

    _K.setZero();

    _K(22, 0) = _tmp75 * (P(22, 0) * _tmp50 + P(22, 1) * _tmp44 + P(22, 2) * _tmp51 +
                          P(22, 23) * _tmp58 + P(22, 3) * _tmp53 + P(22, 4) * _tmp65 +
                          P(22, 5) * _tmp71 + P(22, 6) * _tmp70 + _tmp72);
    _K(23, 0) = _tmp75 * (P(23, 0) * _tmp50 + P(23, 1) * _tmp44 + P(23, 2) * _tmp51 +
                          P(23, 22) * _tmp66 + P(23, 3) * _tmp53 + P(23, 4) * _tmp65 +
                          P(23, 5) * _tmp71 + P(23, 6) * _tmp70 + _tmp73);
  }
}  // NOLINT(readability/fn_size)

// NOLINTNEXTLINE(readability/fn_size)
}  // namespace sym
