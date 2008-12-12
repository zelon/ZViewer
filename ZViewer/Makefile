CC          = gcc
CXX         = g++
INC         = -I"lib" -I"ZViewer\res" -I"ZViewer" -I"commonSrc"
LIBS        =
CFLAGS      = -DUNICODE -D_WINDOWS_ -D_UNICODE -D_WIN32_IE=0x0500
#CFLAGS      = -Wall -Winline -DDEBUG -ggdb

# import from cpp list
-include srclist.txt

#RESS        = $(HOME_DIR)\res\ZViewer.rc
RESS        =
TARGET = ZViewer.exe

CRES = $(RESS:.rc=.rco)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)
DEPS += $(PREDEPS)

PRECOMPILED_HEADER = ZViewer\stdafx.h
PRECOMPILED_HEADER_OUTPUT = $(PRECOMPILED_HEADER:.h=.h.gch)
PREDEPS = ZViewer\stdafx.h.d

.PHONY : clean new all $(TARGET)

all:$(TARGET)

VCDebug:
	ConsoleBuild.bat

VCRebuild:
	ConsoleBuild.bat rebuild

VCClean:
	ConsoleBuild.bat clean

srclist.txt: ConvertFromVCProj.py
	@echo Creating source files list...
	@python ConvertFromVCProj.py > srclist.txt

$(TARGET):$(PRECOMPILED_HEADER_OUTPUT) srclist.txt $(OBJS) $(CRES)
	@echo "CREATE [$@]"
	@$(CXX) -o $@ $(PRECOMPILED_HEADER_OUTPUT) $(OBJS) $(CRES)
	@echo [OK] Build Completed

$(PRECOMPILED_HEADER_OUTPUT): $(PRECOMPILED_HEADER) $(PRECOMPILED_HEADER:.h=.d)
	@echo Compile Precompiled header
	@$(CXX) -x c++-header $(PRECOMPILED_HEADER) -o $(PRECOMPILED_HEADER_OUTPUT) $(INC) $(CFLAGS)

clean:
	@del /Q $(PRECOMPILED_HEADER_OUTPUT) $(OBJS) $(DEPS) $(TARGET) core 2> NUL
	@echo [OK] cleaned

new: 
	@$(MAKE) -s clean
	@$(MAKE) -s

%.rco:%.rc
	@echo Compile RC $< to $@
	cd $(HOME_DIR)\res
	windres $< -o $@
	cd $(HOME_DIR)

%.o:%.cpp
	@echo $<
	@$(CXX) $(INC) $(CFLAGS) -c $< -o $@

%.d:%.cpp
	@echo Dependency Reset $<
	@$(CXX) -MM $(INC) $(CFLAGS) $< > $@.$$$$
	@sed "s,\($(basename $(notdir $@))\)\.o[ :]*,$(subst \,\\,$(basename $@)).o $(subst \,\\,$@) : ,g" < $@.$$$$ > $@
	@del /Q $@.$$$$

# h 면 precompiled header 뿐이다.
$(PREDEPS):$(PRECOMPILED_HEADER)
	@echo Precompiled header Dependency Reset $< to $@
	@$(CXX) -MM $(INC) $(CFLAGS) $< > $@.$$$$
	@sed "s,\($(basename $(basename $(notdir $@)))\)\.o[ :]*,$(subst \,\\,$(PRECOMPILED_HEADER_OUTPUT)) $(subst \,\\,$@) : ,g" < $@.$$$$ > $@
	@del /Q $@.$$$$

-include $(DEPS)

