#
# udd Makefile file
#
# 03-Sep-89, Chuck Cranor
#
# V5.1C
#

DEST	      = /usr/local

HDRS	      = cbt.h \
		data.h \
		defs.h

CC := cc

CFLAGS	      = -g

LDFLAGS	      = -lm

LIBS	      =

LINKER	      = cc

OBJDIR := bin/

MKBIN := mkdir -p $(OBJDIR)

OBJS	      = $(OBJDIR)cbt.o \
		$(OBJDIR)cbt_cast.o \
		$(OBJDIR)cbt_hit.o \
		$(OBJDIR)chr_file.o \
		$(OBJDIR)dgn.o \
		$(OBJDIR)dgn_nomove.o \
		$(OBJDIR)dgn_voices.o \
		$(OBJDIR)dlvl_file.o \
		$(OBJDIR)lock.o \
		$(OBJDIR)opr.o \
		$(OBJDIR)spc.o \
		$(OBJDIR)swb.o \
		$(OBJDIR)swb_file.o \
		$(OBJDIR)trs.o \
		$(OBJDIR)trs_cobjs.o \
		$(OBJDIR)unix.o \
		$(OBJDIR)utl_pplot.o \
		$(OBJDIR)utl_util.o

PRINT	      = pr

PROGRAM	      = $(OBJDIR)udd

SRCS	      = cbt.c \
		cbt_cast.c \
		cbt_hit.c \
		chr_file.c \
		dgn.c \
		dgn_nomove.c \
		dgn_voices.c \
		dlvl_file.c \
		lock.c \
		opr.c \
		spc.c \
		swb.c \
		swb_file.c \
		trs.c \
		trs_cobjs.c \
		unix.c \
		utl_pplot.c \
		utl_util.c

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS) $(LIBS)
		@echo -n "Loading $(PROGRAM) ... "
		@$(MKBIN)
		@$(LINKER) $(OBJS) $(LIBS) -o $(PROGRAM) $(LDFLAGS)
		@echo "done"

clean:;		@rm -rf $(OBJDIR)

index:;		@ctags -wx $(HDRS) $(SRCS)

install:	$(PROGRAM)
		@echo Installing $(PROGRAM) in $(DEST)
		@install -s $(PROGRAM) $(DEST)

print:;		@$(PRINT) $(HDRS) $(SRCS)

program:        $(PROGRAM)

tags:           $(HDRS) $(SRCS); @ctags $(HDRS) $(SRCS)

update:		$(DEST)/$(PROGRAM)

$(DEST)/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		@make DEST=$(DEST) install
###
$(OBJDIR)cbt.o: defs.h cbt.h cbt.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

$(OBJDIR)cbt_cast.o: defs.h cbt.h cbt_cast.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

$(OBJDIR)cbt_hit.o: defs.h cbt.h cbt_hit.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

$(OBJDIR)chr_file.o: defs.h chr_file.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

$(OBJDIR)dgn.o: defs.h dgn.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

$(OBJDIR)dgn_nomove.o: defs.h dgn_nomove.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

$(OBJDIR)dgn_voices.o: dgn_voices.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

$(OBJDIR)dlvl_file.o: defs.h dlvl_file.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

$(OBJDIR)lock.o: lock.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

$(OBJDIR)opr.o: defs.h opr.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

$(OBJDIR)spc.o: defs.h spc.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

$(OBJDIR)swb.o: defs.h data.h swb.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

$(OBJDIR)swb_file.o: defs.h swb_file.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

$(OBJDIR)trs.o: defs.h trs.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

$(OBJDIR)trs_cobjs.o: defs.h trs_cobjs.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

$(OBJDIR)unix.o: defs.h unix.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

$(OBJDIR)utl_pplot.o: defs.h utl_pplot.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

$(OBJDIR)utl_util.o: defs.h utl_util.c
		@$(MKBIN)
		$(CC) $(CFLAGS) -c -o $@ $(*F).c

.PHONY : clean
