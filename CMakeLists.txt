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

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}/../moos-ivp/include 
  ${CMAKE_CURRENT_SOURCE_DIR}/../moos-ivp/include/ivp)

SET(MOOS_LIBRARIES
  MOOS
  ${CMAKE_CURRENT_SOURCE_DIR}/../moos-ivp/lib/libapputil.a
  ${CMAKE_CURRENT_SOURCE_DIR}/../moos-ivp/lib/libmbutil.a)

TARGET_LINK_LIBRARIES(pSBUS
   ${MOOS_LIBRARIES}
   m
   pthread)

