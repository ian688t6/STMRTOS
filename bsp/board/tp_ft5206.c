#include "basic.h"
#include "board_tp.h"
#include "bsp_iic.h"

static uint8_t tp_init(void)
{
	return 0;
}

static uint8_t tp_scan(void)
{
	return 0;
}

static touch_panel_t gst_tp =
{
	.uc_dev = 0X70,
	.init = tp_init,
	.scan = tp_scan,
};

touch_panel_t *touch_panel_get(void)
{
	return &gst_tp;
}


