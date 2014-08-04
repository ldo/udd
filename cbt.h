/*
 * udd cbt.h file
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

/* ONLY useful to CBT routines... */
/* should not be used by external stuff */

#ifdef CBTMAIN
#define CBT_EXTERN 
#else
#define CBT_EXTERN extern
#endif

CBT_EXTERN int l, m, m1, m_str, m_arm, m2, m2_old;
CBT_EXTERN char *mnam;
CBT_EXTERN int d, dead, dice, i1, i1_old, i2, in;
CBT_EXTERN bool autoevade, gone, hflag, mskip;

