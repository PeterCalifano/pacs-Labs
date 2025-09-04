
TIMERDIR = ./cdf_timer/src
CPPFLAGS = -I/opt/local/include/  -I$(TIMERDIR)
CXXFLAGS = -std=c++17 -O3 -fopenmp
LDFLAGS =  -L/opt/local/lib/libomp 
CXX = /opt/local/bin/clang++
DEPS = $(TIMERDIR)/timer.cpp

all: kmc

kmc: kmc.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o kmc kmc.cpp $(DEPS) $(LDFLAGS) $(LIBS)

clean:
	rm -f kmc *.o *.csv 
