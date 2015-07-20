i#project unp
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/libs)
ADD_EXECUTABLE(daytimecli daytimecli.c)
ADD_EXECUTABLE(daytimesvr daytimetcpsvr.c)
TARGET_LINK_LIBRARIES(daytimesvr libunp.a)
TARGET_LINK_LIBRARIES(daytimecli libunp.a)
SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 
