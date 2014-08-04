/*
 * udd swb.c file - main switchboard program
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 * Current version/patchlevel: */
/*#define VERS "UDD V5.1C-06  22-Oct-89" */
#define VERS "UDD V5.1C-07  01-Feb-2001"

#define MAIN /* put globals here */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pwd.h>
#include <stdio.h>
#include "defs.h"
#include "data.h"

int main
  (
    int argc,
    char ** argv
  )
  {
    int alevel = swb_ppnok();
    int cmdch;
    int wiz = swb_wiz();
    char *cmds = "RCPKIEOLSFMD", *cp;
    char nbuf[NAMELEN], *np;
    struct chr *cptr;
    struct passwd *pptr;
    ddd = *argv;
    srandom(time(0));
    dfd = n_dlvl = -1;
    if (argc != 1)
      {
        printf("%s: This command takes no options.\r\n");
        exit(1);
      } /*if*/
    if (alevel == NOPE)
      {
        printf("?You are restricted from this program.\r\n");
        exit(1);
      } /*if*/
    u.c[UC_STATE] = 0;
    unix_init();
    unix_tty_dgn();
    printf("\n\n\n\n\rWelcome to %s!  This is  %s.\n\r", ddd, VERS);
    printf("\r\n\n");
    swb_note("Current note:", FIL_NOT, 0);
    swb_note("ORB finders:", FIL_ORB, 1);
    printf("\r\n");
    for (;;)
      {
        printf("Hit \"L\" for a list of options.\r\n");
prompt_top:
        printf("%s> ", ddd);
        cmdch = getchar();
        if (cmdch < 0)
            cmdch = 'E';
        if (islower(cmdch))
            cmdch = toupper(cmdch);
        if (cmdch == ' ' || cmdch == '\r')
          {
            printf("\r\n");
            goto prompt_top;
          } /*if*/
        cp = index(cmds, cmdch);
        if (cp == NULL)
          {
            printf("\r\n%%Illegal option.\r\n");
            goto prompt_top;
          } /*if*/
        switch(cmdch)
          {
        case 'R':    /* Run */
            printf("Run\r\n");
            if (alevel == MAYBE)
              {
                printf("%%Restricted option.\r\n");
                break;
              } /*if*/
            printf("What is your name noble Sir ? ");
            unix_tty_cook();
            if (!fgets(u.n[0], NAMELEN, stdin))
                strcpy(u.n[0], "\n");
            unix_tty_dgn();
            fflush(stdin);
            u.n[0][strlen(u.n[0])-1] = 0;   /* nuke the \n */
            if (chr_load(u.n[0], NOLOCK) == NOPE)
              {
                printf("%%No such character.\r\n");
                break;
              } /*if*/
            printf("%s's SECRET NAME is : ", u.n[0]);
            unix_tty_pswd();
            if (!fgets(nbuf, NAMELEN, stdin))
                strcpy(nbuf, "\n");
            unix_tty_dgn();
            printf("\r\n");
            nbuf[strlen(nbuf)-1] = 0;
            if (strcmp(nbuf, u.n[1]) != 0)
              {
                printf("%%\007Body snatcher!!!!\007\r\n");
                printf("%s> Exit\r\n", ddd);
                unix_exit(1);
              } /*if*/
            if (chr_load(u.n[0], LOCK) != YEP)
              {
                printf("%s: Sorry, that character is in use (locked).\r\n", ddd);
                break;
              } /*if*/
            if (strcmp(nbuf, u.n[1]) != 0) /* not very possible, but... */
              {
                printf("%s: Sorry, that character is in use (locked).\r\n", ddd);
                chr_save(YEP);                 /* will unlock char */
                break;
              } /*if*/
            u.c[UC_STATE] = DGN_NEWADV;
            autosave = true;
            if (u.c[UC_DGNLVL] == 0)
                u.c[UC_DGNNR] = dlvl_choose();
            else
                dlvl_get_start(u.c[UC_DGNNR]);
            u.c[UC_LASTRUN] = time(0);
            printf("You are now descending into the dungeon:\r\n");
            printf("Please wait while we force open the main door..\r\n");
            printf("\n");
            dgn_main();
            autosave = false;
            if (u.c[UC_STATE] != SWB_CREATE)
                break;
            printf("\r\n%s> ", ddd);
            u.c[UC_STATE] = XXX_NORM;
      /* FALLTHRU */
        case 'C':    /* Create */
            for (;;) /* until successfully managed to save created character or given up */
              {
                bool done_create = false;
                bool saveit;
                int tmp;
                int sts[7];
                do /*once*/
                  {
                    printf("Create\r\n");
                    if (alevel == MAYBE)
                      {
                        printf("%%Restricted option.\r\n");
                        done_create = true;
                        break;
                      } /*if*/
                    printf("\r\n\n\n\n");
                    for (;;) /* generate character attributes */
                      {
                        int reply;
                        printf("Press <CR> to run this character, <LF> to try again\r\n");
                        printf("or \"Q\" to quit.\r\n\n");
                        for (;;)
                          {
                            for (tmp = 1; tmp <= 6 ; tmp++)
                              {
                                sts[tmp] = roll(1,8) + roll(1,6) + 4;
                                printf("%.3s %02d ", st + (tmp - 1) * 3, sts[tmp]);
                              } /*for*/
                            reply = getchar();
                            if (reply < 0 || reply == 'q' || reply == 'Q')
                              {
                                printf("\r\n");
                                saveit = false;
                                break;
                              } /*if*/
                            if (reply != '\n')
                                break;
                            printf("\r");
                          /* and generate another one */
                          } /*for*/
                        if (reply == '\r')
                          {
                            saveit = true;
                            break;
                          } /*if*/
                        printf("Read the directions!!!\007\r\n");
                        sleep(3);
                        fflush(stdin);
                        printf("\n\n\n\n");
                      /* and try again */
                      } /*for*/
                    if (!saveit)
                      {
                        done_create = true;
                        break;
                      } /*if*/
                    for (;;) /* get valid character name */
                      {
                        bool got_name = false; /* to begin with */
                        do /*once*/
                          {
                            printf("\r\nWhat is your name noble Sir ? ");
                            unix_tty_cook();
                            if (!fgets(nbuf, NAMELEN, stdin))
                                strcpy(nbuf, "\n");
                            unix_tty_dgn();
                            fflush(stdin);
                            nbuf[strlen(nbuf) - 1] = 0;
                            np = nbuf;
                            while (*np != 0)
                              {
                                if (*np < ' ' || *np > '~')
                                  {
                                    printf("Try using normal characters!\007\r\n");
                                    break;
                                  } /*if*/
                                np++;
                              } /*while*/
                            if (strlen(nbuf) == 0 || chr_load(nbuf, NOLOCK) == YEP)
                              {
                                if (nbuf[0] == 0)
                                  {
                                    printf("No-one but a monster has no name.\r\n");
                                    break;
                                  }
                                else
                                  {
                                    printf("\"How dare you steal my name!\", quoth the mighty %s.",
                                           nbuf);
                                    printf("\r\nPlease choose another name.\r\n");
                                    break;
                                  } /*if*/
                              } /*if*/
                            strcpy(u.n[0], nbuf);
                            u.c[UC_ALIVE] = 1;
                            for (tmp = 1 ; tmp < 7 ; tmp++)
                              /* UC_STRENGTH, UC_INTEL, UC_WISDOM, UC_CONSTIT, UC_DEX, UC_CHARISMA */
                                u.c[tmp] = sts[tmp];
                            printf("What is %s's SECRET NAME : ", nbuf);
                            unix_tty_pswd();
                            if (!fgets(u.n[1], NAMELEN, stdin))
                                strcpy(u.n[1], "\n");
                            unix_tty_dgn();
                            fflush(stdin);
                            printf("\r\n");
                            u.n[1][strlen(u.n[1]) - 1] = 0;
                            u.c[UC_LOCKED] = 0;
                            u.c[UC_GID] = getgid();
                            u.c[UC_UID] = getuid();
                            if (chr_new() == NOPE)     /* try and claim a slot */
                              {
                                printf("\"How dare you steal my name!\", quoth the mighty %s.",
                                       u.n[0]);
                                printf("\r\nPlease choose another name.\r\n");
                                break;
                              } /*if*/
                          /* passed all checks */
                            got_name = true;
                          }
                        while (false);
                        if (got_name)
                            break;
                      } /*for*/
                    for (;;) /* get character class */
                      {
                        bool got_class = true;
                        printf("What character class--(F)ighter, (C)leric, or (M)agic user? ");
                        int reply = getchar();
                        if (islower(reply))
                            reply = toupper(reply);
                        if (reply == 'F')
                            u.c[UC_CLASS] = CHRCLASS_FIGHTER;
                        else if (reply == 'C')
                            u.c[UC_CLASS] = CHRCLASS_CLERIC;
                        else if (reply == 'M')
                            u.c[UC_CLASS] = CHRCLASS_MAGICIAN;
                        else
                          {
                            printf("\r\nWake up, Jose!!!\r\n");
                            got_class = false;
                          } /*if*/
                        if (got_class)
                            break;
                      } /*for*/
                    printf("%s\r\n", class2[u.c[UC_CLASS]]);
                    u.c[UC_CURHIT] = u.c[UC_MAXHIT] = 6 + 2 * (2 - u.c[UC_CLASS]);
                    u.c[UC_LEVEL] = 1;
                    for (tmp = 12 ; tmp < 65 ; tmp++)
                        u.c[tmp] = 0;
                    u.c[UC_LOCKED] = 1; /* don't let us unlock by mistake */
                    u.c[UC_CREATED] = u.c[UC_LASTRUN] = time(0);
                    if (u.c[UC_CONSTIT] > 14)
                        u.c[UC_MAXHIT] = u.c[UC_CURHIT] = u.c[UC_MAXHIT] + u.c[UC_CONSTIT] - 14;
                    u.c[19] = 15; /* not used anywhere? */
                    u.c[20] = 2;  /* ??? */ /* not used anywhere? */
                    u.c[UC_DGNNR] = dlvl_choose();
                    if (u.c[UC_CLASS] == CHRCLASS_MAGICIAN)
                      {
                        u.c[UC_SPELLSADJ1] = u.c[UC_SPELLS1] = 3;
                        u.c[UC_SHIELD] = -1;
                      } /*if*/
                    if (u.c[UC_CLASS] == CHRCLASS_CLERIC)
                        u.c[UC_SPELLSADJ1] = u.c[UC_SPELLS1] = 2;
                    u.c[UC_GID] = getgid();
                    u.c[UC_UID] = getuid();
                    u.c[UC_EXP] = 0;
                    if (chr_save(NOPE) != YEP)     /* should just update our slot */
                      {
                        printf("%s: Internal error: Can't update char file!\r\n", ddd);
                        printf("%s: This should not happen!\r\n", ddd);
                        done_create = true;
                        break;
                      } /*if*/
                    printf("You are now descending into the dungeon:\r\n");
                    printf("Please wait while we force open the main door...\r\n");
                    u.c[UC_STATE] = DGN_NEWADV;
                    autosave = true;
                    dgn_main();
                    autosave = false;
                    if (u.c[UC_STATE] != SWB_CREATE)
                      {
                        done_create = true;
                      } /*if*/
                  }
                while (false);
                if (done_create)
                    break;
                printf("\r\n%s> ", ddd);
                u.c[UC_STATE] = XXX_NORM;
              } /*for*/
        break;
        case 'P':
        case 'M':
        case 'D':
              {
                int dngnnr;
                if (cmdch == 'P')
                    printf("List All Players\r\n");
                else if (cmdch == 'M')
                    printf("List My Players\r\n");
                else
                  {
                    printf("List Players in a Dungeon\r\n");
                    printf("Dungeon # : ");
                    dngnnr = getchar();
                    if (dngnnr < '0' || dngnnr > '9')
                      {
                        printf("A number please!\007\r\n");
                        break;
                      } /*if*/
                    printf("%c\r\n", dngnnr);
                    dngnnr = dngnnr - '0';
                  } /*if*/
                chr_rset();
                printf("Player     STR INT WIS CON DEX CHA LVL DGN RING ");
                printf("EXP         User          Type\r\n");
                while((cptr = chr_scan()) != NULL)
                  {
                    int tmp;
                    if (cptr->c[UC_ALIVE] == 0)   /* dead? */
                        continue;
                    if (cptr->c[UC_WIZONLY] != 0 && wiz == 0)
                        continue;
                    if (cmdch == 'M' && ntohl(cptr->c[UC_UID]) != getuid()) /* only me? */
                        continue;
                    if (cmdch == 'D' && ntohl(cptr->c[UC_DGNNR]) != dngnnr)      /* dungeon? */
                        continue;
                    printf("%-10.10s ", cptr->nam[0]);
                    for (tmp = 1 ; tmp < 7; tmp++)
                      /* UC_STRENGTH, UC_INTEL, UC_WISDOM, UC_CONSTIT, UC_DEX, UC_CHARISMA */
                        printf("%02d  ", ntohl(cptr->c[tmp]));
                    printf("%02d   %1d  ", ntohl(cptr->c[UC_LEVEL]), ntohl(cptr->c[UC_DGNNR]));
                     if (cptr->c[UC_RING] == 0)
                        printf("none ");
                    else
                        printf(" %02d  ", ntohl(cptr->c[UC_RING]));
                    printf("%-11d ", ntohl(cptr->c[UC_EXP]));
                    if ((pptr = getpwuid(ntohl(cptr->c[UC_UID]))) == NULL)
                        printf("u%-7d ", ntohl(cptr->c[UC_UID]));
                    else
                        printf("%-8s ", pptr->pw_name);
                    printf("%c%c%c%c ", (cptr->c[UC_DGNLVL] == 0) ? ' ' : '*',
                           (cptr->c[UC_DEBUGCHR] == 0) ? ' ' : '+',
                           (cptr->c[UC_WIZONLY] == 0) ? ' ' : '@',
                           (cptr->c[UC_LOCKED] == 0) ? ' ' : 'L');
                    int chrtype = ntohl(cptr->c[UC_CLASS]);
                    if (chrtype == CHRCLASS_CLERIC)
                        printf("CLRC\r\n");
                    else if (chrtype == CHRCLASS_MAGICIAN)
                        printf("MAGE\r\n");
                    else if (cptr->c[UC_SPELLSADJ1] + cptr->c[UC_SPELLSADJ2] + cptr->c[UC_SPELLSADJ3] + cptr->c[UC_SPELLSADJ4] != 0)
                        printf("F/MU\r\n");
                    else if (cptr->c[UC_SPELLS1] + cptr->c[UC_SPELLS2] + cptr->c[UC_SPELLS3] + cptr->c[UC_SPELLS4] != 0)
                        printf("HERO\r\n");
                    else
                        printf("FGTR\r\n");
                  } /*while*/
                printf("\r\n");
              }
        break;
        case 'K':
            printf("Kill\r\n");
            if (alevel == MAYBE)
              {
                printf("%%Restricted option.\r\n");
                break;
              } /*if*/
            printf("What is your name noble Sir? ");
            unix_tty_cook();
            if (!fgets(nbuf, NAMELEN, stdin))
                strcpy(nbuf, "\n");
            unix_tty_dgn();
            nbuf[strlen(nbuf) - 1] = 0;
            if (chr_load(nbuf, NOLOCK) == NOPE)
              {
                printf("%%No such character.\r\n");
                break;
              } /*if*/
            if (u.c[UC_LOCKED] != 0)
              {
                printf("That character is LOCKED (in use)!\r\n");
                printf("Sorry, can't kill a locked character...\r\n");
                break;
              } /*if*/
            if (wiz == 0)
              {
                unix_tty_pswd();
                printf("%s's SECRET NAME is? ", nbuf);
                if (!fgets(nbuf, NAMELEN, stdin))
                    strcpy(nbuf, "\n");
                unix_tty_dgn();
                printf("\r\n");
                nbuf[strlen(nbuf) - 1] = 0;
                if (strncmp(u.n[1], nbuf, NAMELEN) != 0)
                  {
                    printf("%%\007Grave robber\007!\007!\007!\r\n");
                    printf("%s> Exit\r\n", ddd);
                    unix_exit(1);
                  } /*if*/
              }
            else
                strcpy(nbuf, u.n[1]);
            printf("Are you sure you wish to die? ");
            int reply = getchar();
            if (reply == 'y' || reply == 'Y')
              {
                printf("Yes\r\n");
                if (chr_load(u.n[0], LOCK) != YEP)
                  {
                    printf("Sorry, that character was just locked...\r\n");
                    printf("Try again later.\r\n");
                    break;
                  } /*if*/
                printf("Goodbye life.....ARRRGGG.G..G.. .   .    .\r\n");
                if (strcmp(nbuf, u.n[1]) == 0)      /* diff char most likely */
                    chr_lck_nuke(NUKE);
                else
                    chr_save(YEP);             /* someone else char with same
                                                  name, created between the above
                                                  two calls to chr_load... not
                                                  very likely */
              }
            else
                printf("No\r\n");
            printf("\r\n");
        break;
        case 'I':
            printf("Instructions\r\n");
            swb_note("Current instruction file:", FIL_INS, wiz);
        break;
        case 'E':
            printf("Exit\r\n");
            unix_exit(0);
        break;
        case 'O':
            printf("Operator\r\n");
            if (wiz == 0)
              {
                printf("%% 'Operator' is a privileged command.\r\n");
                break;
              } /*if*/
            opr_main();
        break;
        case 'L':
            printf("List Options\r\n");
            printf("Options are:\r\n\n");
            printf("R\tRun a character\r\n");
            printf("C\tCreate a character\r\n");
            printf("P\tList all current players\r\n");
            printf("M\tList my own characters\r\n");
            printf("D\tList all players in one dungeon\r\n");
            printf("K\tKill a character\r\n");
            printf("S\tList all player's status (dates, et al.)\r\n");
            printf("F\tFind experience needed for a level\r\n");
            printf("I\tInstructions\r\n");
            printf("E\tExit program\r\n");
            printf("O\tGo to operator program (privileged option)\r\n");
            printf("L\tList options (this list)\r\n");
            printf("\r\n");
        break;
        case 'F':
            printf("Find experience for level\r\n");
            printf("For what character class Sire?\r\n");
find_top:
            printf(" (M)agician, (C)leric, or (F)ighter ? ");
            int chrtype = getchar();
            if (chrtype < 0)
                break;
            if (islower(chrtype))
                chrtype = toupper(chrtype);
            if (chrtype == 'M')
              {
                printf("Magician\r\n");
                u.c[UC_CLASS] = CHRCLASS_MAGICIAN;
              }
            else if (chrtype == 'C')
              {
                printf("Cleric\r\n");
                u.c[UC_CLASS] = CHRCLASS_CLERIC;
              }
            else if (chrtype == 'F')
              {
                printf("Fighter\r\n");
                u.c[UC_CLASS] = CHRCLASS_FIGHTER;
              }
            else
              {
                printf("\r\nEgad\007!  Try again.\r\n");
                goto find_top;
              } /*if*/
            printf("For level ? ");
            unix_tty_cook();
            if (!fgets(nbuf, NAMELEN, stdin))
                strcpy(nbuf, "\n");
            nbuf[strlen(nbuf) - 1] = 0;
            int levelnr = atoi(nbuf);
            unix_tty_dgn();
            fflush(stdin);
            if (levelnr < 1 || levelnr > 1000)
                printf("That's a little out of my range...\r\n");
            else
                printf("You need %d experience for level %d.\r\n", utl_exp(levelnr), levelnr);
        break;
        case 'S':
            printf("Status\r\n");
            chr_rset();
            printf("Player       Created   Last Run   Total_Gold  Experience    ");
            printf("User          Type\r\n");
            while ((cptr = chr_scan()) != NULL)
              {
                if (cptr->c[UC_ALIVE] == 0)
                    continue;
                if (cptr->c[UC_WIZONLY] != 0 && wiz == 0)
                    continue;
                printf("%-10.10s  %s  ", cptr->nam[0], unix_date(ntohl(cptr->c[UC_CREATED])));
                printf("%s  ", unix_date(ntohl(cptr->c[UC_LASTRUN])));
                printf("%-10d  ", ntohl(cptr->c[UC_TOTALGOLD]));
                printf("%-10d    ", ntohl(cptr->c[UC_EXP]));
                if ((pptr = getpwuid(ntohl(cptr->c[UC_UID]))) == NULL)
                    printf("u%-7d  ", ntohl(cptr->c[UC_UID]));
                else
                    printf("%-8s ", pptr->pw_name);
                printf("%c%c%c%c ", (cptr->c[UC_DGNLVL] == 0) ? ' ' : '*',
                       (cptr->c[UC_DEBUGCHR] == 0) ? ' ' : '+',
                       (cptr->c[UC_WIZONLY] == 0) ? ' ' : '@',
                       (cptr->c[UC_LOCKED] == 0) ? ' ' : 'L');
                int chrtype = ntohl(cptr->c[UC_CLASS]);
                if (chrtype == CHRCLASS_CLERIC)
                    printf("CLRC\r\n");
                else if (chrtype == CHRCLASS_MAGICIAN)
                    printf("MAGE\r\n");
                else if (cptr->c[UC_SPELLSADJ1] + cptr->c[UC_SPELLSADJ2] + cptr->c[UC_SPELLSADJ3] + cptr->c[UC_SPELLSADJ4] != 0)
                    printf("F/MU\r\n");
                else if (cptr->c[UC_SPELLS1] + cptr->c[UC_SPELLS2] + cptr->c[UC_SPELLS3] + cptr->c[UC_SPELLS4] != 0)
                    printf("HERO\r\n");
                else
                    printf("FGTR\r\n");
              } /*while*/
            printf("\r\n");
        break;
        default:
            printf("\r\n%d: This can't happen!\r\n", ddd);
          } /*switch*/
      } /*for*/
  } /*main*/
