/*
 * udd cbt.c file - combat program
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#define CBTMAIN
#include "defs.h"
#include "cbt.h"
#include <math.h>
#include <ctype.h>
#include <stdio.h>

struct slev {
int (*ms)();
int (*cs)();
};

static struct slev fns[4] =
  {
    {ms1, cs1},
    {ms2, cs2},
    {ms3, cs3},
    {ms4, cs4},
  };


int cbt_main(void)
  {
    bool ok;
    int s1, tmp;
    bool aflag = false;
    dead = -1;
    if (u.c[UC_STATE] == CBT_ALTR)  /* ask for tough one! */
      {
        aflag = true;
        u.c[UC_STATE] = CBT_NORM;
      } /*if*/
    switch(u.c[UC_STATE])
      {
    case CBT_NORM:
        if (u.c[63] != 0)
          {
            switch (u.c[63])
              {
            case SPC_THR:
                m = 11;
                mnam = "Dwarf Lord";
                m1 = u.c[UC_DGNLVL] + 10 + roll(1,6);
                m_str = roll(1, mm[m].m) + 5;
                m_arm = roll(1, m1);
            break;
            case SPC_ALT:
                m = 18 + roll(1,2);
                mnam = "Demon Prince";
                m1 = u.c[UC_DGNLVL] + 5 + roll(1,20);
                m_str = roll(1,mm[m].m) + 5;
                m_arm = roll(1, m1);
            break;
            case SPC_DGN1:
            case SPC_DGN2:
                m = 20;
                mnam = "Dragon Lord";
                m1 = u.c[UC_DGNLVL] + roll(1,20);
                m_str = roll(1,mm[m].m);
                m_arm = roll(1, m1) - 1;
            break;
            default:
                printf("cbt: FAT. ERROR, spec unknown!\r\n");
                unix_exit(1);
              } /*switch*/
          }
        else  /* no special */
          {
            ok = false;
            while (!ok)
              {
                ok = true;
                if (aflag) /* ask for nasty (currently undead) */
                  {
                    m = (int)(pow((double)(rnd()), (double)(2.0 - 0.04 * u.c[UC_DGNLVL]))
                        * 6.0 + 1.0);
                  }
                else
                  {
                    m = roll(1,20);
                    if (m > 6)
                        m = (int)(6.0 + pow((double)(rnd()), (double)(2.0 - 0.04 * u.c[UC_DGNLVL]))
                            * 14.0 + 1.0);
                    else
                        m = (int) (pow((double)(rnd()), (double)(2.0 - 0.04 * u.c[UC_DGNLVL]))
                            * 6.0 + 1.0);
                    if (mm[m].mindunlvl > u.c[UC_DGNLVL])
                        ok = false;
                  } /*if*/
                if (mm[m].maxdunlvl < u.c[UC_DGNLVL])
                    ok = false;
                mnam = mm[m].nam;
                m1 = roll(1, (int)(u.c[UC_DGNLVL] * 1.5)) + (int)(rnd()*rnd()*rnd()*3);
                if (m1 < mm[m].minmonlvl)
                    ok = false;
              } /*while*/
            m_str = roll(1, mm[m].m);
            m_arm = roll(1, m1) - 1;
            l = m1;
            s1 = 1; /* s1 ???XXX*/
            if (mm[m].m + m1 >= 1.2 * u.c[UC_MAXHIT] && u.c[UC_CURHIT] > 1)
                if (roll(1,20) <= u.c[UC_CHARISMA])
                  {
                    printf("A level %d %s takes one look at you...", m1, mnam);
                    sleep(1);
                    printf(" snarls and knocks you cold.\r\n");
                    u.c[UC_CURHIT] = u.c[UC_CURHIT] / 2.0;
                    sleep(2);
                    printf("You wake up feeling half dead.\r\n");
                    printf("It has stalked off.\r\n");
                    u.i[6] = u.i[7] = u.i[8] = 0;
                    u.l[u.c[UC_DGN_X]][u.c[UC_DGN_Y]] = 16*u.i[5] + 4*u.i[2] + u.i[1];
                    if (u.i[5] == 0)
                      {
                        u.c[UC_STATE] = DGN_PROMPT;
                        return
                            NOPE;
                      } /*if*/
                    return
                        spc_main();
                  } /*if; if*/
          } /*if*/
        m2 = m2_old = roll(m1, mm[m].m);
        printf("You have encountered a level %d %s!\r\n", m1, mnam);
        if (u.c[UC_DEBUGCHR] == 1)
            printf("STR: %2d, ARM: %2d, HITS: %2d, HDIE: %2d\r\n", m_str, m_arm, m2, mm[m].m);
        ok = false;
        dead = 0;
        if (u.c[42] > 0 && roll(1,20) < 15+u.c[UC_LEVEL] - m1)
            ok = true;
        /*if (roll(1,20) <= u.c[UC_INTEL] + u.c[UC_DEX] / 2.0)
            ok = true;  */
        if (roll(1,20) + m1 <= ((u.c[UC_INTEL] + u.c[UC_DEX]) / 2.0) + u.c[UC_LEVEL])  /*CDC XXX???*/
            ok = true;
        mskip = false;     /* for spells */
        autoevade = false;
        gone = false;
        while (dead == 0)
          {
            if (ok)
              {
ask:
                if (!autoevade)
                  {
                    printf("Press (F)ight%s or (E)vade: ",
                           (u.c[UC_SPELLS1]+u.c[UC_SPELLS2]+u.c[UC_SPELLS3]+u.c[UC_SPELLS4] == 0) ? ""  : ", (C)ast,");
                    in = getchar();
                    if (islower(in))
                      in = toupper(in);
                  }
                else
                    in = 'E';
                if (in < 0)
                    in = 'E';
                if (in == 'C' && u.c[UC_SPELLS1]+u.c[UC_SPELLS2]+u.c[UC_SPELLS3]+u.c[UC_SPELLS4] == 0)
                    in = '?';
                if (in != 'F' && in != 'C' && in != 'E')
                  {
                    printf("The %s is not amused...\r\n", mnam);
                    goto ask;
                  } /*if*/
                if (in == 'E')
                  {
                    if (autoevade)
                      {
                        (rnd() < 0.9) ? printf("Evade\r\n") : printf("Run away\r\n");
                      }
                    else
                      {
                        autoevade = false;
                      } /*if*/
                    if
                      (
                            (u.i[1] == 1 || u.c[UC_STRENGTH] == 3)
                        &&
                            (u.i[2] == 1 || u.i[2] == 3)
                        &&
                            ((u.l[u.c[UC_DGN_X]][u.c[UC_DGN_Y]+1] & 1) == 1)
                        &&
                            ((u.l[u.c[UC_DGN_X]+1][u.c[UC_DGN_Y]] & 4) == 4)
                      )
                      {
                        printf("There's no where to go!\r\n");
                      }
                    else
                     {
                        tmp = u.c[UC_ELVEN_CLOAK]; /* XXX make evade possible if no cloak */
                        if (tmp < 1)
                            tmp = 1;
                        if (roll(2,10) <= u.c[UC_DEX] && sqrt(tmp/25.0)*25.0 >= roll(1,25))
                          {
                            printf("You made it!\r\n");
                            ok = false;
                            while (!ok)
                              {
                                ok = true;
                                in = roll(1,4);
                                if (in == 1 && u.i[2] == 1)
                                  {
                                    ok = true;
                                    continue;
                                  } /*if*/
                                if (in == 2  && fni1(u.l[u.c[UC_DGN_X]][u.c[UC_DGN_Y]+1],1) == 1)
                                  {
                                    ok = true;
                                    continue;
                                  } /*if*/
                                if (in == 3  && fni1(u.l[u.c[UC_DGN_X]+1][u.c[UC_DGN_Y]],2) == 1)
                                  {
                                    ok = true;
                                    continue;
                                  } /*if*/
                                if (in == 4 && u.i[1] == 1)
                                  {
                                    ok = true;
                                    continue;
                                  } /*if*/
                                if (rnd() > 0.5)
                                    u.i[6] = u.i[7] = u.i[8] = 0;
                                u.c[UC_STATE] = DGN_AMOVE;
                                u.c[63] = in;
                                return
                                    NOPE;
                              } /*while*/
                          } /*if*/
                      } /*if*/
                    printf("You're rooted to the spot... The %s bites back!\r\n", mnam);
                  } /*if*/
                if (in == 'C')
                  {
                    printf("Cast\r\n");
                    if (cbt_cast() == YEP)
                        return
                            YEP;
                  } /*if*/
                if (in == 'F')
                  {
                    printf("Fight\r\n");
                    i1 =
                            3 * u.c[UC_DEX]
                        +
                            u.c[UC_STRENGTH]
                        +
                            u.c[UC_WEAPON]
                        +
                            u.c[UC_LEVEL] * (5 - u.c[UC_CLASS])
                        -
                            m1 * mm[m].m / 2.0;
                    /* 3% per DEX, 1% per STR, +1% * wep, +3,4,5% for level,
                       -1...10% per Mlevel */
                    if (u.c[40] > 0)
                        i1 += 5;         /* 5% for PRAY spell */
                    i1_old = i1;       /* save value */
                    i1 *= (u.c[UC_MAXHIT] + u.c[UC_CURHIT]) / (2.0 * (double) u.c[UC_MAXHIT]);
                                       /* deduct for hits taken */
                    dice = roll(1, 100);
                    if (u.c[UC_DEBUGCHR] == 1)
                        printf("CHANCE: %d%% [%d%% max], DIE: %d\r\n", i1, i1_old, dice);
                    if (dice > i1)
                      {
                        printf("You missed%s.\r\n", (dice <= i1_old) ?
                               " due to your weakened condition" : "");
                      }
                    else
                      {
                        d = roll(1, 8 - 2* u.c[UC_CLASS] + u.c[UC_WEAPON]);/* base on class/wep */
                        i2 = u.c[UC_STRENGTH];     /* base STR */
                        if (u.c[44] > 0)
                            i2 += 3;       /* STR spell */
                        if (i2 > 14)
                            d += roll(1, i2 - 14);   /* bonus! */
                        else if (i2 < 7)
                            d -= roll(1, 7 - i2);    /* weakling! */
                        if (u.c[UC_DEBUGCHR] == 1)
                            printf("FORCE: %d\r\n", d);
                        if (m_arm > 0)
                            d -= roll(1, m_arm)+roll(1,m_arm);   /*one for ARM,one for shld*/
                        if (d < 1)
                            printf("It fends you off.\r\n");
                        else
                            cbt_uhitm(d);
                      } /*if*/
                  } /*if*/
              } /*if*/
            ok = true;
            d = 0;
            if (mskip)
              {
                mskip = false;
                continue;            /* monst skips a turn */
              } /*if*/
            if (dead == 0)
              {
                if (m == 20)
                  {
                    printf("The %s breathes fire at you!\007\r\n", mnam);
                    sleep(2);
                    d = roll(2,20) + m1;
                    if (roll(1,20) >= (17 - u.c[UC_LEVEL] * 0.5))
                      {
                        printf("You partially dodge it.\r\n");
                        d = d * 0.5;
                      } /*if*/
                  } /*if*/
                if (d == 0)
                  {
                    i1 = 50 + mm[m].m + m_str + m_arm + u.c[UC_CLASS] * 10 +
                        m1 * (mm[m].m / 2.0) - u.c[UC_LEVEL] * (5 - u.c[UC_CLASS]);
                    /* chance is 50% + monster, mage+20%, cleric+10%, +1..10% mlevel
                       - 3, 4, 5% per level */
                    if (u.c[38] > 0)
                        i1 -= 10;        /* PROT spell */
                    if (u.c[39] > 0)
                        i1 -= 20;        /* shield */
                    if (u.c[40] > 0)
                        i1 -= 10;        /* pray */
                    if (u.c[UC_DEX] > 14)   /* dex bonus */
                        i1 -= (2 * (u.c[UC_DEX] - 14));
                    i1_old = i1;
                    i1 = i1 * ((m2_old + m2) / (double)(2 * m2_old)); /* hits taken */
                    dice = roll(1,100);
                    if (u.c[UC_DEBUGCHR] == 1)
                        printf("CHANCE: %d%% [%d%% max], DIE: %d\r\n", i1, i1_old, dice);
                    if (dice > i1)
                      {
                        printf("It missed%s.\r\n", (dice <= i1_old) ?
                               " due to its weakened condition" : "");
                        continue;        /* for while (dead == 0) */
                      } /*if*/
                  } /*if*/
                if (m > 3 && m < 7) /* high level undead ? */
                  {
                    i2 = 10 * (m - 3);
                    if (u.c[38] > 0)
                        i2 -= 5;         /* prot from evil */
                    if (u.c[40] > 0)
                        i2 -= 5;         /* pray spell */
                    if (roll(1,100) <= i2)
                      {
                        printf("The %s drains an energy level!\r\n", mnam);
                        u.c[UC_EXP] = utl_exp(u.c[UC_LEVEL]) - 1;
                        if (utl_chklvl() == YEP)
                            return
                                YEP;
                        continue;
                      } /*if*/
                  } /*if*/
                if (d == 0)
                    d = roll(1, mm[m].m) + m1;
                if (m == 19)  /* balrog */
                    if (roll(1,3) < 3)
                        printf("The %s uses it's sword!\r\n", mnam);
                    else
                      {
                        printf("The %s uses it's whip!\007\r\n", mnam);
                        d *= 1.5;
                      } /*if*/
                if (u.c[UC_DEBUGCHR] == 1)
                    printf("FORCE: %d\r\n", d);
                d -= 1 + roll(1, u.c[UC_SHIELD]);
                if (d <= 0)
                    printf("You block with your shield.\r\n");
                else
                  {
                    d -= roll(1, u.c[UC_ARMOR]) + 2 - u.c[UC_CLASS];
                    if (d <= 0)
                        printf("Your armor protects you.\r\n");
                    else
                      {
                        printf("It did %d point%s to you.\r\n", d, (d == 1) ? "" : "s");
                        u.c[UC_CURHIT] -= d;
                        if (u.c[UC_CURHIT] < 1)
                          {
                            printf("You died!\r\n");
                            return(utl_death());
                          } /*if*/
                      } /*if*/
                  } /*if*/
              if (m == 15)           /* doppelganger */
                if (roll(1,4) == 3)
                  {
                    printf("The %s looks just like you!\r\n", mnam);
                    if (roll(1,20) < u.c[UC_INTEL] + u.c[UC_LEVEL] - m1)
                        printf("You see through its trick.\r\n");
                    else
                      {
                        printf("You're confused!\r\n");
                        ok = false;
                      } /*if*/
                  } /*if*/
              if (m == 12)           /* harpie */
                if (roll(1,3) == 1)
                  {
                    printf("The %s charms you with her voice...\r\n", mnam);
                    if (roll(1,20) < u.c[UC_INTEL] + u.c[UC_LEVEL] - m1)
                        printf("But you resist her death song!\r\n");
                    else
                      {
                        printf("You're hopelessly in love!\r\n");
                        ok = false;
                      } /*if*/
                  } /*if*/
              } /*if*/
          } /*while*/
        u.i[6] = 0;
        if (!gone)
          {
            d = (mm[m].m * m1 + m_str * m_arm) * 10 / (double) u.c[UC_LEVEL];
            printf("You got %d experience point%s.\r\n", d, (d == 1) ? "" : "s");
            u.c[UC_EXP] += d;
            sleep(1);
            utl_chklvl();
          } /*if*/
        utl_pplot(NOPE);
        if (u.c[63] != 0 || aflag)
          {
            u.c[63] = m1;
            return
                NOPE;          /* called from spc_main()? */
          } /*if*/
        u.c[63] = l;
        if (u.i[7] != 0)
            return
                trs_main();
        u.c[UC_STATE] = 1;
        u.l[u.c[UC_DGN_X]][u.c[UC_DGN_Y]] = 16 * u.i[5] + 4 * u.i[2] + u.i[1];
        if (u.i[5] == 0)
          {
            u.c[UC_STATE] = DGN_PROMPT;
            return
                NOPE;
          } /*if*/
        return
            spc_main();
    case CBT_CAST:
       if (cbt_cast() == YEP)
           return
               YEP;
    break;
    default:
       printf("cbt: unknown cmd\r\n");
       unix_exit(1);
      } /*switch*/
    return
        NOPE;
  } /*cbt_main*/

int cbt_cast(void)
  {
    int in2, lvl = 0, spl = 0;
    if (u.c[UC_STATE] == CBT_CAST)
      {
        hflag = true;
        u.c[UC_STATE] = DGN_PROMPT;
      }
    else
        hflag = false;
    while (spl == 0)
      {
s_top:
        printf("Level:");
        in2 = getchar();
        if (in2 < 0)
            in2 = '\r';
        if (in2 == '\r')
          {
            printf("\r\n");
            return
                NOPE;
          } /*if*/
        if (in2 < '1' || in2 > '4')
          {
            printf("A number for 1 to 4 you jester!\r\n");
            goto s_top;
          } /*if*/
        lvl = in2 - '0';
        if (u.c[UC_SPELLS1 - 1 + lvl] < 1)
          {
            printf("You have no more level %d spells.\r\n", lvl);
            return(NOPE);
          } /*if*/
        printf(" Spell # ");
        in2 = getchar();
        if (in2 == '\r')
            return
                NOPE;
        if
          (
                in2 < '1'
            ||
                in2 > '4' && u.c[UC_CLASS] == CHRCLASS_CLERIC
            ||
                in2 > '6' && u.c[UC_CLASS] != CHRCLASS_CLERIC
          )
            utl_prtspl(u.c[UC_CLASS], lvl);
        else
            spl = in2 - '0';
      } /*while*/
    u.c[UC_SPELLS1 - 1 + lvl]--;
    if (u.c[UC_CLASS] != CHRCLASS_CLERIC)
      {
        printf("%s\r\n", sp[lvl][spl]);
        return
            (*fns[lvl - 1].ms)(spl);
      }
    else
      {
        printf("%s\r\n", sp[lvl+4][spl]);
        return
            (*fns[lvl - 1].cs)(spl);
      } /*if*/
  } /*cbt_cast*/
