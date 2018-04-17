#include <sstream>
#include "swurf/Path.hpp"

#include <iostream>

//TODO very intensive checking for the path and chunks!!
static bool check_path(const std::string& path)
{
  // std::regex p{"^(\/{?[a-zA-z0-9]+:?[^}\/\s]*}?)+$"};
  // return true;
  // return std::regex_match(chunk_str, regex_);
  if(!path.empty())
  {
    return true;
  }
  else
  {
    return false;
  }
}

static bool chunk_is_param(const std::string& chunk)
{
  return (
    (chunk.size() > 2) &&
    (chunk[0] == '{') &&
    (chunk[chunk.size()-1] == '}')
  );
}

static bool chunk_is_param_regex(const std::string& chunk, size_t colon_pos)
{
  return (chunk.size() > 4);
}

static bool chunk_is_fixed(const std::string& chunk)
{
  return (chunk.size() > 1);
}

swurf::Path::Path(const std::string& path)
{
  if(!check_path(path))
  {
    throw swurf::Exception("Invalid path.");
  }
  const std::vector<std::string> chunks = split(path);
  for(const std::string& chunk : chunks)
  {
    if(chunk_is_param(chunk))
    {
      size_t colon_pos = chunk.find(':');
      if(colon_pos == std::string::npos)
      {
        std::string param_name = chunk.substr(1,chunk.size()-2);
        param_chunk_indexes_.push_back(chunks_.size());
        chunks_.push_back(std::make_unique<ParamChunk>(param_name));
      }
      else if(chunk_is_param_regex(chunk, colon_pos))
      {
        std::string param_name = chunk.substr(1,colon_pos-1);
        std::string regex = chunk.substr(colon_pos+1, chunk.size()-colon_pos-2);
        param_chunk_indexes_.push_back(chunks_.size());
        chunks_.push_back(std::make_unique<ParamChunk>(param_name, regex));
      }
      else{
        throw swurf::Exception("Invalid path.");
      }
    }
    else if(chunk_is_fixed(chunk))
    {
      chunks_.push_back(std::make_unique<FixedChunk>(chunk));
    }
    else
    {
      throw swurf::Exception("Invalid path.");
    }
  }
}

bool swurf::Path::operator<(const Path& other) const
{
  if(chunks_.size() < other.chunks_.size())
  {
    return true;
  }
  else if(chunks_.size() > other.chunks_.size())
  {
    return false;
  }

  for(size_t i=0;i<chunks_.size();i++)
  {
    if(chunks_[i]->type() == Chunk::type_t::FIXED)
    {
      if(other.chunks_[i]->type() == Chunk::type_t::PARAM)
      {
        return true;
      }
      else if(chunks_[i]->data() < other.chunks_[i]->data())
      {
        return true;
      }
    }
  }
  return false;
}

size_t swurf::Path::num_chunks() const
{
  return chunks_.size();
}

const swurf::Chunk& swurf::Path::operator[](size_t i) const
{
  if(i < chunks_.size())
  {
    return *chunks_[i];
  }
  else
  {
    throw swurf::Exception("Out of bounds error in path.");
  }
}

bool swurf::Path::match(const std::string& path) const
{
  return match(split(path));
}

bool swurf::Path::match(const std::vector<std::string>& path_chunks) const
{
  if(chunks_.size() == path_chunks.size())
  {
    for(size_t i=0;i<chunks_.size();i++)
    {
      if(!chunks_[i]->match(path_chunks[i]))
      {
        return false;
      }
    }
    return true;
  }
  else
  {
    return false;
  }
}

swurf::ParamMap swurf::Path::params(const std::string& path) const
{
  return params(split(path));
}

swurf::ParamMap swurf::Path::params(const std::vector<std::string>& path_chunks) const
{
  ParamMap params;
  if(chunks_.size() == path_chunks.size())
  {
    for(size_t index : param_chunk_indexes_)
    {
      params.insert(std::make_pair(chunks_[index]->data(), path_chunks[index]));
    }
  }
  return params;
}

std::vector<std::string> swurf::Path::split(const std::string& path)
{
  std::vector<std::string> chunks;
  std::string chunk;
  std::istringstream chunkStream{path};
  while(std::getline(chunkStream, chunk, '/'))
  {
    if(!chunk.empty())
    {
      chunks.push_back(chunk);
    }
  }
  return chunks;
}

std::ostream& operator<<(std::ostream& os, const swurf::Path& path)
{
  for(size_t i=0;i<path.num_chunks();i++)
  {
    os << path[i];
  }
  return os;
}
