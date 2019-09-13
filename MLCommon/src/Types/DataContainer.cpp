#include "DataContainer.h"

namespace ml {

DataContainer::DataContainer(char *data, const size_t size)
    : mData{data}, mDataSize{size} {}

DataContainer::~DataContainer() {}

void DataContainer::SetData(char *data, const size_t size) {
  mData = data;
  mDataSize = size;
}

const char *DataContainer::GetDataPtr() const { return mData; }

size_t DataContainer::GetSize() const { return mDataSize; }

OwnedDataContainer::~OwnedDataContainer() {
  if (mOnDestroy) {
    mOnDestroy(mData);
  }
}

CopiedDataContainer::~CopiedDataContainer() { delete[] mData; }
}  // namespace ml
