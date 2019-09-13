#include <loger.h>
#include <iostream>
#include <unix_notify.h>


using namespace std;

int main()
{

log_loger modol("modol.err.log","./");
modol.ReadWholeLog();
//cout << modol.ReadLog() << endl;


while(1)
{
cout << "Tab enter to update" << endl;
if('a'==getchar())
{

modol.UpdateData();

}
cout << "Tss\n\r" <<  modol.ReadNewData() << "Tbb\n\r" << endl;


}
}
