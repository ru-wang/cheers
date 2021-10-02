#pragma once

#include <type_traits>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

namespace cheers {

template <typename T, typename U>
struct EnableNotVoid : std::enable_if<!std::is_void_v<T>, U> {};

template <typename T, typename U>
using EnableNotVoidT = typename EnableNotVoid<T, U>::type;

template <typename T>
EnableNotVoidT<decltype(std::declval<T>().x()), glm::quat> MakeQuaternion(const T& q) {
  return glm::quat(q.w(), q.x(), q.y(), q.z());
}

template <typename T>
EnableNotVoidT<decltype(std::declval<T>()[0]), glm::quat> MakeQuaternion(const T& q) {
  return glm::quat(q[3], q[0], q[1], q[2]);
}

template <typename T>
EnableNotVoidT<decltype(std::declval<T>()(0)), glm::quat> MakeQuaternion(const T& q) {
  return glm::quat(q(3), q(0), q(1), q(2));
}

template <typename T>
EnableNotVoidT<decltype(std::declval<T>().x()), glm::vec3> MakeVector3(const T& p) {
  return glm::vec3(p.x(), p.y(), p.z());
}

template <typename T>
EnableNotVoidT<decltype(std::declval<T>()[0]), glm::vec3> MakeVector3(const T& p) {
  return glm::vec3(p[0], p[1], p[2]);
}

template <typename T>
EnableNotVoidT<decltype(std::declval<T>()(0)), glm::vec3> MakeVector3(const T& p) {
  return glm::vec3(p(0), p(1), p(2));
}

template <typename T>
glm::mat3 MakeRotation(const T& q) {
  return glm::toMat3(MakeQuaternion(q));
}

template <typename T>
glm::vec3 MakeTranslation(const T& p) {
  return MakeVector3(p);
}

template <typename T, typename U>
glm::mat4 MakeTransform(const T& q, const U& p) {
  auto tf = glm::toMat4(MakeQuaternion(q));
  tf[3] = glm::vec4(MakeVector3(p), 1.0f);
  return tf;
}

}  // namespace cheers
