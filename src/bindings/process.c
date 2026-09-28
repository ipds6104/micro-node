#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>

static JSValue js_process_exit(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    (void)this_val;
    int exit_code = 0;
    if (argc > 0) {
        JS_ToInt32(ctx, &exit_code, argv[0]);
    }
    exit(exit_code);
    return JS_UNDEFINED;
}

static JSValue js_process_cwd(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    (void)this_val;
    (void)argc;
    (void)argv;
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        return JS_NewString(ctx, cwd);
    }
    return JS_NewString(ctx, ".");
}

void mn_init_process(JSContext *ctx, int argc, char **argv) {
    JSValue global = JS_GetGlobalObject(ctx);
    JSValue process = JS_NewObject(ctx);

    // 1. process.version
    JS_SetPropertyStr(ctx, process, "version", JS_NewString(ctx, "v0.1.0-micronode"));

    // 2. process.platform & process.arch
    struct utsname os_info;
    if (uname(&os_info) == 0) {
        JS_SetPropertyStr(ctx, process, "platform", JS_NewString(ctx, "linux"));
        JS_SetPropertyStr(ctx, process, "arch", JS_NewString(ctx, os_info.machine));
    } else {
        JS_SetPropertyStr(ctx, process, "platform", JS_NewString(ctx, "unknown"));
        JS_SetPropertyStr(ctx, process, "arch", JS_NewString(ctx, "unknown"));
    }

    // 3. process.pid
    JS_SetPropertyStr(ctx, process, "pid", JS_NewInt32(ctx, (int32_t)getpid()));

    // 4. process.argv (Array of strings)
    JSValue argv_array = JS_NewArray(ctx);
    for (int i = 0; i < argc; i++) {
        JS_SetPropertyUint32(ctx, argv_array, i, JS_NewString(ctx, argv[i]));
    }
    JS_SetPropertyStr(ctx, process, "argv", argv_array);

    // 5. process.exit & process.cwd
    JS_SetPropertyStr(ctx, process, "exit", JS_NewCFunction(ctx, js_process_exit, "exit", 1));
    JS_SetPropertyStr(ctx, process, "cwd", JS_NewCFunction(ctx, js_process_cwd, "cwd", 0));

    // Pasang `process` ke global
    JS_SetPropertyStr(ctx, global, "process", process);
    JS_FreeValue(ctx, global);
}
