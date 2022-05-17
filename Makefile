#
# Makefile for Six Degrees
#
# Project 2
#
# by Sean Liu, December 2021
# 

CXX      = clang++
CXXFLAGS = -g3 -Wall -Wextra -std=c++11 
INCLUDES = $(shell echo *.h)

SixDegrees: main.o SixDegrees.o CollabGraph.o Artist.o
	${CXX} -o $@ $^
	
unit_test: unit_test_driver.o CollabGraph.o Artist.o
	${CXX} ${CXXFLAGS} unit_test_driver.o CollabGraph.o Artist.o
	
%.o: %.cpp ${INCLUDES}
	${CXX} ${CXXFLAGS} -c $<

clean:
	rm -rf SixDegrees *.o *.dSYM
	
make provide1:
	provide comp15 proj2phase1 SixDegrees.cpp SixDegrees.h CollabGraph.cpp \
	CollabGraph.h main.cpp README Makefile unit_tests.h
	
make provide2:
	provide comp15 proj2phase2 SixDegrees.cpp SixDegrees.h CollabGraph.cpp \
	CollabGraph.h main.cpp README Makefile unit_tests.h emptyGraphData.txt \
	disconnectedGraphData.txt disconnectedGraphCommands.txt \
	invalidArtistsCommands.txt sameArtistCommands.txt my_DCcommands.txt \
	the_DCcommands.txt my_MTinvalidArtists.txt the_MTinvalidArtists.txt \
	my_OGsameArtist.txt the_OGsameArtist.txt the_OGinvalidArtists.txt \
	the_OGinvalidArtists.txt
