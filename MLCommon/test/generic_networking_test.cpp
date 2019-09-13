#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <thread>

#include "Network/socket.h"

TEST(NetworkingTests, checkTransfer) {
  std::thread thread([]() {
    Socket socketServer;
    EXPECT_TRUE(socketServer.listen(12345, 1));
    Socket connectedClient = socketServer.waitForNewConnection();
    char msg[12];
    int bytesRead = connectedClient.readData(msg, sizeof(msg));
    EXPECT_GT(bytesRead, 0);
    connectedClient.writeData(msg, static_cast<size_t>(bytesRead));
    connectedClient.close();
    socketServer.close();
  });

  std::string message("something");
  Socket socketClient;
  EXPECT_TRUE(socketClient.connectToHost("127.0.0.1", 12345));
  socketClient.writeData(message.data(), message.size());
  char expectedMessage[12] = {};
  socketClient.readData(expectedMessage, sizeof(expectedMessage));
  socketClient.close();
  EXPECT_EQ(std::string(expectedMessage), message);

  thread.join();
}
