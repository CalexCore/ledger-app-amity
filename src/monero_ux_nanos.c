/* Copyright 2017 Cedric Mesnil <cslashm@gmail.com>, Ledger SAS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



#include "os.h"
#include "cx.h"
#include "monero_types.h"
#include "monero_api.h"
#include "monero_vars.h"

#include "monero_ux_msg.h"
#include "os_io_seproxyhal.h"
#include "string.h"
#include "glyphs.h"

#include "monero_ux_nanos.h"


/* ----------------------------------------------------------------------- */
/* ---                        NanoS  UI layout                         --- */
/* ----------------------------------------------------------------------- */

const ux_menu_entry_t ui_menu_main[];
void ui_menu_main_display(unsigned int value) ;
const bagl_element_t* ui_menu_main_preprocessor(const ux_menu_entry_t* entry, bagl_element_t* element);

/* ------------------------------- Helpers  UX ------------------------------- */
/*
void ui_info(const char* msg1, const char* msg2, const void *menu_display, unsigned int value) {
  os_memset(&G_monero_vstate.ui_dogsays[0], 0, sizeof(ux_menu_entry_t));
  G_monero_vstate.ui_dogsays[0].callback = menu_display;
  G_monero_vstate.ui_dogsays[0].userid   = value;
  G_monero_vstate.ui_dogsays[0].line1    = msg1;
  G_monero_vstate.ui_dogsays[0].line2    = msg2;

  os_memset(&G_monero_vstate.ui_dogsays[1],0, sizeof(ux_menu_entry_t));
  UX_MENU_DISPLAY(0, G_monero_vstate.ui_dogsays, NULL);
};
*/
/* ----------------------------- FEE VALIDATION ----------------------------- */
#define ACCEPT  0xACCE
#define REJECT  ~ACCEPT

void ui_menu_fee_validation_action(unsigned int value);

const ux_menu_entry_t ui_menu_fee_validation[] = {
  {NULL,  NULL,                           1,      NULL, " Fee",     "xnv?",  0, 0},
  {NULL,  ui_menu_fee_validation_action,  REJECT, NULL,  "Reject",  "Fee",     0, 0},
  {NULL,  ui_menu_fee_validation_action,  ACCEPT, NULL,  "Accept",  "Fee",     0, 0},
  UX_MENU_END
};

const bagl_element_t* ui_menu_fee_validation_preprocessor(const ux_menu_entry_t* entry, bagl_element_t* element) {

  /* --- Amount --- */
  if (entry == &ui_menu_fee_validation[0]) {
    if(element->component.userid==0x22) {      
      element->text = G_monero_vstate.ux_amount;
    }
  }
  return element;
}

void ui_menu_fee_validation_action(unsigned int value) {
  unsigned short sw;
  if (value == ACCEPT) {
    sw = 0x9000;
  } else {
   sw = SW_SECURITY_STATUS_NOT_SATISFIED;
    monero_abort_tx();
  }
  monero_io_insert_u16(sw);  
  monero_io_do(IO_RETURN_AFTER_TX);
  ui_menu_main_display(0);
}

void ui_menu_fee_validation_display(unsigned int value) {
  UX_MENU_DISPLAY(0, ui_menu_fee_validation, ui_menu_fee_validation_preprocessor);
}


/* -------------------------------------- 25 WORDS --------------------------------------- */
void ui_menu_words_clear(unsigned int value);
void ui_menu_words_back(unsigned int value);
#define WORDS N_monero_pstate->words
const ux_menu_entry_t ui_menu_words[] = {
  {NULL,  ui_menu_words_back,                  0,      NULL,  "",  "",    0, 0},
  {NULL,  ui_menu_words_back,                  2,      NULL,  "",  "",    0, 0},
  {NULL,  ui_menu_words_back,                  4,      NULL,  "",  "",    0, 0},
  {NULL,  ui_menu_words_back,                  6,      NULL,  "",  "",    0, 0},
  {NULL,  ui_menu_words_back,                  8,      NULL,  "",  "",    0, 0},
  {NULL,  ui_menu_words_back,                  10,     NULL,  "",  "",    0, 0},
  {NULL,  ui_menu_words_back,                  12,     NULL,  "",  "",    0, 0},
  {NULL,  ui_menu_words_back,                  14,     NULL,  "",  "",    0, 0},
  {NULL,  ui_menu_words_back,                  16,     NULL,  "",  "",    0, 0},
  {NULL,  ui_menu_words_back,                  18,     NULL,  "",  "",    0, 0},
  {NULL,  ui_menu_words_back,                  20,     NULL,  "",  "",    0, 0},
  {NULL,  ui_menu_words_back,                  22,     NULL,  "",  "",    0, 0},
  {NULL,  ui_menu_words_back,                  24,     NULL,  "",  "",    0, 0},
  {NULL,  ui_menu_words_clear,                 -1,     NULL,  "CLEAR",  "SEED",    0, 0},
  UX_MENU_END
};

const bagl_element_t* ui_menu_words_preprocessor(const ux_menu_entry_t* entry, bagl_element_t* element) {
  if ((entry->userid >= 0) && (entry->userid <25)) {
  
    if(element->component.userid==0x21) {      
      element->text = N_monero_pstate->words[entry->userid];
    }
  
    if ((element->component.userid==0x22)&&(entry->userid<24)) {      
      element->text = N_monero_pstate->words[entry->userid+1];
    }
  }

  return element;
}

void ui_menu_words_display(unsigned int value) {
  UX_MENU_DISPLAY(0, ui_menu_words, ui_menu_words_preprocessor);
}
void ui_menu_words_clear(unsigned int value) {
  monero_clear_words();
  ui_menu_main_display(0);
}
void ui_menu_words_back(unsigned int value) {
  ui_menu_main_display(0);
}
/* ----------------------------- USER DEST/AMOUNT VALIDATION ----------------------------- */
void ui_menu_validation_action(unsigned int value);

const ux_menu_entry_t ui_menu_validation[] = {
  {NULL,  NULL,                       1,      NULL, " Amount",       "?xnv?",      0, 0},
  {NULL,  NULL,                       3,      NULL,  "Destination",  "?dest.1?",   0, 0},
  {NULL,  NULL,                       4,      NULL,  "?dest.2?",     "?dest.2?",   0, 0},
  {NULL,  NULL,                       5,      NULL,  "?dest.3?",     "?dest.3?",   0, 0},
  {NULL,  NULL,                       6,      NULL,  "?dest.4?",     "?dest.4?",   0, 0},
  {NULL,  NULL,                       7,      NULL,  "?dest.5?",     "?dest.5?",   0, 0},
  {NULL,  ui_menu_validation_action,  REJECT, NULL,  "Reject",       "TX",         0, 0},
  {NULL,  ui_menu_validation_action,  ACCEPT, NULL,  "Accept",       "TX",         0, 0},
  UX_MENU_END
};

const bagl_element_t* ui_menu_validation_preprocessor(const ux_menu_entry_t* entry, bagl_element_t* element) {

  /* --- Amount --- */
  if (entry == &ui_menu_validation[0]) {
    if(element->component.userid==0x22) {      
      element->text = G_monero_vstate.ux_amount;
    }
  }
  #if 0
  /* --- Fees --- */
  if (entry == &ui_menu_validation[1]) {
    if(element->component.userid==0x22) {      
      element->text = G_monero_vstate.ux_fees;
    }
  }
#endif

   /* --- Destination --- */
  if (entry == &ui_menu_validation[1]) {
    if(element->component.userid==0x22) {     
      os_memset(G_monero_vstate.ux_menu, 0, sizeof(G_monero_vstate.ux_menu)) ;
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*0, 11);
      element->text = G_monero_vstate.ux_menu;
    }
  }
  if (entry == &ui_menu_validation[2]) {
    os_memset(G_monero_vstate.ux_menu, 0, sizeof(G_monero_vstate.ux_menu)) ;
    if(element->component.userid==0x21) {     
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*1, 11);      
    }
    if(element->component.userid==0x22) {     
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*2, 11);
    }
    element->text = G_monero_vstate.ux_menu;
  }
  if (entry == &ui_menu_validation[3]) {
    os_memset(G_monero_vstate.ux_menu, 0, sizeof(G_monero_vstate.ux_menu)) ;
    if(element->component.userid==0x21) {     
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*3, 11);      
    }
    if(element->component.userid==0x22) {     
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*4, 11);
    }
    element->text = G_monero_vstate.ux_menu;
  }
  if (entry == &ui_menu_validation[4]) {
    os_memset(G_monero_vstate.ux_menu, 0, sizeof(G_monero_vstate.ux_menu)) ;
    if(element->component.userid==0x21) {     
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*5, 11);      
    }
    if(element->component.userid==0x22) {     
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*6, 11);
    }
    element->text = G_monero_vstate.ux_menu;
  }
  if (entry == &ui_menu_validation[5]) {
    os_memset(G_monero_vstate.ux_menu, 0, sizeof(G_monero_vstate.ux_menu)) ;
    if(element->component.userid==0x21) {     
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*7, 11);      
    }
    if(element->component.userid==0x22) {     
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*8, 9);
    }
    element->text = G_monero_vstate.ux_menu;
  }

  return element;
}

void ui_menu_validation_display(unsigned int value) {
  UX_MENU_DISPLAY(0, ui_menu_validation, ui_menu_validation_preprocessor);
}

void ui_menu_validation_action(unsigned int value) {
  unsigned short sw;
  if (value == ACCEPT) {
    sw = 0x9000;
  } else {
   sw = SW_SECURITY_STATUS_NOT_SATISFIED;
    monero_abort_tx();
  }
  monero_io_insert_u16(sw);  
  monero_io_do(IO_RETURN_AFTER_TX);
  ui_menu_main_display(0);
}



/* -------------------------------- EXPORT VIEW KEY UX --------------------------------- */
unsigned int ui_export_viewkey_prepro(const  bagl_element_t* element);

const bagl_element_t ui_export_viewkey[] = {
  // type             userid    x    y    w    h    str   rad  fill              fg        bg     font_id                   icon_id  
  { {BAGL_RECTANGLE,  0x00,     0,   0, 128,  32,    0,    0,  BAGL_FILL,  0x000000, 0xFFFFFF,    0,                         0}, 
    NULL, 
    0, 
    0, 0, 
    NULL, NULL, NULL},

  { {BAGL_ICON,       0x00,    3,   12,   7,   7,    0,    0,         0,   0xFFFFFF, 0x000000,    0,                          BAGL_GLYPH_ICON_CROSS  }, 
    NULL, 
    0, 
    0, 0, 
    NULL, NULL, NULL },
    
  { {BAGL_ICON,       0x00,  117,   13,   8,   6,    0,    0,         0,   0xFFFFFF, 0x000000,    0,                          BAGL_GLYPH_ICON_CHECK  }, 
     NULL, 
     0, 
     0, 0, 
     NULL, NULL, NULL },

  { {BAGL_LABELINE,   0x01,    0,   12, 128,  32,    0,    0,         0,   0xFFFFFF, 0x000000,    BAGL_FONT_OPEN_SANS_EXTRABOLD_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, 
    G_monero_vstate.ux_menu,  
    0, 
    0, 0, 
    NULL, NULL, NULL },
    
  { {BAGL_LABELINE,   0x02,    0,   26, 128,  32,    0,    0,         0,   0xFFFFFF, 0x000000,    BAGL_FONT_OPEN_SANS_EXTRABOLD_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, 
    G_monero_vstate.ux_menu,  
    0, 
    0, 0, 
    NULL, NULL, NULL },
    
};

void ui_export_viewkey_display() {
 UX_DISPLAY(ui_export_viewkey, (void*)ui_export_viewkey_prepro);   
}

unsigned int ui_export_viewkey_prepro(const  bagl_element_t* element) {
  if (element->component.userid == 1) {
    snprintf(G_monero_vstate.ux_menu, sizeof(G_monero_vstate.ux_menu), "Export");
    return 1;
  }
  if (element->component.userid == 2) {
    snprintf(G_monero_vstate.ux_menu, sizeof(G_monero_vstate.ux_menu), "View Key");
    return 1;
  }
  snprintf(G_monero_vstate.ux_menu, sizeof(G_monero_vstate.ux_menu), "Please Cancel");
  return 1;
}
 
unsigned int ui_export_viewkey_button(unsigned int button_mask, unsigned int button_mask_counter) {
  unsigned int sw;
  unsigned char x[32];

  monero_io_discard(0);
  os_memset(x,0,32);
  sw = 0x9000;
  
  switch(button_mask) {
  case BUTTON_EVT_RELEASED|BUTTON_LEFT: // CANCEL
    monero_io_insert(x, 32);      
    break;

  case BUTTON_EVT_RELEASED|BUTTON_RIGHT:  // OK
    monero_io_insert(G_monero_vstate.a, 32);
#ifdef DEBUG_HWDEVICE
    monero_io_insert(G_monero_vstate.b, 32);    
#endif
    break;
  
  default:
    return 0;
  }
  monero_io_insert_u16(sw);
  monero_io_do(IO_RETURN_AFTER_TX);
  ui_menu_main_display(0);
  return 0;
}

/* --------------------------------- INFO UX --------------------------------- */


#define STR(x)  #x
#define XSTR(x) STR(x)

const ux_menu_entry_t ui_menu_info[] = {
  {NULL,  NULL,                 -1, NULL,          "Nerva",                   NULL, 0, 0},
  {NULL,  NULL,                 -1, NULL,          "Min Version",  NULL, 0, 0},
  {NULL,  NULL,                 -1, NULL,          "0."XSTR(MONERO_VERSION),  NULL, 0, 0},
  {NULL,  ui_menu_main_display,  3, &C_badge_back, "Back",                     NULL, 61, 40},
  UX_MENU_END
};

#undef STR
#undef XSTR

/* ---------------------------- PUBLIC ADDRESS UX ---------------------------- */

void ui_menu_pubaddr_action(unsigned int value);

const ux_menu_entry_t ui_menu_pubaddr[] = {
  {NULL,  NULL,                  3,          NULL,  "XNV",  "?addr.1?",   0, 0},
  {NULL,  NULL,                  4,          NULL,  "?addr.2?",     "?addr.2?",   0, 0},
  {NULL,  NULL,                  5,          NULL,  "?addr.3?",     "?addr.3?",   0, 0},
  {NULL,  NULL,                  6,          NULL,  "?addr.4?",     "?addr.4?",   0, 0},
  {NULL,  NULL,                  7,          NULL,  "?addr.5?",     "?addr.5?",   0, 0},
  {NULL,  ui_menu_main_display,  0, &C_badge_back, "Back",                     NULL, 61, 40},
  UX_MENU_END
};

const bagl_element_t* ui_menu_pubaddr_preprocessor(const ux_menu_entry_t* entry, bagl_element_t* element) {

   /* --- address --- */
  if (entry == &ui_menu_pubaddr[0]) {
    if(element->component.userid==0x22) {
      os_memset(G_monero_vstate.ux_menu, 0, sizeof(G_monero_vstate.ux_menu)) ;
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*0, 11);
      element->text = G_monero_vstate.ux_menu;
    }
  }
  if (entry == &ui_menu_pubaddr[1]) {
    os_memset(G_monero_vstate.ux_menu, 0, sizeof(G_monero_vstate.ux_menu)) ;
    if(element->component.userid==0x21) {
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*1, 11);
    }
    if(element->component.userid==0x22) {
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*2, 11);
    }
    element->text = G_monero_vstate.ux_menu;
  }
  if (entry == &ui_menu_pubaddr[2]) {
    os_memset(G_monero_vstate.ux_menu, 0, sizeof(G_monero_vstate.ux_menu)) ;
    if(element->component.userid==0x21) {
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*3, 11);
    }
    if(element->component.userid==0x22) {
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*4, 11);
    }
    element->text = G_monero_vstate.ux_menu;
  }
  if (entry == &ui_menu_pubaddr[3]) {
    os_memset(G_monero_vstate.ux_menu, 0, sizeof(G_monero_vstate.ux_menu)) ;
    if(element->component.userid==0x21) {
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*5, 11);
    }
    if(element->component.userid==0x22) {
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*6, 11);
    }
    element->text = G_monero_vstate.ux_menu;
  }
  if (entry == &ui_menu_pubaddr[4]) {
    os_memset(G_monero_vstate.ux_menu, 0, sizeof(G_monero_vstate.ux_menu)) ;
    if(element->component.userid==0x21) {
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*7, 11);
    }
    if(element->component.userid==0x22) {
      os_memmove(G_monero_vstate.ux_menu, G_monero_vstate.ux_address+11*8, 9);
    }
    element->text = G_monero_vstate.ux_menu;
  }

  return element;
}

void ui_menu_pubaddr_display(unsigned int value) {
   monero_base58_public_key(G_monero_vstate.ux_address, G_monero_vstate.A,G_monero_vstate.B, 0);
   UX_MENU_DISPLAY(value, ui_menu_pubaddr, ui_menu_pubaddr_preprocessor);
}

/* --------------------------------- MAIN UX --------------------------------- */

const ux_menu_entry_t ui_menu_main[] = {
  {NULL,      	 ui_menu_pubaddr_display,  0, NULL,              "NERVA Wallet", NULL,  0, 0},
  {NULL,      	 ui_menu_words_display,  2, NULL,              "Show Seed",    NULL,  0, 0},
  {ui_menu_info, NULL,  0, NULL,              "About",       NULL, 0, 0},
  {NULL,         os_sched_exit,  4, &C_icon_dashboard, "Quit app" ,    NULL, 50, 29},
  UX_MENU_END
};

const bagl_element_t* ui_menu_main_preprocessor(const ux_menu_entry_t* entry, bagl_element_t* element) {
  if (entry == &ui_menu_main[0]) {
    if(element->component.userid==0x22) {
      os_memset(G_monero_vstate.ux_menu, 0, sizeof(G_monero_vstate.ux_menu));
      monero_base58_public_key(G_monero_vstate.ux_menu, G_monero_vstate.A,G_monero_vstate.B, 0);
      os_memset(G_monero_vstate.ux_menu+5,'.',2);
      os_memmove(G_monero_vstate.ux_menu+7, G_monero_vstate.ux_menu+97-5,5);
      G_monero_vstate.ux_menu[12] = 0;
      element->text = G_monero_vstate.ux_menu;
    }
  }
  return element;
}

void ui_menu_main_display(unsigned int value) {
   UX_MENU_DISPLAY(value, ui_menu_main, ui_menu_main_preprocessor);
}

void ui_init(void) {
 ui_menu_main_display(0);
 // setup the first screen changing
  UX_CALLBACK_SET_INTERVAL(1000);
}

void io_seproxyhal_display(const bagl_element_t *element) {
  io_seproxyhal_display_default((bagl_element_t *)element);
}
