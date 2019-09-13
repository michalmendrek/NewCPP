#ifndef UNIX_NOTIFY
#define UNIX_NOTIFY

#include <sys/inotify.h>
#include <iostream>
#include <functional>

handle_events(int fd, int *wd)
{



}


class notify
{
  private:

  public:
         notify();
	~notify();
	void register_file(std::string path_and_name, std::functional<void(void)> callback);
	void unregister_file(std::string path_and_name);



};



#endif
