# - Try to find the o2 library
# Once done this will define
#
#  O2_FOUND - system has the o2 library
#  O2_INCLUDE_DIR - the o2 include directory
#  O2_LIBRARIES - Link this to use the o2
#  O2_DEFINITIONS - Compiler switches required for using o2
#

find_library(O2_LIBRARIES
  NAMES o2 o20
)

find_path(O2_INCLUDE_DIR o1.h
  HINTS /usr/include
  PATH_SUFFIXES o2
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(o2
    FOUND_VAR O2_FOUND
    REQUIRED_VARS O2_LIBRARIES O2_INCLUDE_DIR
)

mark_as_advanced(O2_INCLUDE_DIR O2_LIBRARIES)
