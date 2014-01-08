EXTRA_INCLUDE_DIRS=/usr/include/python2.7 ./build/64/include/
EXTRA_LIB_DIRS=./build/64/lib/
EXTRA_LIBS=python2.7 wkhtmltox
LDFLAGS=-shared -Wl,-soname,pywkhtmltox $(addprefix -l,$(EXTRA_LIBS)) $(addprefix -L,$(EXTRA_LIB_DIRS))
CFLAGS=-Wall --std=gnu99 -fPIC -g $(addprefix -I,$(EXTRA_INCLUDE_DIRS))
SOURCES=pywkhtmltox.c
OBJECTS=pywkhtmltox.o
TARGET=pywkhtmltox.so
CС=gcc

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm *.o *.so
