#include <Python.h>
#include "wkhtmltox/image.h"
#include "imageconv.h"
#include "utils.h"
#include "callback.h"
#include "register_conv.h"

static PyObject *ImageConvertor_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ImageConvertor *self;

    self = (ImageConvertor *)type->tp_alloc(type, 0);

    return (PyObject *)self;
}

static void ImageConvertor_dealloc(ImageConvertor *self)
{
	unregister_obj(self);
	wkhtmltoimage_destroy_converter(self->conv_ptr);
	self->ob_type->tp_free((PyObject*)self);
	wkhtmltoimage_deinit();
}

static int ImageConvertor_init(ImageConvertor *self, PyObject *args, PyObject *kwds)
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

    if (!wkhtmltoimage_init(use_graphics_c))
    {
      PyErr_SetString(WkhtmltoxError, "Couldn't initialize wkhtmltox with graphics");
      return -1;
    }

    if (NULL == (self->conv_global = wkhtmltoimage_create_global_settings()))
    {
      PyErr_SetString(WkhtmltoxError, "Couldn't initialize global settings");
      return -1;
    }

    if (NULL == (self->conv_ptr = wkhtmltoimage_create_converter(self->conv_global, NULL)))
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
	wkhtmltoimage_set_error_callback(self->conv_ptr, callback_error);
	wkhtmltoimage_set_finished_callback(self->conv_ptr, callback_finished);
	wkhtmltoimage_set_phase_changed_callback(self->conv_ptr, callback_phase_changed);
	wkhtmltoimage_set_progress_changed_callback(self->conv_ptr, callback_progress_changed);
	wkhtmltoimage_set_warning_callback(self->conv_ptr, callback_warning);

    return 0;
}


static PyObject* ImageConvertor_set_global(ImageConvertor *self, PyObject *args)
{
	const char *key = NULL;
	const char *value = NULL;

	if (!PyArg_ParseTuple(args, "ss", &key, &value))
	{
		PyErr_BadArgument();
		return NULL;
	}

	if (!wkhtmltoimage_set_global_setting(self->conv_global, key, value))
	{
	    char error_msg[40];
		snprintf(error_msg, 40, "Unknown option '%s' or value '%s'", key, value);
		PyErr_SetString(WkhtmltoxError, error_msg);
		return NULL;
	}
	Py_RETURN_NONE;
}


static PyObject* ImageConvertor_get_global(ImageConvertor *self, PyObject *args)
{
	const char *key = NULL;
	char value[DEFAULT_BUF_LEN] = {0};


	if (!PyArg_ParseTuple(args, "s", &key))
	{
		PyErr_BadArgument();
		return NULL;
	}
	if (!wkhtmltoimage_get_global_setting(self->conv_global, key, value, DEFAULT_BUF_LEN))
	{
        PyErr_SetObject(WkhtmltoxError, args);
		return NULL;
	}

	return Py_BuildValue("s", value);
}

static PyObject *ImageConvertor_convert(ImageConvertor *self)
{
	int result_code = 0;

	if (!wkhtmltoimage_convert(self->conv_ptr))
	{
		result_code = wkhtmltoimage_http_error_code(self->conv_ptr);
		char error_msg[40];
		snprintf(error_msg, 40, "Convert error: %i", result_code);
		PyErr_SetString(WkhtmltoxError, error_msg);
		return NULL;
	}

	Py_RETURN_NONE;
}

static PyObject* ImageConvertor_add_callback(ImageConvertor *self, PyObject *args)
{
	PyObject *callback;
	int c_type;
	if (!PyArg_ParseTuple(args, "iO", c_type, &callback))
	{
		PyErr_BadArgument();
		return NULL;
	}
	if (0 == PyCallable_Check(callback))
	{
		PyErr_BadArgument();
		return NULL;
	}
	switch(c_type)
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

static PyObject* ImageConvertor_current_progress(ImageConvertor *self)
{
	PyObject *res;
	if (NULL == (res = PyString_FromString(wkhtmltoimage_progress_string(self->conv_ptr))))
	{
		Py_RETURN_NONE;
	}
	return res;
}

static PyObject* ImageConvertor_current_phase(ImageConvertor *self)
{
	int phase = wkhtmltoimage_current_phase(self->conv_ptr);
	return PyString_FromString(wkhtmltoimage_phase_description(self->conv_ptr, phase));
}


static PyMethodDef ImageConvertor_methods[] = {
    {"set_option", (PyCFunction)ImageConvertor_set_global, METH_VARARGS, "Set global option"},
    {"get_option", (PyCFunction)ImageConvertor_get_global, METH_VARARGS, "Get global option"},
    {"convert", (PyCFunction)ImageConvertor_convert, METH_NOARGS, "Convert Image"},
    {"add_callback", (PyCFunction)ImageConvertor_add_callback, METH_VARARGS, "Register callback"},
    {CURRENT_PROGRSS_METHOD, (PyCFunction)ImageConvertor_current_progress, METH_NOARGS, "Current progress"},
    {CURRENT_PHASE_METHOD, (PyCFunction)ImageConvertor_current_phase, METH_NOARGS, "Current phase"},
    {NULL}  /* Sentinel */
};



PyTypeObject ImageConvertorType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"pywkhtmltox.ImageConvertor",             /*tp_name*/
	sizeof(ImageConvertor),             /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)ImageConvertor_dealloc, /*tp_dealloc*/
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
	"Image convertor",           /* tp_doc */
	0,		               /* tp_traverse */
	0,		               /* tp_clear */
	0,		               /* tp_richcompare */
	0,		               /* tp_weaklistoffset */
	0,		               /* tp_iter */
	0,		               /* tp_iternext */
	ImageConvertor_methods,             /* tp_methods */
	0,             /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)ImageConvertor_init,      /* tp_init */
	0,                         /* tp_alloc */
	ImageConvertor_new,                 /* tp_new */
};