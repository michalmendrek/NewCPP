#ifndef DEPLOY_PIPELINE_H_32CD214
#define DEPLOY_PIPELINE_H_32CD214

#include <plog/Log.h>
#include <string>
#include "configuration.h"
#include "file_mover.h"
#include "install_package_detector.h"
#include "system_cmd_executor.h"
#include "unpacker.h"

class DeployPipeline {
 public:
  DeployPipeline(const Configuration& conf) : conf(conf) {
    checkForRequiredConfKey(CONF_POLICY_PATH);
    checkForRequiredConfKey(CONF_MODULE_DEPLOY_PATH);
    checkForRequiredConfKey(CONF_START_MLPLATFORM_CMD);
    checkForRequiredConfKey(CONF_STOP_MLPLATFORM_CMD);
    checkForRequiredConfKey(CONF_CONFIGURE_MLPLATFORM_CMD);
  }

  bool operator()(const InstallPkg& pkg) {
    if (not SystemCmdExecutor{
            conf.value<std::string>(CONF_STOP_MLPLATFORM_CMD)}) {
      LOGE << "Can't stop MLPLatform, aborting";
      return false;
    }

    std::vector<std::string> archives{pkg.getArchivesPath(true)};
    for (const std::string& archPath : archives) {
      if (not deployArchive(archPath)) {
        LOGE << "Can't deploy archive, fallback required";
        fallback();
        return false;
      }
    }
    if (not deployPolicyFile(pkg.getPolicyPath(true))) {
      LOGE << "Can't deploy policy file, fallback required";
      fallback();
      return false;
    }

    if (not SystemCmdExecutor{
            conf.value<std::string>(CONF_CONFIGURE_MLPLATFORM_CMD)}) {
      LOGE << "FATAL: Can't configure MLPLatform, continue fallback should be "
              "executed by MLPLatform";
    }

    if (not SystemCmdExecutor{
            conf.value<std::string>(CONF_START_MLPLATFORM_CMD)}) {
      LOGE << "FATAL: Can't start MLPLatform";
      return false;
    }
    return true;
  }

 private:
  static const std::string CONF_POLICY_PATH;
  static const std::string CONF_MODULE_DEPLOY_PATH;
  static const std::string CONF_START_MLPLATFORM_CMD;
  static const std::string CONF_STOP_MLPLATFORM_CMD;
  static const std::string CONF_CONFIGURE_MLPLATFORM_CMD;

  const Configuration& conf;

  void fallback() {
    if (not SystemCmdExecutor{
            conf.value<std::string>(CONF_START_MLPLATFORM_CMD)}) {
      LOGE << "FALLBACK FATAL: Can't start MLPLatform";
    } else {
      LOGW << "FALLBACK: Running MLPLatform";
    }
  }

  bool deployArchive(const std::string& path) {
    std::string destPath = conf.value<std::string>(CONF_MODULE_DEPLOY_PATH);
    Unpacker unpacker;
    return unpacker.unpack(path, destPath);
  }

  bool deployPolicyFile(const std::string& path) {
    if (not backupPolicy()) {
      LOGE << "Can't make backup of policy file, aborting.";
      return false;
    }

    std::string policyPath = conf.value<std::string>(CONF_POLICY_PATH);
    bool success = FileMover::moveFile(path, policyPath, true);
    if (not success) {
      LOGE << "Can't deploy " << path;
      restorePolicy();
      return false;
    }

    return true;
  }

  bool backupPolicy() {
    std::string policyPath = conf.value<std::string>(CONF_POLICY_PATH);
    std::ifstream fileStream(policyPath);
    if (not fileStream.good()) {
      return true;  // there is no policy, nothing to backup
    }
    return FileMover::moveFile(policyPath, policyPath + ".old", true);
  }

  void restorePolicy() {
    std::string policyPath = conf.value<std::string>(CONF_POLICY_PATH);
    FileMover::moveFile(policyPath + ".old", policyPath, true);
  }

  void checkForRequiredConfKey(const std::string& key) {
    if (conf.value<std::string>(key).empty()) {
      throw std::runtime_error("Lack of required configuration key:" + key);
    }
  }
};

const std::string DeployPipeline::CONF_POLICY_PATH{"policyPath"};
const std::string DeployPipeline::CONF_MODULE_DEPLOY_PATH{"modulesDeployPath"};
const std::string DeployPipeline::CONF_START_MLPLATFORM_CMD{"startMLAppCmd"};
const std::string DeployPipeline::CONF_STOP_MLPLATFORM_CMD{"stopMLAppCmd"};
const std::string DeployPipeline::CONF_CONFIGURE_MLPLATFORM_CMD{
    "configureMLAppCmd"};

#endif  // ! DEPLOY_PIPELINE_H_32CD214
