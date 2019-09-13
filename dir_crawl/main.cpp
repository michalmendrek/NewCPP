#include <glob.h>
#include <vector>
#include <string>
#include <iostream>
#include <unix_crawler.h>

using namespace std;




int main()
{

crawler instancja("modol2", "/home/michalmendrek/log/dir_crawl/*");


//vector<string> linki;

//linki=instancja.GetLinks();

for(auto it:instancja.GetLinks())
{
cout << it << endl;
}
}
