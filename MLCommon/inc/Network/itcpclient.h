#ifndef ITCPCLIENT_H
#define ITCPCLIENT_H

#include <cstddef>

/*!
 * \brief The ITcpClient interface specifies the requirements for the TCP
 * client in scope of SensorApp.
 */

class ITcpClient
{
public:
    virtual ~ITcpClient() = default;
    virtual bool connect(const char *ip, unsigned short port) = 0;
    virtual int readData(char *buffer, size_t length) = 0;
    virtual int writeData(const char *buffer, size_t length) = 0;
    virtual void closeConnection() = 0;
};

#endif // ITCPCLIENT_H
