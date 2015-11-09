include gmsl-1.1.7/gmsl

program = mreader
srcs = src/mreader.cpp src/dolfs.cpp src/helpers.cpp \
	   src/event_mapper.cpp src/config.cpp
objs = $(srcs:.cpp=.o)

LDFLAGS = 
LIBS =
CCFLAGS = -g -Wall --std=c++11

print-%:
	@echo '$*=$($*)'

all: $(program)

$(program): $(objs)
	g++ $(LDFLAGS) $(objs) $(LIBS) -o $(program)

clean:
	rm -f $(objs) $(program)

src/$(program).o: src/$(program).cpp
	g++ $(CCFLAGS) -c $< -o $@

%.o : %.cpp %.h
	g++ $(CCFLAGS) -c $< -o $@

define character
$(call set,offsets,$(strip $1),$(strip $2));
$(call set,counts,$(strip $1),$(strip $3));
endef

$(call character, Sk, 8fc8, 24)
$(call character, Fe, 9bb4, 2D)
$(call character, Zd, 9870, 1e)
$(call character, Pk, 91fc, 23)
objects = $(call keys,offsets)


.PHONY: $(objects)
$(objects): $(program)
	./$(program) datfiles/Pl$@.dat \
		-s $(call get,offsets,$@) -c $(call get,counts,$@)

