#include "wintools.h"

#include <QList>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <X11/extensions/shape.h>
#ifndef NO_I18N
#include <X11/Xlocale.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <locale.h>
#include <glib.h>

#define MAXSTR 1000
#define MAX_PROPERTY_VALUE_LEN 4096

class Instance {
public:
    inline ~Instance() {
        XCloseDisplay(display);
    }

    Display *display;
    unsigned long window;
    unsigned char *prop;
    int screen = 0;
    bool loaded = false;
};


static Instance *instance = new Instance;

void load()
{
    instance->display = XOpenDisplay(nullptr);
}

static bool check_status(int status, unsigned long window)
{
    if (status != Success) {
        printf("XGetWindowProperty failed!");
        return true;
    }
    if (status == BadWindow) {
        printf("window id # 0x%lx does not exists!", window);
    }
    return false;
}

static unsigned char* get_string_property(char* property_name)
{
    Atom actual_type, filter_atom;
    int actual_format, status;
    unsigned long nitems, bytes_after;

    filter_atom = XInternAtom(instance->display, property_name, True);
    status = XGetWindowProperty(instance->display, instance->window, filter_atom, 0, MAXSTR, False, AnyPropertyType,
                                &actual_type, &actual_format, &nitems, &bytes_after, &instance->prop);
    check_status(status, instance->window);
    return instance->prop;
}

static unsigned long get_long_property(char* property_name)
{
    get_string_property(property_name);
    unsigned long long_property = instance->prop[0] + (instance->prop[1]<<8) + (instance->prop[2]<<16) + (instance->prop[3]<<24);
    return long_property;
}

static Window *winlist (Display *disp, unsigned long *len) {
    Atom prop = XInternAtom(disp,"_NET_CLIENT_LIST",False), type;
    int form;
    unsigned long remain;
    unsigned char *list;

    errno = 0;
    if (XGetWindowProperty(disp,XDefaultRootWindow(disp),prop,0,1024,False,XA_WINDOW,
                &type,&form,len,&remain,&list) != Success) {
        perror("winlist() -- GetWinProp");
        return nullptr;
    }

    return (Window*)list;
}

static char *get_property(Display *disp, Window win, Atom xa_prop_type, char *prop_name, unsigned long *size)
{
    Atom xa_prop_name;
    Atom xa_ret_type;
    int ret_format;
    unsigned long ret_nitems;
    unsigned long ret_bytes_after;
    unsigned long tmp_size;
    unsigned char *ret_prop;
    char *ret;

    xa_prop_name = XInternAtom(disp, prop_name, False);

    /* MAX_PROPERTY_VALUE_LEN / 4 explanation (XGetWindowProperty manpage):
     *
     * long_length = Specifies the length in 32-bit multiples of the
     *               data to be retrieved.
     */
    if (XGetWindowProperty(disp, win, xa_prop_name, 0, MAX_PROPERTY_VALUE_LEN / 4, False,
            xa_prop_type, &xa_ret_type, &ret_format,
            &ret_nitems, &ret_bytes_after, &ret_prop) != Success) {
        return nullptr;
    }

    if (xa_ret_type != xa_prop_type) {
        XFree(ret_prop);
        return nullptr;
    }

    /* null terminate the result to make string handling easier */
    tmp_size = (ret_format / (32 / sizeof(long))) * ret_nitems;
    gsize sized = tmp_size+1;
    ret = (char *)g_malloc(sized);
    memcpy((void*)ret, ret_prop, tmp_size);
    ret[tmp_size] = '\0';

    if (size) {
        *size = tmp_size;
    }

    XFree(ret_prop);
    return ret;
}

static char *winame(int winID)
{
    Atom prop = XInternAtom(instance->display,"WM_NAME",False), type;
    int form;
    unsigned long remain, len;
    unsigned char *list;

    errno = 0;
    if (XGetWindowProperty(instance->display, instance->window, prop, 0, 1024, False, XA_STRING,
                &type,&form,&len,&remain,&list) != Success) {
        perror("winlist() -- GetWinProp");
        return nullptr;
    }

    return (char*)list;
}

LDA_BEGIN_NAMESPACE

bool isFromPID(int win, int pid)
{
    if (!instance->loaded) {
        load();
    }
    unsigned long len;
    Window *list;
    if (!instance->display) {
        puts("no display!");
        return false;
    }

    list = (Window*)winlist(instance->display,&len);
    std::cout << "Total of windows: " << (int)len;
    instance->window = win;
    if (((int)get_long_property((char *)"_NET_WM_PID")) == pid) {
        return true;
    }

    XFree(list);
    return false;
}

QList<int> pidToWins(int pid)
{
    if (!instance->loaded) {
        load();
    }
    QList<int> out;

    int i;
    unsigned long len;
    Window *list;
    if (!instance->display) {
        puts("no display!");
        return out;
    }

    list = (Window*)winlist(instance->display,&len);
    std::cout << "Total of windows: " << (int)len;

    for (i=0;i<(int)len;i++) {
        instance->window = list[i];
        if (((int)get_long_property((char *)"_NET_WM_PID")) == pid) {
            out << (int)instance->window;
        }
    }
    XFree(list);
    return out;
}

LDA_END_NAMESPACE
