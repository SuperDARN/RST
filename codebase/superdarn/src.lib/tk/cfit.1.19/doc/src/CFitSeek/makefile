# Makefile for CFitSeek
# =====================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn 
IGNVER=1
OBJS = CFitSeek.o
OUTPUT = CFitSeek
LIBS=-lcfit.1 -lrtime.1 -lrcnv.1 
SLIB=-lm
 
include $(MAKELIB).$(SYSTEM)
