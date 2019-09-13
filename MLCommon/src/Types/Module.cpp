#include "Module.h"
#include <algorithm>

namespace Types {

Module::Module(const std::string& name) : mName{name} {}

void Module::AddLibrary(const ::std::string& val) {
  mLibraryList.push_back(val);
}

void Module::AddMandatoryData(const SensorStream& val) {
  mMandatoryData.push_back(val);
}

const std::vector<Module::SensorStream>& Module::MandatoryData() const {
  return mMandatoryData;
}

bool Module::IsStreamMandatory(const std::string& name) const {
  auto it = std::find_if(
      mMandatoryData.begin(), mMandatoryData.end(),
      [&](const SensorStream& stream) { return stream.name == name; });

  return it != mMandatoryData.end();
}

const Module::LibraryList& Module::GetConstLibraryList() const {
  return mLibraryList;
}

const std::string& Module::GetConstName() const { return mName; }

void Module::SetModulePath(const ::std::string& val) { mModulePath = val; }

const ::std::string& Module::GetConstModulePath() const { return mModulePath; }

void Module::SetStarted() { mStarted = true; }

void Module::ClearStarted() { mStarted = false; }

const bool& Module::GetStarted() const { return mStarted; }

void Module::SetConnectedOK() { mConnectedOK = true; }

void Module::ClearConnectedOK() { mConnectedOK = false; }

const bool& Module::GetConnectedOK() const { return mConnectedOK; }

const std::string& Module::GetOutputData() const { return mOutputData; }

void Module::SetOutputData(const std::string& name) { mOutputData = name; }

}  // namespace - Types
