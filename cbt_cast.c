/*
 * udd cbt_cast.c file - spells
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include "defs.h"
#include "cbt.h"
#include <stdio.h>
#include <strings.h>
#include <math.h>
#include <ctype.h>

#define cbt_chk() if (cbt_ok() == NOPE) break

int cbt_ok(void)
{
  if (hflag) {
    printf("Too bad you just wasted a combat spell!\r\n");
    return(NOPE);
  }
  return(YEP);
}

int ms1
  (
    int s
  )
  /* MAGE lvl 1 */
{
  int tmp;
  switch (s) {
  case 1:          /* magic missile */
    cbt_chk();
    tmp = roll(1,6);
    cbt_uhitm(tmp);
    break;
  case 2:          /* charm */
    cbt_chk();
    if (m < 7) {
      printf("You can't charm undead.\r\n");
      break;
    }
    if (m == 15) {
      printf("%s is magic resistant\007!\r\n", mnam);
      break;
    }
    if (2*(roll(3,6) - u.c[UC_LEVEL] + m1) > u.c[UC_INTEL] + u.c[UC_CHARISMA]) {
      printf("Your charm failed!\r\n");
      break;
    }
    printf("The %s is charmed.  Press <CR> to kill, or <LF> to evade: ",
           mnam);
    tmp = getchar();
    printf("\n\r");
    if (tmp == '\n') {
      autoevade = mskip = true;
      break;
    }
    if (roll(3,6) - u.c[UC_LEVEL] + m1 <= u.c[UC_CHARISMA]) {
      printf("It dies...\r\n");
      dead = 1;
    } else
      printf("It broke your charm!\r\n");
    break;
  case 3:          /* shield */
    if (u.c[UC_SPELL_SHLD] < 0)
      u.c[UC_SPELL_SHLD] = roll(1,10);
    else
      u.c[UC_SPELL_SHLD] += roll(1,10);
    break;
  case 4:          /* sleep */
    cbt_chk();
    if (m < 7) {
      printf("Undead never sleep.\r\n");
      break;
    }
    if (m == 15) {
      printf("%s is magic resistant\007!\r\n", mnam);
      break;
    }
    if (roll(3,6) - u.c[UC_LEVEL] + m1 > u.c[UC_INTEL]) {
      printf("It wont sleep!\r\n");
      break;
    }
    printf("The %s is sleeping.  Press <CR> to kill, or <LF> to evade: ",
           mnam);
    tmp = getchar();
    printf("\n\r");
    if (tmp == '\n') {
      autoevade = mskip = true;
      break;
    }
    if (roll(3,6) - u.c[UC_LEVEL] + m1 < u.c[UC_INTEL]) {
      printf("It dies...\r\n");
      dead = 1;
    } else
      printf("It woke up!\r\n");
    break;
  case 5:          /* prot/evil */
    if (u.c[UC_SPELL_PROT] < 0)
      u.c[UC_SPELL_PROT] = roll(1,10);
    else
      u.c[UC_SPELL_PROT] += roll(1,10);
    break;
  case 6:          /* light */
    if (u.c[UC_SPELL_LIGHT] < 0)
      u.c[UC_SPELL_LIGHT] = roll(1,10);
    else
      u.c[UC_SPELL_LIGHT] += roll(1,10);
    break;
  default:
    printf("ms1: unknown spell?\r\n");
    unix_exit(1);
  }
  return(NOPE);
}

int ms2
  (
    int s
  )
{
  int tmp;
  switch(s) {
  case 1:          /* phant force */
    cbt_chk();
    if (roll(3,6) + m1 - u.c[UC_LEVEL] + 1 < u.c[UC_INTEL]) {
      printf("The %s believed...!\r\n", mnam);
      dead = 1;
    } else {
      printf("The %s does not believe you.\r\n", mnam);
    }
    break;
  case 2:          /* web */
    cbt_chk();
    if (roll(3,6) * 2 > u.c[UC_INTEL] + u.c[UC_DEX]) {
      printf("You missed the %s!\r\n", mnam);
      break;
    }
    printf("The %s is webbed.  Press <CR> to kill, <LF> to evade: ", mnam);
    tmp = getchar();
    printf("\r\n");
    if (tmp == '\n')
      autoevade = mskip = true;
    else {
      dead = 1;
      printf("It died...\r\n");
    }
    break;
  case 3:          /* l. bolt */
    cbt_chk();
    tmp = roll(u.c[UC_LEVEL], 6);
    if (roll(1,10) - m1 + u.c[UC_LEVEL]  < 6)
      tmp = (int) (tmp / 2.0 + 0.5);
    printf("ZZAAAPPPP!!!!!!!\007\r\n");
    cbt_uhitm(tmp);
    break;
  case 4:          /* str. */
    if (u.c[UC_SPELL_STRG] < 0)
      u.c[UC_SPELL_STRG] = roll(1, 10);
    else
      u.c[UC_SPELL_STRG] += roll(1, 10);
    break;
  case 5:          /* lev. */
    if (u.c[UC_SPELL_LEVT] < 0)
      u.c[UC_SPELL_LEVT] = roll(1, 10);
    else
      u.c[UC_SPELL_LEVT] += roll(1, 10);
    break;
  case 6:          /* invis. */
    if (u.c[UC_SPELL_INVS] < 0)
      u.c[UC_SPELL_INVS] = roll(1, 10);
    else
      u.c[UC_SPELL_INVS] += roll(1, 10);
    break;
  default:
    printf("ms2: unknown spell.\r\n");
    unix_exit(1);
  }
  return(NOPE);
}

int ms3
  (
    int s
  )
{
  int tmp, x1, y1;
  char *passcmd = "1234WDXA", *pc;
  switch (s) {
  case 1:          /* fireball */
    cbt_chk();
    tmp = roll(u.c[UC_LEVEL], 10);
    if (roll(1, 10) - m1 + u.c[UC_LEVEL] < 6)
      tmp = (int) (tmp / 2.0 + 0.5);
    printf("The %s is burning...\r\n", mnam);
    cbt_uhitm(tmp);
    break;
  case 2:          /* confuse */
    cbt_chk();
    if (m == 15) {
      printf("%s is magic resistant!\r\n", mnam);
      break;
    }
    if (roll(3,6) > u.c[UC_INTEL]) {
      printf("The spell failed...\r\n");
      break;
    }
    tmp = mm[m].m * rnd() + m1;
    printf("It gave itself %d damage point%s.\r\n", tmp, (tmp == 1) ? "" :"s");
    m2 -= tmp;
    if (m2 < 1) {
      printf("It killed itself!\r\n");
      if (rnd() > 0.1)
        u.i[8] = 0;
      u.i[6] = 0;
      dead = 1;
    }
    break;
  case 3:          /* pass wall */
    if (!hflag) {
      printf("Not in the middle of a battle dolt!\r\n");
      break;
    }
  pass_top:
    printf("Direction ->");
    tmp = getchar();
    if (islower(tmp))
      tmp = toupper(tmp);
    pc = index(passcmd, tmp);
    if (pc == NULL) {
      printf("Surely you jest Sire...\r\n");
      goto pass_top;
    }
    tmp = pc - passcmd + 1;
    if (tmp > 4)
      tmp -= 4;
    x1 = u.c[UC_DGN_X] + q[tmp][1];
    y1 = u.c[UC_DGN_Y] + q[tmp][2];
    if (x1 < 1 || x1 > 20 || y1 < 1 || y1 > 20 ||
        (u.l[x1][y1] & 240) == 240) {
      printf("Only stone there...\r\n");
      break;
    }
    printf("*POOF*\b\b\b\b\b\b++++++\b\b\b\b\b\b******\n\r");
    u.c[UC_STATE] = DGN_AMOVE;
    u.c[63] = tmp;
    break;
  case 4:          /* hold monst */
    cbt_chk();
    if (m == 15) {
      printf("%s is magic resistant!\007\r\n", mnam);
      break;
    }
    if (roll(3,6) -u.c[UC_LEVEL] + m1 > u.c[UC_CHARISMA])
      printf("The %s breaks free...\r\n", mnam);
    else {
      printf("The %s is held.  Press <CR> to kill, <LF> to evade: ", mnam);
      tmp = getchar();
      printf("\r\n");
      if (tmp == '\n')
        autoevade = mskip = true;
      else {
        dead = 1;
        printf("It died...\r\n");
      }
    }
    break;
  case 5:          /* fear */
    if (u.c[UC_SPELL_FEAR] < 0)
      u.c[UC_SPELL_FEAR] = roll(3,20);
    else
      u.c[UC_SPELL_FEAR] += roll(3,20);
    break;
  case 6:          /* cont. light */
    if (u.c[UC_SPELL_LIGHT] < 0)
      u.c[UC_SPELL_LIGHT] = roll(3,20);
    else
      u.c[UC_SPELL_LIGHT] += roll(3,20);
    break;
  default:
    printf("ms3: unknown spell?\r\n");
    unix_exit(1);
  }
  return(NOPE);
}


int ms4
  (
    int s
  )
{
  int tmp;
  switch(s) {
  case 1:          /* tpt */
    if (roll(3,6) + 1 > u.c[UC_INTEL]) {
      printf("It failed!\r\n");        /* used to be auto-death - CDC */
      break;
    }
    u.c[UC_DGNLVL] = roll(1,20);
    u.c[UC_DGN_X] = roll(1,20);
    u.c[UC_DGN_Y] = roll(1,20);
    utl_inilvl();
    u.c[UC_STATE] = DGN_NEWLOC;
    break;
  case 2:          /* p. word kill */
    if (roll(3,6) == 18) {
      tmp = 0.5 * u.c[UC_CURHIT];
      if (tmp < 1)
        tmp = 1;
      printf("It backfired: you suffer %d hit point%s.\n\r",
             u.c[UC_CURHIT] - tmp, (u.c[UC_CURHIT] - tmp == 1) ? "" : "s");
      u.c[UC_CURHIT] = tmp;
      break;
    }
    if (m == 15 && rnd() > 0.5) {
      printf("%s is magic resistant\007!\r\n", mnam);
      break;
    }
    if (roll(3,6) < u.c[UC_INTEL]) {
      printf("It is affected.\r\nIt died...\r\n");
      dead = 1;
    } else
      printf("This isn't your day.\r\n");
    break;
  case 3:          /* pris wall/blade barrier */
    cbt_chk();
    printf("You are protected, when you release it the %s", mnam);
    if (rnd() > 0.3) {
      printf(" is no longer there.\r\n");
      gone = true;
      dead = 1;
      u.i[6] = u.i[7] = 0;
      break;
    }
    printf(" is waiting for you!\r\n");
    break;
  case 4:          /* time stop */
    if (u.c[UC_SPELL_TMST] < 0)
      u.c[UC_SPELL_TMST] = roll(1,10);
    else
      u.c[UC_SPELL_TMST] += roll(1,10);
    printf("Time is frozen, monsters cannot attack you.\r\n");
    gone = true;
    dead = 1;
    u.i[6] = 0;
    break;
  case 5:          /* wall of fire */
    cbt_chk();
    printf("A wall of flames appears about you...\r\n");
    sleep(2);
    if (rnd() > 0.7) {
      printf("The %s tried to get you through the flames!\r\n", mnam);
      tmp = mm[m].m * rnd() + m1;
      tmp *= 1.5;    /* XXX CDC - make more powerful than confuse */
      printf("It gave itself %d damage point%s.\r\n", tmp,
             (tmp == 1) ? "" :"s");
      m2 -= tmp;
      if (m2 < 1) {
        printf("It killed itself!\r\n");
        if (rnd() > 0.1)
          u.i[8] = 0;
        u.i[6] = 0;
        dead = 1;
      }
      break;
    }
    if (rnd() > 0.4) {
      printf("The %s cannot stand the heat and leaves.\r\n", mnam);
      gone = true;
      dead = 1;
      u.i[6] = 0;
      break;
    }
    printf("The %s is patient.\r\n", mnam);
    break;
  case 6:          /* summon demon */
    cbt_chk();     /* XXX CDC, too painful to do right */
    if (roll(3,6) + 1 > u.c[UC_INTEL]) {
      printf("The demon is taking a bath.\r\n");
      break;
    }
    if (!hflag) {
      u.i[6] = 0;
      printf("The demon demolishes the %s.\r\n", mnam);
      gone = true;
      dead = 1;
    }
    sleep(2);
    if (roll(3,6) + 1 <= u.c[UC_INTEL])
      printf("You dispelled the demon.\r\n");
    else {
      printf("It attacks you!!\r\n");
      mnam = "Demon";
      hflag = false;
      m = 20;
      m1 = u.c[UC_LEVEL] + 5;
      m2,m2_old = roll(m1, 16);
      m_str = roll(1, mm[m].m);
      m_arm = roll(1,m1) - 1;
      gone = false;
      dead = 0;
      u.i[6] = 1;
    }
    break;
  default:
    printf("ms4: unknown spell?\r\n");
    unix_exit(1);
  }
  return(NOPE);
}


int cs1
  (
    int s
  )
{
  int tmp;
  switch (s) {
  case 1:          /* prot/evil */
    ms1(5);
    break;
  case 2:          /* light */
    ms1(6);
    break;
  case 3:          /* cure light */
    u.c[UC_CURHIT] += roll(1,6) + 1;
    if (u.c[UC_CURHIT] > u.c[UC_MAXHIT])
      u.c[UC_CURHIT] = u.c[UC_MAXHIT];
    printf("You now have %d hit point%s.\r\n", u.c[UC_CURHIT],
           (u.c[UC_CURHIT] == 1) ? "" : "s");
    break;
  case 4:          /* turn undead */
    printf("GET YE HENCE VILE CREATURE!!!\r\n");
    cbt_chk();
    if (m > 6) {
      printf("The %s feels insulted at being called undead.\r\n", mnam);
      break;
    }
    if (roll(3,6) + 3 + m1 - u.c[UC_LEVEL] > u.c[UC_WISDOM])
      printf("The %s listens with deaf ears.\r\n", mnam);
    else {
      printf("The %s runs in terror!\r\n", mnam);
      gone = true;
      dead = 1;
      u.i[6] = 0;
    }
    break;
  default:
    printf("cs1: unknown spell?\r\n");
    unix_exit(1);
  }
  return(NOPE);
}


int cs2
  (
    int s
  )
{
  int tmp;
  switch (s) {
  case 1:          /* dtrp */
    if (u.c[UC_SPELL_DTRP] < 1)
      u.c[UC_SPELL_DTRP] = roll(2,10);
    else
      u.c[UC_SPELL_DTRP] += roll(2,10);
    break;
  case 2:          /* silnce */
    if (u.c[UC_SPELL_SLNC] < 1)
      u.c[UC_SPELL_SLNC] = roll(2,10);
    else
      u.c[UC_SPELL_SLNC] += roll(2,10);
    break;
  case 3:          /* pray */
    if (u.c[UC_SPELL_PRAY] < 1)
      u.c[UC_SPELL_PRAY] = roll(2,10);
    else
      u.c[UC_SPELL_PRAY] += roll(2,10);
    break;
  case 4:          /* hold monst */
    ms3(4);
    break;
  default:
    printf("cs2: unknown spell?\r\n");
    unix_exit(1);
  }
  return(NOPE);
}


int cs3
  (
    int s
  )
{
  int tmp;
  switch (s) {
  case 1:          /* cure serious */
    u.c[UC_CURHIT] += roll(2,6) + 2;
    if (u.c[UC_CURHIT] > u.c[UC_MAXHIT])
      u.c[UC_CURHIT] = u.c[UC_MAXHIT];
    printf("You now have %d hit points.\r\n", u.c[UC_CURHIT]);
    break;
  case 2:          /* dispell */
    cbt_chk();
    if (m > 6) {
      printf("The %s feels insulted at being called undead.\r\n", mnam);
      break;
    }
    if (roll(3,6) + 2 > u.c[UC_WISDOM])
      printf("The %s scorns your words.\r\n", mnam);
    else {
      printf("The %s is dispelled into thin air!\r\n", mnam);
      dead = 1;
    }
    break;
  case 3:          /* cont light */
    ms3(6);
    break;
  case 4:          /* plague */
    cbt_chk();
    if (roll(3,6) + 2 > u.c[UC_WISDOM]) {
      printf("The %s just had its penicillin shot!\r\n", mnam);
      break;
    }
    printf("The %s dies of the black plague!!\r\n", mnam);
    u.i[6] = 0;
    dead = 1;
    if (roll(1, 10) == 1 && u.c[UC_CURHIT] > 1) {
      tmp = u.c[UC_CURHIT] / 2.0;   /* XXX CDC used be auto-kill */
      printf("You caught it too!! You suffer %d hit point%s.\r\n",
             u.c[UC_CURHIT] - tmp, (u.c[UC_CURHIT] - tmp) == 1 ? "" : "s");
      u.c[UC_CURHIT] = tmp;
    }
    break;
  default:
    printf("cs3: unknown spell?\r\n");
    unix_exit(1);
  }
  return(NOPE);
}


int cs4
  (
    int s
  )
{
  int tmp;
  switch (s) {
  case 1:          /* holy word */
    ms4(2);
    break;
  case 2:          /* fing. of death */
    cbt_chk();
    printf("Die %s!! ZZAAAPPP!!!!\r\n", mnam);
    if (roll(1,20) <= u.c[UC_WISDOM]) {
      printf("It died...\r\n");
      dead = 1;
    } else
      printf("The %s dodges well.\r\n", mnam);
    break;
  case 3:          /* blade bar */
    ms4(3);
    break;
  case 4:          /* raise dead */
    printf("You can't throw this on yourself!  %s",
           "(You have to be dead first)\r\n");
    printf("Too bad you just wasted a combat spell!\r\n");
    break;
  default:
    printf("cs4: unknown spell?\r\n");
    unix_exit(1);
  }
  return(NOPE);
}
