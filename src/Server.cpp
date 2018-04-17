#include <sys/stat.h>
#include <mutex>
#include <condition_variable>
#include <vector>

#include "swurf/Server.hpp"
#include "swurf/Exception.hpp"
#include <iostream>

swurf::Server::Server()
{
}

swurf::Server::~Server()
{
  if(daemon_)
  {
    MHD_stop_daemon(daemon_);
  }
}

void swurf::Server::turn_debug_on()
{
  debug_ = true;
}

void swurf::Server::serve_files(const std::string files_base_dir)
{
  struct stat info;
  if(stat(files_base_dir.c_str(), &info) == 0)
  {
    files_base_dir_ = files_base_dir;
  }
  else
  {
    throw swurf::Exception("Invalid base directory for serving files", true);
  }
}

void swurf::Server::start(unsigned short port)
{
  unsigned int flags = MHD_NO_FLAG;

  if(debug_)
  {
    flags |= MHD_USE_DEBUG;
  }

  flags |= MHD_USE_THREAD_PER_CONNECTION | MHD_USE_POLL;

  daemon_ = MHD_start_daemon(
    flags,
    port,
    nullptr, nullptr,
    &handle_request, this,
    MHD_OPTION_NOTIFY_COMPLETED, &request_completed, nullptr,
    MHD_OPTION_END
  );

  if(!daemon_)
  {
    running_ = false;
    throw swurf::Exception("Failed to start MHD deamon.");
  }
  else
  {
    running_ = true;
  }
}

bool swurf::Server::running()
{
  return running_;
}

void swurf::Server::serve()
{
  if(running_)
  {
    {
      std::unique_lock<std::mutex> lk(m_);
      running_cv_.wait(lk, [this] { return daemon_ == nullptr; });
    }
  }
  else
  {
    throw swurf::Exception("Blocking calling thread but server is not running.");
  }
}

void swurf::Server::stop()
{
  if(daemon_)
  {
    MHD_stop_daemon(daemon_);
    daemon_ = nullptr;
    running_ = false;
    running_cv_.notify_all();
  }
}

void swurf::Server::add_resource(const std::string& path, const std::string& method, Handler handler)
{
  Path p{path};
  std::pair<Path_map::iterator, bool> handle_search;
  handle_search = resources_.insert(std::make_pair(std::move(p), Method_map()));

  Method_map& m = handle_search.first->second;
  std::pair<Method_map::iterator, bool> method_search;
  method_search = m.insert(std::make_pair(method, handler));
  if(method_search.second == false)
  {
    throw swurf::Exception("Failed to add resource.");
  }
}

void swurf::Server::print_resources()
{
  for(auto& r : resources_)
  {
    std::cout << "path: " << r.first << std::endl;
    for(auto& m : r.second)
    {
      std::cout << "\tmethod: " << m.first << std::endl;
    }
  }
}

int swurf::Server::handle_request(
  void* cls,
  MHD_Connection* con,
  const char* url,
  const char* method, const char* version,
  const char* upload_data, size_t* upload_data_size,
  void** con_cls)
{
  Request* request = static_cast<Request*>(*con_cls);
  Server* self = static_cast<Server*>(cls);

  if(!request)
  {
    request = new Request(con, url, method);
    if(request)
    {
      *con_cls = request;
      std::cout << "created request url:" << url << std::endl;
      return MHD_YES;
    }
  }

  //ToDo process POST/PUT body data and fill it into request.

  Response response{request->con()};

  if(!self->validate_url(request->path()))
  {
    response.status_code(MHD_HTTP_BAD_REQUEST);
    return response.queue();
  }

  if(!self->files_base_dir_.empty())
  {
    if(self->serve_dir(*request, response))
    {
      return response.queue();
    }
  }

  self->serve_resources(*request, response);
  return response.queue();
  return 0;
}

void swurf::Server::request_completed(
  void* cls,
  MHD_Connection* connection,
  void** con_cls, int toe)
{
  Request* request = static_cast<Request*>(*con_cls);
  if(request)
  {
    delete request;
  }
}

//TODO implement a valid url shema check..
bool swurf::Server::validate_url(const std::string& url)
{
  return (url.find("..") == std::string::npos);
}

bool swurf::Server::serve_dir(const Request& request, Response& response)
{
  std::string file;
  if(request.path() == "/")
  {
    file = files_base_dir_ + "/index.html";
  }
  else
  {
    file = files_base_dir_ + request.path();
  }

  struct stat info;
  if(stat(file.c_str(), &info) == 0)
  {
    if(request.method() == MHD_HTTP_METHOD_GET)
    {
      response.file(file);
      response.status_code(MHD_HTTP_OK);
      return true;
    }
    else
    {
      response.status_code(MHD_HTTP_METHOD_NOT_ALLOWED);
      return true;
    }
  }
  else
  {
    return false;
  }
}

bool swurf::Server::serve_resources(Request& request, Response& response)
{
  std::vector<std::string> path_chunks = Path::split(request.path());
  Path_map::iterator pm_it = match_resource(path_chunks);
  if(pm_it != resources_.end())
  {
    Method_map& mmap = pm_it->second;
    Method_map::iterator mm_it = mmap.find(request.method());
    if(mm_it != mmap.end())
    {
      const Path& p =  pm_it->first;
      request.path_params(p.params(path_chunks));
      (mm_it->second)(request, response);
      return true;
    }
    else
    {
      response.status_code(MHD_HTTP_METHOD_NOT_ALLOWED);
      return true;
    }
  }
  else
  {
    return false;
  }
}

swurf::Path_map::iterator swurf::Server::match_resource(const std::vector<std::string>& path_chunks)
{
  Path_map::iterator it = resources_.begin();
  for(it = resources_.begin(); it != resources_.end(); it++)
  {
    if(it->first.match(path_chunks))
    {
      return it;
    }
  }
  return resources_.end();
}
