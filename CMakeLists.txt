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

