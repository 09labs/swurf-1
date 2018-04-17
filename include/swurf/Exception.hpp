#ifndef SWURF_EXCEPTION_HPP_
#define SWURF_EXCEPTION_HPP_

#include <exception>
#include <string>

namespace swurf
{

class Exception : public std::exception
{
public:

  explicit Exception(const std::string& message, bool inclSysMsg=false);

  const char* what() const noexcept;

  int error() const noexcept;

  const std::string& msg() const noexcept;

private:
  int error_;
  std::string msg_;
};

} // namespace swurf
#endif // SWURF_EXCEPTION_HPP_
