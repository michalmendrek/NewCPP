#ifndef ICONFIGMANAGER_H
#define ICONFIGMANAGER_H

#include <string>

/*!
 * \brief The IConfigManager interface specifies the requirements for the configuration
 * manager in scope of SensorApp.
 */

class IConfigManager
{
public:
    virtual ~IConfigManager() = default;
    virtual bool open(const std::string &fileName) = 0;

    virtual bool valueAsBool(const char *name, bool defaultValue) const = 0;
    virtual int valueAsInt(const char *name, int defaultValue) const = 0;
    virtual unsigned int valueAsUInt(const char *name, unsigned int defaultValue) const = 0;
    virtual std::string valueAsString(const char *name, const std::string &defaultValue) const = 0;
};

#endif // ICONFIGMANAGER_H
