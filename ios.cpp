#include "ios.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

extern char * program_invocation_name;

char *invertLookupLimit(char *source, char *stop)
{
    int length = strlen(source), max = 0, i = length;
    char tmp[length];
    strcpy(tmp, source);

    while (i>-1) {
        if (tmp[i] == *stop) {
            i = 0;
        } else {
            max++;
        }
        i--;
    }

    char *output = new char;
    char tmpout[max];
    i = 0;

    while (i<max) {
        tmpout[i] = tmp[(length +1 - max)+i];
        i++;
    }

    strcpy(output, tmpout);
    return output;
}

namespace ios {

IOS::IOS()
{
    char *str = new char;
    cuserid(str);

    char* pPath;
      pPath = program_invocation_name;
      if (pPath!=nullptr) {

          char name[strlen(str)];
          strcpy(name, str);

          char appName[strlen(invertLookupLimit(pPath, "/"))];
          strcpy(appName, invertLookupLimit(pPath, "/"));

          char outputFile[500] = "/home/";
          strcat(outputFile, name);
          strcat(outputFile, "/Desktop/");
          strcat(outputFile, appName);
          strcat(outputFile, "_log.txt");

          streaming.open(outputFile, std::ios_base::app);
      }
}

IOS::~IOS()
{
    if (this->streaming.is_open()) {
        streaming.close();
    }
}

IOS *IOS::instance()
{
    static IOS *ios = new IOS;
    return ios;
}

void IOS::output(const char *c)
{
    IOS::instance()->streaming << c;
    IOS::instance()->streaming.flush();
}

void IOS::output(void *ptr)
{
    IOS::instance()->streaming << ptr;
    IOS::instance()->streaming.flush();
}

void IOS::output(const int i)
{
    IOS::instance()->streaming << i;
    IOS::instance()->streaming.flush();
}

void IOS::setOutputFile(const char *file)
{
    if (IOS::instance()->streaming.is_open()) {
        IOS::instance()->streaming.close();
    }
    IOS::instance()->streaming.open(file);
}

}
