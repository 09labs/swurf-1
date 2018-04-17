#include <string>
#include "swurf/Server.hpp"

#include <iostream>

void hello_world(const swurf::Request& request, swurf::Response& response)
{
  response.body("<html><header><title>This is title</title></header><body>Hello world</body></html>");
  response.content_type("text/html");
  response.status_code(MHD_HTTP_OK);
}

int main()
{
  swurf::Server server{};
  server.turn_debug_on();
  server.add_resource("/hello/world", "GET", &hello_world);
  server.start(5555);
  server.serve();
  return 0;
}
