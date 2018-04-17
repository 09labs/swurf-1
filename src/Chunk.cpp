#include "swurf/Chunk.hpp"

swurf::FixedChunk::FixedChunk(const std::string& chunk_str) : chunk_str_(chunk_str)
{
}

swurf::Chunk::type_t swurf::FixedChunk::type() const
{
  return type_t::FIXED;
}

bool swurf::FixedChunk::match(const std::string& chunk_str) const
{
  return (chunk_str_ == chunk_str);
}

std::string swurf::FixedChunk::data() const
{
  return chunk_str_;
}

std::ostream& swurf::FixedChunk::print(std::ostream& os) const
{
  os << "/" << chunk_str_;
  return os;
}

swurf::ParamChunk::ParamChunk(const std::string& param_name, const std::string& regex) :
  param_name_(param_name), regex_(regex)
{
}

swurf::Chunk::type_t swurf::ParamChunk::type() const
{
  return type_t::PARAM;
}

bool swurf::ParamChunk::match(const std::string& chunk_str) const
{
  return std::regex_search(chunk_str, regex_);
}

std::string swurf::ParamChunk::data() const
{
  return param_name_;
}

std::ostream& swurf::ParamChunk::print(std::ostream& os) const
{
  os << "/{" << param_name_ << "}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const swurf::Chunk& chunk)
{
  return chunk.print(os);
}
