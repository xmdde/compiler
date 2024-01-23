CXX := g++
CXXFLAGS := -std=c++17 -pedantic
INCLUDES := -I ../include

SRC_DIR := ../src
OBJ_DIR := ../src
SRC_FILES := AsmCode.cpp MemoryManager.cpp Procedure.cpp analyzer.cpp parsery.cpp
OBJ_FILES := $(SRC_FILES:.cpp=.o)

BISON_FILE := parser.y
FLEX_FILE := lexer.l
BISON_SRC := $(SRC_DIR)/parsery.cpp
BISON_HEADER := $(SRC_DIR)/parsery.hpp
FLEX_SRC := $(SRC_DIR)/analyzer.cpp

TARGET := kompilator

all: $(TARGET)

$(TARGET): $(addprefix $(OBJ_DIR)/, $(OBJ_FILES))
	$(CXX) $(INCLUDES) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BISON_SRC) $(BISON_HEADER): $(SRC_DIR)/$(BISON_FILE)
	bison -d -o $(BISON_SRC) $<

$(FLEX_SRC): $(SRC_DIR)/$(FLEX_FILE) $(BISON_HEADER)
	flex -o $@ $<

.PHONY: clean

clean:
	rm -f ../src/parsery.o ../src/analyzer.o ../src/AsmCode.o ../src/MemoryManager.o ../src/Procedure.o
	rm -f ../src/parsery.hpp ../src/parsery.cpp ../src/analyzer.cpp
	rm -f ../build/*.log
