/*
 * udd spc.c file - special location program
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "defs.h"

/*static const char * const color[] = { "white", "green", "blue", "red", "black" };*/
static const char * const color[] = { "white", "blue", "green", "red", "black" };
  /* colours of fountain water, in increasing order of potential toxicity */

int spc_main(void)
  /* returns YEP/NOPE indicating if character died. */
  {
    int sav, sav2, tmp, tmp2;
    double dtmp;
    char tmpbuf[NAMELEN];
    int spc = u.i[ROOM_SPECIAL];
    if (u.c[UC_STATE] == SPC_TPTNOW)
        spc = SPC_TPTNOW;
    switch (spc)
      {
    case SPC_UPS:
    case SPC_DNS:
    case SPC_UDS:
        printf("You have found a circular stairway...\r\n");
s_top:
        printf("Do you want to %s%sor stay on the (S)ame level: ",
               (spc != 1) ? "go (U)p, " : "",
               (spc != 2) ? "go (D)own, " : "");
        tmp2 = getchar();
        if (tmp2 < 0)
            tmp2 = 'S';
        if (tmp2 == 'S' || tmp2 == 's')
          {
            printf("Same\r\n");
            break;
          } /*if*/
        if (spc != SPC_DNS && (tmp2 == 'u' || tmp2 == 'U'))
          {
            printf("Up\r\n");
            u.c[UC_DGNLVL]--;
            if (u.c[UC_DGNLVL] < 1)
                return(utl_escape());
            utl_inilvl();
            u.c[UC_STATE] = DGN_NEWLOC;
            return(NOPE);
          } /*if*/
        if (spc != SPC_UPS && (tmp2 == 'd' || tmp2 == 'D'))
          {
            printf("Down\r\n");
            if (u.c[UC_DGNLVL] >= 20)
              {
                printf("You find the way is blocked by an iron gate...\r\n");
                break;
              } /*if*/
            u.c[UC_DGNLVL]++;
            utl_inilvl();
            u.c[UC_STATE] = DGN_NEWLOC;
            return(NOPE);
          } /*if*/
        printf("\r\nDo you have a hearing problem\007?\r\n");
        goto s_top;
    case SPC_EXC:
        printf("You have found the Excelsior Transporter...\r\n");
        if (u.c[UC_HASORB] == 1)
          {
            printf("But the controls just went dead!\r\n");
            break;
          } /*if*/
        if (u.c[UC_SPELL_TMST] > 0)
          {
            printf("But the controls seem frozen!\r\n");
            break;
          } /*if*/
exc_top:
        unix_tty_cook();
        printf("What level do you want to go to?\r\n");
        printf("(1 to 20, or 0 to stay here)? ");
        if (!fgets(tmpbuf, NAMELEN, stdin))
            strcpy(tmpbuf, "0\n");
        tmp = sscanf(tmpbuf, "%d", &tmp2);
        fflush(stdin);
        unix_tty_dgn();
        if (tmp == 1 && tmp2 == 0)
            break;
        if (tmp != 1 || (tmp == 1 && (tmp2 < 1 || tmp2 > 20)))
          {
            printf("Try again bird brain\007!\r\n");
            goto exc_top;
          } /*if*/
        tmp = (abs(u.c[UC_DGNLVL] - tmp2) + 1) * ((tmp2 * u.c[UC_DGNLVL])/2.0) * 25;
        if (tmp > u.c[UC_GOLDFOUND])
          {
            printf("You need %d gold for that trip.\r\n", tmp);
            break;
          } /*if*/
        u.c[UC_GOLDFOUND] -= tmp;
        printf("It cost %d gold, you have %d left.\r\n", tmp, u.c[UC_GOLDFOUND]);
        sav = u.c[UC_DGNLVL];
        u.c[UC_DGNLVL] = tmp2;
        utl_inilvl();
        u.c[UC_DGN_X] = -1;
        for (tmp = 1 ; tmp <= 20; tmp++)
            for (tmp2 = 1 ; tmp2 <= 20 ; tmp2++)
                if ((u.l[tmp][tmp2] & 240) == 64)
                  {
                    u.c[UC_DGN_X] = tmp;
                    u.c[UC_DGN_Y] = tmp2;
                  } /*if; for; for*/
        if (u.c[UC_DGN_X] < 0)
          {
            u.c[UC_DGN_X] = roll(1,20);
            u.c[UC_DGN_Y] = roll(1,20);
          } /*if*/
        u.c[UC_VALUE] = enter_room(u.l[tmp][tmp2]);
        if (roll(3,7) > (u.c[UC_INTEL] + u.c[UC_WISDOM]) / 2)
          {
            printf("The transporter malfunctioned!\r\n");
            if (cbt_ohitu((abs(sav - u.c[UC_DGNLVL])+0.5*(u.c[UC_DGNLVL] +sav)) *
                           (rnd() * rnd() * rnd() / 2.0)) == YEP)
              return(YEP);
          } /*if*/
        u.c[UC_STATE] = DGN_NEWLOC;
        return(NOPE);
    case SPC_PIT:
        tmp = NOPE;
        if (u.c[UC_SPELL_LEVT] > 0)
            printf("You are hovering over a pit...\r\n");
        else
          {
            if
              (
                    roll(3,6) + 3 > u.c[UC_DEX]
                &&
                    sqrt((double)u.c[UC_ELVEN_CLOAK] / 25.0) * 25.0 < roll(1,20)
              )
              {
                printf("You fell in a pit!\r\n");
                tmp = YEP;
              }
            else
                printf("You are on the brink of a pit...\r\n");
          } /*if*/
        if (u.c[UC_DGNLVL] == 20 && tmp == NOPE)
          {
            printf("It's bottomless, watch out!\r\n");
            break;
          } /*if*/
        if (tmp == NOPE)
          {
pit_top:
            printf("Press <CR> to climb down, <LF> to walk around: ");
            tmp2 = getchar();
            printf("\r\n");
            if (tmp2 < 0)
                tmp2 = 10;
            if (tmp2 == '\n')
                break;
            if (tmp2 != '\r')
              {
                printf("Come on\007!\r\n");
                goto pit_top;
              } /*if*/
          } /*if*/
        if (u.c[UC_DGNLVL] == 20)
          {
            printf("It's bottomless\007!!!  IIIEEEEEEEEEEEEE!!!!!!... .  .   .\r\n");
            return(utl_death());
          } /*if*/
        if (tmp == YEP || (u.c[UC_SPELL_LEVT] < 1 && roll(3,6)+1 >= u.c[UC_DEX]))
          {
            if (tmp != YEP)
                printf("You slipped down the side!\r\n");
            if (cbt_ohitu(u.c[UC_DGNLVL]) == YEP)
                return(YEP);
          }
        else
            printf("You made it!\r\n");
        u.c[UC_DGNLVL]++;
        utl_inilvl();
        u.c[UC_STATE] = DGN_NEWLOC;
        return(NOPE);
    case SPC_TPT:
    case SPC_TPTNOW:
        if (spc == SPC_TPT)
            printf("Z\007ZAP!  You've been teleported!\r\n");
        sleep(2);
tpt_top:
        tmp = u.c[UC_DGN_X];
        tmp2 = u.c[UC_DGN_Y];
        if (((tmp + tmp2) & 1) == 0)
            u.c[UC_DGNLVL]--;
        else
            u.c[UC_DGNLVL]++;
        if (u.c[UC_DGNLVL] < 1)
            u.c[UC_DGNLVL] = 1;
        else if (u.c[UC_DGNLVL] > 20)
            u.c[UC_DGNLVL] = 20;
        tmp = tmp + u.c[UC_DGNLVL] * 7 + tmp2 * 13;
        tmp2 = tmp2 + u.c[UC_DGNLVL] * 6 + tmp * 17;
        while (tmp > 20)
            tmp -= 20;
        while (tmp2 > 20)
            tmp2 -= 20;
        u.c[UC_DGN_X] = tmp;
        u.c[UC_DGN_Y] = tmp2;
        if (rnd() > 0.8)
            goto tpt_top;
        utl_inilvl();
        u.c[UC_STATE] = DGN_NEWLOC;
        return(NOPE);
    case SPC_FTN:
        tmp = roll(1,5);
        printf("You see a Fountain with %s water...\r\n", color[tmp - 1]);
ftn_top:
        printf("Press <CR> to take a drink, <LF> to walk by: ");
        tmp2 = getchar();
        printf("\r\n");
        if (tmp2 < 0)
            tmp2 = 10;
        if (tmp2 == '\n')
            break;
        if (tmp2 != '\r')
          {
            printf("Sluggard!  Follow directions.\007\r\n");
            goto ftn_top;
          } /*if*/
        dtmp = rnd();
        if (dtmp  >= 0.4 + (0.1 * tmp))
          {
            printf("It tastes good!\r\n");
            u.c[UC_CURHIT] += roll(1, 3 * u.c[UC_DGNLVL]);
            if (u.c[UC_CURHIT] > u.c[UC_MAXHIT])
                u.c[UC_CURHIT] = u.c[UC_MAXHIT];
            printf("You now have %d hit points.\r\n", u.c[UC_CURHIT]);
            break;
          } /*if*/
        if (dtmp <= 0.1 * tmp)
          {
            printf("Poison!  Gurgle..  ..  ..   .\r\n");
            if (cbt_ohitu(u.c[UC_DGNLVL]) == YEP)
                return(YEP);
            printf("You only have %d hit point%s.\r\n", u.c[UC_CURHIT],
                   (u.c[UC_CURHIT] == 1) ? "" : "s");
            break;
          } /*if*/
        if (dtmp > 0.6)
          {
            printf("It's just water.\r\n");
            break;
          } /*if*/
        if (rnd() > 0.5)
          {
            if (utl_adj_ex() == YEP)
                return(YEP);
          }
        else
            utl_adj_st();
    break;
    case SPC_ALT:
alt_top1:
        printf("You have found a Holy Altar...\r\n");
        tmp = roll(1,20);        /* desc ok if = 1 */
alt_top2:
        printf("Do you wish to (W)orship%s or (I)gnore it? ",
               (tmp == 1) ? ", (D)esecrate," : "");
        sav2 = getchar();
        if (sav2 < 0)
            sav2 = 'I';
        if (islower(sav2))
            sav2 = toupper(sav2);
        if (sav2 == 'D' && tmp == 1)
          {
            printf("Desecrate\r\n");
            printf("You do vile and unspeakable things to the altar.\r\n");
            if (roll(1,4) != 2)
              {
                printf("Nothing happens here.\r\n");
                if (roll(1,10) == 7)
                    printf("{Hope that made you feel better.}\r\n");
                break;
              } /*if*/
            if (roll(1,20) <= 3)
              {
                printf("A Voice booms out \"I shall be avenged.\"\r\n");
                u.c[UC_VALUE] = spc;
                u.c[UC_STATE] = CBT_NORM;
                if (cbt_main() == YEP)
                    return(YEP);
                goto alt_top1;
              } /*if*/
            printf("The sound of thunder shatters the air.\r\n");
            printf("The altar crumbles to dust before your eyes.\r\n");
            u.i[ROOM_SPECIAL] = 0;
            if (roll(1,10) < 3)
                u.i[ROOM_SPECIAL] = SPC_PIT;
            save_room(false);
            if (roll(1,4) != 4)
              {
                u.c[UC_STATE] = DGN_NEWLOC;
                return(NOPE);
              } /*if*/
            printf("Something seems to be left behind...\r\n");
            u.c[UC_VALUE] = u.c[UC_DGNLVL] + 10;
            u.i[ROOM_TREASURE] = 1;
            if (roll(1,10) == 3)
                u.i[ROOM_TREASURE_BOOBYTRAPPED] = 1;
            return(trs_main());
          } /*if*/
        if (sav2 == 'W')
          {
            printf("Worship\r\n");
            printf("Press <CR> to give money, <LF> to just pray: ");
            tmp = getchar();
            if (tmp < 0 || (tmp == '\n' && rnd() > 0.4))
              {
                printf("\r\n");
                break;
              } /*if*/
            if (tmp == '\n')
                sav2 = 'I';
            if (tmp == '\r')
              {
                printf("\r\n");
alt_top3:
                printf("How much of your %d gold ? ", u.c[UC_GOLDFOUND]);
                unix_tty_cook();
                if (!fgets(tmpbuf, NAMELEN, stdin))
                    strcpy(tmpbuf, "\n");
                sav = sscanf(tmpbuf, "%d", &tmp2);
                unix_tty_dgn();
                fflush(stdin);
                if (sav != 1 || tmp2 < 0 || tmp2 < 50 || tmp2 < 0.1 * u.c[UC_GOLDFOUND])
                  {
                    printf("How dare you insult us, you ");
                    goto trash;
                  }
                else
                  {
                    if (tmp2 > u.c[UC_GOLDFOUND])
                      {
                        printf("You don't have that much!\r\n");
                        goto alt_top3;
                      } /*if*/
                    u.c[UC_GOLDFOUND] -= tmp2;
                    if (rnd() > 0.9)
                      {
                        if (utl_adj_ex() == YEP)
                            return(YEP);
                        break;
                      } /*if*/
                    if (rnd() > 0.9)
                      {
                        utl_adj_st();
                        break;
                      } /*if*/
                    if (rnd() > 0.5)
                      {
                        printf("Thank you for your donation.\r\n");
                        break;
                      } /*if*/
                    tmp = rnd() * rnd() * 11.0 + 1;
                    tmp2 =
                            rnd() * (tmp2 / ((double)u.c[UC_GOLDFOUND] + 1.0)) * 20.0
                        +
                            rnd() * 20.0 + 1;
                    if (u.c[UC_SPELL_LIGHT - 1 + tmp] < 0) /* UC_SPELL_xxx */
                        u.c[UC_SPELL_LIGHT - 1 + tmp] = tmp2;
                    else
                        u.c[UC_SPELL_LIGHT - 1 + tmp] += tmp2;
                    printf("You've been heard.\r\n");
                    break;
                  } /*if*/
              } /* tmp == '\r' (worship) */
          } /* sav2 == 'W' */
        if (sav2 == 'I')
          {
            printf("Ignore\r\n");
            if (rnd() > 0.7)
              break;
trash:
            printf("Dirty Pagan Trash!\r\n");
            u.c[UC_STATE] = CBT_ALTR;
            u.c[UC_VALUE] = 0;
            if (cbt_main() == YEP)
                return(YEP);
            goto alt_top1;
          } /*if*/
        printf("\r\nIgnorant pagan\007!\r\n");
        goto alt_top2;
    case SPC_DGN1:
    case SPC_DGN2:
        sav = u.c[UC_DGNLVL];
        tmp = u.c[UC_DGN_X];
        tmp2 = u.c[UC_DGN_Y];
        sav2 = u.i[ROOM_SPECIAL] = 0;
        u.i[ROOM_TREASURE] = 1;
        u.c[UC_VALUE] = spc;
        u.c[UC_STATE] = CBT_NORM;
        printf("You have encountered a Dragon in its lair\007!\r\n");
        if (cbt_main() == YEP)
            return(YEP);
        if (u.c[UC_DGNLVL] == sav && u.c[UC_DGN_X] == tmp && u.c[UC_DGN_Y] == tmp2)
          {
            if (spc == SPC_DGN2)
                sav2 = SPC_ORB;
          }
        else
          {
            u.i[ROOM_TREASURE] = 0;
          } /*if*/
        if (u.i[ROOM_TREASURE] != 0)
            if (trs_main() == YEP)
                return(YEP);         /* this is safe because u.i[ROOM_SPECIAL] == 0 */
        u.i[ROOM_SPECIAL] = sav2;
        save_room(false);
        if (u.i[ROOM_SPECIAL] != SPC_ORB)
            break;
        utl_pplot(NOPE/*guess*/);
  /* fallthru */
    case SPC_ORB:
        u.i[ROOM_SPECIAL] = 0;
        save_room(false);
        printf("You have found the \007ORB!!!!\r\n");
orb_top:
        printf("Press <CR> to pick it up, <LF> to leave it: ");
        tmp2 = getchar();
        if (tmp2 < 0)
            tmp2 = 10;
        printf("\r\n");
        if (tmp2 == '\r')
          {
            printf("You've got it!\r\n");
            u.c[UC_HASORB] = 1;
            break;
          } /*if*/
        if (tmp2 != '\n')
          {
            printf("?What?\007\r\n");
            goto orb_top;
          } /*if*/
        printf("Lost your chance, Mister!\r\n");
        break;
    case SPC_ELV:
        printf("You feel heavy for a moment, but the sensation disappears...\r\n");
        if (u.c[UC_DGNLVL] == 1)
            return(utl_escape());
        u.c[UC_DGNLVL]--;
        utl_inilvl();
        u.c[UC_STATE] = DGN_NEWLOC;
        return(NOPE);
    case SPC_THR:
        printf("You see a massive throne covered with jewels and\r\n");
        printf("mysterious runes....\r\n");
thr_top:
        printf("Do you want to (S)it on it, (P)ry out some jewels,\r\n");
        printf("(R)ead the runes, or totally (I)gnore it: ");
        tmp2 = getchar();
        if (tmp2 < 0)
            tmp2 = 'I';
        if (islower(tmp2))
            tmp2 = toupper(tmp2);
        if (tmp2 == 'I')
          {
            printf("Ignore\r\n");
            break;
          } /*if*/
        if (tmp2 == 'S')
          {
            printf("Sit down\r\n");
            sleep(1);
            if (rnd() < 0.05)
              {
                u.c[UC_EXP] = utl_exp(u.c[UC_LEVEL]+1);
                printf("A loud gong sounds.\r\n");
                utl_chklvl();
                break;
              } /*if*/
            if (rnd() < 0.9)
              {
                printf("Nothing happens...\r\n");
                break;
              } /*if*/
            if (rnd() <= 0.1)
              {
                u.c[UC_STATE] = SPC_TPTNOW;
                printf("Z\007ZAP!  You've been teleported!\r\n");
                return(spc_main());
              } /*if*/
            printf("The Dwarven King returns....\r\n");
            u.c[UC_VALUE] = spc;
            u.c[UC_STATE] = CBT_NORM;
            if (cbt_main() == YEP)
                return(YEP);
            break;
          } /*if*/
        if (tmp2 == 'P')
          {
            printf("Pry\r\n");
            sleep(1);
            if (rnd() < 0.1)
              {
                printf("The Dwarven King returns....\r\n");
                u.c[UC_VALUE] = spc;
                u.c[UC_STATE] = CBT_NORM;
                if (cbt_main() == YEP)
                    return(YEP);
                break;
              } /*if*/
            if (rnd() > 0.3)
              {
                printf("They won't come off!\r\n");
                break;
              } /*if*/
            printf("They pop into your greedy hands!\r\n");
            tmp = 6000.0 * rnd() * rnd() * u.c[UC_DGNLVL] + 500.0;
            printf("They are worth %d gold!\r\n", tmp);
            u.c[UC_GOLDFOUND] += tmp;
            dtmp = u.c[UC_DGNLVL] / (double) u.c[UC_LEVEL];
            if (dtmp > 1.0)
                dtmp = 1.0;
            u.c[UC_EXPGAIN] += tmp * dtmp;
            break;
          } /*if*/
        if (tmp2 != 'R')
          {
            printf("\r\nREAD THE DIRECTIONS, STUPID\007!\r\n");
            goto thr_top;
          } /*if*/
        printf("Read\r\n");
        if (rnd() > 0.1)
          {
            printf("You don't understand them.\r\n");
            break;
          } /*if*/
        if (rnd() > 0.5)
          {
            printf("The Dwarven King returns...\r\n");
            u.c[UC_VALUE] = spc;
            u.c[UC_STATE] = CBT_NORM;
            if (cbt_main() == YEP)
                return(YEP);
            break;
          } /*if*/
        printf("The letters blur before your eyes.\r\n");
        printf("You feel giddy as power surges in you.\r\n");
        sleep(2);
        printf("You seem to have changed.\r\n");
        tmp = UC_SPELLS1 + rnd() * rnd() * 4.0;
        u.c[tmp]++; /* add a spell, more likely a lower-level spell */
        if (rnd() > 0.666)
            u.c[tmp-6] += 1; /* also increment corresponding UC_SPELLSAVEx */
    break;
    case SPC_SAF:
        if (u.c[UC_SAFE_COMBN] == 0)
            u.c[UC_SAFE_COMBN] = roll(1,4) + 10 * roll(1,4);
        if(debug_mode())
          {
            printf("[Combo = %d]\r\n", u.c[UC_SAFE_COMBN]);
          } /*if*/
saf_top1:
        printf("You see a small door with four colored lights in a row.\r\n");
        printf("They are red, green, blue, and orange...\r\n");
saf_top2:
        printf("Press <CR> to try a combination, <LF> to avoid it: ");
        tmp2 = getchar();
        if (tmp2 < 0)
            tmp2 = 10;
        printf("\r\n");
        if (tmp2 == '\n')
            break;
        if (tmp2 != '\r')
          {
            printf("Trying to be cute\007?\r\n");
            goto saf_top2;
          } /*if*/
        tmp = 0;
        sav = 0;
        printf("Press two colors: ");
        while (tmp < 2 && tmp != -1)
          {
            tmp2 = getchar();
            if (islower(tmp2))
                tmp2 = toupper(tmp2);
            switch(tmp2)
              {
            case 'R':
                printf("Red ");
                sav += 1;
            break;
            case 'G':
                printf("Green ");
                sav += 2;
            break;
            case 'B':
                printf("Blue ");
                sav += 3;
            break;
            case 'O':
                printf("Orange ");
                sav += 4;
            break;
            default:
                printf("\r\nThat's not a legal color\007!\r\n");
                printf("Try red, green, blue, or orange.\r\n");
                tmp = -2;
              } /*switch*/
            tmp++;
            sav *= 10;
          } /*while*/
        if (tmp < 0)
            goto saf_top2;
        if (sav == u.c[UC_SAFE_COMBN] * 10)
          {
            printf("\r\nYou got it\007!\r\n");
            u.c[UC_SAFE_COMBN] = 0;
            tmp = 5000 * rnd() * u.c[UC_DGNLVL] + 1000;
            printf("%d worth in gems and jewels pour out\007!\r\n", tmp);
            u.c[UC_GOLDFOUND] += tmp;
            dtmp = u.c[UC_DGNLVL] / (double) u.c[UC_LEVEL];
            if (dtmp > 1.0)
                dtmp = 1.0;
            u.c[UC_EXPGAIN] += tmp * dtmp;
            u.c[UC_STATE] = DGN_NEWLOC;
            return(NOPE);
          } /*if*/
        printf("\r\nAn electric shock jolts your body\007!\r\n");
        if (cbt_ohitu(u.c[UC_DGNLVL] * 2) == YEP)
            return(YEP);
        goto saf_top1;
    case SPC_RCK:
        printf("You're in solid rock!!!!!!!!\r\n");
        printf("You die of course...\r\n");
        sleep(2);
        return(utl_death());
    default:
        printf("spc: unknown special %d!\r\n", spc);
        unix_exit(1);
      } /*switch*/
    u.c[UC_STATE] = DGN_PROMPT;
    return(NOPE);
  } /*spc_main*/
