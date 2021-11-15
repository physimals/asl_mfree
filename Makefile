include ${FSLCONFDIR}/default.mk

PROJNAME = asl_mfree

# The FSL build system changed
# substantially in FSL 6.0.6
# FSL >= 6.0.6
ifeq (${FSL_GE_606}, true)
  LIBS         = -lfsl-newimage -lfsl-miscmaths -lfsl-utils \
                 -lfsl-NewNifti -lfsl-cprob -lfsl-znz
# FSL <= 6.0.5
else
  ifeq ($(shell uname -s), Linux)
	MATLIB := -lopenblas
  endif

  USRINCFLAGS = -I${INC_NEWMAT} -I${INC_ZLIB} \
                -I${FSLDIR}/extras/include/armawrap
  USRLDFLAGS  = -L${LIB_NEWMAT} -L${LIB_ZLIB}             \
                -lnewimage -lmiscmaths -lutils -lNewNifti \
                -lprob -lznz ${MATLIB} -lm -lz
endif

XFILES = asl_mfree

OBJS = readoptions.o asl_mfree_functions.o

# Pass Git revision details
GIT_VERSION := $(shell git describe --dirty)
GIT_DATE    := $(shell git log -1 --format=%ad --date=local)
CXXFLAGS    += -DGIT_VERSION=\"${GIT_VERSION}\" -DGIT_DATE="\"${GIT_DATE}\""

all: ${XFILES}

asl_mfree: ${OBJS} asl_mfree.o
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}
