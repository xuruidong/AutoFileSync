#include <sys/stat.h>
#include <iostream>

using namespace std;

bool is_dir(const char * path) 
{
#if 0
	struct stat buff;  
	
	if (lstat(name,&buff) < 0 ) {
	  	return false; //if not exist name ,ignore
	}
	
	/*if is directory return 1 ,else return 0*/ 
	if( S_ISDIR(buff.st_mode) )
		return true;
#endif
	struct stat s;
    if ( stat ( path, &s ) == 0 ) {
        if ( s.st_mode & S_IFDIR ) {
            cout << "DIR" << endl;
			return true;
        } else if ( s.st_mode & S_IFREG ) {
            cout << "FILE" << endl;
        } else {
            cout << "?" << endl;
        }
    } 
	
	return false;
}

