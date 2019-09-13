#ifndef DATACONTAINER_H
#define DATACONTAINER_H

#include <algorithm>
#include <cstddef>
#include <functional>

namespace ml {

class DataContainer {
 public:
  DataContainer() = default;
  DataContainer(char* data, const size_t size);
  virtual ~DataContainer();

  void SetData(char* data, const size_t size);
  const char* GetDataPtr() const;
  size_t GetSize() const;

 protected:
  char* mData{nullptr};
  size_t mDataSize{0};
};

class OwnedDataContainer : public DataContainer {
 public:
  using OnDestroy = std::function<void(char*)>;
  ~OwnedDataContainer() override;
  void SetOnDestroy(const OnDestroy& onDestroy) {
    this->mOnDestroy = onDestroy;
  }

 private:
  OnDestroy mOnDestroy;
};

class CopiedDataContainer : public DataContainer {
 public:
  CopiedDataContainer(CopiedDataContainer&& other) : DataContainer() {
    swap(other);
  }

  CopiedDataContainer& operator=(CopiedDataContainer&& src) {
    swap(src);
    return *this;
  }

  CopiedDataContainer(const CopiedDataContainer& other)
      : CopiedDataContainer(other.mData, other.mDataSize) {}

  CopiedDataContainer& operator=(const CopiedDataContainer& src) {
    if (&src != this) {
      delete[] mData;
      mData = new char[src.mDataSize];
      mDataSize = src.mDataSize;
      std::copy(src.mData, src.mData + src.mDataSize, mData);
    }
    return *this;
  }

  CopiedDataContainer(const char* data, const size_t size)
      : DataContainer(new char[size], size) {
    std::copy(data, data + size, mData);
  }

  virtual ~CopiedDataContainer();

  void swap(CopiedDataContainer& other) {
    std::swap(other.mData, mData);
    std::swap(other.mDataSize, mDataSize);
  }
};

}  // namespace - ml

#endif  // ! DATACONTAINER_H
