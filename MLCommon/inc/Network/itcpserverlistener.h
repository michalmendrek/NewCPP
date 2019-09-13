#ifndef ITCPSERVERLISTENER_H
#define ITCPSERVERLISTENER_H

/*!
 * \brief The ITcpServerListener interface specifies the requirements for the
 * server listener in scope of SensorApp.
 */

class ITcpServerListener
{
public:
    virtual ~ITcpServerListener() = default;
    virtual void onNewConnection(int descriptor) = 0;
    virtual void onDataRead(char *buffer, int length) = 0;
    virtual void onDisconnected(int descriptor) = 0;
};

#endif // ITCPSERVERLISTENER_H
