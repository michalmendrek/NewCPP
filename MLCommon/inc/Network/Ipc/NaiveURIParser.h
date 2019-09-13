#ifndef NAIVEURIPARSER_H_123F2BA
#define NAIVEURIPARSER_H_123F2BA

#include <algorithm>
#include <string>

/**
 * @brief Note is very naive implementation of uri parser, not for production.
 */
class NaiveURIParser {
 public:
  std::string protocol;
  std::string path;
  std::string host;
  std::string port;

  NaiveURIParser(const std::string& uri) {
    std::string tmp = consumeProtocol(uri);
    tmp = consumeHostAndPort(tmp);
    consumePath(tmp);
    valid &= (not path.empty()) or (not host.empty()) or (not port.empty());
  }

  operator bool() { return valid; }

 private:
  bool valid{true};

  std::string consumeProtocol(const std::string& uri) {
    if (uri.empty()) {
      valid = false;
      return {};
    }
    std::string::size_type pos = uri.find(":");
    if (pos == std::string::npos) {
      valid = false;
      return {};
    }

    protocol = uri.substr(0, pos);
    std::transform(protocol.begin(), protocol.end(), protocol.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return (pos < uri.length() - 1) ? uri.substr(pos + 1) : "";
  }

  std::string consumeHostAndPort(const std::string& partialUri) {
    if (partialUri.empty()) {
      return {};
    }

    std::string::size_type pos = partialUri.find("//");
    if (pos != 0) {
      return partialUri;
    }

    std::string::size_type pos2 = partialUri.find("/", pos + 2);
    if (pos2 == std::string::npos) {
      extractHostAndPort(partialUri.substr(2));
      return {};

    } else {
      extractHostAndPort(partialUri.substr(2, pos2));
      return (pos2 < partialUri.length() - 1) ? partialUri.substr(pos2 + 1)
                                              : "";
    }
  }

  void extractHostAndPort(const std::string& partialUri) {
    std::string::size_type pos = partialUri.find(":");
    if (pos == std::string::npos) {
      host = partialUri;

    } else {
      host = partialUri.substr(0, pos);
      port = (pos < partialUri.length() - 1) ? partialUri.substr(pos + 1) : "";
    }
  }

  void consumePath(const std::string& partialUri) {
    if (partialUri.empty() or partialUri.front() != '/') {
      return;
    }
    path = partialUri;
  }
};

#endif  //! NAIVEURIPARSER_H_123F2BA
