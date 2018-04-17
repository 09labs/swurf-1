#ifndef SWURF_REQUEST_HPP_
#define SWURF_REQUEST_HPP_

#include <string>

#include "MHD.hpp"
#include "swurf/Path.hpp"
#include "swurf/ParamMap.hpp"

namespace swurf
{

class Request
{
public:
  Request(MHD_Connection* con, const char* url, const char* method);
  ~Request();

  MHD_Connection* con() const;

  std::string url() const;
  std::string path() const;

  std::string method() const;

  void path_params(const ParamMap& params);
  std::string path_param(const std::string& param) const;

private:
  MHD_Connection* con_ = nullptr;
  std::string url_;
  std::string path_;
  std::string method_;
  std::string body_;
  ParamMap path_params_;

  //TODO query parameters and post/put data...
  //ParamMap query_params_;
  //MHD_PostProcessor* postprocessor_ = nullptr;
};

} // namespace swurf
#endif // SWURF_REQUEST_HPP_
