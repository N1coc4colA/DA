#ifndef IOS_H
#define IOS_H

#include <iostream>
#include <fstream>

namespace ios {
class IOS
{
public:
    IOS();
    ~IOS();

    static IOS *instance();
    static void setOutputFile(const char *);
    static void output(const char *);
    static void output(void *);
    static void output(const int);

private:
    std::ofstream streaming;
};
}

#endif // IOS_H
