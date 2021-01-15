#--------------------------------------------------------
# The CMakeLists.txt for:                       pSBUS
# Author(s):
#--------------------------------------------------------

SET(SRC
  SBUS.cpp
  SBUS_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pSBUS ${SRC})

TARGET_LINK_LIBRARIES(pSBUS
   ${MOOS_LIBRARIES}
   mbutil
   apputil
   m
   pthread)

find_program(CTAGS ctags)
if (CTAGS)
    FIND_FILE(MAKE_CTAGS make_ctags.sh ../..)
    ADD_CUSTOM_TARGET(sbus_ctags ALL
        ${MAKE_CTAGS} .
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS pSBUS
    )
endif(CTAGS)
