EXTRA_INCLUDE_DIRS=/usr/include/python2.7 ../source/64/include/
EXTRA_LIB_DIRS=../source/64/lib/
EXTRA_LIBS=python2.7 wkhtmltox
LDFLAGS=-shared -Wl,-soname,_pywkhtmltox $(addprefix -l,$(EXTRA_LIBS)) $(addprefix -L,$(EXTRA_LIB_DIRS))
CFLAGS=-Wall -O0 -fno-inline -fno-strict-aliasing --std=gnu99 -fPIC -g $(addprefix -I,$(EXTRA_INCLUDE_DIRS))
CXXFLAGS=-Wall -O0 -fno-inline -fno-strict-aliasing -fPIC -g $(addprefix -I,$(EXTRA_INCLUDE_DIRS))
SOURCES=register_conv.cpp utils.c pdfconv.c imageconv.c callback.c pywkhtmltox.c
OBJECTS=register_conv.o utils.o pdfconv.o imageconv.o callback.o pywkhtmltox.o
TARGET=_pywkhtmltox.so
CС=gcc
CXX=g++

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm *.o *.so
