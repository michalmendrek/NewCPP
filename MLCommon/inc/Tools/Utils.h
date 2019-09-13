#ifndef UTILS_H
#define UTILS_H

#include <ctime>
#include <functional>
#include <memory>
/*!
 * \brief Contains the useful utils that can be reused in any application.
 */

namespace tools {

using DirConsumer = std::function<void(const std::string &path)>;

#define MARK_UNUSED(arg) (void)arg;

#define DISABLE_COPY(Class)      \
 private:                        \
  Class(const Class &) = delete; \
  Class &operator=(const Class &) = delete;

#define DISABLE_MOVE(Class) \
 private:                   \
  Class(Class &&) = delete; \
  Class &operator=(Class &&) = delete;

#define DISABLE_COPY_MOVE(Class) \
  DISABLE_COPY(Class)            \
  DISABLE_MOVE(Class)

#define ML_DECLARE_INTERFACE(Class) \
  DISABLE_COPY_MOVE(Class)          \
 public:                            \
  Class() = default;                \
  virtual ~Class() = default;

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

bool endsWith(const std::string &str, const std::string &suffix);

long timestamp();

std::string removeFileExtension(const std::string &filePath);

std::string getExecutableDir();

void forEachDirectory(const std::string &path, const DirConsumer &consumer);
void forEachFile(const std::string &path, const DirConsumer &consumer);

std::time_t getLastModificationTime(const std::string &filePath);

char separator();

}  // namespace tools

#endif  // UTILS_H
