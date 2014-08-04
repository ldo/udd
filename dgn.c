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

char *cmdstr = "01234SWDXACR\r\n\033KQUH";   /* for pointer */

int dgn_main(void)
{
  int i = u.c[63], x, y;
  int l, in, valid;
  char *icp;
  int done = NOPE;
  while (done == NOPE) {
    i = u.c[63];
    switch (u.c[UC_STATE]) {
    case DGN_NEWADV:
      u.c[UC_STATE] = DGN_NEWLOC;
      if (u.c[UC_DGNLVL] != 0) {   /* resume? */
        utl_inilvl();
        printf("\r\n\nResuming your expedition sire.\r\n");
      } else {
        u.c[UC_DGNLVL] = dd.dstart;
        u.c[UC_MAXHIT] = u.c[UC_CURHIT];
        for (i = 31; i <= 36 ; i++)
          u.c[i] = u.c[i - 6];
        u.c[UC_DGN_X] = u.c[UC_DGNLVL] / 20.0 + 1;
        u.c[UC_DGN_Y] = u.c[UC_DGNLVL] % 20 + 1;
        if (u.c[UC_DGN_X] < 1 || u.c[UC_DGN_X] > 20)
          u.c[UC_DGN_X] = 1;
        if (u.c[UC_DGN_Y] < 1 || u.c[UC_DGN_Y] > 20)
          u.c[UC_DGN_Y] = 1;
        u.c[UC_DGNLVL] = 1;
        u.c[UC_GOLDFOUND] = u.c[UC_EXPGAIN] = 0;
        for (i = 37 ; i <= 47 ; i++)
          u.c[i] = 0;
        utl_inilvl();
      }
      /* fall through */
    case DGN_NEWLOC:
      i = fni(u.l[u.c[UC_DGN_X]][u.c[UC_DGN_Y]]);
      if (u.i[5] == 15 && rnd() < 0.950) {       /* oops, solid rock! */
        utl_pplot(NOPE);
        printf("You have entered solid rock!\r\n");
        printf("The Gods are kind, so, instead of dying, ");
        printf("you have been moved.\r\n");
        u.c[UC_STATE] = SPC_TPTNOW;
        done = spc_main();
        break;
      }
      /* regenerate dungeon, espc if he's got the ORB */
      if (rnd() < 0.1 || (u.c[UC_HASORB] == 1 && rnd() > 0.6)) {
        u.i[6] = 1;
        if (rnd() > 0.5) {
          u.i[7] = 1;
          if (rnd() > 0.5)  /* CDC .1? */
            u.i[8] = 1;
          else
            u.i[8] = 0;
        }
      }
      utl_pplot(YEP);
      l = u.c[UC_DGNLVL];
      u.c[63] = 0;
      u.c[UC_STATE] = XXX_NORM;
      /* encounters */
      if (u.i[6] == 1 && ( (u.c[46] > 0 && rnd() > 0.7) ||
                           (u.c[UC_ELVEN_BOOTS] / 30.0 > rnd()) )) {
        printf("You have not been seen...\r\n");
        printf("Press <CR> to approach, or <LF> to ignore it: ");
        in = getchar();
        printf("\r\n");
        if (in != '\r')
          u.i[6] = u.i[7] = 0;
        else {
          if (u.c[47] > 0 || (u.c[46] > 0 && rnd() > 0.3))
            u.i[6] = 0;      /* TMST, or INVS */
          else {
            if (u.c[45] > 0 && rnd() > 0.2)
              u.i[7] = 1;    /* STRG for treasure */
            done = cbt_main(); /* XXX mand break? */
            break;
          }
        }
      } else if (u.i[6] == 1) {
        if (u.c[47] > 0 || (u.c[46] > 0 && rnd() > 0.3))
          u.i[6] = 0;      /* TMST, or INVS */
        else {
          if (u.c[45] > 0 && rnd() > 0.2)
            u.i[7] = 1;    /* STRG for treasure */
          done = cbt_main(); /* XXX mand break? */
          break;
        }
      }
      /* treasure */
      u.c[63] = l;
      if (u.i[7] != 0) {
        done = trs_main();
        break;
      }
      u.c[UC_STATE] = XXX_NORM;
      /* specials */
      if (u.c[UC_STATE] != SPC_NORM) {
        done = spc_main();
        break;
      }
      u.i[6] = u.i[7] = u.i[8] = 0;
      u.l[u.c[UC_DGN_X]][u.c[UC_DGN_Y]] =
        1024*u.i[8]+512*u.i[7]+256*u.i[6]+16*u.i[5]+4*u.i[2]+u.i[1];
      if (u.i[5] != 0) {
        done = spc_main();
        break;
      }
      u.c[UC_STATE] = DGN_PROMPT;
      /* fall through */
    case DGN_PROMPT:
      x = u.c[UC_DGN_X];
      y = u.c[UC_DGN_Y];
      if (u.c[UC_CURHIT] > u.c[UC_MAXHIT])
        u.c[UC_CURHIT] = u.c[UC_MAXHIT];
      dgn_voices();
      valid = 0;
      if (u.c[UC_DEBUGCHR] == 1)
        printf("X=%d, Y=%d\r\n", u.c[UC_DGN_X], u.c[UC_DGN_Y]);
    dgn_no:
      while (valid == 0) {
        printf("->");
        in = getchar();
        if (in < 0) { /* XXX */
          printf("getchar() ERROR -- STOP\r\n");
          unix_exit(1);
        }
        if (islower(in))
          in = toupper(in);
        icp = index(cmdstr, in);
        if (icp != NULL)
          valid = i = icp - cmdstr + 1;
        else
          printf("\rNo\007\r");
      }
      if (i > 10) {        /* non-movement command ? */
        i = i - 2;
        done = dgn_nomove(i - 8);
        break;
      }
      i--;
      if (i > 4)
        i -= 5;
      valid = 0;
      switch (i) {
      case 1:
        if (u.i[2] != 1) {
          valid = 1;
          printf("North\r\n");
        }
        break;
      case 4:
        if (u.i[1] != 1) {
          valid = 1;
          printf("West\r\n");
        }
        break;
      case 2:
        if (fni1(u.l[x][y+1], 1) != 1) {
          valid = 1;
          printf("East\r\n");
        }
        break;
      case 3:
        if (fni1(u.l[x+1][y], 2) != 1) {
          valid = 1;
          printf("South\r\n");
        }
        break;
      default:
        valid = 1;
        printf("Stay here\r\n");
        i = 0;
        break;
      }
      if (valid == 0) {
        printf("\rNo\007\r");
        goto dgn_no;
      }
    case DGN_AMOVE:
      u.l[u.c[UC_DGN_X]][u.c[UC_DGN_Y]] =
        1024*u.i[8]+512*u.i[7]+256*u.i[6]+16*u.i[5]+4*u.i[2]+u.i[1];
      u.c[UC_DGN_X] += q[i][1];
      u.c[UC_DGN_Y] += q[i][2];
      if (u.c[UC_DGN_X] < 1 || u.c[UC_DGN_X] > 20 || u.c[UC_DGN_Y] < 1 || u.c[UC_DGN_Y] > 20)
        done = utl_escape();
      else
        u.c[UC_STATE] = DGN_NEWLOC;
      break;
    default:
      printf("panic: dgn ERROR!  Unknown command: %d\n\r", u.c[UC_STATE]);
      unix_exit(1);
    }
  }
}
