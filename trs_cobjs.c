/*
 * udd trs_cobjs.c file - special treasure objects
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include <math.h>
#include <stdio.h>
#include "defs.h"

int trs_chest(void)
{
  int tmp, trs;
  double adj;
  printf("You have discovered a chest...\r\n");
 trs_c_top:
  utl_dtrp();
  printf("Press <CR> to open it, <LF> to leave it: ");
  tmp = getchar();
  printf("\r\n");
  if (tmp < 0)
    tmp = '\n';
  if (tmp == '\n') {
    u.c[UC_STATE] = DGN_PROMPT;
    return(MAYBE);
  }
  if (tmp != '\r') {
    printf("STUPID DOLT\007!\r\n");
    goto trs_c_top;
  }
  if (u.i[8] == 1 && rnd() > 0.5) {
    printf("CHEST EXPLODES\007!\r\n");
    if (cbt_ohitu(u.c[63] + 5) == YEP)
      return(YEP);
  }
  trs = 3000.0 * rnd() * u.c[63] + 500;
  printf("It hold %d in gold.\r\n", trs);
  u.c[UC_GOLDFOUND] += trs;
  adj = u.c[UC_DGNLVL] / (double) u.c[UC_LEVEL];
  if (adj > 1.0)
    adj = 1.0;
  u.c[UC_EXPGAIN] += trs * adj;
  u.c[UC_STATE] = XXX_NORM;
  return(NOPE);
}

int trs_obj(void)
{
  int rl, tmp, trs;
  do {
    rl = roll(1,8);
    switch (rl) {
    case 1:        /* wepon */
      printf("You have found a magic %s!\r\n", wep[u.c[UC_CLASS]]);
    top1:
      printf("Press <CR> to pick it up, <LF> to leave it behind: ");
      tmp = getchar();
      printf("\r\n");
      if (tmp < 0)
        tmp = 10;
      if (tmp == '\n') 
        break;
      if (tmp != '\r') {
        printf("THINK STUPID\007!\r\n");
        goto top1;
      }
      if (rnd() > 0.833) {
        printf("It's a hostile %s!\r\n", wep[u.c[UC_CLASS]]);
        if (cbt_ohitu(u.c[UC_WEAPON]) == YEP)
          return(YEP);
        break;
      }
      if (u.c[UC_WEAPON] > u.c[63]) {
        printf("You already have a %s +%d.\r\n", wep[u.c[UC_CLASS]], u.c[UC_WEAPON]);
        break;
      }
      u.c[UC_WEAPON]++;
      printf("You have found a %s%s +%d.\r\n", 
             (u.c[UC_WEAPON] > 0) ? "Magic " : "", wep[u.c[UC_CLASS]], u.c[UC_WEAPON]);
      break;
    case 2:        /* arm */
      if (u.c[UC_ARMOR] < u.c[63] + 2)
        tmp = u.c[UC_ARMOR] + 1;
      else
        tmp = u.c[UC_ARMOR];
      printf("You have found %s%s Armor +%d.\r\n", 
             (tmp > 0) ? "Magic " : "", arm[u.c[UC_CLASS]], tmp);
      if (tmp == u.c[UC_ARMOR])
        printf("Too bad you already have one.\r\n");
      else
        u.c[UC_ARMOR]++;
      break;
    case 3:        /* shield */
      if (u.c[UC_SHIELD] < u.c[63] + 2)
        tmp = u.c[UC_SHIELD] + 1;
      else
        tmp = u.c[UC_SHIELD];
      printf("You have found a %sShield +%d.\r\n",
             (tmp > 0) ? "Magic " : "", tmp);
      if (u.c[UC_CLASS] == 2) {
        printf("Too bad you can't use it!\r\n");
        break;
      }
      if (u.c[UC_SHIELD] == tmp) 
        printf("You already have one of those.\r\n");
      u.c[UC_SHIELD] = tmp;
      break;
    case 4:        /* book */
      printf("You have found a book...\r\n");
    top4:
      printf("Press <CR> to read it, <LF> to ignore it: ");
      tmp = getchar();
      printf("\r\n");
      if (tmp < 0)
        tmp = 10;
      if (tmp == '\n')
        break;
      if (tmp != '\r') {
        printf("BRAINLESS\007!\r\n");
        goto top4;
      }
      if (rnd() > 0.5) 
        utl_adj_st();
      else
        if (utl_adj_ex() == YEP)
          return(YEP);
      break;
    case 5:        /* torch */
      printf("You found a magic torch.\r\nIt starts burning.\r\n");
      sleep(2);
      if (u.c[37] < 0)
        u.c[37] = roll(3,10);
      else
        u.c[37] += roll(3,10);
      u.i[7] = 0;
      if (u.i[5] == 0)
        utl_pplot(NOPE);
      u.c[UC_STATE] = XXX_NORM;
      return(NOPE);
      break;
    case 6:        /* ring */
      trs = rnd() * rnd() * rnd() * u.c[63] + 1;
      printf("You have found a Ring of Regeneration +%d.\r\n", trs);
      if (u.c[UC_RING] >= trs) {
        printf("You already have a better one.\r\n");
        break;
      }
    top6:
      printf("Press <CR> to pick it up, <LF> to leave it behind: ");
      tmp = getchar();
      printf("\r\n");
      if (tmp < 0)
        tmp = 10;
      if (tmp == '\n')
        break;
      if (tmp == '\r') {
        u.c[UC_RING] = trs;
        break;
      }
      printf("TRY AGAIN CHOWDERHEAD\007!\r\n");
      goto top6;
    case 7:        /* cloak */
      trs = rnd() * rnd() * u.c[63] + 2;
      printf("You have found an Elven Cloak +%d.\r\n", trs);
      if (u.c[UC_ELVEN_CLOAK] >= trs) {
        printf("You already have a better one.\r\n");
        break;
      }
    top7:
      printf("Press <CR> to put it on, <LF> to leave it behind: ");
      tmp = getchar();
      printf("\r\n");
      if (tmp < 0)
        tmp = 10;
      if (tmp == '\n')
        break;
      if (tmp == '\r') {
        u.c[UC_ELVEN_CLOAK] = trs;
        break;
      }
      printf("Ever try a hearing aid?\r\n");
      goto top7;
    case 8:        /* boots */
      trs = rnd() * rnd() * u.c[63] + 2;
      printf("You have found a pair of Elven Boots +%d.\r\n", trs);
      if (u.c[UC_ELVEN_BOOTS] >= trs) {
        printf("You already have a better pair.\r\n");
        break;
      }
    top8:
      printf("Press <CR> to put them on, <LF> to leave them behind: ");
      tmp = getchar();
      printf("\r\n");
      if (tmp < 0)
        tmp = 10;
      if (tmp == '\n')
        break;
      if (tmp == '\r') {
        u.c[UC_ELVEN_BOOTS] = trs;
        break;
      }
      printf("HEY, LISTEN STUPID\007!\r\n");
      goto top8;
    default:
      printf("trs_cobjs: internal error!\r\n");
      unix_exit(1);
    }
  } while (rnd() > 0.8 - 0.02 * (u.c[UC_DGNLVL] - 1));
  u.c[UC_STATE] = XXX_NORM;
  return(NOPE);
}
