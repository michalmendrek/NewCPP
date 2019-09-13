#include <winsock2.h>
#include <ws2tcpip.h>

#include <memory>

#include "Network/socket.h"
#include "network_init.h"
#include "plog/Log.h"

class Socket::PrivateImpl {
 public:
  SOCKET descriptor{INVALID_SOCKET};
  std::shared_ptr<ml::NetworkSetup> net;

  PrivateImpl() : net(ml::NetworkSetup::initNetwork()) {}
  PrivateImpl(const PrivateImpl &other)
      : descriptor(other.descriptor), net(ml::NetworkSetup::initNetwork()) {}

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
  return privateImpl && (privateImpl->descriptor != INVALID_SOCKET);
}

bool Socket::listen(unsigned short port, int maxConnectionCount) {
  close();

  privateImpl->descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (privateImpl->descriptor == INVALID_SOCKET) {
    LOGE << "Couldn't create a socket, error:" << WSAGetLastError();
    return false;
  }

  // setup the sockaddr_in structure for use in bind call
  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = htons(INADDR_ANY);

  if (bind(privateImpl->descriptor, reinterpret_cast<sockaddr *>(&address),
           sizeof(address)) == SOCKET_ERROR) {
    LOGE << "Couldn't bind a socket";
    return false;
  }

  if (::listen(privateImpl->descriptor, maxConnectionCount) == SOCKET_ERROR) {
    LOGE << "Couldn't listen incoming connections";
    return false;
  }

  return true;
}

Socket Socket::waitForNewConnection() {
  Socket result;
  if (privateImpl->descriptor != INVALID_SOCKET) {
    result.privateImpl->descriptor =
        accept(privateImpl->descriptor, nullptr, nullptr);
  }
  return result;
}

bool Socket::connectToHost(const char *ip, unsigned short port) {
  close();

  privateImpl->descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (privateImpl->descriptor == INVALID_SOCKET) {
    LOGE << "Couldn't create a socket";
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
  if (privateImpl->descriptor == INVALID_SOCKET) {
    LOGW << "Socket is not initialized properly to read data";
    return -1;
  }

  return recv(privateImpl->descriptor, buffer, static_cast<int>(length), 0);
}

int Socket::writeData(const char *buffer, size_t length) {
  if (privateImpl->descriptor == INVALID_SOCKET) {
    LOGW << "Socket is not initialized properly to write data";
    return -1;
  }

  auto writtenBytes = 0;
  do {
    auto writtenChunk =
        send(privateImpl->descriptor, buffer, static_cast<int>(length), 0);
    if (writtenChunk == SOCKET_ERROR) {
      LOGW << "An error has occured during the sending data";
      return -1;
    }
    writtenBytes += writtenChunk;
  } while (static_cast<size_t>(writtenBytes) < length);

  return writtenBytes;
}

void Socket::close() {
  if (privateImpl->descriptor != INVALID_SOCKET) {
    closesocket(privateImpl->descriptor);
    privateImpl->descriptor = INVALID_SOCKET;
  }
}

int Socket::Read(void *data, size_t count) {
  return privateImpl->descriptor != INVALID_SOCKET
             ? readData(static_cast<char *>(data), count)
             : -1;
}
