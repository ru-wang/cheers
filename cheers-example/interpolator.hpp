#pragma once

#include <cmath>

#include <Eigen/Core>
#include <Eigen/Geometry>

template <typename Scalar> using SE3 = Eigen::Matrix<Scalar, 7, 1>;
template <typename Scalar> using se3 = Eigen::Matrix<Scalar, 6, 1>;

template <typename Scalar>
SE3<Scalar> SE3Identity() {
  return SE3<Scalar>::Unit(6);
}

template <typename Scalar>
SE3<Scalar> SE3Random(Scalar translation_scale) {
  SE3<Scalar> tf;
  tf.template head<3>() = translation_scale * Eigen::Matrix<Scalar, 3, 1>::Random();
  tf.template tail<4>() = Eigen::Quaternion<Scalar>::UnitRandom().coeffs();
  return tf;
}

template <typename Scalar>
SE3<Scalar> SE3Concat(const SE3<Scalar>& lhs, const SE3<Scalar>& rhs) {
  using Quaternion = Eigen::Quaternion<Scalar>;
  SE3<Scalar> tf;
  tf.template head<3>() =
      Quaternion(lhs.template tail<4>()) * rhs.template head<3>() + lhs.template head<3>();
  tf.template tail<4>() =
      (Quaternion(lhs.template tail<4>()) * Quaternion(rhs.template tail<4>())).coeffs();
  tf.template tail<4>().normalize();
  return tf;
}

template <typename T, typename Scalar = typename T::Scalar>
auto Skew(const Eigen::MatrixBase<T>& phi) {
  Eigen::Matrix<Scalar, 3, 3> phi_hat;
  phi_hat.diagonal().setZero();
  phi_hat(1, 2) = -(phi_hat(2, 1) = phi[0]);
  phi_hat(2, 0) = -(phi_hat(0, 2) = phi[1]);
  phi_hat(0, 1) = -(phi_hat(1, 0) = phi[2]);
  return phi_hat;
}

template <typename T, typename Scalar = typename T::Scalar>
auto Expmap(const Eigen::MatrixBase<T>& phi) {
  Eigen::AngleAxis<Scalar> aa(phi.norm(), phi.stableNormalized());
  return Eigen::Quaternion<Scalar>(aa);
}

template <typename T, typename Scalar = typename T::Scalar>
auto Logmap(const Eigen::QuaternionBase<T>& q) {
  Eigen::AngleAxis<Scalar> aa(q);
  return Eigen::Matrix<Scalar, 3, 1>(aa.angle() * aa.axis());
}

template <typename T, typename Scalar = typename T::Scalar>
auto JacobianSO3(const Eigen::MatrixBase<T>& theta, Scalar epsilon) {
  using Matrix3 = Eigen::Matrix<Scalar, 3, 3>;
  using Vector3 = Eigen::Matrix<Scalar, 3, 1>;

  Vector3 a = theta.stableNormalized();
  Scalar n = theta.norm();
  Scalar n2 = n * n;
  Scalar n3 = n * n2;

  Scalar sin = std::sin(n);
  Scalar cos = std::cos(n);

  if (n3 <= epsilon)
    return Matrix3(Matrix3::Identity() + sin * Skew(a));

  Scalar sinc = sin / n;
  return Matrix3(sinc * Matrix3::Identity() +
                 (1.0 - sinc) * a * a.transpose() +
                 (1.0 - cos) / n * Skew(a));
}

template <typename T, typename Scalar = typename T::Scalar>
auto JacobianSO3Inverse(const Eigen::MatrixBase<T>& theta, Scalar epsilon) {
  using Matrix3 = Eigen::Matrix<Scalar, 3, 3>;
  using vector3 = Eigen::Matrix<Scalar, 3, 1>;

  vector3 a = theta.stableNormalized();
  Scalar half_n = 0.5 * theta.norm();

  Scalar sin = std::sin(half_n);
  Scalar cos = std::cos(half_n);

  if (sin <= epsilon)
    return Matrix3(Matrix3::Identity() + half_n * Skew(a));

  Scalar half_n_cot = half_n * cos / sin;
  return Matrix3((1.0 - half_n_cot) * a * a.transpose() +
                 half_n_cot * Matrix3::Identity() -
                 half_n * Skew(a));
}

template <typename Scalar>
class Interpolator {
public:
  template <typename T>
  Interpolator(const Eigen::MatrixBase<T>& tf_1, const Eigen::MatrixBase<T>& tf_2)
      : m_tf_1(tf_1), m_tf_2(tf_2), m_se3_21(ComputeTangent()) {}

  SE3<Scalar> operator()(double alpha) const {
    alpha = Truncate(alpha);
    if (alpha == 0.0) return m_tf_1;
    if (alpha == 1.0) return m_tf_2;
    return Interpolate(alpha);
  }

private:
  static constexpr Scalar Truncate(Scalar alpha) {
    return alpha < 0.0 ? 0.0 : (alpha > 1.0 ? 1.0 : alpha);
  }

  se3<Scalar> ComputeTangent() const {
    using namespace Eigen;

    SE3<Scalar> tf_21;
    Map<Matrix<Scalar, 3, 1>> p_21(tf_21.data());
    Map<Quaternion<Scalar>> q_21(tf_21.data() + 3);
    q_21 = Map<const Quaternion<Scalar>>(m_tf_2.data() + 3) *
           Map<const Quaternion<Scalar>>(m_tf_1.data() + 3).conjugate();
    p_21.noalias() = m_tf_2.template head<3>() - q_21 * m_tf_1.template head<3>();

    se3<Scalar> se3;
    auto rho = se3.template head<3>();
    auto phi = se3.template tail<3>();
    phi.noalias() = Logmap(q_21);
    rho.noalias() = JacobianSO3Inverse(phi, DEFAULT_EPSILON) * p_21;
    return se3;
  }

  SE3<Scalar> Interpolate(Scalar alpha) const {
    using namespace Eigen;

    se3<Scalar> se3 = alpha * m_se3_21;
    auto rho = se3.template head<3>();
    auto phi = se3.template tail<3>();

    SE3<Scalar> tf;
    Map<Matrix<Scalar, 3, 1>> p(tf.data() + 0);
    Map<Quaternion<Scalar>> q(tf.data() + 3);

    q = Expmap(phi);
    p = q * m_tf_1.template head<3>() + JacobianSO3(phi, DEFAULT_EPSILON) * rho;
    q *= Quaternion<Scalar>(m_tf_1.template tail<4>());
    return tf;
  }

  static constexpr Scalar DEFAULT_EPSILON = 1.0e-10;

  const SE3<Scalar> m_tf_1;
  const SE3<Scalar> m_tf_2;
  const se3<Scalar> m_se3_21;
};

template <typename T>
Interpolator(const Eigen::MatrixBase<T>&, const Eigen::MatrixBase<T>&)
    -> Interpolator<typename T::Scalar>;
