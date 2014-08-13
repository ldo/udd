/*
 * udd dgn.c file - main dungeon program
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#include "defs.h"

static const char * const cmdstr = "01234SWDXACR\r\n\033KQUH";   /* for pointer */

#define NVOCS 12
#define ADDDIR 9

static const char * const vocs[] = {
  "You hear a mysterious sound from behind....." ,
  "A voice says 'PLUGH'....." ,
  "Beware the Gulf of Nazguk!!!!   A voice screams in the distance." ,
  "You hear footsteps and breathing behind you...." ,
  "A cold wind suddenly springs up and dies...." ,
  "\"TURN BACK!!!!\"  A voice screams, \"For you too will die ahead!!!...\"",
  "Suddenly, there is silence........." ,
  "A mutilated body lies on the floor nearby." ,
  "The room vibrates as if an army is passing by." ,
  "A resonant voice says \"May I take your hat and goat Sir?\"." ,
  "You hear slurping noises to the " ,
  "You hear rustling noises from the "
};

static const char * const dirs[] = { "north.", "east.", "south.", "west." };

void dgn_voices(void)
{
  int i;
  if ((random() % 101) > 5)
    return;
  i = random() % NVOCS;
  printf("%s", vocs[i]);
  if (i > ADDDIR)
    printf("%s", dirs[ random() % 4 ]);
  printf("\r\n");
}

void dgn_main(void)
  /* the main part of the game--keeps making moves until player exits or dies. */
  {
    for (;;)
      {
        int done = NOPE;
        int value = u.c[UC_VALUE];
        switch (u.c[UC_STATE])
          {
        case DGN_NEWADV:
            u.c[UC_STATE] = DGN_NEWLOC;
            if (u.c[UC_DGNLVL] != 0)   /* resume? */
              {
                utl_inilvl();
                printf("\r\n\nResuming your expedition sire.\r\n");
              }
            else
              {
                int j;
                u.c[UC_MAXHIT] = u.c[UC_CURHIT];
                for (j = 31; j <= 36 ; j++)
                  /* UC_SPELLS1, UC_SPELLS2, UC_SPELLS3, UC_SPELLS4, unused, unused = UC_SPELLSAVE1, UC_SPELLSAVE2, UC_SPELLSAVE3, UC_SPELLSAVE4, unused, unused */
                  /* reset spell counts to saved spell counts */
                    u.c[j] = u.c[j - 6];
                u.c[UC_DGN_X] = dd.dstart / 20 + 1;
                u.c[UC_DGN_Y] = dd.dstart % 20 + 1;
                if (u.c[UC_DGN_X] < 1 || u.c[UC_DGN_X] > 20)
                    u.c[UC_DGN_X] = 1;
                if (u.c[UC_DGN_Y] < 1 || u.c[UC_DGN_Y] > 20)
                    u.c[UC_DGN_Y] = 1;
                u.c[UC_DGNLVL] = 1; /* entrance/exit always on top level */
                u.c[UC_GOLDFOUND] = u.c[UC_EXPGAIN] = 0;
                for (j = 37 ; j <= 47 ; j++) /* turn off all UC_SPELL_xxx */
                    u.c[j] = 0;
                utl_inilvl();
              } /*if*/
        /* fall through */
        case DGN_NEWLOC:
              {
                int level;
                value = enter_room(u.l[u.c[UC_DGN_X]][u.c[UC_DGN_Y]]);
                if (u.i[ROOM_SPECIAL] == SPC_RCK && rnd() < 0.950)       /* oops, solid rock! */
                  {
                    utl_pplot(NOPE);
                    printf("You have entered solid rock!\r\n");
                    printf("The Gods are kind, so, instead of dying, ");
                    printf("you have been moved.\r\n");
                    u.c[UC_STATE] = SPC_TPTNOW;
                    done = spc_main();
                    break;
                  } /*if*/
                /* regenerate dungeon, espc if he's got the ORB */
                if (rnd() < 0.1 || (u.c[UC_HASORB] == 1 && rnd() > 0.6))
                  {
                    u.i[ROOM_MONSTER] = 1;
                    if (rnd() > 0.5)
                      {
                        u.i[ROOM_TREASURE] = 1;
                        if (rnd() > 0.5)  /* CDC .1? */
                            u.i[ROOM_TREASURE_BOOBYTRAPPED] = 1;
                        else
                            u.i[ROOM_TREASURE_BOOBYTRAPPED] = 0;
                      } /*if*/
                  } /*if*/
                utl_pplot(YEP);
                level = u.c[UC_DGNLVL];
                u.c[UC_VALUE] = 0;
                u.c[UC_STATE] = XXX_NORM;
                /* encounters */
                if
                  (
                        u.i[ROOM_MONSTER] == 1
                    &&
                        (
                            u.c[UC_SPELL_INVS] > 0 && rnd() > 0.7
                        ||
                            u.c[UC_ELVEN_BOOTS] / 30.0 > rnd()
                        )
                  )
                  {
                    int in;
                    printf("You have not been seen...\r\n");
                    printf("Press <CR> to approach, or <LF> to ignore it: ");
                    in = getchar();
                    printf("\r\n");
                    if (in != '\r')
                        u.i[ROOM_MONSTER] = u.i[ROOM_TREASURE] = 0;
                    else
                      {
                        if (u.c[UC_SPELL_TMST] > 0 || (u.c[UC_SPELL_INVS] > 0 && rnd() > 0.3))
                            u.i[ROOM_MONSTER] = 0;      /* TMST, or INVS */
                        else
                          {
                            if (u.c[UC_SPELL_FEAR] > 0 && rnd() > 0.2)
                                u.i[ROOM_TREASURE] = 1;    /* STRG for treasure */
                            done = cbt_main(); /* XXX mand break? */
                            break;
                          } /*if*/
                      } /*if*/
                  }
                else if (u.i[ROOM_MONSTER] == 1)
                  {
                    if (u.c[UC_SPELL_TMST] > 0 || (u.c[UC_SPELL_INVS] > 0 && rnd() > 0.3))
                        u.i[ROOM_MONSTER] = 0;      /* TMST, or INVS */
                    else
                      {
                        if (u.c[UC_SPELL_FEAR] > 0 && rnd() > 0.2)
                            u.i[ROOM_TREASURE] = 1;    /* STRG for treasure */
                        done = cbt_main(); /* XXX mand break? */
                        break;
                      } /*if*/
                  } /*if*/
                /* treasure */
                u.c[UC_VALUE] = level;
                if (u.i[ROOM_TREASURE] != 0)
                  {
                    done = trs_main();
                    break;
                  } /*if*/
                u.c[UC_STATE] = XXX_NORM;
                /* specials */
                if (u.c[UC_STATE] != SPC_NORM)
                  {
                    done = spc_main();
                    break;
                  } /*if*/
                u.i[ROOM_MONSTER] = u.i[ROOM_TREASURE] = u.i[ROOM_TREASURE_BOOBYTRAPPED] = 0;
                save_room(true);
                if (u.i[ROOM_SPECIAL] != 0)
                  {
                    done = spc_main();
                    break;
                  } /*if*/
                u.c[UC_STATE] = DGN_PROMPT;
              }
        /* fall through */
        case DGN_PROMPT:
              {
                bool valid;
                const int x = u.c[UC_DGN_X];
                const int y = u.c[UC_DGN_Y];
                if (u.c[UC_CURHIT] > u.c[UC_MAXHIT])
                    u.c[UC_CURHIT] = u.c[UC_MAXHIT];
                dgn_voices();
                valid = false;
                if (debug_mode())
                    printf("X=%d, Y=%d\r\n", u.c[UC_DGN_X], u.c[UC_DGN_Y]);
dgn_no:
                while (!valid)
                  {
                    int in;
                    const char *icp;
                    printf("->");
                    in = getchar();
                    if (in < 0) /* XXX */
                      {
                        printf("getchar() ERROR -- STOP\r\n");
                        unix_exit(1);
                      } /*if*/
                    if (islower(in))
                        in = toupper(in);
                    icp = index(cmdstr, in);
                    if (icp != NULL)
                      {
                        value = icp - cmdstr + 1;
                        valid = true;
                      }
                    else
                        printf("\rNo\007\r");
                  } /*while*/
                if (value > 10)        /* non-movement command ? */
                  {
                    value = value - 2;
                    done = dgn_nomove(value - 8);
                    break;
                  } /*if*/
                value--;
                if (value > 4)
                    value -= 5;
                valid = false;
                switch (value)
                  {
                case 1:
                    if (u.i[ROOM_WALL_NORTH] != 1)
                      {
                        valid = true;
                        printf("North\r\n");
                      } /*if*/
                break;
                case 4:
                    if (u.i[ROOM_WALL_WEST] != 1)
                      {
                        valid = true;
                        printf("West\r\n");
                      } /*if*/
                break;
                case 2:
                    if (check_room(u.l[x][y+1], ROOM_WALL_WEST) != 1)
                      {
                        valid = true;
                        printf("East\r\n");
                      } /*if*/
                break;
                case 3:
                    if (check_room(u.l[x+1][y], ROOM_WALL_NORTH) != 1)
                      {
                        valid = true;
                        printf("South\r\n");
                      } /*if*/
                break;
                default:
                    valid = true;
                    printf("Stay here\r\n");
                    value = 0;
                break;
                  } /*switch*/
                if (!valid)
                  {
                    printf("\rNo\007\r");
                    goto dgn_no;
                  } /*if*/
              }
        /* fall through */
        case DGN_AMOVE:
            save_room(true);
            u.c[UC_DGN_X] += q[value][1];
            u.c[UC_DGN_Y] += q[value][2];
            if (u.c[UC_DGN_X] < 1 || u.c[UC_DGN_X] > 20 || u.c[UC_DGN_Y] < 1 || u.c[UC_DGN_Y] > 20)
                done = utl_escape();
            else
                u.c[UC_STATE] = DGN_NEWLOC;
        break;
        default:
            printf("panic: dgn ERROR!  Unknown command: %d\n\r", u.c[UC_STATE]);
            unix_exit(1);
          } /*switch*/
        if (done != NOPE)
            break;
      } /*for*/
  } /*dgn_main*/
