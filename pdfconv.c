#include <Python.h>
#include "wkhtmltox/pdf.h"
#include "pdfconv.h"
#include "utils.h"
#include "callback.h"
#include "register_conv.h"

static PyObject *PDFConvertor_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PDFConvertor *self;

    self = (PDFConvertor *)type->tp_alloc(type, 0);

    return (PyObject *)self;
}

static void PDFConvertor_dealloc(PDFConvertor *self)
{
	unregister_obj(self->conv_ptr);
	wkhtmltopdf_destroy_converter(self->conv_ptr);
	wkhtmltopdf_destroy_global_settings(self->conv_global);
	self->ob_type->tp_free((PyObject*)self);
	wkhtmltopdf_deinit();
}

static int PDFConvertor_init(PDFConvertor *self, PyObject *args, PyObject *kwds)
{
    PyObject *use_graphics = Py_False;
    int use_graphics_c = 0;

    static char *kwlist[] = {"use_graphics", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O!", kwlist, &PyBool_Type, &use_graphics))
    {
      return -1;
    }

    if (PyObject_IsTrue(use_graphics))
	{
      use_graphics_c = 1;
    }

    if (!wkhtmltopdf_init(use_graphics_c))
    {
      PyErr_SetString(WkhtmltoxError, "Couldn't initialize wkhtmltox with graphics");
      return -1;
    }

    if (NULL == (self->conv_global = wkhtmltopdf_create_global_settings()))
    {
      PyErr_SetString(WkhtmltoxError, "Couldn't initialize global settings");
      return -1;
    }

    if (NULL == (self->conv_ptr = wkhtmltopdf_create_converter(self->conv_global)))
    {
      PyErr_SetString(WkhtmltoxError, "Couldn't initialize convertor");
      return -1;
    }
    if (REGISTER_ERROR_FAIL == register_obj(self->conv_ptr, (PyObject *)self))
	{
		return -1;
	}
	self->conv_callbacks.error_cb = PyList_New(0);
	self->conv_callbacks.finish_cb = PyList_New(0);
	self->conv_callbacks.phase_cb = PyList_New(0);
	self->conv_callbacks.progress_cb = PyList_New(0);
	self->conv_callbacks.warning_cb = PyList_New(0);
	wkhtmltopdf_set_error_callback(self->conv_ptr, callback_error);
	wkhtmltopdf_set_finished_callback(self->conv_ptr, callback_finished);
	wkhtmltopdf_set_phase_changed_callback(self->conv_ptr, callback_phase_changed);
	wkhtmltopdf_set_progress_changed_callback(self->conv_ptr, callback_progress_changed);
	wkhtmltopdf_set_warning_callback(self->conv_ptr, callback_warning);

    return 0;
}

static PyObject *PDFConvertor_convert(PDFConvertor *self)
{
	int result_code = 0;

	if (!wkhtmltopdf_convert(self->conv_ptr))
	{
		result_code = wkhtmltopdf_http_error_code(self->conv_ptr);
		char error_msg[40];
		snprintf(error_msg, 40, "Convert error: %i", result_code);
		PyErr_SetString(WkhtmltoxError, &error_msg);
		return NULL;
	}

	Py_RETURN_NONE;
}

static PyObject *PDFConvertor_add_object(PDFConvertor *self, PyObject *args)
{
	wkhtmltopdf_object_settings *settings = NULL;
	PyObject *dict;

	if (!PyArg_ParseTuple(args, "O!", &PyDict_Type, &dict)) {
		PyErr_BadArgument();
		return NULL;
	}

	if (NULL == (settings = wkhtmltopdf_create_object_settings()))
	{
		PyErr_SetString(WkhtmltoxError, "Can't create page");
		return NULL;
	}

	PyObject *key, *value;
	Py_ssize_t pos = 0;

	while (PyDict_Next(dict, &pos, &key, &value))
	{
		if (!wkhtmltopdf_set_object_setting(settings, PyString_AsString(key), PyString_AsString(value)))
		{
			char error_msg[80];
			snprintf(error_msg, 80, "Invalid page setting: %s=%s", PyString_AsString(key), PyString_AsString(value));
			PyErr_SetString(WkhtmltoxError, error_msg);
			return NULL;
		}
	}

	wkhtmltopdf_add_object(self->conv_ptr, settings, NULL);

	Py_RETURN_NONE;
}

static PyObject* PDFConvertor_set_global(PDFConvertor *self, PyObject *args)
{
	const char *key = NULL;
	const char *value = NULL;

	if (!PyArg_ParseTuple(args, "ss", &key, &value))
	{
		PyErr_BadArgument();
		return NULL;
	}

	if (!wkhtmltopdf_set_global_setting(self->conv_global, key, value))
	{
	    char error_msg[40];
		snprintf(error_msg, 40, "Unknown option '%s' or value '%s'", key, value);
		PyErr_SetString(WkhtmltoxError, error_msg);
		return NULL;
	}
	Py_RETURN_NONE;
}

static PyObject* PDFConvertor_current_progress(PDFConvertor *self)
{
	PyObject *res;
	if (NULL == (res = PyString_FromString(wkhtmltopdf_progress_string(self->conv_ptr))))
	{
		Py_RETURN_NONE;
	}
	return res;
}

static PyObject* PDFConvertor_current_phase(PDFConvertor *self)
{
	int phase = wkhtmltopdf_current_phase(self->conv_ptr);
	return PyString_FromString(wkhtmltopdf_phase_description(self->conv_ptr, phase));
}

static PyObject* PDFConvertor_get_global(PDFConvertor *self, PyObject *args)
{
	const char *key = NULL;
	char value[DEFAULT_BUF_LEN] = {0};


	if (!PyArg_ParseTuple(args, "s", &key))
	{
		PyErr_BadArgument();
		return NULL;
	}
	if (!wkhtmltopdf_get_global_setting(self->conv_global, key, value, DEFAULT_BUF_LEN))
	{
        PyErr_SetObject(WkhtmltoxError, args);
		return NULL;
	}

	return Py_BuildValue("s", value);
}

static PyObject* PDFConvertor_add_callback(PDFConvertor *self, PyObject *args)
{
	PyObject *callback;
	int callback_type = -1;
	if (!PyArg_ParseTuple(args, "Oi", &callback, &callback_type))
	{
		PyErr_BadArgument();
		return NULL;
	}
	if (0 == PyCallable_Check(callback))
	{
		PyErr_BadArgument();
		return NULL;
	}
	switch(callback_type)
	{
		case PDF_CALLBACK_WARNING:
			if (-1 == PyList_Append(self->conv_callbacks.warning_cb, callback)) {
				goto errorconv;
			}
			break;
		case PDF_CALLBACK_ERROR:
			if (-1 == PyList_Append(self->conv_callbacks.error_cb, callback)) {
				goto errorconv;
			}
			break;
		case PDF_CALLBACK_FINISH:
			if (-1 == PyList_Append(self->conv_callbacks.finish_cb, callback)) {
				goto errorconv;
			}
			break;
		case PDF_CALLBACK_PROGRESS:
			if (-1 == PyList_Append(self->conv_callbacks.progress_cb, callback)) {
				goto errorconv;
			}
			break;
		case PDF_CALLBACK_PHASE:
			if (-1 == PyList_Append(self->conv_callbacks.phase_cb, callback)) {
				goto errorconv;
			}
			break;
		default:
			PyErr_BadArgument();
			return NULL;
		
	}
	Py_XINCREF(callback);
	return callback;
errorconv:
PyErr_SetString(WkhtmltoxError, "Couldn\'t register callback");
return NULL;
}


static PyMethodDef PDFConvertor_methods[] = {
    {"set_option", (PyCFunction)PDFConvertor_set_global, METH_VARARGS, "Set global option"},
    {"get_option", (PyCFunction)PDFConvertor_get_global, METH_VARARGS, "Get global option"},
    {"convert", (PyCFunction)PDFConvertor_convert, METH_NOARGS, "Convert pdf"},
    {"add_page", (PyCFunction)PDFConvertor_add_object, METH_VARARGS, "Add page object"},
	{"add_callback", (PyCFunction)PDFConvertor_add_callback, METH_VARARGS, "Add finish callback"},
	{CURRENT_PROGRSS_METHOD, (PyCFunction)PDFConvertor_current_progress, METH_NOARGS, "Current progress"},
    {CURRENT_PHASE_METHOD, (PyCFunction)PDFConvertor_current_phase, METH_NOARGS, "Current phase"},
    {NULL}  /* Sentinel */
};

PyTypeObject PDFConvertorType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"pywkhtmltox.PDFConvertor",             /*tp_name*/
	sizeof(PDFConvertor),             /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)PDFConvertor_dealloc, /*tp_dealloc*/
	0,                         /*tp_print*/
	0,                         /*tp_getattr*/
	0,                         /*tp_setattr*/
	0,                         /*tp_compare*/
	0,                         /*tp_repr*/
	0,                         /*tp_as_number*/
	0,                         /*tp_as_sequence*/
	0,                         /*tp_as_mapping*/
	0,                         /*tp_hash */
	0,                         /*tp_call*/
	0,                         /*tp_str*/
	0,                         /*tp_getattro*/
	0,                         /*tp_setattro*/
	0,                         /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
	"PDF convertor",           /* tp_doc */
	0,		               /* tp_traverse */
	0,		               /* tp_clear */
	0,		               /* tp_richcompare */
	0,		               /* tp_weaklistoffset */
	0,		               /* tp_iter */
	0,		               /* tp_iternext */
	PDFConvertor_methods,             /* tp_methods */
	0,             /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)PDFConvertor_init,      /* tp_init */
	0,                         /* tp_alloc */
	PDFConvertor_new,                 /* tp_new */
};