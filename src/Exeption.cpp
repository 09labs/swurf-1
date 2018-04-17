#include <cerrno>
#include <cstdint>
#include <cstring>
#include <string>

#include "swurf/Exception.hpp"

swurf::Exception::Exception(const std::string& message, bool inclSysMsg) : msg_(message)
{
  error_ = errno;
  if (inclSysMsg)
  {
    msg_.append(": ");
    msg_.append(strerror(error_));
  }
}

const char* swurf::Exception::what() const noexcept
{
  return msg_.c_str();
}

int32_t swurf::Exception::error() const noexcept
{
  return error_;
}

const std::string& swurf::Exception::msg() const noexcept
{
  return msg_;
}
