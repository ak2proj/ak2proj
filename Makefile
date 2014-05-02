CFLAGS=-c -std=c++1y -Wall -Wextra -pedantic -Werror -Os -MD -Wno-unused-private-field
LDFLAGS=
SOURCES=$(shell find -name "*.cpp")
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=emu

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $< -o $@

clean:
	@find . -name "*.o" -delete
	@find . -name "*.d" -delete
	@rm -rf $(EXECUTABLE)

-include $(SOURCES:.cpp=.d)
