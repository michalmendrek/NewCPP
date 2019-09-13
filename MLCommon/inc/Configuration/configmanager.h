#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "iconfigmanager.h"

#include "tinyxml2/tinyxml2.h"

class ConfigManager : public IConfigManager
{
public:
    ConfigManager() = default;

    bool open(const std::string &fileName) override;
    bool valueAsBool(const char *name, bool defaultValue) const override;
    int valueAsInt(const char *name, int defaultValue) const override;
    unsigned int valueAsUInt(const char *name, unsigned int defaultValue) const override;
    std::string valueAsString(const char *name, const std::string &defaultValue) const override;

private:
    tinyxml2::XMLDocument mConfigXml;
};

#endif // CONFIGMANAGER_H
