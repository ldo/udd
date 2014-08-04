/*
 * udd utl_pplot.c file - position plot
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include <string.h>
#include <stdio.h>
#include "defs.h"

static char obuf[BUFSIZ];
static int ccpos;         /* current cursor position */
static int cobps;         /* current end of obuf position */

#define fnd3(c, z) vaz[check_room(u.p[c][z], ROOM_WALL_VISIBLE_NORTH) + 1]
#define fnd2(c, z) maz[check_room(u.p[c][z], ROOM_WALL_VISIBLE_WEST)]
#define fnd1(c, z) ((check_room(u.p[c][z], ROOM_WALL_VISIBLE_WEST) == 0) ? " " : "I")

/* generate opt. output */
static int optout
  (
    char * s
  )
  /* appends s (stripped of leading and trailing blanks) as a separate
    line to obuf, such that it will appear at a position on the line
    offset by the number of leading blanks. All this messing about seems
    to be to try to minimize cursor movement. Returns the updated cursor
    position (which is never used by caller). */
  {
    char *sp;
    int firstc = 1, lcv;
    sp = s + strlen(s) - 1;
    while(*sp == ' ' && sp > s) /* strip trailing blanks */
        *sp-- = '\0';
    sp = s;
#ifdef OUT_DEBUG
    printf("%s\r\n", s);
#endif
    obuf[cobps++] = '\n'; /* down one line */
    obuf[cobps] = '\0';
    while (*sp == ' ') /* don't output leading blanks, just update cursor position */
      {
        sp++;
        firstc++;
      } /*while*/
    if (*sp == '\0') /* empty string */
      {
        obuf[cobps] = '\0';
        return
            ccpos;
      } /*if*/
    if (ccpos / 2 > firstc)          /* too far to backspace */
      {
        obuf[cobps++] = '\r'; /* back to start of line */
        strcpy(obuf + cobps, s); /* offset by leading blanks */
        ccpos = strlen(s) + 1;
        cobps += ccpos - 1;
        return
            ccpos;
      } /*if*/
    if (ccpos > firstc)                /* backspace */
      {
        for (lcv = 0; lcv < ccpos - firstc; lcv++)
            obuf[cobps++] = '\b';
        ccpos = firstc;
      } /*if*/
    while (ccpos < firstc)             /* space over if needed */
      {
        obuf[cobps++] = ' ';
        ccpos++;
      } /*while*/
    obuf[cobps] = '\0';
    strcat(obuf + cobps, sp);
    lcv = strlen(sp);
    ccpos += lcv;
    cobps += lcv;
    return
        ccpos;
  } /*optout*/


static void analyze_room
  (
    int r, /* encoded bitmask representing room state */
    int flags[9]
  )
  /* unpacks bits from r into flags. */
  {
    flags[ROOM_WALL_WEST] = flags[ROOM_WALL_VISIBLE_WEST] = (r & 3);           /* sec/nonsec top */
    r = r >> 2;
    flags[ROOM_WALL_NORTH] = flags[ROOM_WALL_VISIBLE_NORTH] = (r & 3);           /* sec/nonsec side */
    r = r >> 2;
    if (u.c[UC_SPELL_LIGHT] < 1 && flags[ROOM_WALL_VISIBLE_WEST] == 3 && rnd() < 0.85)
        flags[ROOM_WALL_VISIBLE_WEST] = 1;
    if (u.c[UC_SPELL_LIGHT] < 1 && flags[ROOM_WALL_VISIBLE_NORTH] == 3 && rnd() < 0.85)
        flags[ROOM_WALL_VISIBLE_NORTH] = 1;
    flags[ROOM_SPECIAL] = (r & 15);                   /* special */
    r = r >> 4;
    flags[ROOM_MONSTER] = (r & 1);
    r = r >> 1;
    flags[ROOM_TREASURE] = (r & 1);
    flags[ROOM_TREASURE_BOOBYTRAPPED] = r >> 1;
  } /*analyze_room*/

int enter_room
  (
    int r /* encoded bitmask representing room state */
  )
  /* analyzes the current room into u.i. */
  {
    analyze_room(r, u.i);
    return
        u.i[ROOM_SPECIAL];
  } /*enter_room*/

void save_room
  (
    bool full_contents
  )
  /* packs u.i into an encoded bitmask for the current room. */
  {
    u.l[u.c[UC_DGN_X]][u.c[UC_DGN_Y]] =
            (full_contents ?
                1024 * u.i[ROOM_TREASURE_BOOBYTRAPPED] + 512 * u.i[ROOM_TREASURE] + 256 * u.i[ROOM_MONSTER]
            :
                0
            )
        +
            16 * u.i[ROOM_SPECIAL] + 4 * u.i[ROOM_WALL_NORTH] + u.i[ROOM_WALL_WEST];
  } /*save_room*/

int check_room
  (
    int r, /* encoded bitmask representing room state */
    int a /* index into i1 array for value to return, or 9 to check for presence of special/monster/treasure */
  )
  /* analyzes a room other than the current one. */
  {
    int flags[9];
    analyze_room(r, flags);
    if (a < ROOM_ANYTHING)
        return
            flags[a];
    if ((flags[ROOM_SPECIAL] | flags[ROOM_MONSTER] | flags[ROOM_TREASURE]) == 0)
        return
            0;
    return
        1;
  } /*check_room*/


static char *fnp
  (
    char * a,
    const char * b,
    int p
  )
  /* put B in A at P */
  {
    int la, lb, lcv;
    la = strlen(a);
    lb = strlen(b);
    if (p > la)                          /* off the edge ? */
        strcat(a, b);
    else if ((p - 1) + lb > la)        /* will go off end? */
      {
        strcpy(a + (p - 1), b);
        return(a);
      }
    else
      {                             /* must be all the way in a */
        for (lcv = 0 ; lcv < lb ; lcv++)
            a[(p - 1) + lcv] = b[lcv];
      } /*if*/
    return
        a;
  } /*fnp*/

static char *fnp1
  (
    char * a,
    const char * b,
    int p
  )
  /* cond put B in A at P */
  {
    int la, lb, lcv, null;
    char *ap;
    la = strlen(a);
    lb = strlen(b);
    if (p > la)
        strcat(a, b);
    else
      {
        null = (lb + (p - 1) > la) ? 1 : 0;
        ap = a + (p - 1);
        for (lcv = 0 ; lcv < lb ; lcv++)
            if (p + lcv > la)
                *ap++ = *b++;
            else
              {
                if (*ap != ' ')
                  {
                    ap++;
                    b++;
                  }
                else
                    *ap++ = *b++;
              } /*if*/
        if (null != 0)
            *ap = '\0';
      } /*if*/
    return
        a;
  } /*fnp1*/


void utl_pplot
  (
    int regen
  )
  /* displays the room the player is in. */
  {
    int lcv, lcv3, x, y;
    char tbuf[80], tbuf2[80], tbuf3[80];
    if (regen == YEP)
      {
        for (lcv = 37; lcv <= 47 ; lcv++) /* decrement all UC_SPELL_xxx in effect */
            if (u.c[lcv] > 0)
                u.c[lcv]--;
        if (u.c[UC_RING] < 0)
            u.c[UC_RING] = 0;
        u.c[UC_CURHIT] += u.c[UC_RING];
      } /*if*/
    printf("\r\n\n");
    x = u.c[UC_DGN_X];
    y = u.c[UC_DGN_Y];
    for (lcv = 0 ; lcv < 11 ; lcv++)
      {
        if (u.c[SPLBASE + lcv] > 0) /* report state of all UC_SPELL_xxx */
            printf("%.4s:", lcv*4 + "LGHTPROTSHLDPRAYDTRPSLNCLEVTSTRGFEARINVSTMST");
      } /*for*/
    printf("\r\n\n");
    for (lcv = -1 ; lcv <= 2 ; lcv++)
        for (lcv3 = -1 ; lcv3 <= 2 ; lcv3++)
            if (x + lcv > 21 || y + lcv3 > 21)
                u.p[lcv + 2][lcv3 + 2] = 0;
            else
                u.p[lcv + 2][lcv3 + 2] = u.l[x + lcv][y + lcv3];
    ccpos = 1;
    cobps = 0;
    if (x != 1 && check_room(u.p[2][2], ROOM_WALL_VISIBLE_NORTH) == 0)       /* print top if one */
      {
        strcpy(tbuf,"      ");
        strcat(tbuf,fnd3(1,2));
        optout(tbuf);
        strcpy(tbuf,"      ");
        strcpy(tbuf2, tbuf);
        strcat(tbuf, fnd1(1,2));
        strcat(tbuf, "     ");
        strcat(tbuf, fnd1(1,3));
        strcat(tbuf2, fnd2(1,2));
        strcat(tbuf2, "     ");
        strcat(tbuf2, fnd2(1,3));
        if (u.c[UC_SPELL_LIGHT] > 0)
            if (check_room(u.p[1][2], ROOM_ANYTHING) != 0)     /* light spell */
                tbuf2[9] = '*';
        optout(tbuf);
        optout(tbuf2);
        optout(tbuf);
      } /*if*/
    if (y != 1 && check_room(u.p[2][2], ROOM_WALL_VISIBLE_WEST) == 0)
      {
        strcpy(tbuf, fnd3(2,1));
      }
    else
      {
        strcpy(tbuf, "       ");
      } /*if*/
    fnp1(tbuf, fnd3(2,2), 7);
    if (check_room(u.p[2][3], ROOM_WALL_VISIBLE_WEST) == 0)
        fnp1(tbuf, fnd3(2,3), 13);
    if (u.i[ROOM_WALL_VISIBLE_NORTH] == 0)
      {
        fnp1(tbuf, fnd1(1,2), 7);
        fnp1(tbuf, fnd1(1,3), 13);
      } /*if*/
    optout(tbuf);
    strcpy(tbuf, "                   ");
    strcpy(tbuf2,"                   ");
    if (y != 1 && check_room(u.p[2][2], ROOM_WALL_VISIBLE_WEST) == 0)
      {
        fnp1(tbuf, fnd1(2,1), 1);
        fnp1(tbuf2,fnd2(2,1), 1);
        if (u.c[UC_SPELL_LIGHT] > 0 && check_room(u.p[2][1], ROOM_ANYTHING) != 0)
            fnp(tbuf2, "*", 4);
      } /*if*/
    fnp1(tbuf, fnd1(2,2), 7);
    fnp1(tbuf2,fnd2(2,2), 7);
    fnp1(tbuf, fnd1(2,3),13);
    fnp1(tbuf2,fnd2(2,3),13);
    if (tbuf[12] == ' ')
      {
        fnp1(tbuf, fnd1(2,4), 19);
        fnp1(tbuf2,fnd2(2,4), 19);
        if (u.c[UC_SPELL_LIGHT] > 0 && check_room(u.p[2][3], ROOM_ANYTHING) != 0)
            fnp(tbuf2, "*", 16);
      } /*if*/
    fnp1(tbuf2, "X", 10);
    strcpy(tbuf3, tbuf);
    if (u.i[ROOM_TREASURE] == 1)
        fnp(tbuf, "$", 11);
    if (u.i[ROOM_MONSTER] == 1)
        fnp(tbuf, "#", 11);
    if ((u.i[ROOM_MONSTER] | u.i[ROOM_TREASURE]) == 0 && u.i[ROOM_SPECIAL] != 0)
        fnp(tbuf,spc[u.i[ROOM_SPECIAL] - 1], 9);
    optout(tbuf);
    optout(tbuf2);
    optout(tbuf3);
    lcv3 = strlen(tbuf3);
    if (7 > lcv3 || tbuf3[6]  == ' ')
        strcpy(tbuf,fnd3(3,1));
    else
        strcpy(tbuf,"       ");
    fnp1(tbuf, fnd3(3,2), 7);
    if (13 > lcv3 || tbuf3[12] == ' ')
        fnp1(tbuf, fnd3(3,3), 13);
    if (tbuf[9] == ' ')
      {
        fnp1(tbuf, fnd1(3,2), 7);
        fnp1(tbuf, fnd1(3,3),13);
      } /*if*/
    optout(tbuf);
    lcv = strlen(tbuf);
    if (10 > lcv || tbuf[9] == ' ')
      {
        strcpy(tbuf,"      ");
        strcat(tbuf,fnd1(3,2));
        strcat(tbuf,"     ");
        strcat(tbuf,fnd1(3,3));
        strcpy(tbuf2,"      ");
        strcat(tbuf2,fnd2(3,2));
        strcat(tbuf2,"     ");
        strcat(tbuf2,fnd2(3,3));
        if (u.c[UC_SPELL_LIGHT] > 0 && check_room(u.p[3][2], ROOM_ANYTHING) != 0)
            fnp(tbuf2, "*", 10);
        strcpy(tbuf3, "      ");
        strcat(tbuf3,fnd3(4,2));
        optout(tbuf);
        optout(tbuf2);
        optout(tbuf);
        optout(tbuf3);
      } /*if*/
    printf("%s\n\r", obuf);
  } /*utl_pplot*/
