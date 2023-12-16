clean :
	rm -f $(OBJ) $(EXEC)
	rm -f ../src/parsery.hpp	../src/parsery.cpp ../src/analyzer.cpp
	rm -f ../build/*.imp ../build/*.mr

SRC_DIR=../src
INC_DIR=../include


EXEC := kompilator
SRC := $(wildcard $(SRC_DIR)/*.cpp)
INC := $(wildcard $(INC_DIR)/*.h)
OBJ := $(SRC:%.cpp=%.o)
$(SRC) : parsery.cpp analyzer.cpp
$(CXX) = g++
HEADERS := -I $(INC_DIR)

parsery.cpp:
	bison -d -o ../src/parsery.cpp ../src/parser.y
analyzer.cpp:
	flex -o ../src/analyzer.cpp ../src/lexer.l

%.o: %.cpp $(INC)
	g++ -std=c++17 -pedantic $(HEADERS) -c $< -o $@

$(EXEC): $(SRC) $(INC) $(OBJ)
	$(CXX) $(HEADERS) $(OBJ) -o $@
run: $(EXEC)