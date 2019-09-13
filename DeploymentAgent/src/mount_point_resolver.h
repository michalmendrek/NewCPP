#ifndef MOUNT_POINT_RESOLVER_H_12A845
#define MOUNT_POINT_RESOLVER_H_12A845

#include <mntent.h>
#include <stdio.h>
#include <unistd.h>
#include <functional>
#include <string>

class MountPointResolver {
 public:
  using MountFSNameConsumer = std::function<void(const std::string& fsName,
                                                 const std::string& mountDir)>;
  static void resolve(const MountFSNameConsumer& consumer) {
    FILE* filePtr = setmntent("/etc/mtab", "r");
    if (filePtr != nullptr) {
      struct mntent* entity;
      while (true) {
        entity = getmntent(filePtr);
        if (entity == nullptr) {
          break;
        }
        consumer(entity->mnt_fsname, entity->mnt_dir);
      }
      endmntent(filePtr);
    }
  }
};

#endif  // ! MOUNT_POINT_RESOLVER_H_12A845
