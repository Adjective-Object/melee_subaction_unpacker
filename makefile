program = mreader
srcs = src/mreader.cpp src/dolfs.cpp src/helpers.cpp src/event_mapper.cpp
objs = $(srcs:.cpp=.o)

LDFLAGS = 
LIBS =
CCFLAGS = -g -Wall --std=c++11

all: $(program)

$(program): $(objs)
	g++ $(LDFLAGS) $(objs) $(LIBS) -o $(program)

clean:
	rm -f $(objs) $(program)

src/$(program).o: src/$(program).cpp
	g++ $(CCFLAGS) -c $< -o $@

%.o : %.cpp %.h
	g++ $(CCFLAGS) -c $< -o $@


