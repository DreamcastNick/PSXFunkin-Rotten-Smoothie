TARGET = funkin
TYPE = ps-exe

SRCS = src/main.c \
       src/mutil.c \
       src/random.c \
       src/archive.c \
       src/font.c \
       src/trans.c \
       src/loadscr.c \
       src/menu.c \
       src/stage.c \
	   src/event.c \
       src/debug.c \
       src/save.c \
       src/psx.c \
       src/str.c \
       src/io.c \
       src/gfx.c \
       src/audio.c \
       src/pad.c \
       src/timer.c \
       src/movie.c \
       src/stage/dummy.c \
       src/stage/week1.c \
       src/stage/week2.c \
       src/stage/week3.c \
	   src/stage/trio.c \
	   src/stage/kitchen.c \
       src/animation.c \
       src/character.c \
	   src/character/apple.c \
       src/character/bf.c \
       src/character/speaker.c \
	   src/character/orange.c \
       src/character/dad.c \
       src/character/spook.c \
       src/character/pico.c \
       src/character/monster.c \
	   src/character/exep3.c \
       src/character/gf.c \
	   src/character/menuo.c \
	   src/character/menup.c \
	   src/character/menugf.c \
       src/object.c \
       src/object/combo.c \
       src/object/splash.c \
       src/pause.c \
       src/psn00b/vlc.c \
       src/psn00b/vlc2.s \
       mips/common/crt0/crt0.s

CPPFLAGS += -Wall -Wextra -pedantic -mno-check-zero-division
LDFLAGS += -Wl,--start-group
# TODO: remove unused libraries
LDFLAGS += -lapi
#LDFLAGS += -lc
LDFLAGS += -lc2
LDFLAGS += -lcard
LDFLAGS += -lcd
#LDFLAGS += -lcomb
LDFLAGS += -lds
LDFLAGS += -letc
LDFLAGS += -lgpu
#LDFLAGS += -lgs
#LDFLAGS += -lgte
#LDFLAGS += -lgun
#LDFLAGS += -lhmd
#LDFLAGS += -lmath
LDFLAGS += -lmcrd
#LDFLAGS += -lmcx
LDFLAGS += -lpad
LDFLAGS += -lpress
#LDFLAGS += -lsio
LDFLAGS += -lsnd
LDFLAGS += -lspu
#LDFLAGS += -ltap
LDFLAGS += -flto -Wl,--end-group

include mips/common.mk
