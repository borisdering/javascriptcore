static void
invoke_callback(JSCValue *callback)
{
    g_autoptr(JSCValue) result = jsc_value_function_call(callback, G_TYPE_NONE);

    g_autoptr(JSCException) exc = jsc_context_get_exception(jsc_value_get_context(callback));
    if (exc) {
        g_autofree char *message = jsc_exception_report(exc);
        g_printerr("%s", message);
    }
}

static gboolean
on_interval_timeout(gpointer userdata)
{
    invoke_callback(static_cast<JSCValue *>(userdata));
    return G_SOURCE_CONTINUE;
}

static gboolean
on_timeout(gpointer userdata)
{
    invoke_callback(static_cast<JSCValue *>(userdata));
    return G_SOURCE_REMOVE;
}

static unsigned
js_setInterval(JSCValue *callback,
               unsigned  interval_ms)
{
    JSCContext *ctx = jsc_value_get_context(callback);
    if (!jsc_value_is_function(callback)) {
        jsc_context_throw_with_name(ctx,
                                    "TypeError",
                                    "setInterval: argument #1 is not a function");
        return 0;
    }

    return g_timeout_add_full(G_PRIORITY_DEFAULT,
                              interval_ms,
                              on_interval_timeout,
                              g_object_ref(callback), /* userdata */
                              g_object_unref);        /* destroy_notify */
}

static unsigned
js_setTimeout(JSCValue *callback,
              unsigned  interval_ms)
{
    JSCContext *ctx = jsc_value_get_context(callback);
    if (!jsc_value_is_function(callback)) {
        jsc_context_throw_with_name(ctx,
                                    "TypeError",
                                    "setTimeout: argument #1 is not a function");
        return 0;
    }

    return g_timeout_add_full(G_PRIORITY_DEFAULT,
                              interval_ms,
                              on_timeout,
                              g_object_ref(callback), /* userdata */
                              g_object_unref);        /* destroy_notify */
}

static void
register_event_loop_functions(JSCContext *ctx)
{
    g_autoptr(JSCValue) fn_setInterval =
        jsc_value_new_function(ctx,
                               "setInterval",
                               G_CALLBACK(js_setInterval),
                               NULL, /* userdata */
                               NULL, /* destroy_notify */
                               G_TYPE_UINT, /* return type */
                               2,
                               JSC_TYPE_VALUE,
                               G_TYPE_UINT);
    jsc_context_set_value(ctx, "setInterval", fn_setInterval);

    g_autoptr(JSCValue) fn_setTimeout =
        jsc_value_new_function(ctx,
                               "setTimeout",
                               G_CALLBACK(js_setTimeout),
                               NULL, /* userdata */
                               NULL, /* destroy_notify */
                               G_TYPE_UINT, /* return type */
                               2,
                               JSC_TYPE_VALUE,
                               G_TYPE_UINT);
    jsc_context_set_value(ctx, "setTimeout", fn_setTimeout);

    g_autoptr(JSCValue) fn_clearInterval =
        jsc_value_new_function(ctx,
                               "clearInterval",
                               G_CALLBACK(g_source_remove),
                               NULL, /* userdata */
                               NULL, /* destroy_notify */
                               G_TYPE_NONE,
                               1,
                               G_TYPE_UINT);
    jsc_context_set_value(ctx, "clearInterval", fn_clearInterval);
    jsc_context_set_value(ctx, "clearTimeout", fn_clearInterval);
}