#ifndef __REGISTER_CONV_H__
#define __REGISTER_CONV_H__

#include <Python.h>

#define DEFAULT_BUF_LEN	255

typedef enum {
	REGISTER_ERROR_SUCCESS = 0,
	REGISTER_ERROR_FAIL
} register_error_t;

#ifdef __cplusplus
extern "C" {
#endif
void unregister_obj(void *conv);
PyObject *get_registered(void *conv);
register_error_t register_obj(void *conv, PyObject *obj);
void unregister_all();
#ifdef __cplusplus
}
#endif

#endif /* __REGISTER_CONV_H__ */
