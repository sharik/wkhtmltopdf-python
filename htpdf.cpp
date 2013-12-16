#include <Python.h>
#include "wkhtmltox/pdf.h"

#define HTPDF_SUCCESS	1
#define HTPDF_DEFAULT_BUF_LEN	255

/*
Complex properties:
	customHeaders[1].Accept-Language=ru

TODO:
  * Implement Converter and object settings as classes
  * Add image converter
  * Throw exceptions on failures
  * Build complex property strings from python dicts / tuples / lists (screw it?)
*/

static wkhtmltopdf_global_settings *g_GlobalSettings = NULL;
typedef struct {
	wkhtmltopdf_str_callback warning_cb;
	wkhtmltopdf_str_callback error_cb;
	wkhtmltopdf_int_callback finish_cb;
	wkhtmltopdf_int_callback progress_cb;
	wkhtmltopdf_void_callback phase_cb;
} convertor_callbacks_t;

typedef struct {
	wkhtmltopdf_converter *conv_ptr;
	wkhtmltopdf_global_settings *conv_global;
	convertor_callbacks_t callbacks;
} convertor_t;



// !!!!!! TODO: throw on failure !!!!!!!!!!!!!!!!!!!!!!!!!

/** Init/Deinit functions **/
static PyObject *htpdf_init(PyObject *self, PyObject *args)
{
	PyObject *result = Py_False;
	int use_graphics = 0;

	if (!PyArg_ParseTuple(args, "i", &use_graphics))
        return NULL;
	if (HTPDF_SUCCESS == wkhtmltopdf_init(use_graphics))
	{
		result = Py_True;
	}

	if (Py_True == result)
	{
		if (NULL == (g_GlobalSettings = wkhtmltopdf_create_global_settings()))
		{
			result = Py_False;
		}
	}

	Py_INCREF(result);
	return result;
}

static PyObject *htpdf_deinit(PyObject *self, PyObject *args)
{
	PyObject *result = Py_False;

	if (HTPDF_SUCCESS == wkhtmltopdf_deinit())
	{
		result = Py_True;
	}

	Py_INCREF(result);
	return result;
}

/** Settings initializers **/
static PyObject *htpdf_create_obj_settings(PyObject *self, PyObject *args)
{
	PyObject *result = Py_False;
	wkhtmltopdf_object_settings *os = NULL;
	
	if (NULL != (os = wkhtmltopdf_create_object_settings()))
	{
		result = Py_BuildValue("K", (unsigned long long)os);
	}

	if (Py_False == result)
	{
		Py_INCREF(result);
	}
	return result;
}

/** Settings getters/setters **/
static PyObject *htpdf_set_global(PyObject *self, PyObject *args)
{
	PyObject *result = Py_False;
	const char *name = NULL;
	const char *value = NULL;

	if (!PyArg_ParseTuple(args, "ss", &name, &value))
        return NULL;

	if (HTPDF_SUCCESS == wkhtmltopdf_set_global_setting(g_GlobalSettings, name, value))
	{
		result = Py_True;
	}

	Py_INCREF(result);
	return result;
}

// TODO: don't let the buffer to fuck you
static PyObject *htpdf_get_global(PyObject *self, PyObject *args)
{
	PyObject *result = Py_False;
	const char *name = NULL;
	char value[HTPDF_DEFAULT_BUF_LEN] = {0};

	if (!PyArg_ParseTuple(args, "ss", &name, &value))
        return NULL;

	if (HTPDF_SUCCESS == wkhtmltopdf_get_global_setting(g_GlobalSettings, name, value, HTPDF_DEFAULT_BUF_LEN))
	{
		result = Py_BuildValue("s", value);
	}
	else
	{
		Py_INCREF(result);
	}

	return result;
}

static PyObject *htpdf_set_object(PyObject *self, PyObject *args)
{
	PyObject *result = Py_False;
	uint64_t settings_ptr = 0;
	const char *name = NULL;
	const char *value = NULL;
	wkhtmltopdf_object_settings *settings = NULL;

	if (!PyArg_ParseTuple(args, "Kss", &settings_ptr, &name, &value))
        return NULL;

    settings = (wkhtmltopdf_object_settings*)settings_ptr;

	if (HTPDF_SUCCESS == wkhtmltopdf_set_object_setting(settings, name, value))
	{
		result = Py_True;
	}

	Py_INCREF(result);
	return result;
}

// TODO: don't let the buffer to fuck you
static PyObject *htpdf_get_object(PyObject *self, PyObject *args)
{
	PyObject *result = Py_False;
	uint64_t settings_ptr = 0;
	const char *name = NULL;
	char value[HTPDF_DEFAULT_BUF_LEN] = {0};
	wkhtmltopdf_object_settings *settings = NULL;

	if (!PyArg_ParseTuple(args, "Kss", &settings_ptr, &name, &value))
        return NULL;

    settings = (wkhtmltopdf_object_settings*)settings_ptr;

	if (HTPDF_SUCCESS == wkhtmltopdf_get_object_setting(settings, name, value, HTPDF_DEFAULT_BUF_LEN))
	{
		result = Py_BuildValue("s", value);
	}
	else
	{
		Py_INCREF(result);
	}

	return result;
}


/** Converter initializers **/
static PyObject *htpdf_create_converter(PyObject *self, PyObject *args)
{
	PyObject *result = Py_False;
	wkhtmltopdf_converter *conv = NULL;
	
	if (NULL != (conv = wkhtmltopdf_create_converter(g_GlobalSettings)))
	{
		result = Py_BuildValue("K", (unsigned long long)conv);
	}

	if (Py_False == result)
	{
		Py_INCREF(result);
	}
	return result;
}

static PyObject *htpdf_destroy_converter(PyObject *self, PyObject *args)
{
	PyObject *result = Py_None;
	uint64_t conv_ptr = 0;
	wkhtmltopdf_converter *conv = NULL;

	if (!PyArg_ParseTuple(args, "K", &conv_ptr))
        return NULL;
	
	conv = (wkhtmltopdf_converter*)conv_ptr;
	wkhtmltopdf_destroy_converter(conv);

	Py_INCREF(result);
	return result;
}


/** Converting itself **/
static PyObject *htpdf_conv_add_obj(PyObject *self, PyObject *args)
{
	PyObject *result = Py_None;
	uint64_t conv_ptr = 0;
	uint64_t obj_ptr = 0;
	wkhtmltopdf_converter *conv = NULL;
	wkhtmltopdf_object_settings *obj = NULL;

	if (!PyArg_ParseTuple(args, "KK", &conv_ptr, &obj_ptr))
        return NULL;
	
	conv = (wkhtmltopdf_converter*)conv_ptr;
	obj = (wkhtmltopdf_object_settings*)obj_ptr;

	wkhtmltopdf_add_object(conv, obj, NULL);

	Py_INCREF(result);
	return result;
}

static PyObject *htpdf_convert(PyObject *self, PyObject *args)
{
	PyObject *result = Py_False;
	uint64_t conv_ptr = 0;
	wkhtmltopdf_converter *conv = NULL;

	if (!PyArg_ParseTuple(args, "K", &conv_ptr))
    	return NULL;

    conv = (wkhtmltopdf_converter*)conv_ptr;
	if (HTPDF_SUCCESS == wkhtmltopdf_convert(conv))
	{
		result = Py_True;
	}

	Py_INCREF(result);
	return result;
}

/** Callbacks **/
static PyObject *finish_cb = NULL;

// #define CONCAT(a, b) _CONCAT(a, b,)

// #define COOL_CALLBACK(type, event) \
// static PyObject *CONCAT(event,_cb) = NULL; \
// static void CONCAT(type, _callback_helper) (wkhtmltopdf_converter * converter, const type val) { \
// 	PyObject *arglist; \
// 	if (CONCAT(event,_cb)) \
// 	{	\
// 		arglist = Py_BuildValue("(i)", val);	\
// 		if (NULL == PyObject_CallObject(CONCAT(event,_cb), arglist))	\
// 		{	\
// 			PyErr_SetString(PyExc_RuntimeError, "failed to call callback function");	\
// 		}	\
// 	} \
// }
// COOL_CALLBACK(int, warning)


static void int_callback_helper(wkhtmltopdf_converter * converter, const int val)
{
	PyObject *arglist;

	if (finish_cb)
	{
		arglist = Py_BuildValue("(i)", val);
		if (NULL == PyObject_CallObject(finish_cb, arglist))
		{
			PyErr_SetString(PyExc_RuntimeError, "failed to call callback function");
		}
	}
}

static PyObject *htpdf_set_finished_callback(PyObject *self, PyObject *args)
{
	PyObject *temp;
	uint64_t conv_ptr = 0;
	wkhtmltopdf_converter *conv = NULL;

	if (PyArg_ParseTuple(args, "KO:set_finish_callback", &conv_ptr, &temp)) {
    	if (!PyCallable_Check(temp)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        
        Py_XINCREF(temp);       /* Add a reference to new callback */
	    Py_XDECREF(finish_cb);  /* Dispose of previous callback */
        finish_cb = temp;       /* Remember new callback */
    }

    conv = (wkhtmltopdf_converter*)conv_ptr;
	wkhtmltopdf_set_finished_callback(conv, int_callback_helper);

	Py_INCREF(Py_None);
	return Py_None;
}

/*
    wkhtmltopdf_global_settings *wkhtmltopdf_create_global_settings()
    wkhtmltopdf_object_settings *wkhtmltopdf_create_object_settings()
    
    bint wkhtmltopdf_set_global_setting(wkhtmltopdf_global_settings *settings, char *name, char *value)
    bint wkhtmltopdf_get_global_setting(wkhtmltopdf_global_settings *settings, char *name, char *value, int vs)
    bint wkhtmltopdf_set_object_setting(wkhtmltopdf_object_settings *settings, char *name, char *value)
    bint wkhtmltopdf_get_object_setting(wkhtmltopdf_object_settings *settings, char *name, char *value, int vs)
    
    wkhtmltopdf_converter *wkhtmltopdf_create_converter(wkhtmltopdf_global_settings *settings)
    void wkhtmltopdf_destroy_converter(wkhtmltopdf_converter *converter)
    
    bint wkhtmltopdf_convert(wkhtmltopdf_converter *converter)
    void wkhtmltopdf_add_object(wkhtmltopdf_converter *converter, wkhtmltopdf_object_settings *setting, char *data)
    
    int wkhtmltopdf_http_error_code(wkhtmltopdf_converter *converter)
*/

static PyMethodDef HtpdfMethods[] = {
 	{"init",  htpdf_init, METH_VARARGS, "Initialize wkhtmltopdf."},
 	{"deinit",  htpdf_init, METH_NOARGS, "Destroy wkhtmltopdf."},
 	{"create_obj_settings",  htpdf_create_obj_settings, METH_NOARGS, "Create object settings."},
 	{"set_global",  htpdf_set_global, METH_VARARGS, "Set global property."},
 	{"get_global",  htpdf_get_global, METH_VARARGS, "Get global property."},
 	{"set_object",  htpdf_set_object, METH_VARARGS, "Set object property."},
 	{"get_object",  htpdf_get_object, METH_VARARGS, "Get object property."},
 	{"create_converter",  htpdf_create_converter, METH_NOARGS, "Create converter."},
 	{"destroy_converter",  htpdf_destroy_converter, METH_VARARGS, "Destroy converter."},
 	{"conv_add_object",  htpdf_conv_add_obj, METH_VARARGS, "Add object to converter."},
 	{"convert",  htpdf_convert, METH_VARARGS, "Convert, mothafucka."},
 	{"set_finish_callback",  htpdf_set_finished_callback, METH_VARARGS, "Set finish callback."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
inithtpdf(void)
{
    (void) Py_InitModule("htpdf", HtpdfMethods);
}

