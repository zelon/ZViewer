CC          = gcc
CXX         = g++
INC         = -I"lib" -I"ZViewer\res" -I"ZViewer" -I"commonSrc"
LIBS        =
CFLAGS      = -DUNICODE -D_WINDOWS_ -D_UNICODE
#CFLAGS      = -Wall -Winline -DDEBUG -ggdb

# import from cpp list
-include srclist.txt

#RESS        = $(HOME_DIR)\res\ZViewer.rc
RESS        =
TARGET = ZViewer.exe

CRES = $(RESS:.rc=.rco)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

.PHONY : clean srclist new all $(TARGET)

all:$(TARGET)
srclist.txt: ConvertFromVCProj.py
	python ConvertFromVCProj.py > srclist.txt

$(TARGET):srclist.txt $(OBJS) $(CRES)
	@echo "CREATE [$@]"
	$(CXX) -o $@ $(OBJS) $(CRES)

clean:
	@del /Q $(OBJS) $(DEPS) $(TARGET) core

new: 
	@$(MAKE) -s clean
	@$(MAKE) -s

%.rco:%.rc
	@echo Compile RC $< to $@
	cd $(HOME_DIR)\res
	windres $< -o $@
	cd $(HOME_DIR)

%.o:%.cpp
	@echo Compile $< to $@
	@$(CXX) $(INC) $(CFLAGS) -c $< -o $@

%.d:%.cpp
	@echo Dependency Reset $< to $@
	@$(CXX) -MM $(INC) $(CFLAGS) $< > $@.$$$$
	@sed "s,\($(basename $@)\)\.o[ :]*,\1.o $@ : ,g" < $@.$$$$ > $@
	@del /Q $@.$$$$

-include $(DEPS)

