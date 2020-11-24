#include "iod.h"

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

namespace iod {

IOD::IOD()
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

IOD::~IOD()
{
    if (this->streaming.is_open()) {
        streaming.close();
    }
}

IOD *IOD::instance()
{
    static IOD *IOD = new class IOD();
    return IOD;
}

void IOD::output(const char *c)
{
    IOD::instance()->streaming << c;
    IOD::instance()->streaming.flush();
}

void IOD::output(void *ptr)
{
    IOD::instance()->streaming << ptr;
    IOD::instance()->streaming.flush();
}

void IOD::output(const int i)
{
    IOD::instance()->streaming << i;
    IOD::instance()->streaming.flush();
}

void IOD::setOutputFile(const char *file)
{
    if (IOD::instance()->streaming.is_open()) {
        IOD::instance()->streaming.close();
    }
    IOD::instance()->streaming.open(file);
}

}
