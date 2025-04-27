#ifndef PROTO_PRINTF
#define PROTO_PRINTF

typedef void (proto_putc_t)(const char c, void *user);
typedef unsigned int (proto_printf_t)(proto_putc_t *proto_putc, void *user, const char *fmt, ...);

#ifndef NO_PROTO_PRINTF
extern unsigned int proto_printf(proto_putc_t *proto_putc, void *user, const char *fmt, ...);
#endif

#endif // PROTO_PRINTF
