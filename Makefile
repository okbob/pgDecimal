# contrib/decimal/Makefile

MODULES = decimal

EXTENSION = decimal
DATA = decimal--1.0.sql

REGRESS = decimal

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/decimal
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
