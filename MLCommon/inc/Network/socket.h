#ifndef SOCKET_H
#define SOCKET_H

#include <SensorData/Stream/IReadStream.h>

#include <memory>

class Socket : public IReadStream {
 public:
  Socket();
  Socket(const Socket &socket);
  Socket &operator=(const Socket &socket);
  virtual ~Socket() override;

  bool listen(unsigned short port, int maxConnectionCount);
  Socket waitForNewConnection();

  bool connectToHost(const char *ip, unsigned short port);
  int readData(char *buffer, size_t length);
  int writeData(const char *buffer, size_t length);
  void close();

  // IReadStream
  int Read(void *data, size_t count) override;

  operator bool() const;

 private:
  class PrivateImpl;
  std::unique_ptr<PrivateImpl> privateImpl;
};

#endif  // SOCKET_H
