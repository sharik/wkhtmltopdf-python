#include "wkhtmltox/pdf.h"
#include "pdfconv.h"
#include "imageconv.h"
#include "utils.h"
#include "callback.h"

extern PyObject* WkhtmltoxError;
extern PyTypeObject PDFConvertorType;
extern PyTypeObject ImageConvertorType;



static PyObject * pywkhtmltox_get_version()
{
	PyObject * result;
	result = Py_BuildValue("s", wkhtmltopdf_version());
	Py_XINCREF(result);
	return result;
};

static PyMethodDef pywkhtmltox_methods[] = {
    {"get_version", (PyCFunction)pywkhtmltox_get_version, METH_NOARGS, "Get Wkhtmltox version"},
    {NULL, NULL}
};

#if PY_MAJOR_VERSION >= 3
struct PyModuleDef moduledef = {
  PyModuleDef_HEAD_INIT,
  "_pywkhtmltox",
  "Wkhtmltox python module",
  -1,
  pywkhtmltox_methods,
  NULL,
  NULL,
  NULL,
  NULL
};
#endif



#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC PyInit__pywkhtmltox(void) {
    PyObject* m = PyModule_Create(&moduledef);
#else
PyMODINIT_FUNC init_pywkhtmltox(void)
{

	PyObject* m = Py_InitModule3("_pywkhtmltox", pywkhtmltox_methods, "Wkhtmltox bindings");
#endif

	PDFConvertorType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&PDFConvertorType) < 0) {
		fprintf(stderr, "Invalid PyTypeObject `PDFConvertorType`\n");
		goto error;
	}
	
	ImageConvertorType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&ImageConvertorType) < 0) {
		fprintf(stderr, "Invalid PyTypeObject `ImageConvertorType`\n");
		goto error;
	}
		
    WkhtmltoxError = PyErr_NewException("_pywkhtmltox.WkhtmltoxError", NULL, NULL);
	PyModule_AddObject(m, "WkhtmltoxError", (PyObject*)WkhtmltoxError);

	Py_INCREF(&WkhtmltoxError);
	Py_INCREF(&PDFConvertorType);
	Py_INCREF(&ImageConvertorType);
	PyModule_AddObject(m, "PDFConvertor", (PyObject *) &PDFConvertorType);
	PyModule_AddObject(m, "ImageConvertor", (PyObject *) &ImageConvertorType);

#if PY_MAJOR_VERSION >= 3
    return m;
#endif

error:
#if PY_MAJOR_VERSION >= 3
  return NULL;
#else
  return;
#endif
}