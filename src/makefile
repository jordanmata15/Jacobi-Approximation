PROGRAM=JacobiApproximation
CXX=mpic++
CXXFLAGS=-std=c++11 -g -Wall
LDLIBS=

# keep our cpp/hpp files separate from .o and executable
SRCDIR=.
BUILD=../build
SCRIPTING=../scripting

SRC:=$(shell find $(SRCDIR)/ -name "*.cpp")
OBJS:=$(SRC:%.cpp=$(BUILD)/%.o)

all: ${OBJS} $(BUILD)/${PROGRAM}

$(BUILD)/$(PROGRAM):	$(OBJS)
	$(CXX) $(CXXFLAGS) $(BUILD)/$(OBJS) -o $(BUILD)/$(PROGRAM) ${LDLIBS}

$(BUILD)/%.o: $(SRCDIR)/%.cpp $(SRCDIR)/%.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< ${LDLIBS}

run:
	mpirun -np 4 ${BUILD}/${PROGRAM}

plot:
	python3 $(SCRIPTING)/plot.py

clean:
	rm -rf $(BUILD)/core* ${BUILD}/$(OBJS) $(BUILD)/$(PROGRAM)