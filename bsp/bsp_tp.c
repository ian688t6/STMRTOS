#include "basic.h"
#include "board.h"
#include "rtos.h"
#include "board_tp.h"
#include "bsp_iic.h"
#include "bsp_tp.h"

void bsp_tp_init(void)
{
	board_t *pst_board = board_get();
	touch_panel_t *pst_tp = pst_board->pf_touch_panel_get();
	bsp_iic_t *pst_iic = bsp_iic_register(pst_tp->uc_dev);
	
	return;
}
