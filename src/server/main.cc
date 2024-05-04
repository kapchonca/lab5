#include "../h/server.h"

int main() {
  try {
    tcp::endpoint endpoint(tcp::v4(), 8888);
    AudioServer server(endpoint);
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
