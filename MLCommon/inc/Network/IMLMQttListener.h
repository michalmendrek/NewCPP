#ifndef IMLMQTTLISTENER_H
#define IMLMQTTLISTENER_H

#include <string>

namespace ml {

class IMLMQttListener
{
public:

    IMLMQttListener() = default;
    virtual ~IMLMQttListener() = default;

    virtual void dispatchMessage(const ::std::string& topic, const void* message, const int len) = 0;

private:


    IMLMQttListener(const IMLMQttListener&) = delete;

    IMLMQttListener& operator =(const IMLMQttListener&) = delete;
};

} // namespace - ml

#endif // ! IMLMQTTLISTENER_H
