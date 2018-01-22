CC=gcc
SRC = ../src/
OUT = ../build/
IDIR=../src/include

OFLAG=-O0
GFLAG=-ggdb
ifeq ($(DFLAG),)
  DFLAG=-D__i5__
endif

CFLAGS = -U__STRICT_ANSI__ -Wstrict-prototypes -Wold-style-definition -fno-omit-frame-pointer -U__linux__ -Ulinux -Wall -Wshadow -Wmissing-field-initializers -Wredundant-decls -Wno-unused-function -m64 -mno-red-zone -fPIE -fno-stack-protector -Wno-unused-but-set-variable -Wno-packed-bitfield-compat -DHAVE_PTHREAD $(GFLAG) $(OFLAG) -I$(IDIR)

LFLAGS=-lpthread -lm

HDRS=$(SRC)include/bench.h $(SRC)include/pond_types.h $(SRC)include/util.h $(SRC)lib/toc.h $(SRC)include/config.h $(SRC)include/pthread_compat.h $(SRC)include/wool0.h $(SRC)lib/wool_toc.h $(SRC)include/definitions.h $(SRC)include/runtime0.h $(SRC)include/wool0_static.h

OBJ_ENG=lib/engine/bindings.o lib/engine/bitmap.o lib/engine/bitvector.o lib/engine/helper.o lib/engine/master.o lib/engine/pond.o lib/engine/program.o lib/engine/shell.o lib/engine/syssched.o lib/engine/util.o lib/engine/worker.o

OBJ_STA=lib/engine/bench.o

OBJ_GMG=lib/engine/topology_gothmog.o
OBJ_TIL=lib/engine/topology_tilera.o
OBJ_PHI=lib/engine/topology_xeon_phi.o
OBJ_I7=lib/engine/topology_i7.o
OBJ_I5=lib/engine/topology_i5.o

OBJ_CORE=$(OBJ_ENG) $(OBJ_STA)

OBJ_WOOL0=lib/runtimes/wool0.2/bindings.o lib/runtimes/wool0.2/wool.o lib/runtimes/wool0.2/wool_static.o lib/runtimes/wool0.2/bench.o
OBJ_WOOL0_DVS=lib/runtimes/wool0.2/dvs/bindings.o lib/runtimes/wool0.2/dvs/dvs.o lib/runtimes/wool0.2/dvs/wool.o
OBJ_WOOL0_PALIRRIA=lib/runtimes/wool0.2/dvs/palirria/bindings.o
OBJ_WOOL0_ASTEAL=lib/runtimes/wool0.2/asteal/bindings.o
OBJ_WOOL0_ASTEAL_DVS=lib/runtimes/wool0.2/dvs/asteal/bindings.o


OBJ_WOOL=lib/runtimes/wool/wool.o lib/runtimes/wool/bindings.o
OBJ_BOOL=lib/runtimes/asteal/asteal.o lib/runtimes/asteal/bindings.o
OBJ_POOL=lib/runtimes/palirria/palirria.o lib/runtimes/palirria/bindings.o
OBJ_RT=$(OBJ_WOOL0) $(OBJ_WOOL0_DVS) $(OBJ_WOOL0_PALIRRIA) $(OBJ_WOOL0_ASTEAL) $(OBJ_WOOL0_ASTEAL_DVS)

OBJ_USR_BOTS=usr/bots/common/bots_common.o usr/bots/common/bots_main.o usr/bots/fft/fft.o usr/bots/fib/fib.o usr/bots/floorplan/floorplan.o usr/bots/nqueens/nqueens.o usr/bots/sort/sort.o usr/bots/sparselu/sparselu.o usr/bots/strassen/strassen.o usr/bots/uts/uts.o usr/bots/uts/brg_sha1.o
OBJ_USR_SNTC=usr/synthetic/varloop.o
OBJ_USR_WOOL=usr/wool/fib.o usr/wool/loop.o usr/wool/inloop.o usr/wool/stress.o usr/wool/mm5.o usr/wool/mm7.o usr/wool/skew.o
OBJ_USR_CILK=usr/cilk/knapsack.o usr/cilk/lu.o usr/cilk/matmul.o
OBJ_USR=$(OBJ_USR_BOTS) $(OBJ_USR_SNTC) $(OBJ_USR_WOOL) $(OBJ_USR_CILK)

OBJECTS=$(OBJ_CORE) $(OBJ_RT) $(OBJ_USR)

.SECONDEXPANSION:

$(OBJ_GMG): $(SRC)$$(shell echo $$(subst .o,.c,$$@))
	@echo CC $<; \
	mkdir -p $(OUT)`dirname $@`; \
	$(CC) -std=c99 -D__GOTHMOG__ $(CFLAGS) -o $@ -c $< ;

 $(OBJ_TIL): $(SRC)$$(shell echo $$(subst .o,.c,$$@))
	@echo CC $<; \
	mkdir -p $(OUT)`dirname $@`; \
	$(CC) -std=c99 -D__TILERA__ $(CFLAGS) -o $@ -c $< ;

 $(OBJ_PHI): $(SRC)$$(shell echo $$(subst .o,.c,$$@))
	@echo CC $<; \
	mkdir -p $(OUT)`dirname $@`; \
	$(CC) -std=c99 -D__XEON_PHI__ $(CFLAGS) -o $@ -c $< ;

 $(OBJ_I7): $(SRC)$$(shell echo $$(subst .o,.c,$$@))
	@echo CC $<; \
	mkdir -p $(OUT)`dirname $@`; \
	$(CC) -std=c99 -D__i7__ $(CFLAGS) -o $@ -c $< ;

 $(OBJ_I5): $(SRC)$$(shell echo $$(subst .o,.c,$$@))
	@echo CC $<; \
	mkdir -p $(OUT)`dirname $@`; \
	$(CC) -std=c99 -D__i5__ $(CFLAGS) -o $@ -c $< ;

$(OBJECTS): $(SRC)$$(shell echo $$(subst .o,.c,$$@))
	@echo CC $<; \
	mkdir -p $(OUT)`dirname $@`; \
	$(CC) -std=c99 $(DFLAG) $(CFLAGS) -o $@ -c $< ;

.PHONY : fresh clean all gmg til phi i7 i5

all : | clean gmg clean til clean phi clean i7 clean i5

pond-gmg: $(OBJ_CORE) $(OBJ_RT) $(OBJ_USR) $(OBJ_GMG)
	@echo LD $@; \
	echo ; \
	$(CC) -std=c99 $(CFLAGS) $(DFLAG) $(LFLAGS) $^ -o $(OUT)$@;

pond-til: $(OBJ_CORE) $(OBJ_RT) $(OBJ_USR) $(OBJ_TIL)
	@echo LD $@; \
	echo ; \
	$(CC) -std=c99 $(CFLAGS) $(DFLAG) $(LFLAGS) $^ -o $(OUT)$@;

pond-phi: $(OBJ_CORE) $(OBJ_RT) $(OBJ_USR) $(OBJ_PHI)
	@echo LD $@; \
	echo ; \
	$(CC) -std=c99 $(CFLAGS) $(DFLAG) $(LFLAGS) $^ -o $(OUT)$@;

pond-i7: $(OBJ_CORE) $(OBJ_RT) $(OBJ_USR) $(OBJ_I7)
	@echo LD $@; \
	echo ; \
	$(CC) -std=c99 $(CFLAGS) $(DFLAG) $(LFLAGS) $^ -o $(OUT)$@;

pond-i5: $(OBJ_CORE) $(OBJ_RT) $(OBJ_USR) $(OBJ_I5)
	@echo LD $@; \
	echo ; \
	$(CC) -std=c99 $(CFLAGS) $(DFLAG) $(LFLAGS) $^ -o $(OUT)$@;

clean :
	@if test -d ./$(OUT)lib; then rm -r ./$(OUT)lib;fi; \
	 if test -d ./$(OUT)usr; then rm -r ./$(OUT)usr;fi; \
	 if test -f ./pond-gmg; then rm ./pond-gmg;fi; \
	 if test -f ./pond-til; then rm ./pond-til;fi; \
	 if test -f ./pond-phi; then rm ./pond-phi;fi; \
	 if test -f ./pond-i7; then rm ./pond-i7;fi; \
	 if test -f ./pond-i5; then rm ./pond-i5;fi;

fresh : all

gmg: DFLAG=-D__GOTHMOG__
gmg: pond-gmg

til: DFLAG=-D__TILERA__
til: pond-til

phi: DFLAG=-D__XEON_PHI__
phi: pond-phi

i7: DFLAG=-D__i7__
i7: pond-i7

i5: DFLAG=-D__i5__
i5: pond-i5

clearscr:
	clear

#pond.o scheduler.o bindings.o worker.o util.o bench.o wool.o bindings.o bots_common.o bots_main.o fft.o fib.o floorplan.o nqueens.o sort.o sparselu.o strassen.o uts.o brg_sha1.o varloop.o fib.o loop.o inloop.o stress.o

# lib/engine/ lib/stat/ lib/wool/ usr/bots/common/ usr/bots/fft/ usr/bots/fib/ usr/bots/floorplan/ usr/bots/nqueens/ usr/bots/sort/ usr/bots/sparselu/ usr/bots/strassen/ usr/bots/uts/ usr/synthetic/ usr/wool/ 

# lib/engine/pond.o lib/engine/scheduler.o lib/engine/bindings.o lib/engine/worker.o lib/engine/util.o lib/stat/bench.o lib/wool/wool.o lib/wool/bindings.o usr/bots/common/bots_common.o usr/bots/common/bots_main.o usr/bots/fft/fft.o usr/bots/fib/fib.o usr/bots/floorplan/floorplan.o usr/bots/nqueens/nqueens.o usr/bots/sort/sort.o usr/bots/sparselu/sparselu.o usr/bots/strassen/strassen.o usr/bots/uts/uts.o usr/bots/uts/brg_sha1.o usr/synthetic/varloop.o usr/wool/fib.o usr/wool/loop.o usr/wool/inloop.o usr/wool/stress.o

#lib/engine/pond.o lib/engine/scheduler.o lib/engine/bindings.o lib/engine/worker.o lib/engine/util.o lib/stat/bench.o lib/asteal/wool.o lib/asteal/bindings.o lib/palirria/wool.o lib/palirria/bindings.o lib/wool/wool.o lib/wool/bindings.o usr/bots/common/bots_common.o usr/bots/common/bots_main.o usr/bots/fft/fft.o usr/bots/fib/fib.o usr/bots/floorplan/floorplan.o usr/bots/nqueens/nqueens.o usr/bots/sort/sort.o usr/bots/sparselu/sparselu.o usr/bots/strassen/strassen.o usr/bots/uts/uts.o usr/bots/uts/brg_sha1.o usr/synthetic/varloop.o usr/wool/fib.o usr/wool/loop.o usr/wool/inloop.o usr/wool/stress.o


