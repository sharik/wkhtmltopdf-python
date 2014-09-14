#include <Python.h>
#include "utils.h"
#include "callback.h"
#include "pdfconv.h"
#include "register_conv.h"

static void callback_finished(void *conv, const int val)
{
	PDFConvertor *obj = get_registered(conv);
	if (NULL == obj) {
		PyErr_SetString(WkhtmltoxError, "Unknown converter");
		return;
	}
	PyObject *arglist;

	if (PyList_Check(obj->conv_callbacks.finish_cb))
	{
		arglist = Py_BuildValue("(oi)", (PyObject *)obj, val);
		PyObject *iterator = PyObject_GetIter(obj->conv_callbacks.finish_cb);
		PyObject *item;
		while (item = PyIter_Next(iterator))
		{
			PyObject_CallObject(item, arglist);
			Py_DECREF(item);
		}
		Py_DECREF(iterator);
	}
	if (PyErr_Occurred())
	{
		return;
	}
}

static void callback_error(void *conv, const char *msg)
{
	PDFConvertor *obj = get_registered(conv);
	if (NULL == obj) {
		PyErr_SetString(WkhtmltoxError, "Unknown converter");
		return;
	}
	PyObject *arglist;

	if (PyList_Check(obj->conv_callbacks.error_cb))
	{
		arglist = Py_BuildValue("(Os)", (PyObject *)obj, *msg);
		PyObject *iterator = PyObject_GetIter(obj->conv_callbacks.error_cb);
		PyObject *item;
		while (item = PyIter_Next(iterator))
		{
			PyObject_CallObject(item, arglist);
			Py_DECREF(item);
		}
		Py_DECREF(iterator);
	}
	if (PyErr_Occurred())
	{
		return;
	}
}

static void callback_progress_changed(void *conv, int val)
{
	PDFConvertor *obj = get_registered(conv);
	if (NULL == obj) {
		PyErr_SetString(WkhtmltoxError, "Unknown converter");
		return;
	}
	PyObject *arglist;

	if (PyList_Check(obj->conv_callbacks.progress_cb))
	{
		arglist = Py_BuildValue("(OO)", (PyObject *)obj, (PyObject *)PyObject_CallMethod((PyObject *)obj, (char *)current_progress_method, NULL));
		PyObject *iterator = PyObject_GetIter(obj->conv_callbacks.error_cb);
		PyObject *item;
		while (item = PyIter_Next(iterator))
		{
			PyObject_CallObject(item, arglist);
			Py_DECREF(item);
		}
		Py_DECREF(iterator);
	}
	if (PyErr_Occurred())
	{
		return;
	}
}

static void callback_warning(void *conv, const char *msg)
{
	PDFConvertor *obj = get_registered(conv);
	if (NULL == obj) {
		PyErr_SetString(WkhtmltoxError, "Unknown converter");
		return NULL;
	}
	PyObject *arglist;

	if (PyList_Check(obj->conv_callbacks.warning_cb))
	{
		arglist = Py_BuildValue("(Os)", (PyObject *)obj, *msg);
		PyObject *iterator = PyObject_GetIter(obj->conv_callbacks.warning_cb);
		PyObject *item;
		while (item = PyIter_Next(iterator))
		{
			PyObject_CallObject(item, arglist);
			Py_DECREF(item);
		}
		Py_DECREF(iterator);
	}
	if (PyErr_Occurred())
	{
		return;
	}
}

static void callback_phase_changed(void *conv)
{
	PDFConvertor *obj = get_registered(conv);
	if (NULL == obj) {
		PyErr_SetString(WkhtmltoxError, "Unknown converter");
		return;
	}
	PyObject *arglist;

	if (PyList_Check(obj->conv_callbacks.warning_cb))
	{
		arglist = Py_BuildValue("(OO)", (PyObject *)obj, (PyObject *)PyObject_CallMethod((PyObject *)obj, (char *)current_phase_method, NULL));
		PyObject *iterator = PyObject_GetIter(obj->conv_callbacks.warning_cb);
		PyObject *item;
		while (item = PyIter_Next(iterator))
		{
			PyObject_CallObject(item, arglist);
			Py_DECREF(item);
		}
		Py_DECREF(iterator);
	}
	if (PyErr_Occurred())
	{
		return;
	}
}

