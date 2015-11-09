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
$(call character, Ca, 9478, 23)
$(call character, Dr, 8e0c, 16)
$(call character, Fc, 93AC, 2e)
$(call character, Fx, 92C4, 2e)
$(call character, Gk, 97E8, 24)
$(call character, Gn, 9258, 1d)
$(call character, Gw, 9800, 29)
$(call character, Kb, D080, a5)
$(call character, Kp, 96E8, 22)
$(call character, Lg, 8ae8, 21)
$(call character, Lk, 9950, 19)
$(call character, Mr, 8ac8, 18)
$(call character, Ms, 98f8, 29)
$(call character, Mt, 91a8, 1d)
$(call character, Ns, 92f4, 23)
$(call character, Pc, 9170, 27)
$(call character, Pe, a2b8, 1f)
$(call character, Pk, 91fc, 23)
$(call character, Pp, 8b40, 28)
$(call character, Pr, 8b50, 29)
$(call character, Ss, 9550, 20)
$(call character, Ys, 8EC8, 21)
$(call character, Zd, 9870, 1e)

objects = $(call keys,offsets)


.PHONY: $(objects)
$(objects): $(program)
	./$(program) datfiles/Pl$@.dat \
		-s $(call get,offsets,$@) -c $(call get,counts,$@)

