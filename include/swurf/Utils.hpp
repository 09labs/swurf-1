#ifndef SWURF_UTILS_HPP_
#define SWURF_UTILS_HPP_

#include <string>
#include <unordered_map>
#include <random>

namespace swurf
{
  std::string random_string(std::string::size_type length)
  {
    static auto& chrs = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    thread_local static std::mt19937 rg{std::random_device{}()};
    thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

    std::string s;
    s.reserve(length);
    while(length--)
    {
      s += chrs[pick(rg)];
    }

    return s;
  }

} // namespace swurf
#endif // SWURF_UTILS_HPP_
