#include <common.h>
#include <post.h>

#if CONFIG_POST & CONFIG_SYS_POST_BSPEC1
extern void run_ledtest(void);
int led_post_test(int flags)
{
	run_ledtest();
	return 0;
}
#endif

