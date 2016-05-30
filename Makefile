OPENGL_LIBS = -lglut -lGL -lGLU

CXX = $(shell wx-config --version=3.0 --cxx)

SRC = logsim.cc names.cc scanner.cc network.cc parser.cc monitor.cc devices.cc userint.cc gui.cc gui_monitor.cc guitest.cc

L_OBJECTS = logsim.o names.o scanner.o network.o parser.o monitor.o devices.o userint.o gui.o gui_monitor.o error.o

G_OBJECTS = guitest.o names.o network.o monitor.o devices.o gui.o gui_monitor.o

# implementation

.SUFFIXES:	.o .cc

.cc.o :
	$(CXX) -std=c++11 -c `wx-config --version=3.0 --cxxflags` -g -o $@ $<

all:    logsim guitest

logsim:	$(L_OBJECTS)
	$(CXX) -o logsim $(L_OBJECTS) `wx-config --version=3.0 --libs std,aui,richtext --gl_libs` $(OPENGL_LIBS)

guitest: $(G_OBJECTS)
	$(CXX) -o guitest $(G_OBJECTS) `wx-config --version=3.0 --libs std,aui --gl_libs` $(OPENGL_LIBS)

testnames: names.o testnames.cc
	$(CXX) -std=c++11 -o testnames names.o testnames.cc

testscanner: names.o scanner.o testscanner.cc
	$(CXX) -std=c++11 -o testscanner names.o scanner.o testscanner.cc

testparser: error.o names.o scanner.o network.o monitor.o devices.o parser.o parsertest.cc
	$(CXX) -std=c++11 -o testparser names.o scanner.o network.o monitor.o devices.o parser.o error.o parsertest.cc

testsig: names.o network.o monitor.o devices.o testsig.cc
	$(CXX) -std=c++11 -o testsig names.o network.o monitor.o devices.o testsig.cc



clean:
	rm -f *.o logsim guitest

depend:
	makedepend $(SRC)

# DO NOT DELETE

logsim.o: logsim.h names.h devices.h network.h monitor.h parser.h userint.h error.h
logsim.o: gui.h gui_monitor.h
names.o: names.h
network.o: network.h names.h
parser.o: parser.h names.h network.h devices.h monitor.h error.h
monitor.o: monitor.h names.h network.h devices.h
devices.o: devices.h names.h network.h
userint.o: userint.h names.h network.h devices.h monitor.h
gui.o: gui.h gui_monitor.h gui_constants.h names.h devices.h network.h monitor.h
gui_monitor.o: gui_monitor.h gui.h gui_constants.h names.h devices.h network.h monitor.h
guitest.o: guitest.h names.h devices.h network.h monitor.h gui.h
