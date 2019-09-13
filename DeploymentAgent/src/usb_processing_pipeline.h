#ifndef USB_PROCESSING_PIPELINE_H_234BAC
#define USB_PROCESSING_PIPELINE_H_234BAC

#include <plog/Log.h>
#include <string>
#include "configuration.h"
#include "file_mover.h"
#include "install_package_detector.h"
#include "temporary_folder.h"
#include "usb_mass_storage_monitor.h"

class UsbProcessingPipeline {
 public:
  using PackageProcessor = std::function<bool(const InstallPkg&)>;

  UsbProcessingPipeline(const Configuration& config,
                        const PackageProcessor processor)
      : config(config), processor(processor) {}

  void operator()(const UsbMassStorage& msd) {
    // NOTE: RAII based even if not visible for first look
    const std::string& path = msd.mountPath;
    LOGI << "Processing Mass Stoarage at " << path;

    InstallPackageDetector detector;
    if (not detector.process(path)) {
      LOGE << "No packages to install detected, aborting.";
      return;
    }

    TemporaryFolder tmpFolder{config.value<std::string>("usbTempPath")};
    if (not tmpFolder) {
      LOGE << "No destination, aborting.";
      return;
    }

    InstallPkg pkg{detector.getPackage()};
    if (not movePackage(pkg, tmpFolder.getFullPath())) {
      LOGE << "Package couldn't be moved, aborting.";
      return;
    }

    if (not processor(pkg)) {
      LOGE << "Package couldn't be processed, aborting.";
    } else {
      LOGI << "Succesfully handled mass storage";
    }
  }

 private:
  const Configuration& config;
  PackageProcessor processor;

  bool movePackage(InstallPkg& pkg, const std::string& path) {
    bool result = FileMover::moveFiles(pkg.allFiles(true), path);
    if (result) {
      pkg.setRootPath(path);
    }
    return result;
  }
};

#endif  // ! USB_PROCESSING_PIPELINE_H_234BAC
