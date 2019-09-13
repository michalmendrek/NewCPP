#ifndef LOG_LOGER
#define LOG_LOGER

#include <iostream>
#include <fstream>


class log_loger
{
 private:
	std::string   LogFileName;
	std::string   LogContent;
	std::string   LogContentOld;
	std::string   LogFilePath;
	std::ifstream LogFile;
	std::string   EraseOldContent(std::string&  ActualData, const std::string& OldContent);
 public:

	         log_loger(std::string FileName, std::string Path);
	        ~log_loger();
     std::string ReadNewData();
     std::string ReadLog();
	  void   UpdateData();
          void   ReadWholeLog();

};




#endif
