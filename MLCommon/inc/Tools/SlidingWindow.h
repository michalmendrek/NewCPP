
#ifndef SLIDING_WINDOW_H
#define SLIDING_WINDOW_H

#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <utility>

namespace ml {

template <typename DATA_TYPE, typename METADATA_TYPE>
class SlidingWindowContainer {
 public:
  using DataWithMeta = std::pair<METADATA_TYPE, DATA_TYPE>;
  using DataWithMetaShrPtr = std::shared_ptr<DataWithMeta>;
  using SlidingWindowStorageType = std::deque<DataWithMetaShrPtr>;
  using InternalData = std::pair<SlidingWindowStorageType&,
                                 typename SlidingWindowStorageType::iterator&>;
  using StorageVisitor = std::function<void(const SlidingWindowStorageType&)>;

  static DataWithMetaShrPtr pack(const METADATA_TYPE& meta,
                                 const DATA_TYPE& data) {
    return std::make_shared<DataWithMeta>(meta, data);
  }
};

template <typename DATA_TYPE, typename METADATA_TYPE,
          template <typename, typename> class POLICY>
class SlidingWindow {
 public:
  using BaseType = SlidingWindowContainer<DATA_TYPE, METADATA_TYPE>;
  using CleanupPolicyType = POLICY<DATA_TYPE, METADATA_TYPE>;

  SlidingWindow(const CleanupPolicyType& policy)
      : nextReadElement(storage.begin()), cleanupPolicy(policy) {}

  void AddData(const METADATA_TYPE& meta, const DATA_TYPE& data) {
    std::lock_guard<std::mutex> lock(mutex);
    storage.push_back(BaseType::pack(meta, data));
    if (nextReadElement == storage.end()) {
      nextReadElement = storage.begin();
    }
    cleanupPolicy.cleanup(storage, nextReadElement);
  }

  bool HasNextData() {
    std::lock_guard<std::mutex> lock(mutex);
    return nextReadElement != storage.end();
  }

  typename BaseType::DataWithMetaShrPtr GetNextData() {
    std::lock_guard<std::mutex> lock(mutex);
    if (nextReadElement != storage.end()) {
      typename BaseType::DataWithMetaShrPtr result = *nextReadElement;
      nextReadElement++;
      cleanupPolicy.cleanup(storage, nextReadElement);
      return result;

    } else {
      return typename BaseType::DataWithMetaShrPtr{};
    }
  }

  typename BaseType::InternalData getInternalData() {
    return {storage, nextReadElement};
  }

  void accessStorage(const typename BaseType::StorageVisitor& visitor) const {
    std::lock_guard<std::mutex> lock(mutex);
    visitor(storage);
  }

  CleanupPolicyType& getCleanupPolicy() { return cleanupPolicy; }

  SlidingWindow(const SlidingWindow& other) = delete;
  //  SlidingWindow(const SlidingWindow& other)
  //      : storage(other.storage),
  //        nextReadElement(other.nextReadElement),
  //        cleanupPolicy(other.cleanupPolicy) {}

  SlidingWindow& operator=(const SlidingWindow& other) = delete;
  //  SlidingWindow& operator=(const SlidingWindow& other) {
  //    storage = other.storage;
  //    nextReadElement = other.nextReadElement;
  //    cleanupPolicy = other.cleanupPolicy;
  //    return *this;
  //  }

  SlidingWindow(SlidingWindow&& other)
      : storage(std::move(other.storage)),
        nextReadElement(std::move(other.nextReadElement)),
        cleanupPolicy(std::move(other.cleanupPolicy)) {}

  SlidingWindow& operator=(SlidingWindow&& other) {
    std::swap(other.storage, storage);
    std::swap(other.nextReadElement, nextReadElement);
    std::swap(other.cleanupPolicy, cleanupPolicy);
    return *this;
  }

 private:
  typename BaseType::SlidingWindowStorageType storage;
  typename BaseType::SlidingWindowStorageType::iterator nextReadElement;

  mutable std::mutex mutex;
  CleanupPolicyType cleanupPolicy;
};

template <typename DATA_TYPE, typename METADATA_TYPE>
class MaxHistoryCleanupPolicy {
 public:
  using Storage =
      typename SlidingWindowContainer<DATA_TYPE,
                                      METADATA_TYPE>::SlidingWindowStorageType;

  MaxHistoryCleanupPolicy(size_t historyDepth) : historyDepth(historyDepth) {}
  MaxHistoryCleanupPolicy(MaxHistoryCleanupPolicy&& other) noexcept
      : historyDepth(other.historyDepth) {}
  MaxHistoryCleanupPolicy(const MaxHistoryCleanupPolicy& other) noexcept
      : historyDepth(other.historyDepth) {}
  MaxHistoryCleanupPolicy& operator=(MaxHistoryCleanupPolicy&& other) {
    std::swap(other.historyDepth, historyDepth);
    return *this;
  }

  MaxHistoryCleanupPolicy& operator=(const MaxHistoryCleanupPolicy& other) {
    historyDepth = other.historyDepth;
    return *this;
  }

  void cleanup(Storage& storage, typename Storage::iterator& nextReadElement) {
    while (static_cast<size_t>(std::distance(storage.begin(),
                                             nextReadElement)) > historyDepth) {
      storage.pop_front();
    }
  }

  void setHistoryDepth(size_t depth) { historyDepth = depth; }
  size_t getHistoryDepth() { return historyDepth; }

 private:
  size_t historyDepth;
};

}  // namespace
#endif  // SLIDING_WINDOW_H
