#pragma once

#include <stdexcept>
#include <fmt/format.h>

#define THROW(msg, ...) throw std::logic_error(fmt::format(msg, __VA_ARGS__))
#define CHECK(cond, msg, ...) if (!(cond)) THROW(msg, __VA_ARGS__)
