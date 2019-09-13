#include "configmanager.h"

#include <plog/Log.h>

/*!
 * \brief The ConfigManager class represents the simple configuration manager based on
 * tinyxml2 library.
 * \class ConfigManager
 */

bool ConfigManager::open(const std::string &fileName)
{
    LOGD << "filename = " << fileName;

    try
    {
        if (mConfigXml.LoadFile(fileName.data()) == tinyxml2::XML_SUCCESS)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    catch (const ::std::exception& e)
    {
        LOGD << "Exception: " << e.what();
        return false;
    }
}

bool ConfigManager::valueAsBool(const char *name, bool defaultValue) const
{
    const auto xmlElement = mConfigXml.RootElement()->FirstChildElement(name);
    return xmlElement ? xmlElement->BoolText(defaultValue) : defaultValue;
}

int ConfigManager::valueAsInt(const char *name, int defaultValue) const
{
    const auto xmlElement = mConfigXml.RootElement()->FirstChildElement(name);
    return xmlElement ? xmlElement->IntText(defaultValue) : defaultValue;
}

unsigned int ConfigManager::valueAsUInt(const char *name, unsigned int defaultValue) const
{
    const auto xmlElement = mConfigXml.RootElement()->FirstChildElement(name);
    return xmlElement ? xmlElement->UnsignedText(defaultValue) : defaultValue;
}

std::string ConfigManager::valueAsString(const char *name, const std::string &defaultValue) const
{
    const auto xmlElement = mConfigXml.RootElement()->FirstChildElement(name);
    if (xmlElement == nullptr)
    {
        return defaultValue;
    }

    const auto text = xmlElement->GetText();
    return text ? std::string(text) : defaultValue;
}
