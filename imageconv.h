#ifndef __IMAGECONV_H__
#define __IMAGECONV_H__

#include <Python.h>
#include "wkhtmltox/image.h"
#include "callback.h"

PyTypeObject ImageConvertorType;

typedef struct {
	PyObject_HEAD;
	convertor_callbacks_t conv_callbacks;
	wkhtmltoimage_global_settings *conv_global;
	wkhtmltoimage_converter *conv_ptr;
} ImageConvertor;

#endif /* __IMAGECONV_H__ */