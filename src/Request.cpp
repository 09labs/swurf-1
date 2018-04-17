#include "swurf/Request.hpp"

swurf::Request::Request(MHD_Connection* con, const char* url, const char* method) :
  con_(con), url_(url), method_(method)
{
  path_ = url_.substr(0,url_.find("?"));
}

swurf::Request::~Request()
{
}

MHD_Connection* swurf::Request::con() const
{
  return con_;
}

std::string swurf::Request::url() const
{
  return url_;
}

std::string swurf::Request::path() const
{
  return path_;
}

std::string swurf::Request::method() const
{
  return method_;
}

void swurf::Request::path_params(const ParamMap& params)
{
  path_params_ = params;
}

std::string swurf::Request::path_param(const std::string& param) const
{
  ParamMap::const_iterator pos = path_params_.find(param);
  if(pos != path_params_.end())
  {
    return pos->second;
  }
  else
  {
    return "";
  }
}
