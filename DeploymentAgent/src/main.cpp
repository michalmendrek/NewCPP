#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Log.h>
#include <sys/stat.h>
#include "archive_detector.h"
#include "configuration.h"
#include "deploy_pipeline.h"
#include "file_mover.h"
#include "file_remover.h"
#include "install_package_detector.h"
#include "system_observable.h"
#include "system_observer.h"
#include "unpacker.h"
#include "usb_mass_storage_monitor.h"
#include "usb_processing_pipeline.h"

int main(void) {
  static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
  plog::init(plog::debug, &consoleAppender);

  Configuration config;

  DeployPipeline deployPipeline{config};
  UsbProcessingPipeline usbPipeline{config, deployPipeline};
  SystemObservable usbObs(UsbMassStorageMonitor{}, usbPipeline);

  std::string deploySourcePath{"/tmp/ml-deploy"};
  mkdir(deploySourcePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  SystemObservable folderObs(
      FolderMonitor{deploySourcePath},
      [](const std::string &path) { LOGD << "New file at " << path; });

  SystemObserver sysObs;
  sysObs.addObserver(std::move(usbObs));
  sysObs.addObserver(std::move(folderObs));
  sysObs.monitor([]() { return true; });
  /*
      //TODO:temporary solution

      SystemObserver sysObs{"/tmp/ml-deploy"};
      sysObs.monitor([](){return true;});
  */
}
