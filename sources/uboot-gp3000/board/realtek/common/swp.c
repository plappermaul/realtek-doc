#include <common.h>
#include <swp.h>

/* software patch */
typedef void (sw_patch_t) (void);
/* Defined in preloader.lds, points to SW-patch section. */

extern sw_patch_t *LS_bootm_stack;
extern sw_patch_t *LS_sw_patch_start[];

int board_late_init(void)
{
   printf("II: Init Software Patch!\n");
   sw_patch_t **sw_patch = LS_sw_patch_start;
   while (sw_patch!=&LS_bootm_stack) {
       (*sw_patch)();
       ++sw_patch;
   }

   return 0;
}

void sw_test(void)
{
   printf("II: %s\r", __FUNCTION__);
}

PATCH_REG(sw_test, 0);
