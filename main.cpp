#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>
#include <unistd.h>
#include <cstdio>
#include <sys/types.h>

using namespace std;

const int attributeNum = 68;

int done_count = 0;

ofstream arff("test.arff");

void processFile(char *path)
{
	ifstream input(path);
	size_t i;
	for(i = strlen(path) - 1; i >= 0; --i)
		if(path[i] == '/')
			break;
	int fatigue_value;
	if(path[i + 1] == '1')
		fatigue_value = 10;
	else
		fatigue_value = path[i + 2] - '0';
	arff << fatigue_value << ',';
	for(int i = 0; i < attributeNum; ++i)
	{
		double x, y;
		input >> x >> y;
		arff << x << ',' << y;
		if(i < attributeNum - 1)
			arff << ',';
	}
	arff << endl;
	done_count++;
}

int processDirs(const char *path)
{
    struct dirent *direntp = NULL;
    DIR *dirp = NULL;
    size_t path_len;

    if(!path)
        return -1;
    path_len = strlen(path);
    if(!path || !path_len || (path_len > _POSIX_PATH_MAX))
        return -1;

    dirp = opendir(path);
    if(dirp == NULL)
        return -1;
    while((direntp = readdir(dirp)) != NULL)
    {
        struct stat fstat;
        char full_name[_POSIX_PATH_MAX + 1];
        if((path_len + strlen(direntp->d_name) + 1) > _POSIX_PATH_MAX)
            continue;

        strcpy(full_name, path);
        if(full_name[path_len - 1] != '/')
            strcat(full_name, "/");
        strcat(full_name, direntp->d_name);

        if((strcmp(direntp->d_name, ".") == 0) || (strcmp(direntp->d_name, "..") == 0))
            continue;

        if(stat(full_name, &fstat) < 0)
            continue;
        if(S_ISDIR(fstat.st_mode))
        {
            processDirs(full_name);
            continue;
        }
        processFile(full_name);

    }
}

int main(int argc, char** argv)
{
    arff << "@relation fatigue" << endl << endl;
    arff << "@attribute fatigue_value real" << endl;
    for(int i = 0; i < attributeNum; ++i)
    {
        arff << "@attribute a" << i << "_x real" << endl;
        arff << "@attribute a" << i << "_y real" << endl;
    }
    arff << endl << "@data" << endl;

    if(argc != 2)
    {
        cout << "ARFF_Generator <Dir>" << endl;
        return -1;
    }

    processDirs(argv[1]);

    arff.close();

    cout << "Processed: " << done_count << endl;
    return 0;
}
