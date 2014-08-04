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

CBT_EXTERN int monster_level, monster_type, min_monster_hits, m_str, m_arm, monster_hits, monster_hits_old;
CBT_EXTERN const char *mnam;
CBT_EXTERN int strike_force, dead;
CBT_EXTERN bool autoevade, gone, hflag, mskip;

