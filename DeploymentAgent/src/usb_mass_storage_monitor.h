#ifndef USB_MASS_STORAGE_MONITOR_H_99ACC
#define USB_MASS_STORAGE_MONITOR_H_99ACC

#include <libudev.h>
#include <plog/Log.h>
#include <algorithm>
#include <functional>
#include <ios>
#include <iostream>
#include <string>
#include <vector>
#include "mount_point_resolver.h"

class UsbMassStorage {
 public:
  const std::string mountPath;
  const std::string fsName;
  const std::string vendor;
  const std::string productId;

  UsbMassStorage() {}

  UsbMassStorage(const std::string& fsName, const std::string& mountPath,
                 const std::string& vendor, const std::string& productId)
      : mountPath(mountPath),
        fsName(fsName),
        vendor(vendor),
        productId(productId) {}

  friend std::ostream& operator<<(std::ostream& os,
                                  const UsbMassStorage& item) {
    os << "fsName=" << item.fsName << ", mountPath=" << item.mountPath << ", "
       << item.vendor << ":" << item.productId;
    return os;
  }

  operator bool() const { return mountPath.length() > 0; }
};

class UsbMassStorageMonitor {
 public:
  using DeviceVector = std::vector<UsbMassStorage>;
  using ProcessConsumer = std::function<void(const UsbMassStorage& msd)>;

  UsbMassStorageMonitor()
      : udev(udev_new()), mon(udev_monitor_new_from_netlink(udev, "udev")) {
    if (!udev) {
      throw std::ios_base::failure("udev_new() failed");
    }

    if (!mon) {
      cleanup();
      throw std::ios_base::failure("udev_monitor_new_from_netlink() failed");
    }

    if (udev_monitor_filter_add_match_subsystem_devtype(mon, "usb",
                                                        "usb_device") < 0) {
      cleanup();
      throw std::ios_base::failure(
          "udev_monitor_filter_add_match_subsystem_devtype() failed");
    }

    if (udev_monitor_enable_receiving(mon) < 0) {
      cleanup();
      throw std::ios_base::failure("udev_monitor_enable_receiving() failed");
    }
    LOGI << "Mass storage observer start";
  }

  ~UsbMassStorageMonitor() { cleanup(); }

  UsbMassStorageMonitor(UsbMassStorageMonitor&& other)
      : delayedDevices(std::move(other.delayedDevices)) {
    std::swap(udev, other.udev);
    std::swap(mon, other.mon);
  }

  UsbMassStorageMonitor& operator=(UsbMassStorageMonitor&& other) {
    std::swap(delayedDevices, other.delayedDevices);
    std::swap(udev, other.udev);
    std::swap(mon, other.mon);
    return *this;
  }

  DeviceVector enumerateDevices() {
    udev_enumerate* enumerate = udev_enumerate_new(udev);

    udev_enumerate_add_match_subsystem(enumerate, "scsi");
    udev_enumerate_add_match_property(enumerate, "DEVTYPE", "scsi_device");
    udev_enumerate_scan_devices(enumerate);

    udev_list_entry* devices = udev_enumerate_get_list_entry(enumerate);
    udev_list_entry* entry;
    DeviceVector result;

    udev_list_entry_foreach(entry, devices) {
      const char* path = udev_list_entry_get_name(entry);
      struct udev_device* dev = udev_device_new_from_syspath(udev, path);
      if (dev) {
        const UsbMassStorage& processed = processDevice(dev);
        if (processed) {
          result.emplace_back(processed);
        }
      }
      udev_device_unref(dev);
    }

    udev_enumerate_unref(enumerate);
    return result;
  }

  int getFileDescriptor() const { return udev_monitor_get_fd(mon); }

  void handleIncommingEvents() {
    struct udev_device* dev = udev_monitor_receive_device(mon);
    if (dev) {
      const char* action = udev_device_get_action(dev);
      // action == null -> device already exists
      if (strncmp("add", action, 3) == 0) {
        LOGD << "Detected new usb device: ";
        delayedDevices.emplace_back(RawDevice{dev});

      } else if (strncmp("remove", action, 6) == 0) {
        LOGD << "Detected removal of usb device";
        auto it = std::find_if(
            delayedDevices.begin(), delayedDevices.end(),
            [=](const RawDevice& item) { return item.checkIfSameDevice(dev); });
        if (it != delayedDevices.end()) {
          delayedDevices.erase(it);
        }
      }
      udev_device_unref(dev);
    }
  }

  void process(const ProcessConsumer& consumer) {
    auto it = std::partition(delayedDevices.begin(), delayedDevices.end(),
                             [&](const RawDevice& device) {
                               const UsbMassStorage& processed =
                                   processDevice(device.dev);
                               if (processed) {
                                 consumer(processed);
                               }
                               return not processed;
                             });
    if (it != delayedDevices.end()) {
      delayedDevices.erase(it, delayedDevices.end());
    }
  }

 private:
  class RawDevice {
   public:
    struct udev_device* dev{nullptr};

    RawDevice(const RawDevice& other) = delete;
    RawDevice& operator=(const RawDevice& other) = delete;

    RawDevice(RawDevice&& other) { std::swap(other.dev, dev); }

    RawDevice& operator=(RawDevice&& other) {
      std::swap(dev, other.dev);
      return *this;
    }

    explicit RawDevice(struct udev_device* d) : dev(d) { udev_device_ref(dev); }

    ~RawDevice() { udev_device_unref(dev); }

    bool checkIfSameDevice(struct udev_device* otherDev) const {
      if (dev == nullptr) {
        return false;
      }
      const char* thisId = udev_device_get_devpath(dev);
      const char* otherId = udev_device_get_devpath(otherDev);
      if (thisId == nullptr or otherId == nullptr) {
        return false;
      }
      return strncmp(thisId, otherId, 255) == 0;
    }

    bool operator==(const RawDevice& other) const {
      const char* thisId = udev_device_get_devpath(dev);
      const char* otherId = udev_device_get_devpath(other.dev);
      if (thisId == nullptr or otherId == nullptr) {
        return false;
      }
      return strncmp(thisId, otherId, 255) == 0;
    }
  };

  std::vector<RawDevice> delayedDevices;
  struct udev* udev{nullptr};
  struct udev_monitor* mon{nullptr};

  void cleanup() {
    udev_monitor_unref(mon);
    udev_unref(udev);
  }

  UsbMassStorage processDevice(struct udev_device* dev) {
    struct udev_device* block = GetChild(dev, "block");
    struct udev_device* scsi_disk = GetChild(dev, "scsi_disk");

    struct udev_device* usb =
        udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");

    if (block && scsi_disk && usb) {
      std::string mountPoint;
      std::string devFsName{udev_device_get_devnode(block)};
      MountPointResolver::resolve([&](const std::string& fsName,
                                      const std::string& mountDir) {
        if (std::equal(devFsName.begin(), devFsName.end(), fsName.begin())) {
          mountPoint = mountDir;
          LOGD << "New USB device mounted: " << mountPoint;
        }
      });
      udev_device_unref(block);
      udev_device_unref(scsi_disk);
      if (mountPoint.empty()) {
        return {};
      }
      return {devFsName, mountPoint,
              udev_device_get_sysattr_value(usb, "idVendor"),
              udev_device_get_sysattr_value(usb, "idProduct")};
    }

    if (block) {
      udev_device_unref(block);
    }

    if (scsi_disk) {
      udev_device_unref(scsi_disk);
    }
    return {};
  }

  struct udev_device* GetChild(struct udev_device* parent,
                               const char* subsystem) {
    struct udev_device* child = nullptr;
    struct udev_enumerate* enumerate = udev_enumerate_new(udev);

    udev_enumerate_add_match_parent(enumerate, parent);
    udev_enumerate_add_match_subsystem(enumerate, subsystem);
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry* devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry* entry;

    udev_list_entry_foreach(entry, devices) {
      const char* path = udev_list_entry_get_name(entry);
      child = udev_device_new_from_syspath(udev, path);
      break;
    }

    udev_enumerate_unref(enumerate);
    return child;
  }
};

#endif  // ! USB_MASS_STORAGE_MONITOR_H_99ACC
