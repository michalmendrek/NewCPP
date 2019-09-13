#ifndef CONFIGURATION_H_9786BCF
#define CONFIGURATION_H_9786BCF

#include <picojson/picojson.h>
#include <plog/Log.h>
#include <fstream>
#include <string>

template <class T>
struct ReturnedValue {
  typedef T ReturnType;
  typedef T InnerType;
};

template <>
struct ReturnedValue<int> {
  typedef int ReturnType;
  typedef double InnerType;
};

class Configuration {
 public:
  Configuration() : Configuration("deployment_config.json") {}

  Configuration(const std::string& configFile) {
    std::ifstream fileStream(configFile);
    if (not fileStream.good()) {
      throw std::runtime_error("Can't locate '" + configFile + "'");
    }

    fileStream.open(configFile, std::ios::binary);
    fileStream >> configJson;
  }

  template <class T>
  typename ReturnedValue<T>::ReturnType value(const std::string& key) const {
    return extractValue<typename ReturnedValue<T>::ReturnType,
                        typename ReturnedValue<T>::InnerType>(key);
  }

 private:
  picojson::value configJson;

  template <typename OUT, typename IN>
  const OUT extractValue(const std::string& key) const {
    if (not configJson.is<picojson::object>()) {
      return OUT{};
    }
    const picojson::object& obj = configJson.get<picojson::object>();
    auto& tmp = const_cast<picojson::object&>(obj)[key];
    return tmp.is<IN>() ? static_cast<OUT>(tmp.get<IN>()) : OUT{};
  }
};

#endif  // ! CONFIGURATION_H_9786BCF
