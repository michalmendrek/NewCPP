#include <unix_crawler.h>
#include <glob.h>
#include <string>
#include <vector>


crawler::crawler(const std::string& Name, const std::string& Path)
{

 glob_t glob_result;
 glob(Path.c_str(),GLOB_TILDE,NULL,&glob_result);
 std::vector<std::string> files;
  for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        files.push_back(std::string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);

  for(auto it:files)
  {
    if(it.find(Name)!=std::string::npos){LinksToLog.push_back(it);}
  }


}


crawler::~crawler()
{

}



std::vector<std::string> CrawlOverDirectory(const std::string& ModuleName)
{
  glob_t glob_result;
  glob(ModuleName.c_str(),GLOB_TILDE,NULL,&glob_result);
  std::vector<std::string> files;
  for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        files.push_back(std::string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);

    return files;
}


std::vector<std::string> crawler::GetLinks()
{

return LinksToLog;
}
