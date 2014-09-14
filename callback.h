#ifndef __CALLBACK_H__
#define __CALLBACK_H__

#include <Python.h>

typedef struct {
	PyObject *warning_cb;
	PyObject *error_cb;
	PyObject *finish_cb;
	PyObject *progress_cb;
	PyObject *phase_cb;
} convertor_callbacks_t;

typedef enum {
	PDF_CALLBACK_WARNING = 0,
	PDF_CALLBACK_ERROR,
	PDF_CALLBACK_FINISH,
	PDF_CALLBACK_PROGRESS,
	PDF_CALLBACK_PHASE
} callback_type_t;

#endif /* __CALLBACK_H__ */