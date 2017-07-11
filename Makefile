# source file directory
SRC=./src

# output file
OUT=./out

# head file
INC=-I./include

# pthread lib
PTH=-pthread

# real time lib
RT=-rt

# compiler and arguments
CC=clang++ -Wall -std=c++14 $(INC)

# just add source file to this
FILES=main.cpp \
	  File.cpp 

# replace, *.cpp into ./src/*.cpp
SRCS=$(FILES:%.cpp=$(SRC)/%.cpp)

# replace, *.cpp into ./out/*.o
OBJS=$(FILES:%.cpp=$(OUT)/%.o)

# target, default make target
TARGET=$(OUT)/main

# biuld target
$(TARGET):$(OBJS)
	$(CC) $(PTH) -o $@ $^

# to all ./out/*.o:./src/*.cpp
$(OUT)/%.o:$(SRC)/%.cpp
	$(CC) $(PTH) -o $@ -c $^

# phony target
clean:
	rm -rf $(OUT)/*.o $(OUT)/main

