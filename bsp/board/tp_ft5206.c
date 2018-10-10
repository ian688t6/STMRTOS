#include "basic.h"
#include "board_tp.h"

static touch_panel_t gst_tp;

touch_panel_t *touch_panel_get(void)
{
	return &gst_tp;
}
