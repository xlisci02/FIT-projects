
#  File: Makefile
#  ISA 2018 - Project - Export DNS information using Syslog protocol
#  Author: Matus Liscinsky
#  Login: xlisci02
#  Mail: xlisci02@stud.fit.vutbr.cz
#  Date: 16.11.2018

CXX= g++
CXXFLAGS= -std=c++11 #-Wall -Wextra -Werror -pedantic
LDLIBS= -lpcap
EXECUTABLE= dns-export
RM= rm -f
SRC= $(wildcard *.cc)
OBJ= $(SRC:%.cc=%.o)

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LDLIBS) -o $(EXECUTABLE)

dns-export.o: dns-export.cc dns-export.h syslog.h stats.h dns-msg.h pkt-headers.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

dns-msg.o: dns-msg.cc dns-msg.h stats.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

pkt-headers.o: pkt-headers.cc pkt-headers.h dns-msg.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

stats.o: stats.cc stats.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

syslog.o: syslog.cc syslog.h dns-export.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(EXECUTABLE)
