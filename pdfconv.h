#ifndef __PDFCONV_H__
#define __PDFCONV_H__

#include <Python.h>
#include "wkhtmltox/pdf.h"
#include "callback.h"

PyTypeObject PDFConvertorType;

typedef struct {
	PyObject_HEAD;
	convertor_callbacks_t conv_callbacks;
	wkhtmltopdf_global_settings *conv_global;
	wkhtmltopdf_converter *conv_ptr;
} PDFConvertor;

#endif /* __PDFCONV_H__ */

