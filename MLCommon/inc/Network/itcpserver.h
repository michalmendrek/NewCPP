#ifndef ITCPSERVER_H
#define ITCPSERVER_H

#include "itcpserverlistener.h"

/*!
 * \brief The ITcpServer interface specifies the requirements for the TCP
 * server in scope of SensorApp.
 */

class ITcpServer
{
public:
    virtual ~ITcpServer() = default;

    virtual void registerTcpServerListener(ITcpServerListener* listener) = 0;
    virtual bool listen(unsigned short port) = 0;
    virtual void close() = 0;
};

#endif // ITCPSERVER_H
