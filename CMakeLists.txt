cmake_minimum_required(VERSION 2.7)

add_executable(asl_mfree asl_mfree.cc readoptions.cc asl_mfree_functions.cc)

if(APPLE)
  # Force use of libstdc++ on Mac as FSL uses this and we cannot link otherwise
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libstdc++")
  # Get rid of CMake warning
  set(CMAKE_MACOSX_RPATH ON)
endif(APPLE)

find_path(NEWMAT_INCLUDE_DIR newmat.h
            HINTS $ENV{FSLDIR}/extras/include/newmat REQUIRED)
find_path(FSL_INCLUDE_DIR newimage/newimage.h
            HINTS $ENV{FSLDIR}/include REQUIRED)
find_path(FSL_EXT_INCLUDE_DIR libprob/libprob.h
            HINTS $ENV{FSLDIR}/extras/include REQUIRED)

Message("-- FSL headers in ${FSL_INCLUDE_DIR} ${NEWMAT_INCLUDE_DIR} ${FSL_EXT_INCLUDE_DIR}")

find_library(NEWMAT_LIBRARY NAMES newmat libnewmat
             HINTS $ENV{FSLDIR}/extras/lib REQUIRED )
find_library(UTILS_LIBRARY NAMES utils libutils
             HINTS $ENV{FSLDIR}/lib REQUIRED )
find_library(MISCMATHS_LIBRARY NAMES miscmaths libmiscmaths
             HINTS $ENV{FSLDIR}/lib REQUIRED )
find_library(NEWIMAGE_LIBRARY NAMES newimage libnewimage
             HINTS $ENV{FSLDIR}/lib REQUIRED )
find_library(FSLIO_LIBRARY NAMES fslio libfslio
             HINTS $ENV{FSLDIR}/lib REQUIRED )
find_library(NIFTIIO_LIBRARY NAMES niftiio libniftiio
             HINTS $ENV{FSLDIR}/lib REQUIRED )
find_library(ZNZ_LIBRARY NAMES znz libznz
             HINTS $ENV{FSLDIR}/lib REQUIRED )
find_library(Z_LIBRARY NAMES z libz zlib zlibd 
               HINTS $ENV{FSLDIR}/lib REQUIRED )

set(LIBS ${UTILS_LIBRARY} ${NEWIMAGE_LIBRARY} ${FSLIO_LIBRARY} ${MISCMATHS_LIBRARY} ${NIFTIIO_LIBRARY} ${ZNZ_LIBRARY} ${Z_LIBRARY} ${NEWMAT_LIBRARY})

Message("-- Using libznz: ${ZNZ_LIBRARY}")
Message("-- Using miscmaths: ${MISCMATHS_LIBRARY}")
Message("-- Using fslio: ${FSLIO_LIBRARY}")
Message("-- Using newimage: ${NEWIMAGE_LIBRARY}")
Message("-- Using niftiio: ${NIFTIIO_LIBRARY}")
Message("-- Using newmat: ${NEWMAT_LIBRARY} ${NEWMAT_INCLUDE_DIR}")
Message("-- Using newimage: ${NEWIMAGE_LIBRARY}")
Message("-- Using zlib: ${Z_LIBRARY}")

target_link_libraries(asl_mfree ${LIBS})
include_directories(AFTER ${NEWMAT_INCLUDE_DIR} ${FSL_INCLUDE_DIR} ${FSL_EXT_INCLUDE_DIR})

# Extract GIT revision for versioning
execute_process(COMMAND
  git describe --dirty
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_SHA1
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND
  git log -1 --format=%ad --date=local
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_DATE
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

if (NOT GIT_SHA1)
  set(GIT_SHA1 unknown)
endif(NOT GIT_SHA1)

if (NOT GIT_DATE)
  set(GIT_DATE unknown)
endif(NOT GIT_DATE)

add_definitions(-DGIT_VERSION="${GIT_SHA1}" -DGIT_DATE="${GIT_DATE}")

INSTALL(TARGETS asl_mfree	DESTINATION bin)
