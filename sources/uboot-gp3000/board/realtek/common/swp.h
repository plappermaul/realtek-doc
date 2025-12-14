typedef void (void_func)(void);
#define PATCH_REG(x, lvl) \
        void_func * __swp_##x __attribute__ ((section (".soft_patch." #lvl))) = x

