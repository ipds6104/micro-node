#include "console.h"
#include <stdio.h>
#include <stdlib.h>

static void print_args(FILE *stream, JSContext *ctx, int argc, JSValueConst *argv) {
    for (int i = 0; i < argc; i++) {
        const char *str = JS_ToCString(ctx, argv[i]);
        if (!str) continue;
        fprintf(stream, "%s%s", i > 0 ? " " : "", str);
        JS_FreeCString(ctx, str);
    }
    fprintf(stream, "\n");
    fflush(stream);
}

static JSValue js_console_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    (void)this_val;
    print_args(stdout, ctx, argc, argv);
    return JS_UNDEFINED;
}

static JSValue js_console_error(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    (void)this_val;
    // Cetak ke stderr (dengan prefix ANSI merah jika diinginkan, atau standar)
    fprintf(stderr, "\033[31m[ERROR]\033[0m ");
    print_args(stderr, ctx, argc, argv);
    return JS_UNDEFINED;
}

static JSValue js_console_warn(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    (void)this_val;
    fprintf(stderr, "\033[33m[WARN]\033[0m ");
    print_args(stderr, ctx, argc, argv);
    return JS_UNDEFINED;
}

void mn_init_console(JSContext *ctx) {
    JSValue global = JS_GetGlobalObject(ctx);
    JSValue console = JS_NewObject(ctx);

    JS_SetPropertyStr(ctx, console, "log", JS_NewCFunction(ctx, js_console_log, "log", 1));
    JS_SetPropertyStr(ctx, console, "error", JS_NewCFunction(ctx, js_console_error, "error", 1));
    JS_SetPropertyStr(ctx, console, "warn", JS_NewCFunction(ctx, js_console_warn, "warn", 1));

    JS_SetPropertyStr(ctx, global, "console", console);
    JS_FreeValue(ctx, global);
}
