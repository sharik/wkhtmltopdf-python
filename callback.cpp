#include <map>
#ifdef __cplusplus
extern "C" {
#endif
#include <Python.h>
#ifdef __cplusplus
}
#endif
#include "wkhtmltox/pdf.h"
#include "callback.h"

typedef std::map<wkhtmltopdf_converter*, convertor_callbacks_t> callback_map_t;
static callback_map_t callbacks;

#ifdef __cplusplus
extern "C" {
#endif

lookup_error_t add_callback(callback_type_t cb_type, wkhtmltopdf_converter *conv, PyObject *callback)
{
	lookup_error_t res = LOOKUP_ERROR_SUCCESS;
	switch(cb_type)
	{
		case PDF_CALLBACK_WARNING:
			callbacks[conv].warning_cb = callback;
		break;
		case PDF_CALLBACK_ERROR:
			callbacks[conv].error_cb = callback;
		break;
		case PDF_CALLBACK_FINISH:
			callbacks[conv].finish_cb = callback;
		break;
		case PDF_CALLBACK_PROGRESS:
			callbacks[conv].progress_cb = callback;
		break;
		case PDF_CALLBACK_PHASE:
			callbacks[conv].phase_cb = callback;
		break;
		default:
			res = LOOKUP_ERROR_FAIL;
		break;
	}
	return res;
}

PyObject *get_callback(callback_type_t cb_type, wkhtmltopdf_converter *conv)
{
	PyObject *res = NULL;
	if (callbacks.count(conv))
	{
		switch(cb_type)
		{
			case PDF_CALLBACK_WARNING:
				res = callbacks[conv].warning_cb;
			break;
			case PDF_CALLBACK_ERROR:
				res = callbacks[conv].error_cb;
			break;
			case PDF_CALLBACK_FINISH:
				res = callbacks[conv].finish_cb;
			break;
			case PDF_CALLBACK_PROGRESS:
				res = callbacks[conv].progress_cb;
			break;
			case PDF_CALLBACK_PHASE:
				res = callbacks[conv].phase_cb;
			break;
		}
	}
	return res;
}

void del_callback(callback_type_t cb_type, wkhtmltopdf_converter *conv)
{
	switch(cb_type)
	{
		case PDF_CALLBACK_WARNING:
			callbacks[conv].warning_cb = NULL;
		break;
		case PDF_CALLBACK_ERROR:
			callbacks[conv].error_cb = NULL;
		break;
		case PDF_CALLBACK_FINISH:
			callbacks[conv].finish_cb = NULL;
		break;
		case PDF_CALLBACK_PROGRESS:
			callbacks[conv].progress_cb = NULL;
		break;
		case PDF_CALLBACK_PHASE:
			callbacks[conv].phase_cb = NULL;
		break;
	}
}

void clean_all(wkhtmltopdf_converter *conv)
{
	callbacks.erase(conv);
}

#ifdef __cplusplus
}
#endif
