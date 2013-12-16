EXTRA_INCLUDE_DIRS=/usr/include/python2.7 ./build/64/include/
EXTRA_LIB_DIRS=./build/64/lib/
EXTRA_LIBS=python2.7 wkhtmltox
LDFLAGS=-shared -Wl,-soname,htpdf $(addprefix -l,$(EXTRA_LIBS)) $(addprefix -L,$(EXTRA_LIB_DIRS))
CXXFLAGS=-fPIC -g $(addprefix -I,$(EXTRA_INCLUDE_DIRS))
SOURCES=htpdf.cpp
OBJECTS=htpdf.o
TARGET=htpdf.so
CXX=g++

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm *.o *.so
