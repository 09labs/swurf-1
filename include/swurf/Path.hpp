#ifndef SWURF_PATH_HPP_
#define SWURF_PATH_HPP_

#include <string>
#include <memory>
#include <vector>
#include <ostream>

#include "swurf/Exception.hpp"
#include "swurf/Chunk.hpp"
#include "swurf/ParamMap.hpp"

namespace swurf
{

class Path
{
public:
  Path(const std::string& path);

  size_t num_chunks() const;
  const Chunk& operator[](size_t i) const;
  bool operator<(const Path& other) const;

  bool match(const std::string& path) const;
  bool match(const std::vector<std::string>& path_chunks) const;

  ParamMap params(const std::string& path) const;
  ParamMap params(const std::vector<std::string>& path_chunks) const;

  static std::vector<std::string> split(const std::string& path);
private:
  using Chunk_ptr = std::unique_ptr<Chunk>;
  std::vector<Chunk_ptr> chunks_;
  std::vector<size_t> param_chunk_indexes_;
};

} // namespace swurf

//This needs to be in public namespace.
std::ostream& operator<<(std::ostream& os, const swurf::Path& path);
#endif // SWURF_PATH_HPP_
