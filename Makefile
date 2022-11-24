CC=g++
CFLAGS = -g -O2 -Wall -std=c++11

DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin
DIR_INCLUDE =./include

SRC_Server = $(wildcard ${DIR_SRC}/server.cpp ${DIR_SRC}/cJSON.cpp)
SRC_Client = $(wildcard ${DIR_SRC}/client.cpp ${DIR_SRC}/cJSON.cpp)

OBJ_server = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC_Server} ))
OBJ_client = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC_Client} ))

TARGET_Server = server
TARGET_Client = client

BIN_TARGET_Server = ${DIR_BIN}/${TARGET_Server} 
BIN_TARGET_Client = ${DIR_BIN}/${TARGET_Client}

TARGET = ${DIR_BIN}/${TARGET_Server}  ${DIR_BIN}/${TARGET_Client}  

check_dir_obj = $(shell if [ ! -d $(DIR_OBJ) ];then mkdir -p $(DIR_OBJ); fi)
check_dir_bin = $(shell if [ ! -d $(DIR_BIN) ];then mkdir -p $(DIR_BIN); fi)

all: $(TARGET)  

${DIR_OBJ}/%.o:${SRC_Server} ${SRC_Client}
	$(CC) ${CFLAGS} -c $? -I$(DIR_INCLUDE)
	$(check_dir_obj)
	mv *.o $(DIR_OBJ)/

${BIN_TARGET_Client} : ${OBJ_client}
	$(check_dir_bin)
	${CC} ${CFLAGS} ${OBJ_client}  -o $@ -lpthread  

${BIN_TARGET_Server} : ${OBJ_server}
	$(check_dir_bin)
	${CC} ${CFLAGS} ${OBJ_server}  -o $@ -lpthread  

.PHONY : clean 
clean:
	rm -f ${DIR_OBJ}/*.o ${BIN_TARGET_Server} ${BIN_TARGET_Client}

