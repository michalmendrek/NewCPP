#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "itcpclient.h"
#include "SensorData/Stream/IWriteStream.h"
#include "socket.h"

class TcpClient : public ITcpClient,
                  public IWriteStream
{
public:
    TcpClient() = default;

    bool connect(const char *ip, unsigned short port) override;
    int readData(char *buffer, size_t length) override;
    int writeData(const char *buffer, size_t length) override;
    void closeConnection() override;

    // IWriteStream
    int Write(const void* data, size_t count) override;

private:
    Socket mSocket;
};

#endif // TCPCLIENT_H
