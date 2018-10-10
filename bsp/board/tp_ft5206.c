#include "basic.h"
#include "board_tp.h"

static touch_panel_t gst_tp =
{
	.uc_dev = 0X70,
};

touch_panel_t *touch_panel_get(void)
{
	return &gst_tp;
}
