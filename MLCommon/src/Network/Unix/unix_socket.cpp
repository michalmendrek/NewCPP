#include <memory>

#include "Network/socket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "plog/Log.h"

class Socket::PrivateImpl {
 public:
  static constexpr int INVALID_SOCKET = -1;
  int descriptor{INVALID_SOCKET};

  PrivateImpl() {}
  PrivateImpl(const PrivateImpl &other) : descriptor(other.descriptor) {}

  PrivateImpl &operator=(const PrivateImpl &other) {
    descriptor = other.descriptor;
    return *this;
  }
};

Socket::Socket()
    : privateImpl(std::unique_ptr<PrivateImpl>(new PrivateImpl())) {}

Socket::Socket(const Socket &socket)
    : privateImpl(
          std::unique_ptr<PrivateImpl>(new PrivateImpl(*socket.privateImpl))) {}

Socket &Socket::operator=(const Socket &other) {
  privateImpl->descriptor = other.privateImpl->descriptor;
  return *this;
}

Socket::~Socket() { close(); }

Socket::operator bool() const {
  return privateImpl &&
         (privateImpl->descriptor != PrivateImpl::INVALID_SOCKET);
}

bool Socket::listen(unsigned short port, int maxConnectionCount) {
  close();
  privateImpl->descriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (privateImpl->descriptor < 0) {
    LOGE << "Couldn't create a socket";
    close();
    return false;
  }

  // setup the sockaddr_in structure for use in bind call
  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = htons(INADDR_ANY);

  if (bind(privateImpl->descriptor, reinterpret_cast<sockaddr *>(&address),
           sizeof(address)) < 0) {
    LOGE << "Couldn't bind a socket";
    close();
    return false;
  }

  if (::listen(privateImpl->descriptor, maxConnectionCount) < 0) {
    LOGE << "Couldn't listen incoming connections";
    close();
    return false;
  }

  return true;
}

Socket Socket::waitForNewConnection() {
  Socket result;
  if (privateImpl->descriptor != PrivateImpl::INVALID_SOCKET) {
    result.privateImpl->descriptor =
        accept(privateImpl->descriptor, nullptr, nullptr);
  }
  return result;
}

bool Socket::connectToHost(const char *ip, unsigned short port) {
  close();

  privateImpl->descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (privateImpl->descriptor == PrivateImpl::INVALID_SOCKET) {
    LOGE << "Couldn't create a socket";
    close();
    return false;
  }

  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr.s_addr = inet_addr(ip);

  return ::connect(privateImpl->descriptor,
                   reinterpret_cast<sockaddr *>(&serverAddress),
                   sizeof(serverAddress)) == 0;
}

int Socket::readData(char *buffer, size_t length) {
  if (privateImpl->descriptor == PrivateImpl::INVALID_SOCKET) {
    LOGW << "Socket is not initialized properly to read data";
    return -1;
  }

  return static_cast<int>(recv(privateImpl->descriptor, buffer, length, 0));
}

int Socket::writeData(const char *buffer, size_t length) {
  if (privateImpl->descriptor == PrivateImpl::INVALID_SOCKET) {
    LOGW << "Socket is not initialized properly to write data";
    return -1;
  }

  auto writtenBytes = 0;
  do {
    ssize_t writtenChunk = send(privateImpl->descriptor, buffer, length, 0);
    if (writtenChunk < 0) {
      LOGW << "An error has occured during the sending data";
      return -1;
    }
    writtenBytes += writtenChunk;
  } while (static_cast<size_t>(writtenBytes) < length);

  return writtenBytes;
}

void Socket::close() {
  if (privateImpl->descriptor != PrivateImpl::INVALID_SOCKET) {
    shutdown(privateImpl->descriptor, SHUT_RDWR);
    privateImpl->descriptor = PrivateImpl::INVALID_SOCKET;
  }
}

int Socket::Read(void *data, size_t count) {
  return readData(static_cast<char *>(data), count);
}
