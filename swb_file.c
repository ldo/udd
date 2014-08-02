/*
 * udd swb_file.c file - switchboard's file routines
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "defs.h"

static char buf[BUFSIZ];

int swb_ppnok(void)
  /* NOPE=can't run program, MAYBE=can't Create, Run or Kill, YEP=full access */
{
    const char * const str = getenv("UDD_PLAYER");
    const int priv = str != NULL ? atoi(str) : 2;
    return
        priv == 2 ?
            YEP
        : priv == 1 ?
            MAYBE
        :
            NOPE;
  return(YEP);
}


int swb_wiz(void)
  /* 1=access to operator program and no password kill */
  /* 0=normal user */
{
    const char * const str = getenv("UDD_WIZ");
    return
        str != NULL && strlen(str) != 0;
}



void swb_note
  (
    char * msg,
    char * file,
    int wiz
  )
{
  FILE *fp;
  if ((fp = fopen(file, "r")) == NULL)
    return;
  printf("%s\r\n", msg);
  while (fgets(buf, BUFSIZ, fp) != NULL) 
    if (buf[0] != '!' || wiz != 0)
      printf("%s\r", buf);
  printf("\n");
  fclose(fp);
}
