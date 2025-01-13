#ifndef RAK_UDP_TRACKER_INFO_H
#define RAK_UDP_TRACKER_INFO_H

#include <string>
#include <vector>

namespace rak {

class udp_tracker_info {
public:
  bool equals (const std::string u) { return url.compare(u) == 0; }

  void set (const std::string u, const char* h, const int p) {
    url = u;
    hostname.assign(h);
    port = p;
    broken = false;
  }
  void set_broken() { broken = true; }
  bool get_broken() { return broken; }

  std::string get_url() { return url; }
  std::string get_hostname() { return hostname; }
  int get_port() { return port; }

private:
  bool broken;
  std::string url;
  std::string hostname;
  int port;
};

class udp_tracker_vector : public std::vector<udp_tracker_info> {
public:
  udp_tracker_info get_info(const std::string url) {
    for (size_t i=0; i<size(); i++) {
      if (at(i).equals(url)) {
        return at(i);
      }
    }
    return create_info(url);
  }

private:
  typedef std::vector<udp_tracker_info>       base_type;
  typedef typename base_type::reference       reference;
  using base_type::size;
  using base_type::at;

  udp_tracker_info create_info(const std::string url) {
    char hostname[1024] = {0};
    int port;
    udp_tracker_info new_info;

    if (sscanf(url.c_str(), "udp://%1023[^:]:%i/announce", hostname, &port) == 2 && hostname[0] != '\0' && port > 0 && port < (1 << 16)) {
      new_info.set(url, hostname, port);
      base_type::push_back(new_info);
      return new_info;
    }

    new_info.set_broken();
    return new_info;
  }
};

}

#endif
