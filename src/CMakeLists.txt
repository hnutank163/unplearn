#project unp
SET(CMAKE_C_COMPILER "clang")
SET(COMM_SRC err.c sock_wrap.c)
ADD_EXECUTABLE(daytimecli daytimecli.c ${COMM_SRC})
ADD_EXECUTABLE(daytimesvr daytimetcpsvr.c ${COMM_SRC})
ADD_EXECUTABLE(echosvr  echosvr.c ${COMM_SRC})
ADD_EXECUTABLE(echocli echocli.c ${COMM_SRC})
SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 
