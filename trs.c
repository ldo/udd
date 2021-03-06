/*
 * udd trs.c file - treasure program
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "defs.h"

#define TRS_NORM  0
#define TRS_CHEST 1
#define TRS_OBJ   2

int trs_main(void)
{
  const int value = u.c[UC_VALUE];
  int tmp, typ = TRS_NORM;
  int trs;
  double adj;
  if (debug_mode())
    printf("trs: l = %d\r\n", value);
  tmp = roll(1, 100);
  if (tmp > 80 && tmp <= 90)
    typ = TRS_CHEST;
  else if (tmp > 90)
    typ = TRS_OBJ;
  switch (typ) {
  case TRS_CHEST:
    if ((trs = trs_chest()) == YEP)
      return(YEP);
    if (trs == MAYBE)
      break;
    /* fall through */
  case TRS_OBJ:
    if (trs_obj() == YEP)
      return(YEP);
    break;
  case TRS_NORM:
    if (tmp <= 30) {
      printf("You see a pile of silver...\r\n");
      trs = 100 * rnd() * value + 10;
    } else
      if (tmp <= 40) {
        printf("You see a pile of gold...\r\n");
        trs = 500 * rnd() * value + 50;
      } else
        if (tmp <= 60) {
          printf("You see a pile of platinum...\r\n");
          trs = 1000 * rnd() * value + 100;
        } else
          if (tmp <= 72) {
            printf("You see some gems...\r\n");
            trs = 500.0 * sqrt((double)rnd()) * value + 150.0;
          } else {
            printf("You see a jewel...\r\n");
            trs = 6000.0 * rnd() * rnd() * rnd() * value + 500;
          }
trs_top:
    utl_dtrp();
    printf("Press <CR> to pick it up, <LF> to leave it behind: ");
    tmp = getchar();
    printf("\r\n");
    if (tmp < 0)
      tmp = '\n';
    if (tmp == '\n') {
      u.c[UC_STATE] = XXX_NORM;
      break;
    }
    if (tmp != '\r') {
      printf("DUMMY!\007\r\n");
      goto trs_top;
    }
    if (u.i[ROOM_TREASURE_BOOBYTRAPPED] == 1)
      if (cbt_ohitu((int) (u.c[UC_DGNLVL] / 1.2)) == YEP)
        return(YEP);
    printf("The treasure is worth %d gold.\r\n", trs);
    u.c[UC_GOLDFOUND] += trs;
    u.c[UC_STATE] = XXX_NORM;
    adj = u.c[UC_DGNLVL] / (double) u.c[UC_LEVEL];
    if (adj > 1.0)
      adj = 1.0;
    u.c[UC_EXPGAIN] += trs * adj;
    break;
  default:
    printf("trs: internal error\r\n");
    unix_exit(1);
  }
  u.i[ROOM_MONSTER] = u.i[ROOM_TREASURE] = u.i[ROOM_TREASURE_BOOBYTRAPPED] = 0;
  save_room(false);
  if (u.i[ROOM_SPECIAL] == 0) {
    u.c[UC_STATE] = DGN_PROMPT;
    return(NOPE);
  }
  utl_pplot(NOPE);
  return(spc_main());
}
