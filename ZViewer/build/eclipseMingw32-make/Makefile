# TARGET is execute filename
TARGET = a.out
COMPILER = g++
BASE_SRC = D:\eclipseWorkspaceForCpp\ZViewer
COMPILER_OPTION = -g -I$(BASE_SRC)\lib\freeimage -I$(BASE_SRC)\ZViewer\src -I$(BASE_SRC)\lib -I$(BASE_SRC)\ZViewer -I$(BASE_SRC)\commonSrc
# ===== end of declaration var =====
all: $(TARGET)

# To use pre-compiled header, uncomment below 3 line and make stdafx.h
#stdafx.h.gch: stdafx.h
#	$(COMPILER) $(COMPILER_OPTION) -x c++-header stdafx.h -o stdafx.h.gch
#PRECOMPILED = stdafx.h.gch
# end of pre-compiled settings

$(TARGET): $(PRECOMPILED)  Main.o stdafx.o
	$(COMPILER) $(COMPILER_OPTION) -o $(TARGET) Main.o stdafx.o

Main.o: $(BASE_SRC)\ZViewer\Main.cpp
	$(COMPILER) $(COMPILER_OPTION) $(BASE_SRC)\ZViewer\Main.cpp -c -o Main.o

stdafx.o: $(BASE_SRC)\stdafx.cpp $(BASE_SRC)\stdafx.h
	$(COMPILER) $(COMPILER_OPTION) $(BASE_SRC)\ZViewer\stdafx.cpp -c -o stdafx.o

clean:
	rm -f *.o
	rm -f $(TARGET)

	rm -f $(PRECOMPILED)

