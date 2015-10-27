all: mreader

program = mreader
srcs = src/mreader.cpp src/dolfs.cpp
objs = $(srcs:.cpp=.o)

LDFLAGS = 
LIBS =
CCFLAGS = -g -Wall --std=c++11

$(program): $(objs)
	g++ $(LDFLAGS) $(objs) $(LIBS) -o mreader

clean:
	rm -f $(objs) $(program)

%.o : %.cpp
	g++ $(CCFLAGS) -c $*.cpp -o $@


