#ifndef SYSTEM_CMD_EXECUTOR_H_124324
#define SYSTEM_CMD_EXECUTOR_H_124324

#include <stdlib.h>
#include <string>

class SystemCmdExecutor {
 public:
  SystemCmdExecutor(const std::string& cmd)
      : returnStatus(system(cmd.c_str())) {}

  operator bool() const { return returnStatus == 0; }
  int getExitStatus() { return returnStatus; }

 private:
  int returnStatus;
};

#endif  // ! SYSTEM_CMD_EXECUTOR_H_124324
