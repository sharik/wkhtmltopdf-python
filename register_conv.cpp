#ifdef __cplusplus
extern "C" {
#endif
#include <Python.h>
#ifdef __cplusplus
}
#endif
#include <map>
#include "register_conv.h"
#include "utils.h"
typedef std::map<void*, PyObject *> reg_map_t;
static reg_map_t reg_map;

#ifdef __cplusplus
extern "C" {
#endif
	
register_error_t register_obj(void *conv, PyObject *obj)
{
	if (reg_map.count(conv))
	{
		PyErr_SetString(WkhtmltoxError, "Convertor has already registered");
		return REGISTER_ERROR_FAIL;
	}
	reg_map[conv] = obj;
	
	return REGISTER_ERROR_SUCCESS;
}

PyObject *get_registered(void *conv)
{
	PyObject *res = NULL;
	if (reg_map.count(conv))
	{
		res = reg_map[conv];
	}
	
	return res;
}

void unregister_obj(void *conv)
{
	reg_map.erase(conv);
}

void unregister_all()
{
//	reg_map.clean();
}


#ifdef __cplusplus
}
#endif
