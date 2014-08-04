/*
 * udd utl_util.c file - generic util functions
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include <math.h>
#include <sys/types.h>
#include <sys/file.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "defs.h"

int roll
  (
    int a,
    int b
  )
{
  int tot = 0;
  if (b < 1)
    return(0);
  while (a-- > 0)
    tot += (random() % b) + 1;
  return(tot);
}

void utl_inilvl(void)
{
  int lcv1, lcv2;
  int ptr = 0;
  dlvl_loadum(u.c[UC_DGNNR], u.c[UC_DGNLVL]);
  for (lcv1 = 0 ; lcv1 < 22 ; lcv1++)
    for (lcv2 = 0 ; lcv2 < 22 ; lcv2++)
      u.l[lcv1][lcv2] = 0;
  for (lcv1 = 1; lcv1 <= 20; lcv1++) {
    u.l[lcv1][21] = 1;
    u.l[21][lcv1] = 4;
  }
  u.l[21][21] = 0;
  for (lcv1 = 1 ; lcv1 < 21 ; lcv1++)
    for (lcv2 = 1 ; lcv2 < 21 ; lcv2++)
      u.l[lcv1][lcv2] = dd.dmap[ptr++] + utl_populate();
}

int utl_populate(void)
{
  if (rnd() > 0.4)
    return(0);
  if (rnd() <= 0.5) {
    if (rnd() > 0.5)
      return(256);
    if (rnd() > 0.05)
      return(768);
    return(1792);
  }
  if (rnd() > 0.2)
    return(512);
  return(1536);
}


int utl_winner(void)
{
  int fd;
  if ((fd = open(FIL_ORB, O_CREAT|O_APPEND|O_RDWR, 0644)) < 0) {
    printf("\r\nCan't open ORB file %s!\r\n", FIL_ORB);
    perror("open");
    printf("\rPlease report this error.  In the meantime stay\r\n");
    printf("mortal for a little while longer.\r\n");
    return(NOPE);
  }
  write(fd, "\t", 1);
  if (write(fd, u.n[0], strlen(u.n[0])) < 0) {
    printf("Error writing to ORB file!\r\n");
    perror("write");
    printf("\rGet help...\r\n");
    close(fd);
    return(NOPE);
  }
  write(fd, "\r\n", 2);
  close(fd);
  /* try to remove him from the file */
  chr_save(NOPE);
  if (chr_load(u.n[0], NOLOCK) != NOPE)
    chr_lck_nuke(NUKE);
  return(YEP);
}


int utl_escape(void)
{
  int i;
  printf("\r\n\n\nYou made it out of the dungeon!\r\n");
  printf("\n\n\n\n\n\n");
  if (u.c[UC_HASORB] == 1) {        /* winner??? */
    printf("You have the orb with you\007!\007!\007!\007\r\n");
    printf("You have been declared a national hero\007!\007!\r\n");
    printf("The gods will now give you immortality!\r\n");
    u.c[UC_STATE] = DGN_NEWADV;
    if (u.c[UC_DEBUGCHR] != 0 || utl_winner() == YEP)
      return(YEP);
  }
  u.c[UC_CURHIT] = u.c[UC_MAXHIT];
  u.c[UC_EXP] +=  u.c[UC_EXPGAIN];
  u.c[UC_TOTALGOLD] += u.c[UC_GOLDFOUND];
  for (i = 31; i <= 36 ; i++)
    u.c[i] = u.c[i - 6];
  utl_chklvl();
  printf("\n\r\n");
  utl_status();
ask:
  printf("\r\nPress <CR> to reenter the dungeon, <LF> to leave: ");
  i = getchar();
  printf("\r\n");
  if (i < 0)
    i = 10;
  u.c[UC_DGNLVL] = u.c[UC_DGN_X] = u.c[UC_DGN_Y] = 0;
  u.c[UC_STATE] = DGN_NEWADV;
  if (i == 13)
    return(NOPE);
  if (i != 10) {
    printf("Don't you listen?\r\n");
    goto ask;
  }
  return(chr_save(YEP));
}

int utl_death(void)
{
  int i;
  while (u.c[UC_CLASS] == CHRCLASS_CLERIC && u.c[UC_SPELLS4] > 0) {    /* can he save himself? */
    printf("RAISE DEAD!!!!\r\n");
    sleep(2);
    u.c[UC_SPELLS4]--;
    u.c[UC_CONSTIT]--;
    if (u.c[UC_CONSTIT] == 0 || roll(1,10) > u.c[UC_CONSTIT])
      printf("It failed!! [%d spell%s left]\r\n", u.c[UC_SPELLS4],
             (u.c[UC_SPELLS4] == 1) ? "" : "s");
    else {
      u.c[UC_CURHIT] = roll(1,u.c[UC_MAXHIT]);
      printf("You're alive with %d hit points, and a constitution of %d.\r\n",
              u.c[UC_CURHIT], u.c[UC_CONSTIT]);
      u.c[UC_STATE] = DGN_PROMPT;
      return(NOPE);
    }
  }
  printf("\r\n\n\nAnother%s mighty %s bites the dust.\r\n",
         (u.c[UC_LEVEL] < 4) ? " not so" : "", class[u.c[UC_CLASS]]);
  printf("\n\n\n\n");
  for (i = 0 ; i < 65 ; i++)
    u.c[i] = 0;
  u.c[UC_LOCKED] = 1;
  if (chr_save(YEP) == NOPE) {
    printf("utl_death: Internal error!  This shouldn't happen,\r\n");
    printf("           but since you're dead it doesn't really matter!\r\n");
  }
  printf("Do you wish to (R)eturn to %s, (T)ry again, or (E)xit : ", ddd);
  i = getchar();
  if (islower(i))
    i = toupper(i);
  if (i == 'T') {
    printf("Try again\r\n");
    u.c[UC_STATE] = SWB_CREATE;
    return(YEP);
  }
  if (i == 'R') {
    printf("Return to %s\r\n", ddd);
    u.c[UC_STATE] = XXX_NORM;
    return(YEP);
  }
  printf("Exit\r\n");
  printf("Fare well, brave sire.\r\n");
  unix_exit(0);
}


void utl_stat(void)
{
  int i;
  printf("%s\r\n", u.n[0]);
  for (i = 0 ; i < 6 ; i++)
    printf("%.3s  %02d\r\n", st + 3*i, u.c[i+1]);
}

void utl_status(void)
{
  printf("\r\nLevel\t\t%d\r\n", u.c[UC_LEVEL]);
  printf("Experience\t%d\r\n", u.c[UC_EXP]);
  printf("Gold found\t%d\r\n", u.c[UC_GOLDFOUND]);
  printf("Hit points\t%d\r\n\n", u.c[UC_CURHIT]);
  if (u.c[UC_SPELLS1]+u.c[UC_SPELLS2]+u.c[UC_SPELLS3]+u.c[UC_SPELLS4] != 0)
    printf("Spells: %d  %d  %d  %d\r\n", u.c[UC_SPELLS1], u.c[UC_SPELLS2], u.c[UC_SPELLS3], u.c[UC_SPELLS4]);
  else
    printf("Spells: <none>\r\n");
  printf("\n");
}

void utl_eqp(void)
{
  printf("\r\nEquipment:\r\n\n");
  if (u.c[UC_WEAPON] >= 0)
    printf("%s%s +%d\n\r", (u.c[UC_WEAPON] > 0) ? "Magic " : "",
      wep[u.c[UC_CLASS]], u.c[UC_WEAPON]);
  if (u.c[UC_ARMOR] >= 0)
    printf("%s%s Armor +%d\n\r", (u.c[UC_ARMOR] > 0) ? "Magic " : "",
      arm[u.c[UC_CLASS]], u.c[UC_ARMOR]);
  if (u.c[UC_SHIELD] >= 0)
    printf("%sShield +%d\n\r", (u.c[UC_SHIELD] > 0) ? "Magic " : "", u.c[UC_SHIELD]);
  if (u.c[UC_RING] > 0)
    printf("Ring of regeneration +%d\r\n", u.c[UC_RING]);
  if (u.c[UC_ELVEN_CLOAK] > 0)
    printf("Elven cloak +%d\r\n", u.c[UC_ELVEN_CLOAK]);
  if (u.c[UC_ELVEN_BOOTS] > 0)
    printf("Elven boots +%d\r\n", u.c[UC_ELVEN_BOOTS]);
  if (u.c[UC_HASORB] == 1)
    printf("\r\n\007The ORB\r\n");
  printf("\r\n");
}

void utl_dtrp(void)
{
  if (u.c[UC_SPELL_DTRP] > 0 && u.i[8] == 1 && rnd() > 0.05)
    printf("You detect traps: ");
}

int utl_exp
  (
    int lvl
  )
  /* find exp for a level */
{
  double z, z1, z2;
  int lcv;
  if (lvl > 10) {
    z2 = lvl - 10.0;
    z2 = pow(z2, (double) 1.5);
    z = utl_exp(10) + utl_exp(10) * z2;
    return((int) z);
  }
  if (u.c[UC_CLASS] == CHRCLASS_FIGHTER)
    z = 2000.0;
      else if (u.c[UC_CLASS] == CHRCLASS_MAGICIAN)
        z = 2500.0;
          else z = 1500.0;
  for (lcv = 3 ; lcv <= lvl; lcv++)
    z *= 2.0;
  if (lvl == 1)
    z = 0.0;
  return((int) z);
}


int utl_chklvl(void)
{
  int i1;
  if (u.c[UC_EXP] < utl_exp(u.c[UC_LEVEL])) {
    printf("You went down a level\007!\r\n");
    i1 = roll(1, 4 + 2*(2 - u.c[UC_CLASS]));
    if (u.c[UC_CONSTIT] > 14)
      i1 = i1 - 14 + u.c[UC_CONSTIT];
    printf("You lost %d hit point%s.\r\n", i1, (i1 == 1) ? "" : "s");
    u.c[UC_MAXHIT] -= i1;
    u.c[UC_CURHIT] -= i1;
    u.c[UC_LEVEL] -= 1;
    if (u.c[UC_LEVEL] < 1 || u.c[UC_CURHIT] < 1) {
      printf("You died!\r\n");
      return(utl_death());
    }
    utl_sprog();
    if (chr_save(NOPE) != YEP)
      printf("[Strange, I can't check point your character!]\n\r");
    return(NOPE);
  }
  if (u.c[UC_EXP] < utl_exp(u.c[UC_LEVEL]+1))
    return(NOPE);
  printf("You went up a level\007!\r\n");
  i1 = roll(1, 4 + 2*(2 - u.c[UC_CLASS]));
  if (u.c[UC_CONSTIT] > 14)
    i1 = i1 - 14 + u.c[UC_CONSTIT];
  printf("You gain %d hit point%s.\r\n", i1, (i1 == 1) ? "" : "s");
  u.c[UC_MAXHIT] += i1;
  u.c[UC_CURHIT] += i1;
  u.c[UC_LEVEL] += 1;
  if (u.c[UC_EXP] >= utl_exp(u.c[UC_LEVEL] + 1))
    u.c[UC_EXP] = utl_exp(u.c[UC_LEVEL] + 1) - 1;
  utl_sprog();
    if (chr_save(NOPE) != YEP)
      printf("[Strange, I can't check point your character!]\n\r");
  return(NOPE);
}

void utl_sprog(void)
/* increases the power of spells available to a non-fighter character, based on their level advancement. */
{
  int lcv, tmp;
  if (u.c[UC_CLASS] == CHRCLASS_FIGHTER)
    return;
  for (lcv = 1 ; lcv < 5 ; lcv++) {
    if (u.c[UC_CLASS] == CHRCLASS_CLERIC)
      tmp = u.c[UC_LEVEL] - (lcv + 1.0) / 0.75 + 1;
    else /* CHRCLASS_MAGICIAN */
      tmp = u.c[UC_LEVEL] - lcv / 0.8 + 1;
    if (tmp < 0)
      tmp = 0;
    if (lcv == 1)
      tmp += 2;
    u.c[UC_SPELLS1 - 1 + lcv] = u.c[UC_SPELLS1 - 1 + lcv] + tmp - u.c[UC_SPELLSADJ1 - 1 + lcv];
    u.c[UC_SPELLSADJ1 - 1 + lcv] = tmp;
  }
}

void utl_prtspl
  (
    int c7,
    int lvl
  )
{
  int i = 0;
  int mx = 6;
  int lcv;
  printf("\r\n");
  if (c7 == 1)
    mx = i = 4;
  i += lvl;
  for (lcv = 1 ; lcv <= mx ; lcv++)
    printf("%d. %s\r\n", lcv, sp[i][lcv]);
}

void utl_adj_st(void)
{
  int i1, i2, i3;
  i1 = roll(1,2);
  i2 = roll(1,6);
  i3 = 1;
  while (rnd() + u.c[UC_DGNLVL] * 0.02 > 0.9)
    i3++;
  if (i1 == 1)
    while (u.c[i2] - i3 < 1)
      i3--;
  printf("Your %.3s just went %s by %d.\r\n", st + ((i2 - 1) * 3),
         (i1 == 1) ? "down" : "up", i3);
  if (i1 == 1)
    i3 = -i3;
  u.c[i2] += i3;
  printf("It is now %d.\r\n", u.c[i2]);
  if (chr_save(NOPE) != YEP)
    printf("[Strange, I can't check point your character!]\n\r");
}

int utl_adj_ex(void)
{
  int i1, i2;
  i1 = roll(1,2);
  i2 = (rnd() * 500 + 2) * u.c[UC_DGNLVL];
  printf("You just %s %d experience points.\r\n",
         (i1 == 1) ? "lost" : "gained", i2);
  if (i1 == 1)
    i2 = -i2;
  u.c[UC_EXP] += i2;
  if (chr_save(NOPE) != YEP)
    printf("[Strange, I can't check point your character!]\n\r");
  return(utl_chklvl());
}
