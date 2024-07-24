


#CXX           = g++ 

#FLAGS =  -fPIC -DLINUX -O2 -g -I/usr/include -I./include

CXX = g++ -fPIC -DLINUX -O2 -I./include

#DEPLIBS	=	-lCAENDPPLib -lm -lc -lrt -lpthread

#LIBS	=	-L/usr/lib

ROOTINCL  := $(shell root-config --incdir)
ROOTLIB  := $(shell root-config --libdir)


LIBS          := -L -lpthread -lutil -lusb-1.0 -L $(ROOTLIB) -lGeom -lRGL -lGed -lTreePlayer -lCore -lHist -lGraf -lGraf3d -lMathCore -lGpad -lTree -lRint -lRIO -lPostscript -lMatrix -lPhysics -lMinuit -lGui -lASImage -lASImageGui -pthread -lm -ldl -rdynamic -lstdc++

#INCLUDES = /usr/include/CAENDPPLib.h /usr/include/CAENDPPLibTypes.h ./include/*

.PHONY: all clean
	
all: clean DSRoot
	
clean:
		@rm -rf DSRoot *.o

#mainDict.cxx : LinkDef.h
#	rootcling -f $@ $^
#	$(info [0%]  Dictionary)


#DSRoot: main.o 
#	$(CXX) -o DSRoot main.o $(DEPLIBS) $(LIBS)
#	@echo "Done "$@

guiDict.cxx : include/DSFrame.h include/DSParams.h include/DSOpt.h include/DSCalib.h include/LinkDef.h
	@rootcling -f $@ $^
	$(info [-10%]  Dictionary)	

DSRoot:  DSFunc.o DSReadout.o DSFrame.o DSParams.o DSOpt.o DSCalib.o DSRoot.o
		$(info [70%]  Linking)
		@$(CXX) -o DSRoot src/DSRoot.o guiDict.cxx src/DSFrame.o src/DSParams.o src/DSOpt.o src/DSCalib.o src/DSReadout.o src/DSFunc.o $(LIBS) -L. -lCAENDPPLib `root-config --cflags --glibs`
		$(info [100%] Built target DSRoot)
	

DSFunc.o:  src/DSFunc.c
		$(info [10%] Generating DSFunc.o)
		@$(CXX) -o src/DSFunc.o -c src/DSFunc.c	`root-config --cflags --glibs`	
		
DSReadout.o:  src/DSReadout.c
		$(info [20%] Generating DSReadout.o)
		@$(CXX) -o src/DSReadout.o -c src/DSReadout.c	`root-config --cflags --glibs`			

DSFrame.o:  src/DSFrame.c
		$(info [30%] Generating DSFrame.o)
		@$(CXX) -o src/DSFrame.o -c src/DSFrame.c `root-config --cflags --glibs`		
		
DSParams.o:  src/DSParams.c
		$(info [40%] Generating DSParams.o)
		@$(CXX) -o src/DSParams.o -c src/DSParams.c `root-config --cflags --glibs`				

DSOpt.o: src/DSOpt.c guiDict.cxx
		$(info [50%] Generation DSOpt.o)
		@$(CXX) -o src/DSOpt.o -c src/DSOpt.c `root-config --cflags --glibs`			
		
DSCalib.o: src/DSCalib.c guiDict.cxx
		$(info [60%] Generation DSCalib.o)
		@$(CXX) -o src/DSCalib.o -c src/DSCalib.c `root-config --cflags --glibs`					
	
DSRoot.o: src/DSRoot.c guiDict.cxx
		$(info [70%] Generation DSRoot.o)
		@$(CXX) -o src/DSRoot.o -c src/DSRoot.c `root-config --cflags --glibs`	
		

	
