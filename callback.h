#ifndef _CALLBACK_H_
#define _CALLBACK_H_

#if 0
struct callback_table_t;
typedef struct callback_table_t callback_table_t;
#endif

typedef enum {
	PDF_CALLBACK_WARNING = 0,
	PDF_CALLBACK_ERROR,
	PDF_CALLBACK_FINISH,
	PDF_CALLBACK_PROGRESS,
	PDF_CALLBACK_PHASE
#if 0
	,PDF_CALLBACK_INVALID
#endif
} callback_type_t;

typedef enum {
	LOOKUP_ERROR_SUCCESS = 0,
	LOOKUP_ERROR_FAIL
} lookup_error_t;

typedef struct {
	PyObject *warning_cb;
	PyObject *error_cb;
	PyObject *finish_cb;
	PyObject *progress_cb;
	PyObject *phase_cb;
} convertor_callbacks_t;

#ifdef __cplusplus
extern "C" {
#endif
lookup_error_t add_callback(callback_type_t cb_type, wkhtmltopdf_converter *conv, PyObject *callback);
PyObject *get_callback(callback_type_t cb_type, wkhtmltopdf_converter *conv);
void del_callback(callback_type_t cb_type, wkhtmltopdf_converter *conv);
void clean_all(wkhtmltopdf_converter *conv);
#ifdef __cplusplus
}
#endif

#endif /* _CALLBACK_H_ */
