/*
 * udd defs.h file
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include <stdbool.h>

#ifdef MAIN
#define EXTERN
#else
#define EXTERN extern
#endif

/*
 * C O N F I G U R A T I O N   S E C T I O N
 *
 */

/*
 * define L_STRICT if you want a character to be left in a locked
 * state after a SIGHUP (default=undef).
 */

#undef  L_STRICT

/*
 * file names: FIL_XXX     - full path of file
 *             FIL_XXX_LK  - if you are using links for locking this is the
 *                           lock file name, otherwise it is ignored.
 *             FIL_XXX_LD  - if you are using links for locking this is the
 *                           lock file directory, otherwise it is ignored.
 * files: CHR - Character file
 *        NOT - message of the day
 *        ORB - winners file
 *        INS - instruction file
 *        LVL - dungeon structure file
 */

#define FIL_CHR "./char.dat"
#define FIL_CHR_LK "char.lck"
#define FIL_CHR_LD "."
#define FIL_NOT "./not.txt"
#define FIL_ORB "./orb.txt"
#define FIL_INS "./ins.txt"
#define FIL_LVL "./lvl.dat"
#define FIL_LVL_LK "lvl.lck"
#define FIL_LVL_LD "."

/*
 * E N D   O F   C O N F I G U R A T I O N   S E C T I O N
 *
 */

/* don't edit below this line */

/* generic constants */

#define NMONST 21 /* number of monster types */
#define NAMELEN 64
#define SPLBASE 37
#define DNAM_SZ 20
#define CBT_NORM 1
#define CBT_ALTR 2
#define CBT_CAST 3
#define DGN_NEWADV 0
#define DGN_NEWLOC 1
#define DGN_PROMPT 2
#define DGN_AMOVE 3
#define SPC_NORM 1
#define SPC_TPTNOW 20   /* must be > 15 or < 1  to avoid conflict */
#define SPC_DNS  1
#define SPC_UPS  2
#define SPC_UDS  3
#define SPC_EXC  4
#define SPC_PIT  5
#define SPC_TPT  6
#define SPC_FTN  7
#define SPC_ALT  8
#define SPC_DGN1 9
#define SPC_DGN2 10
#define SPC_ORB  11
#define SPC_ELV  12
#define SPC_THR  13
#define SPC_SAF  14
#define SPC_RCK  15
#define SWB_CREATE 2
#define XXX_NORM 1
#define NOPE 1
#define YEP  2
#define MAYBE 3
#define LOCK 4
#define NOLOCK 5
#define NUKE 6
#define rnd() ((random() % 1000) / 1000.0)  /* basic emulation */

struct monst {
  const char *nam;
  int m; /* monster strength/resistance to death */
  int mindunlvl, maxdunlvl, minmonlvl;
};

struct state {
  char n[2][NAMELEN];                 /* name, secret name */
  int c[65];                /* various stuff */
  int l[22][22];                 /* current level map */
  int i[9], i1[9];               /* room analysis */
  int p[5][5];                   /* current pos */
};

struct chr {
  char nam[2][NAMELEN];
  int c[65];
};

/* usage of "various stuff" array */
#define UC_ALIVE 0 /* character is actually alive */
#define UC_STRENGTH 1 /* strength attribute */
#define UC_INTEL 2 /* intelligence attribute */
#define UC_WISDOM 3 /* wisdom attribute */
#define UC_CONSTIT 4 /* constitution attribute */
#define UC_DEX 5 /* dexterity attribute */
#define UC_CHARISMA 6 /* charisma attribute */
#define UC_CLASS 7 /* character class */
#define UC_LEVEL 8 /* character level */
#define UC_EXP 9 /* experience */
#define UC_MAXHIT 10 /* max hit points */
#define UC_CURHIT 11 /* current hit points */
#define UC_GOLDFOUND 12 /* how much gold character is carrying in dungeon */
#define UC_TOTALGOLD 13 /* how much gold character has brought out of dungeon */
/* 14 not used */
#define UC_DGNLVL 15 /* number of level within dungeon */
#define UC_DGN_X 16 /* x-coordinate within dungeon */
#define UC_DGN_Y 17 /* y-coordinate within dungeon */
#define UC_DGNNR 18 /* number of dungeon */
/* 19 and 20 initialized in swb.c, but not used anywhere else? */
#define UC_EXPGAIN 21 /* how much experience user will gain on successfully leaving the dungeon */
#define UC_WEAPON 22 /* weapon power, -1 for none, >0 for magic (actual weapon is class-dependent) */
#define UC_ARMOR 23 /* armour power, -1 for none, >0 for magic (actual armour type is class-dependent) */
#define UC_SHIELD 24 /* type of shield, if any: 0 none, -1 non-magic, 1 magic */
#define UC_SPELLSADJ1 25 /* deduction from next spell advancement at level 1 */
#define UC_SPELLSADJ2 26 /* deduction from next spell advancement at level 2 */
#define UC_SPELLSADJ3 27 /* deduction from next spell advancement at level 3 */
#define UC_SPELLSADJ4 28 /* deduction from next spell advancement at level 4 */
/* 29, 30 not used */
#define UC_SPELLS1 31 /* nr of spells available at level 1 */
#define UC_SPELLS2 32 /* nr of spells available at level 2 */
#define UC_SPELLS3 33 /* nr of spells available at level 3 */
#define UC_SPELLS4 34 /* nr of spells available at level 4 */
/* 35, 36 not used */
/* spells in effect: */
#define UC_SPELL_LIGHT 37 /* light */
#define UC_SPELL_PROT 38 /* protection against evil */
#define UC_SPELL_SHLD 39 /* shield */
#define UC_SPELL_PRAY 40 /* pray */
#define UC_SPELL_DTRP 41 /* detect traps */
#define UC_SPELL_SLNC 42 /* silence */
#define UC_SPELL_LEVT 43 /* levitation (avoid falling down pits) */
#define UC_SPELL_STRG 44 /* strength */
#define UC_SPELL_FEAR 45 /* fear */
#define UC_SPELL_INVS 46 /* invisibility */
#define UC_SPELL_TMST 47 /* time-stop */
/* end spells in effect */
#define UC_GID 48 /* group ID of user owning character */
#define UC_UID 49 /* user ID of user owning character */
#define UC_HASORB 50 /* player has orb */
#define UC_RING 51 /* power of ring of regeneration, if any */
#define UC_ELVEN_CLOAK 52 /* power of elven cloak, if any */
#define UC_ELVEN_BOOTS 53 /* power of elven boots, if any */
/* 54, 55, 56 not used */
#define UC_LOCKED 57 /* character is locked (in use) */
#define UC_SAFE_COMBN 58 /* secret combination to the safe */
#define UC_CREATED 59 /* timestamp when character was created */
#define UC_LASTRUN 60 /* timestamp when character was last run */
#define UC_DEBUGCHR 61 /* character created for debugging only */
#define UC_WIZONLY 62 /* character only accessible to wizard players */
/* 63? */
#define UC_STATE 64 /* controls what to do with character next */

struct dgnstr {
  char dnam[DNAM_SZ];
  short int dstart;
  unsigned char dmap[400];
};


/* pre-defined data structs in data.h */
extern const struct monst mm[NMONST];  /* monster matrix */
extern const int q[5][3];        /* direction vectors */
extern const char * const vaz[5];             /* maze parts */
extern const char * const maz[4];             /* more parts */
extern const char * const st;                 /* stat names */
extern const char * const spc[16];            /* specials */
extern const char * const class[];            /* class names, indexed by class */
extern const char * const class2[];           /* slightly different class names, for some reason */
extern const char * const arm[];              /* arm types, indexed by class */
extern const char * const wep[];              /* weaponry, indexed by class */
extern const char * const sp[9][7];           /* spell names */

/* character classes--note lots of code makes assumptions about these numbers */
#define CHRCLASS_FIGHTER 0
#define CHRCLASS_CLERIC 1
#define CHRCLASS_MAGICIAN 2

/* regular game state vars */

EXTERN struct state u;           /* me! */
EXTERN struct dgnstr dd;         /* current dungeon map */
EXTERN char *ddd;                /* our name */
EXTERN int n_dlvl;               /* # of dungeons (for operator prog) */
EXTERN int dfd;                  /* RDONLY fd for lvl file */
EXTERN int autosave;             /* try save if sighup */

/* J. Random functions */

int cbt_cast(void), cbt_main(void), cbt_ohitu(int dam), cbt_ok(void), cbt_uhitm(int dam);
int chr_lck_nuke(int lock), chr_load(char * name, int lock), chr_save(int unlock), chr_new(void);
void chr_rset(void);
struct chr *chr_scan(void);
int cs1(int s), cs2(int s), cs3(int s), cs4(int s);
int dgn_main(void), dgn_nomove(int cmd);
void dgn_voices(void);
int dlvl_choose(void), dlvl_loadum(int dno, int lvl);
void dlvl_init(void), dlvl_get_start(int dno);
int fni(int r), fni1(int r, int s);
char *fnp(char * a, const char * b, int p), *fnp1(char * a, const char * b, int p);
int lock_open(char * file, int mode, char * lockdir, char * lockfile, int maxtime);
int lock_close(int fd, char * lockdir, char * lockfile);
int ms1(int s), ms2(int s), ms3(int s), ms4(int s);
void opr_main(void);
int optout(char * s), roll(int a, int b);
void sighup(int), sigstop(int), swb_note(char * msg, char * file, int wiz);
int spc_main(void), swb_ppnok(void), swb_wiz(void), trs_chest(void), trs_main(void),
  trs_obj(void);
char *unix_date(long dat);
void unix_exit(int n), unix_init(void), unix_tty_cook(void), unix_tty_dgn(void),
  unix_tty_pswd(void);
int utl_adj_ex(void), utl_chklvl(void), utl_death(void), utl_escape(void), utl_exp(int lvl),
  utl_populate(void), utl_winner(void);
void utl_adj_st(void), utl_dtrp(void), utl_eqp(void), utl_inilvl(void), utl_pplot(int flag),
  utl_prtspl(int c7, int lvl), utl_sprog(void), utl_stat(void), utl_status(void);
