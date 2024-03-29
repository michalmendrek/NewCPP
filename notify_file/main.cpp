#include <sys/stat.h>
#include "system_observable.h"
#include "folder_monitor.h"
#include <iostream>
#include "system_observer.h"

int main(void) {


  std::string deploySourcePath{"/home/michalmendrek/log/notify_file/tmp/file.log"};


  //mkdir(deploySourcePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  SystemObservable folderObs(FolderMonitor{deploySourcePath}, [](const std::string &path) { std::cout << "New file at " << path << std::endl; });

  SystemObserver sysObs;
  sysObs.addObserver(std::move(folderObs));
  sysObs.monitor([]() { std::cout << "New file at: " << std::endl; return true; });
  /*
      //TODO:temporary solution

      SystemObserver sysObs{"/tmp/ml-deploy"};
      sysObs.monitor([](){return true;});
  */

while(1);
}
