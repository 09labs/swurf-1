#include <string>
#include <cstdio>
#include <fcntl.h>
#include <sys/stat.h>
#include "swurf/Response.hpp"

#include <iostream>

swurf::Response::Response(MHD_Connection* con, unsigned int status_code) :
  con_(con), status_code_(status_code)
{
}

swurf::Response::~Response()
{
  if(resp_)
  {
    MHD_destroy_response(resp_);
  }
}

MHD_Connection* swurf::Response::connection() const
{
  return con_;
}

unsigned int swurf::Response::status_code() const
{
  return status_code_;
}

void swurf::Response::status_code(unsigned int status_code)
{
  status_code_ = status_code;
}

std::string swurf::Response::content_type() const
{
  return content_type_;
}

void swurf::Response::content_type(const std::string& content_type)
{
  content_type_ = content_type;
}

std::string swurf::Response::body() const
{
  return body_;
}

void swurf::Response::body(const std::string& body)
{
  body_ = body;
}

std::string swurf::Response::file() const
{
  return file_;
}

void swurf::Response::file(const std::string& file)
{
  file_ = file;
}


// TODO ADD callback option for handling internal errors..
bool swurf::Response::queue()
{
  create_MHD_response_ptr();

  if(resp_)
  {
    //TODO render response function
    //add headers, cookies and footers...
    render();
    return MHD_queue_response(con_, status_code_, resp_);
  }
  return false;
}

void swurf::Response::create_MHD_response_ptr()
{
  if(!file_.empty())
  {
    int fd = -1;
    fd = open(file_.c_str(), O_RDONLY);
    if(fd < 0)
    {
      resp_ = MHD_create_response_from_buffer(0, (void*) "", MHD_RESPMEM_MUST_COPY);
      status_code_ = MHD_HTTP_INTERNAL_SERVER_ERROR;
    }
    else
    {
      struct stat buf;
      fstat(fd, &buf);
      resp_ = MHD_create_response_from_fd(buf.st_size, fd);
      status_code_ = MHD_HTTP_OK;
    }
  }
  else
  {
    std::cout << "body: " << body_ << std::endl;
    resp_ = MHD_create_response_from_buffer(body_.size(), (void*) body_.data(), MHD_RESPMEM_MUST_COPY);
  }
}

void swurf::Response::render()
{
  if(content_type_.empty())
  {
    guess_content_type();
  }

  MHD_add_response_header(resp_, "Content-type", content_type_.c_str());
}


void swurf::Response::guess_content_type()
{
  if(status_code_ != MHD_HTTP_OK)
  {
    content_type_ = "text/plain";
  }
  else if(file_.empty())
  {
    if(body_.empty())
    {
      content_type_ = "text/plain";
    }
    else if((body_[0] == '{') && (body_[body_.size()-1] == '}'))
    {
      content_type_ = "application/json";
    }
    else if((body_[0] == '<') && (body_[body_.size()-1] == '>'))
    {
      content_type_ = "application/xml";
    }
    else
    {
      content_type_ = "text/plain";
    }
  }
  else
  {
    size_t ext_pos = file_.find_last_of(".");
    if(ext_pos == std::string::npos)
    {
      content_type_ = "text/plain";
    }
    else
    {
      std::string ext = file_.substr(ext_pos+1);
      if((ext == "html") || (ext == "htm"))
      {
        content_type_ = "text/html";
      }
      else if(ext == "css")
      {
        content_type_ = "text/css";
      }
      else if(ext == "js")
      {
        content_type_ = "text/javascript";
      }
      else
      {
        content_type_ = "text/plain";
      }
    }
  }
}
