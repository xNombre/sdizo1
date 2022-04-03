CXX = g++
CXXFLAGS = -std=c++17 -O3
LDFLAGS =  -flto

SRC = *.cpp
OBJ = $(SRC:.cc=.o)
EXEC = sdizo1

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LBLIBS)

clean:
	rm -rf $(EXEC)
