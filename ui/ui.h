#ifndef __UI_H__
#define __UI_H__

#include "GUI.h"

#if GUI_WINSUPPORT
  #include "WM.h"

  #include "CHECKBOX.h"
  #include "FRAMEWIN.h"
  #include "PROGBAR.h"
  #include "TEXT.h"
  #include "BUTTON.h"
  #include "SLIDER.h"
  #include "HEADER.h"
  #include "GRAPH.h"
  #include "ICONVIEW.h"
  #include "LISTVIEW.h"
  #include "TREEVIEW.h"
#endif

#define UI_MTOUCH_FLAG_DOWN			(0x01 << 5)
#define UI_MTOUCH_FLAG_MOVE			(0x02 << 5)
#define UI_MTOUCH_FLAG_RELEASE		(0x00 << 5)
#define UI_MTOUCH_FLAG_MASK			(0x03 << 5)

typedef struct
{
	uint8_t		uc_tcont;
	uint8_t		uc_tstat;
	uint16_t	aus_x[TOUCH_PANEL_MAX_TCONT];
	uint16_t	aus_y[TOUCH_PANEL_MAX_TCONT];
} ui_mtouch_sta_t;

extern void ui_init(void);

extern void ui_mtouch_enable(void);

#endif
