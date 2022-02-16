#include <glib.h>
#include <iostream>

#ifdef G_PRIORITY_DEFAULT
#define SOME "TEST" 
#endif // DEBUG

static gboolean something() 
{
    return false;
}

int main() 
{
    char* some = (char*) "test";
    g_object_unref(g_object_ref(some));
    std::cout << "hello world!" << std::endl;
}
