#ifndef SWURF_CHUNK_HPP_
#define SWURF_CHUNK_HPP_

#include <string>
#include <regex>
#include <ostream>

namespace swurf
{

class Chunk
{
public:
  enum class type_t{FIXED, PARAM};

  virtual type_t type() const = 0;
  virtual bool match(const std::string& chunk_str) const = 0;
  virtual std::string data() const = 0;
  virtual std::ostream& print(std::ostream& os) const = 0;
};

class FixedChunk : public Chunk
{
public:
  FixedChunk(const std::string& chunk_str);
  type_t type() const override;
  bool match(const std::string& chunk_str) const override;
  std::string data() const override;
  std::ostream& print(std::ostream& os) const override;
private:
  std::string chunk_str_;
};

class ParamChunk : public Chunk
{
public:
  ParamChunk(const std::string& param_name, const std::string& regex="[^/]+");
  type_t type() const override;
  bool match(const std::string& chunk_str) const override;
  std::string data() const override;
  std::ostream& print(std::ostream& os) const override;
private:
  std::string param_name_;
  std::regex regex_;
};

} // namespace swurf

//This needs to be in public namespace.
std::ostream& operator<<(std::ostream& os, const swurf::Chunk& chunk);

#endif // SWURF_CHUNK_HPP_
