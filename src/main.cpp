#include <jsc/jsc.h>
#include <cstring>
#include <glib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

/**
 * @brief Basic log function used to display log messages to stdout.
 * 
 * @param message 
 */
void log(const char *message)
{
    std::cout << message << std::endl;
}

/**
 * @brief Adds the console.log functions to the JSC context 
 * 
 * @param context JSC context pointer
 */
static void add_log_functions(JSCContext *context)
{
    // create basic log function
    JSCValue *log_function = jsc_value_new_function(
        context,
        "_log",
        G_CALLBACK(log),
        nullptr,
        nullptr,
        G_TYPE_NONE,
        1,
        G_TYPE_STRING);
    jsc_context_set_value(context, "_log", log_function);

    // replace the actual global object and forward those messages
    // to the jsc callback to display those messages to std out
    char *console_log = "var console = { log: _log, info: _log, warn: _log, debug: _log, error: _log };";
    jsc_context_evaluate(context, console_log, strlen(console_log));
}

static gboolean on_interval(gpointer userdata)
{
    g_autoptr(JSCValue) result = jsc_value_function_call(
        static_cast<JSCValue *>(userdata),
        G_TYPE_NONE);

    g_autoptr(JSCException) exception = jsc_context_get_exception(
        jsc_value_get_context(static_cast<JSCValue *>(userdata)));
    if (exception)
    {
        g_autofree char *message = jsc_exception_report(exception);
        g_printerr("%s", message);
    }

    return G_SOURCE_CONTINUE;
}

static unsigned setinterval(JSCValue *callback, unsigned int interval_ms)
{
    g_autoptr(JSCContext) context = jsc_value_get_context(callback);
    if (!jsc_value_is_function(callback))
    {
        jsc_context_throw_with_name(context,
                                    "TypeError",
                                    "setTimeout: argument #1 is not a function");
        return 0;
    }

    return g_timeout_add_full(G_PRIORITY_DEFAULT,
                              interval_ms,
                              on_interval,
                              g_object_ref(callback), /* userdata */
                              nullptr);               /* destroy_notify */
}

/**
 * @brief add a setInterval function implementation to the JSC context.
 * 
 * @param context JSC context pointer
 */
static void add_setinterval_function(JSCContext *context)
{
    // add setInterval function to JSC context
    g_autoptr(JSCValue) function = jsc_value_new_function(
        context,
        "setInterval",
        G_CALLBACK(setinterval),
        nullptr,
        nullptr,
        G_TYPE_NONE,
        2,
        JSC_TYPE_VALUE,
        G_TYPE_UINT);
    jsc_context_set_value(context, "setInterval", function);
}

// ugly global code variable - contains all loaded Javascript code
// need to pack it later on to pass it to the g main loop context
static std::string code;

static gboolean run(gpointer data)
{
    g_autoptr(JSCContext) context = static_cast<JSCContext *>(data);

    // run actuall javascript code
    JSCValue *result = jsc_context_evaluate(context, code.c_str(), strlen(code.c_str()));

    // try to get any exception,...
    JSCException *exception = jsc_context_get_exception(context);

    if (exception)
    {
        std::cerr << "[message]: " << jsc_exception_get_message(exception) << std::endl;
        std::cerr << "[linenumber]: " << jsc_exception_get_line_number(exception) << std::endl;
        std::cerr << "[stacktrace]: " << jsc_exception_get_backtrace_string(exception) << std::endl;
    }

    return G_SOURCE_REMOVE;
}

/**
 * @brief Construct a new add exit function object
 * 
 * @param main_loop 
 * @param context 
 */
static void add_exit_function(GMainLoop *main_loop, JSCContext *context)
{
    jsc_context_set_value(
        context,
        "exit",
        jsc_value_new_function(context,
                               "exit",
                               G_CALLBACK(g_main_loop_quit),
                               g_main_loop_ref(main_loop),        /* userdata */
                               (GDestroyNotify)g_main_loop_unref, /* destroy_notify */
                               G_TYPE_NONE,
                               0));
}

// namespace jsc 
// {
//     class runtime {
//     public: 
//         runtime() 
//         {
//             // create a main run loop
//             this->main_loop = g_main_loop_new(NULL, FALSE);

//             // create virutal machine
//             this->virtual_machine = jsc_virtual_machine_new();

//             // create context
//             this->context = jsc_context_new_with_virtual_machine(virtual_machine);
//         }

//         void evaluate(std::string code) 
//         {

//         }

//         void run() 
//         {

//         }

//     private: 
//         JSCVirtualMachine* virtual_machine;
//         JSCValue* context;
//         GMainLoop* main_loop;

//         std::string code;


//     };
// }

/**
 * @brief Main function.
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char **argv)
{
    // try to parse first argument wich should hold a file path
    // to be able to load the contents into a string
    std::ifstream input_file(argv[1]);
    if (!input_file.is_open())
    {
        std::cerr << "Could not open the file - '" << argv[1] << "'" << std::endl;
        return EXIT_FAILURE;
    }
    code = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

    // create a main run loop
    GMainLoop *main = g_main_loop_new(NULL, FALSE);

    // create virutal machine
    JSCVirtualMachine *virtual_machine = jsc_virtual_machine_new();

    // create context
    JSCContext *context = jsc_context_new_with_virtual_machine(virtual_machine);

    add_log_functions(context);
    add_setinterval_function(context);
    add_exit_function(main, context);

    g_idle_add(run, context);
    g_main_loop_run(main);
    return 0;
}