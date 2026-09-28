#ifndef MN_BINDINGS_CONSOLE_H
#define MN_BINDINGS_CONSOLE_H

#include <quickjs/quickjs.h>

/* Inisialisasi global `console` object dengan method log, error, warn */
void mn_init_console(JSContext *ctx);

#endif /* MN_BINDINGS_CONSOLE_H */
