template<typename T>
struct GTypeDeleter
{
    void operator()(T *ptr) const { g_object_unref(ptr); }
};
template<typename T>
using GUniquePtr = std::unique_ptr<T, GTypeDeleter<T>>;
class Promise {
public:
    Promise(const Promise&) = delete;
    Promise& operator=(const Promise&) = delete;
    Promise(Promise&&) = default;
    Promise& operator=(Promise&&) = default;
    Promise(JSCValue *resolve, JSCValue *reject) :
        m_resolve{JSC_VALUE(g_object_ref(resolve))},
        m_reject{JSC_VALUE(g_object_ref(reject))}
    {
    }
    void resolve(JSCValue *value) const
    {
        g_object_unref(jsc_value_function_call(m_resolve.get(),
            JSC_TYPE_VALUE, value, G_TYPE_NONE));
    }
    void reject(JSCValue *value) const
    {
        g_object_unref(jsc_value_function_call(m_reject.get(),
            JSC_TYPE_VALUE, value, G_TYPE_NONE));
    }
private:
    GUniquePtr<JSCValue> m_resolve;
    GUniquePtr<JSCValue> m_reject;
};
using PromiseHandler = std::function<void (Promise &&p)>;
static JSCValue *
makePromise(JSCContext *context, PromiseHandler &&handler)
{
    struct Data {
        PromiseHandler handler;
    };
    static const auto jsPromiseExecutorCallback = [](JSCValue *resolve, JSCValue *reject, void *userData)
    {
        auto *data = static_cast<Data*>(userData);
        Promise p {resolve, reject};
        data->handler(std::move(p));
    };
    GUniquePtr<JSCValue> jsPromiseExecutor {
        jsc_value_new_function(context,
        nullptr,      // name
        G_CALLBACK(+jsPromiseExecutorCallback),
        new Data {std::move(handler)},
        [](void *data) { delete static_cast<Data *>(data); },
        G_TYPE_NONE,  // return_type
        2,            // n_params
        JSC_TYPE_VALUE,
        JSC_TYPE_VALUE)
    };
    GUniquePtr<JSCValue> jsPromiseConstructor {
        jsc_context_get_value(context, "Promise")
    };
    JSCValue *jsPromise {
        jsc_value_constructor_call(jsPromiseConstructor.get(),
            JSC_TYPE_VALUE, jsPromiseExecutor.get(), G_TYPE_NONE)
    };
    return jsPromise;
}
