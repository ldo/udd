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

#define north_side_str(c, z) vaz[check_room(u.p[c][z], ROOM_WALL_VISIBLE_NORTH) + 1]
  /* what to display for north side of a room */
#define west_side_inner_str(c, z) maz[check_room(u.p[c][z], ROOM_WALL_VISIBLE_WEST)]
  /* what to display for west side of a room, inner part */
#define west_side_outer_str(c, z) ((check_room(u.p[c][z], ROOM_WALL_VISIBLE_WEST) == 0) ? " " : "I")
  /* what to display for west side of a room, outer parts */

static int optout
  (
    char * s /* non-const so trailing blanks can be stripped */
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
    flags[ROOM_SPECIAL] = (r & 15);
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
    int a /* index into state array for value to return, or ROOM_ANYTHING to check for presence of special/monster/treasure */
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


static char *insert_at
  (
    char * a,
    const char * b,
    int p /* 1-based string index */
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
        return
            a;
      }
    else
      {                             /* must be all the way in a */
        for (lcv = 0 ; lcv < lb ; lcv++)
            a[(p - 1) + lcv] = b[lcv];
      } /*if*/
    return
        a;
  } /*insert_at*/

static char *insert_at_blank
  (
    char * a,
    const char * b,
    int p /* 1-based string index */
  )
  /* cond put B in A at P--only replace nonblank chars. */
  {
    const int la = strlen(a);
    const int lb = strlen(b);
    if (p > la)
        strcat(a, b);
    else
      {
        const bool addnull = lb + (p - 1) > la;
        char *ap = a + (p - 1);
        int lcv;
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
        if (addnull)
            *ap = '\0';
      } /*if*/
    return
        a;
  } /*insert_at_blank*/


void utl_pplot
  (
    int regen
  )
  /* displays the room the player is in, and any visible surrounds. */
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
    for (lcv = -1 ; lcv <= 2 ; lcv++) /* copy neighbouring rooms into u.p */
        for (lcv3 = -1 ; lcv3 <= 2 ; lcv3++)
            if (x + lcv > 21 || y + lcv3 > 21)
                u.p[lcv + 2][lcv3 + 2] = 0;
            else
                u.p[lcv + 2][lcv3 + 2] = u.l[x + lcv][y + lcv3];
    ccpos = 1;
    cobps = 0;
  /* fixme: check_room calls on u.p[2][2] seem unnecessary when flags for current room are already in u.i */
    if (x != 1 && check_room(u.p[2][2], ROOM_WALL_VISIBLE_NORTH) == 0)       /* print top if one */
      {
      /* can see through to N */
        strcpy(tbuf,"      "); /* don't show N side of room to NW */
        strcat(tbuf,north_side_str(1,2)); /* N side of room to N */
        optout(tbuf); /* don't show N side of room to NE */
        strcpy(tbuf,"      ");
        strcpy(tbuf2, tbuf);
        strcat(tbuf, west_side_outer_str(1,2)); /* W side of room to N */
        strcat(tbuf, "     "); /* interior of room to N */
        strcat(tbuf, west_side_outer_str(1,3)); /* W side of room to E */
        strcat(tbuf2, west_side_inner_str(1,2)); /* W side of room to N */
        strcat(tbuf2, "     "); /* interior of room to N */
        strcat(tbuf2, west_side_inner_str(1,3)); /* W side of room to E */
        if (u.c[UC_SPELL_LIGHT] > 0)
            if (check_room(u.p[1][2], ROOM_ANYTHING) != 0)     /* light spell */
                tbuf2[9] = '*'; /* indicate there's something there */
        optout(tbuf); /* outer parts of E/W walls */
        optout(tbuf2); /* inner part of E/W walls */
        optout(tbuf); /* outer parts of E/W walls */
      } /*if*/
    if (y != 1 && check_room(u.p[2][2], ROOM_WALL_VISIBLE_WEST) == 0)
      {
      /* see through to north side of room on west */
        strcpy(tbuf, north_side_str(2,1));
      }
    else
      {
        strcpy(tbuf, "       ");
      } /*if*/
    insert_at_blank(tbuf, north_side_str(2,2), 7); /* N side of room we're in */
    if (check_room(u.p[2][3], ROOM_WALL_VISIBLE_WEST) == 0)
      /* see through to north side of room on east */
        insert_at_blank(tbuf, north_side_str(2,3), 13);
    if (u.i[ROOM_WALL_VISIBLE_NORTH] == 0)
      {
        insert_at_blank(tbuf, west_side_outer_str(1,2), 7); /* W side of room to N */
        insert_at_blank(tbuf, west_side_outer_str(1,3), 13); /* W side of room to NE */
      } /*if*/
    optout(tbuf);
    strcpy(tbuf, "                   ");
    strcpy(tbuf2,"                   ");
    if (y != 1 && check_room(u.p[2][2], ROOM_WALL_VISIBLE_WEST) == 0)
      {
      /* can see through to W */
        insert_at_blank(tbuf, west_side_outer_str(2,1), 1); /* W side of room to W */
        insert_at_blank(tbuf2,west_side_inner_str(2,1), 1); /* W side of room to W */
        if (u.c[UC_SPELL_LIGHT] > 0 && check_room(u.p[2][1], ROOM_ANYTHING) != 0)
            insert_at(tbuf2, "*", 4); /* indicate there's something there */
      } /*if*/
    insert_at_blank(tbuf, west_side_outer_str(2,2), 7); /* W side of room we're in */
    insert_at_blank(tbuf2,west_side_inner_str(2,2), 7); /* W side of room we're in */
    insert_at_blank(tbuf, west_side_outer_str(2,3),13); /* W side of room to E */
    insert_at_blank(tbuf2,west_side_inner_str(2,3),13); /* W side of room to E */
    if (tbuf[12] == ' ')
      {
      /* can see through to E */
        insert_at_blank(tbuf, west_side_outer_str(2,4), 19); /* W side of 2nd room to E = E side of room to E */
        insert_at_blank(tbuf2,west_side_inner_str(2,4), 19); /* W side of 2nd room to E = E side of room to E */
        if (u.c[UC_SPELL_LIGHT] > 0 && check_room(u.p[2][3], ROOM_ANYTHING) != 0)
            insert_at(tbuf2, "*", 16); /* indicate there's something there */
      } /*if*/
    insert_at_blank(tbuf2, "X", 10); /* you are here */
    strcpy(tbuf3, tbuf);
    if (u.i[ROOM_TREASURE] == 1)
        insert_at(tbuf, "$", 11);
    if (u.i[ROOM_MONSTER] == 1)
        insert_at(tbuf, "#", 11);
    if ((u.i[ROOM_MONSTER] | u.i[ROOM_TREASURE]) == 0 && u.i[ROOM_SPECIAL] != 0)
        insert_at(tbuf,spc[u.i[ROOM_SPECIAL] - 1], 9);
    optout(tbuf);
    optout(tbuf2);
    optout(tbuf3);
    lcv3 = strlen(tbuf3);
    if (7 > lcv3 || tbuf3[6]  == ' ')
        strcpy(tbuf,north_side_str(3,1)); /* N side of room to SW */
    else
        strcpy(tbuf,"       ");
    insert_at_blank(tbuf, north_side_str(3,2), 7); /* N side of room to S */
    if (13 > lcv3 || tbuf3[12] == ' ')
        insert_at_blank(tbuf, north_side_str(3,3), 13); /* N side of room to SE */
    if (tbuf[9] == ' ')
      {
        insert_at_blank(tbuf, west_side_outer_str(3,2), 7); /* W side of room to S */
        insert_at_blank(tbuf, west_side_outer_str(3,3),13); /* W side of room to SE */
      } /*if*/
    optout(tbuf);
    lcv = strlen(tbuf);
    if (10 > lcv || tbuf[9] == ' ')
      { /* can see through to S */
        strcpy(tbuf,"      ");
        strcat(tbuf,west_side_outer_str(3,2)); /* W side of room to S */
        strcat(tbuf,"     ");
        strcat(tbuf,west_side_outer_str(3,3)); /* W side of room to SE */
        strcpy(tbuf2,"      ");
        strcat(tbuf2,west_side_inner_str(3,2)); /* W side of room to S */
        strcat(tbuf2,"     ");
        strcat(tbuf2,west_side_inner_str(3,3)); /* W side of room to SE */
        if (u.c[UC_SPELL_LIGHT] > 0 && check_room(u.p[3][2], ROOM_ANYTHING) != 0)
            insert_at(tbuf2, "*", 10); /* indicate there's something there */
        strcpy(tbuf3, "      ");
        strcat(tbuf3,north_side_str(4,2)); /* N side of 2nd room to S = S side of room to S */
        optout(tbuf);
        optout(tbuf2);
        optout(tbuf);
        optout(tbuf3);
      } /*if*/
    printf("%s\n\r", obuf);
  } /*utl_pplot*/
