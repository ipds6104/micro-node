#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <quickjs/quickjs.h>

#include "bindings/console.h"
#include "bindings/process.h"

static char *read_file(const char *filename, size_t *out_size) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Gagal membuka file");
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(size + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    size_t read_bytes = fread(buf, 1, size, f);
    buf[read_bytes] = '\0';
    fclose(f);

    if (out_size) *out_size = read_bytes;
    return buf;
}

static void print_exception(JSContext *ctx, JSValue ex) {
    JSValue stack = JS_GetPropertyStr(ctx, ex, "stack");
    if (!JS_IsUndefined(stack)) {
        const char *stack_str = JS_ToCString(ctx, stack);
        fprintf(stderr, "\033[31mUncaught Exception:\033[0m\n%s\n", stack_str);
        JS_FreeCString(ctx, stack_str);
    } else {
        const char *err_str = JS_ToCString(ctx, ex);
        fprintf(stderr, "\033[31mUncaught Exception:\033[0m %s\n", err_str);
        JS_FreeCString(ctx, err_str);
    }
    JS_FreeValue(ctx, stack);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Penggunaan: %s <file.js> [argumen...]\n", argv[0]);
        fprintf(stderr, "Micro-Node: Runtime JavaScript minimalis dari nol (Fase 1)\n");
        return 1;
    }

    const char *script_file = argv[1];
    size_t code_len = 0;
    char *code = read_file(script_file, &code_len);
    if (!code) {
        return 1;
    }

    // 1. Inisialisasi JavaScript Engine (Runtime & Context)
    JSRuntime *rt = JS_NewRuntime();
    if (!rt) {
        fprintf(stderr, "Fatal: Gagal membuat JS Runtime\n");
        free(code);
        return 1;
    }

    JSContext *ctx = JS_NewContext(rt);
    if (!ctx) {
        fprintf(stderr, "Fatal: Gagal membuat JS Context\n");
        JS_FreeRuntime(rt);
        free(code);
        return 1;
    }

    // 2. Daftarkan Native C Bindings ke dalam JavaScript Global Scope
    mn_init_console(ctx);
    mn_init_process(ctx, argc - 1, argv + 1);

    // 3. Evaluasi dan Jalankan Script JavaScript
    JSValue result = JS_Eval(ctx, code, code_len, script_file, JS_EVAL_TYPE_GLOBAL);

    int exit_code = 0;
    if (JS_IsException(result)) {
        JSValue ex = JS_GetException(ctx);
        print_exception(ctx, ex);
        JS_FreeValue(ctx, ex);
        exit_code = 1;
    }

    // 4. Bersihkan memori
    JS_FreeValue(ctx, result);
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
    free(code);

    return exit_code;
}
