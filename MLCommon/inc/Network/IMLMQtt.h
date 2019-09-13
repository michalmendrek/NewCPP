#ifndef IMLMQTT_H
#define IMLMQTT_H

#include <string>
 
namespace ml {

class IMLMQtt
{

public:

    IMLMQtt() = default;
    virtual ~IMLMQtt() = default;

    virtual void Init() = 0;

    virtual void SendMessage(const std::string& topic, const std::string& message) = 0;

    virtual void Start() = 0;

    virtual void Stop() = 0;

    virtual void Shutdown() = 0;

private:

    IMLMQtt(const IMLMQtt&) = delete;

    IMLMQtt& operator =(const IMLMQtt&) = delete;
};

} // namespace - ml

#endif // ! IMLMQTT_H
