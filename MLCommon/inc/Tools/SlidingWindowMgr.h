#ifndef SLIDING_WINDOW_MGR_H_78234
#define SLIDING_WINDOW_MGR_H_78234

#include <string>
#include <unordered_map>
#include "SlidingWindow.h"

namespace ml {

template <typename DATA_TYPE, typename METADATA_TYPE>
class SlidingWindowMgr {
 public:
  using WindowType =
      SlidingWindow<DATA_TYPE, METADATA_TYPE, MaxHistoryCleanupPolicy>;

  void addWindow(const std::string& name, size_t windowSize) {
    MaxHistoryCleanupPolicy<DATA_TYPE, METADATA_TYPE> policy{windowSize};
    windows.emplace(std::make_pair(name, WindowType{policy}));
  }

  WindowType& getWindow(const std::string& name) {
    auto it = windows.find(name);
    if (it == windows.end()) {
      throw std::runtime_error("No window with name: " + name);
    }
    return it->second;
  }

  void addData(const std::string& name, const METADATA_TYPE& meta,
               const DATA_TYPE& data) {
    auto it = windows.find(name);
    if (it != windows.end()) {
      it->second.AddData(meta, data);
    } else {
      throw std::runtime_error("No window with name: " + name +
                               " can't add data.");
    }
  }

 private:
  std::unordered_map<std::string, WindowType> windows;
};
}
#endif  // !SLIDING_WINDOW_MGR_H_78234
