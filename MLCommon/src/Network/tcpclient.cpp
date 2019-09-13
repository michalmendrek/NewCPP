#include "tcpclient.h"

/*!
 * \brief The class is TCP client implementation based on linux raw sockets.
 * \class TcpClient
 */

bool TcpClient::connect(const char *ip, unsigned short port)
{
    return mSocket.connectToHost(ip, port);
}

int TcpClient::readData(char *buffer, size_t length)
{
    return mSocket.readData(buffer, length);
}

int TcpClient::writeData(const char *buffer, size_t length)
{
    return mSocket.writeData(buffer, length);
}

void TcpClient::closeConnection()
{
    mSocket.close();
}

int TcpClient::Write(const void *data, size_t count)
{
    return writeData(static_cast<const char*>(data), count);
}
