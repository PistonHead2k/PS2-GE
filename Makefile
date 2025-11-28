#Makefile by Tom Marks
#Needs to run in linux subsystem (bash)

EE_BIN=ELF-ISO/test.elf #linkable file
EE_OBJS=main.o #linker
EE_LIBS= -ldma -lgraph -ldraw -lmath3d -lpacket -lkernel -ldebug -linput -lpad

EE_DVP = dvp-as
EE_VCL = vcl

EE_CFLAGS += --std=c23 -Wno-unused-variable
EE_LDFLAGS = -L$(PSDSDK)/ee/common/lib -L$(PS2SDK)/ee/lib

EE_OPTFLAGS= -O2


PS2SDK=/usr/local/ps2dev/ps2sdk

ISO_TGT=ELF-ISO/build.iso

include Makefiles/Makefile.eeglobal
include Makefiles/Makefile.pref

all: $(ISO_TGT)
$(ISO_TGT): $(EE_BIN)
	mkisofs -l -udf -graft-points -o $(ISO_TGT) $(EE_BIN) DVD #/home/=/mnt/d/PS2/Dev/DVD

clean:
	rm -rf $(ISO_TGT) $(EE_BIN) $(EE_OBJS)

rmiso:
	rm -rf $(ISO_TGT)

#Update ISO
u:
	$(MAKE) clean; $(MAKE) all

#Make c array from raw file
raw:
	bin2c texture.raw texture.c texture

# Original VCL tool preferred. 
# It can be runned on WSL, but with some tricky commands: 
# https://github.com/microsoft/wsl/issues/2468#issuecomment-374904520
%.vsm: %.vcl
	$(EE_VCL) $< >> $@

%.o: %.vsm
	$(EE_DVP) $< -o $@
