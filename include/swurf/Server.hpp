#ifndef SWURF_SERVER_HPP_
#define SWURF_SERVER_HPP_

#include <string>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>
#include <map>
#include <unordered_map>

#include "MHD.hpp"
#include "swurf/Exception.hpp"
#include "swurf/Request.hpp"
#include "swurf/Response.hpp"

namespace swurf
{

using Handler = std::function<void(const Request&, Response&)>;
using Method_map = std::unordered_map<std::string, Handler>;
using Path_map = std::map<Path, Method_map>;

class Server
{
public:
  Server();
  ~Server();

  void turn_debug_on();
  void serve_files(const std::string files_base_dir);

  void start(unsigned short port);
  bool running();
  void serve();
  void stop();

  void add_resource(const std::string& path, const std::string& method, Handler handler);
  void print_resources();
private:
  MHD_Daemon* daemon_ = nullptr;
  bool running_ = false;
  std::mutex m_;
  std::condition_variable running_cv_;
  bool debug_ = false;
  std::string files_base_dir_;
  Path_map resources_;

  /*
   * TODO
   * Turn other options into a vector of MHD_OptionItem.
   * Use it in MHD_start_daemon with 'MHD_OPTION_ARRAY, opts_.MHD_opts().data()'.
   */
  //std::vector<MHD_OptionItem> MHD_opts();

  static int handle_request(
    void* cls, MHD_Connection* con,
    const char* url, const char* method, const char* version,
    const char* upload_data, size_t* upload_data_size,
    void** con_cls);

  static void request_completed(
    void* cls, MHD_Connection* connection,
    void** con_cls, int toe);

  bool validate_url(const std::string& url);
  bool serve_dir(const Request& request, Response& response);
  bool serve_resources(Request& request, Response& response);
  Path_map::iterator match_resource(const std::vector<std::string>& path_chunks);
};

} // namespace swurf
#endif // SWURF_SERVER_HPP_
