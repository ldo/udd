/*
 * udd cbt_hit.c file - combat hit functions
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include <stdio.h>
#include "defs.h"
#include "cbt.h"

int cbt_uhitm
  (
    int damage
  )
{
  printf("You did %d point%s of damage.\r\n", damage, (damage == 1) ? "" : "s");
  monster_hits -= damage;
  if (monster_hits < 1) {
    printf("It died...\r\n");
    if (rnd() > 0.9) /* XXX */
      u.i[8] = 0;
    u.i[6] = 0;
    dead = 1;
  }
}

int cbt_ohitu
  (
    int damage /* ignored */
  )
{
  int pts;
  pts = rnd() * (monster_level * (4 - u.c[UC_CLASS])) + 1;
  printf("You suffer %d hit point%s.\r\n", pts, (pts == 1) ? "" : "s");
  u.c[UC_CURHIT] -= pts;
  if (u.c[UC_CURHIT] < 1) {
    printf("Your life has been terminated.\r\n");
    return(utl_death());
  }
  return(NOPE);
}
