#ifndef SWURF_RESPONSE_HPP_
#define SWURF_RESPONSE_HPP_

#include <string>
#include "MHD.hpp"

namespace swurf
{

class Response
{
public:
  Response(
    MHD_Connection* con,
    unsigned int status_code=MHD_HTTP_NOT_FOUND
  );

  ~Response();

  MHD_Connection* connection() const;

  unsigned int status_code() const;
  void status_code(unsigned int status_code);

  std::string content_type() const;
  void content_type(const std::string& content_type);

  std::string body() const;
  void body(const std::string& body);

  std::string file() const;
  void file(const std::string& file);

  bool queue();
private:
  MHD_Connection* con_ = nullptr;
  MHD_Response* resp_ = nullptr;
  unsigned int status_code_;
  std::string content_type_;
  std::string body_;
  std::string file_;

  void create_MHD_response_ptr();
  void render();
  void guess_content_type();
};

} // namespace swurf
#endif // SWURF_RESPONSE_HPP_
