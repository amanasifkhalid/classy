FLAGS = -std=c++11
LIBS = -lsoci_core -lsoci_sqlite3 -ldl -lpthread -lsqlite3

EXE = classy.exe

SOURCES = $(wildcard cpp/*.cpp)
OBJECTS = $(SOURCES:%.cpp=%.o)

release: FLAGS += -03 -DNDEBUG
release: classy

classy: $(OBJECTS)
	g++ $(FLAGS) $(LIBS) $(OBJECTS) -o $(EXE)

%.o: %.cpp
	g++ $(FLAGS) -c $*.cpp -o $*.o

db:
	sqlite3 classy.db < sql/schema.sql

clean:
	rm -f $(OBJECTS) $(EXE)* classy.db
