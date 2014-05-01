CC=clang++
LD=clang++
CFLAGS=-c -std=c++1y -Wall -Wextra -pedantic -Werror -stdlib=libc++ -Weffc++ -Os -MD -Wno-unused-private-field
LDFLAGS=-stdlib=libc++ -lc++abi
SOURCES=$(shell find -name "*.cpp")
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=emu

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	@find . -name "*.o" -delete
	@find . -name "*.d" -delete
	@rm -rf $(EXECUTABLE)

-include $(SOURCES:.cpp=.d)
