#include "ipc_test.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#ifdef __linux__
#include <sys/socket.h>
#include <sys/un.h>
#include "Network/Unix/IpcSocketUnix.h"
#elif defined(__WIN32__) || defined(__WIN64__)
#include "Network/Windows/IpcSocketWindows.h"
#include "Network/socket.h"
#endif

IpcTest::IpcTest(const std::vector<IpcTest::ShrSocket> &sockets)
    : sockets{sockets},
      receiver{sockets, *this, 20},
      con{sockets},
      transmiter{sockets} {}

void IpcTest::start() {
  receiver.Start();
  transmiter.Start();
  con.Start();
}

void IpcTest::stop() {
  receiver.StopWithJoin();
  transmiter.StopWithJoin();
  con.StopWithJoin();
}

void IpcTest::dispatchData(const ml::DataContainer &data,
                           const std::string &socketName,
                           const int descriptor) {
  (void)socketName;
  (void)descriptor;

  for (ShrSocket socket : sockets) {
#ifdef __linux__
    ml::IpcSocketUnix &s = dynamic_cast<ml::IpcSocketUnix &>(*socket);
#elif defined(__WIN32__) || defined(__WIN64__)
    ml::IpcSocketWindows &s = dynamic_cast<ml::IpcSocketWindows &>(*socket);
#endif
    if (s.getCopyOfClientsDescriptor().size() > 0) {
      transmiter.sendData(socket->getSocketUri(),
                          std::make_shared<ml::DataContainer>(data));
    }
  }
}

TEST(NetworkingTests, checkIPCTransfer) {
#ifdef __linux__
  std::string path = "/tmp/test";
  std::string uri = "unix:" + path;
#elif defined(__WIN32__) || defined(__WIN64__)
  std::string uri = "tcp://localhost:12347";
#endif
  std::vector<IpcTest::ShrSocket> sockets{ml::IIpcSocket::create(uri, {})};
  IpcTest test(sockets);
  test.start();

  std::string message("Test!!!");
  char expectedMessage[8];
#ifdef __linux__
  int sock = socket(AF_UNIX, SOCK_STREAM, 0);
  EXPECT_GE(sock, 0);
  struct sockaddr_un address;
  memset(&address, 0, sizeof(address));
  address.sun_family = AF_UNIX;
  size_t addressLength = std::min(path.length(), sizeof(address.sun_path) - 1);
  strncpy(address.sun_path, path.data(), addressLength);
  EXPECT_EQ(connect(sock, reinterpret_cast<struct sockaddr *>(&address),
                    sizeof(struct sockaddr_un)),
            0);
  EXPECT_GT(write(sock, message.data(), message.size()), 0);
  EXPECT_GT(read(sock, expectedMessage, sizeof(expectedMessage)), 0);
  close(sock);
#elif defined(__WIN32__) || defined(__WIN64__)
  Socket socket;
  EXPECT_TRUE(socket.connectToHost("127.0.0.1", 12347));
  socket.writeData(message.data(), message.size());
  socket.readData(expectedMessage, sizeof(expectedMessage));
  socket.close();
#endif

  EXPECT_EQ(std::string(expectedMessage), message);
  test.stop();
}
