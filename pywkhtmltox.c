#include <Python.h>
#include "wkhtmltox/pdf.h"
#include <stdio.h>

#define DEFAULT_BUF_LEN	255

typedef struct {
	wkhtmltopdf_str_callback warning_cb;
	wkhtmltopdf_str_callback error_cb;
	wkhtmltopdf_int_callback finish_cb;
	wkhtmltopdf_int_callback progress_cb;
	wkhtmltopdf_void_callback phase_cb;
} convertor_callbacks_t;

typedef struct {
	PyObject_HEAD;
	wkhtmltopdf_global_settings *conv_global;
	wkhtmltopdf_converter *conv_ptr;
	convertor_callbacks_t conv_callbacks;
} PDFConvertor;


static void PDFConvertor_dealloc(PDFConvertor *self)
{
	wkhtmltopdf_destroy_converter(self->conv_ptr);
	self->ob_type->tp_free((PyObject*)self);
	wkhtmltopdf_deinit();
}

static PyObject *PDFConvertor_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PDFConvertor *self;

    self = (PDFConvertor *)type->tp_alloc(type, 0);

    return (PyObject *)self;
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
      return -1;
    }
    
    if (NULL == (self->conv_global = wkhtmltopdf_create_global_settings()))
    {
      return -1;
    }
    
    if (NULL == (self->conv_ptr = wkhtmltopdf_create_converter(self->conv_global)))
    {
      return -1;
    }

    return 0;
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
		PyErr_SetNone(PyExc_KeyError);
		return NULL;
	}
	Py_RETURN_NONE;
}

static PyObject* PDFConvertor_get_global(PDFConvertor *self, PyObject *args)
{
	const char *key = NULL;
	PyObject *result;
	char value[DEFAULT_BUF_LEN] = {0};
  
  
	if (!PyArg_ParseTuple(args, "s", &key))
	{
		PyErr_BadArgument();
		return NULL;
	}
	if (!wkhtmltopdf_get_global_setting(self->conv_global, key, value, DEFAULT_BUF_LEN))
	{
		PyErr_SetNone(PyExc_KeyError);
		return NULL;
	}
	
	result = Py_BuildValue("s", value);
	Py_XINCREF(result);
	
	return result;
}

static PyObject *PDFConvertor_convert(PDFConvertor *self)
{
	int result_code = 0;
	
	if (!wkhtmltopdf_convert(self->conv_ptr))
	{
		result_code = wkhtmltopdf_http_error_code(self->conv_ptr);
		char error_msg[40];
		snprintf(error_msg, 40, "Convert error: %i", result_code);
		PyErr_SetString(PyExc_Exception, error_msg);
		return NULL;
	}
	
	Py_RETURN_NONE;
}

static PyObject *PDFConvertor_add_object(PDFConvertor *self, PyObject *args)
{
	wkhtmltopdf_object_settings *settings = NULL;
	PyObject *dict;
	
	if (!PyArg_ParseTuple(args, "O!", &PyDict_Type, &dict)) {
		PyErr_SetString(PyExc_AttributeError, "Invalid attributes");
		return NULL;
	}
	
	if (NULL == (settings = wkhtmltopdf_create_object_settings()))
	{
		PyErr_SetString(PyExc_Exception, "Can't create object settings");
		return NULL;
	}
	
	PyObject *key, *value;
	Py_ssize_t pos = 0;
	
	while (PyDict_Next(dict, &pos, &key, &value))
	{
		if (!wkhtmltopdf_set_object_setting(settings, PyString_AsString(key), PyString_AsString(value)))
		{
			char error_msg[80];
			snprintf(error_msg, 80, "Invalid attribute: %s=%s", PyString_AsString(key), PyString_AsString(value));
			PyErr_SetString(PyExc_AttributeError, error_msg);
			return NULL;
		}
	}
	
	wkhtmltopdf_add_object(self->conv_ptr, settings, NULL);
	
	Py_RETURN_NONE;
}


static PyMethodDef PDFConvertor_methods[] = {
    {"set_option", (PyCFunction)PDFConvertor_set_global, METH_VARARGS, "Set global option"},
    {"get_option", (PyCFunction)PDFConvertor_get_global, METH_VARARGS, "Get global option"},
    {"convert", (PyCFunction)PDFConvertor_convert, METH_NOARGS, "Convert pdf"},
    {"add_object", (PyCFunction)PDFConvertor_add_object, METH_VARARGS, "Add object"},
    {NULL}  /* Sentinel */
};


static PyTypeObject PDFConvertorType = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"pywkhtmltox.PDFConvertor",             /*tp_name*/
	sizeof(PDFConvertorType),             /*tp_basicsize*/
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


static PyMethodDef wkhtmltox_methods[] = {
	{NULL}  /* Sentinel */
};

PyMODINIT_FUNC
initpywkhtmltox(void) 
{
	PyObject* m;

	PDFConvertorType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&PDFConvertorType) < 0)
		return;

	m = Py_InitModule3("pywkhtmltox", wkhtmltox_methods,
						"Wkhtmltox python module");

	Py_INCREF(&PDFConvertorType);
	PyModule_AddObject(m, "PDFConvertor", (PyObject *) &PDFConvertorType);
}
