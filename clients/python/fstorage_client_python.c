/*-
 * Copyright (c) 2009-2010 by Fstorage Team.
 *
 * Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
 */

#include <Python.h>
#include <fstorage.h>

#define ERR_OK  0

typedef struct {
	PyObject_HEAD
	fs_ctx_t *ctx;
} pyfs_obj_t;

static PyObject *
pyfstorage_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
	pyfs_obj_t *self = (pyfs_obj_t *)type->tp_alloc(type, 0);
	self->ctx = NULL;
	
	return ((PyObject *)self);
}

static void
pyfstorage_dealloc(pyfs_obj_t *self)
{
	
	fstorage_fini(self->ctx);
	self->ob_type->tp_free(self);
}

static int
pyfstorage_init(pyfs_obj_t *self, PyObject *args, PyObject *kwargs)
{
	int err, debug = 0;
	char *fsuri, *logfile = NULL;
	
	if (!PyArg_ParseTuple(args, "s|is", &fsuri, &debug, &logfile))
		return (-1);
	
	if (self->ctx != NULL) {
		PyErr_SetString(PyExc_Exception, "__init__ was called before");
		return (-1);
	}
	
    debug = debug < 0 ? 0 : (debug > 5 ? 5 : debug);
    
	if ((err = fstorage_init(fsuri, debug, logfile, &self->ctx)) != ERR_OK) {
		PyErr_SetString(PyExc_Exception, fstorage_strerror(err));
		return (-1);
	}
	
	return (0);
}

static PyObject *
pyfstorage_file_send(pyfs_obj_t *self, PyObject *args, PyObject *kwargs)
{
	int err;
	char *fkey, *filepath;
	
	if (!self) {
		PyErr_SetString(PyExc_Exception, "file_send is not static method");
		return (NULL);
	}
	
	if (!PyArg_ParseTuple(args, "ss", &fkey, &filepath))
		return (NULL);
	
	if ((err = fstorage_file_send(self->ctx, fkey, filepath)) != ERR_OK) {
		PyErr_SetString(PyExc_Exception, fstorage_strerror(err));
		return (NULL);
	}
	
	return (Py_BuildValue("i", err));
}

static PyObject *
pyfstorage_file_recv(pyfs_obj_t *self, PyObject *args, PyObject *kwargs)
{
	int err;
	size_t size;
	char *fkey , *filepath;
	
	if (!self) {
		PyErr_SetString(PyExc_Exception, "file_recv is not static method");
		return (NULL);
	}
	
	if (!PyArg_ParseTuple(args, "ss", &fkey, &filepath))
		return (NULL);
	
	if ((err = fstorage_file_recv(self->ctx, fkey, filepath, &size)) != ERR_OK) {
		PyErr_SetString(PyExc_Exception, fstorage_strerror(err));
		return (NULL);
	}
	
	return (Py_BuildValue("i", size));
}

static PyObject *
pyfstorage_file_replace(pyfs_obj_t *self, PyObject *args, PyObject *kwargs)
{
    int err;
    char *fkey, *filepath;
    
    if (!self) {
        PyErr_SetString(PyExc_Exception, "file_replace is not static method");
        return (NULL);
    }
    
    if (!PyArg_ParseTuple(args, "ss", &fkey, &filepath))
        return (NULL);
    
    if ((err = fstorage_file_replace(self->ctx, fkey, filepath)) != ERR_OK) {
        PyErr_SetString(PyExc_Exception, fstorage_strerror(err));
        return (NULL);
    }
    
    return (Py_BuildValue("i", err));
}

static PyObject *
pyfstorage_file_remove(pyfs_obj_t *self, PyObject *args, PyObject *kwargs)
{
    int err;
    char *fkey;
    
    if (!self) {
        PyErr_SetString(PyExc_Exception, "file_replace is not static method");
        return (NULL);
    }
    
    if (!PyArg_ParseTuple(args, "s", &fkey))
        return (NULL);
    
    if ((err = fstorage_file_remove(self->ctx, fkey)) != ERR_OK) {
        PyErr_SetString(PyExc_Exception, fstorage_strerror(err));
        return (NULL);
    }
    
    return (Py_BuildValue("i", err));
}

static PyObject *
pyfstorage_ns_encode(PyObject *self, PyObject *args)
{
	int err = 0;
	uint64_t num = 0;
	int base = 0;
	char ciphertext[256];
	
	memset(ciphertext, 0, 256);
	
	if (!PyArg_ParseTuple(args, "li", &num, &base))
		return (NULL);
	
	if ((err = fstorage_ns_encode(num, base, (char *)&ciphertext)) != ERR_OK) {
		PyErr_SetString(PyExc_Exception, fstorage_strerror(err));
		return (NULL);
	}
	
	return (Py_BuildValue("s", (const char *)ciphertext));
}

static PyObject *
pyfstorage_ns_decode(PyObject *self, PyObject *args)
{
	int err = 0;
	uint64_t num = 0;
	int base = 0;
	char *ciphertext = NULL;
	
	if (!PyArg_ParseTuple(args, "si", &ciphertext, &base))
		return (NULL);
	
	if ((err = fstorage_ns_decode(ciphertext, base, &num)) != ERR_OK) {
		PyErr_SetString(PyExc_Exception, fstorage_strerror(err));
		return (NULL);
	}
	
	return (Py_BuildValue("l", num));
}

static PyObject *
pyfstorage_akey_create(PyObject *self, PyObject *args)
{
	int err = 0;
	uint32_t expire_time = 0;
	uint64_t salt = 0;
	int base = 0;
	char *fkey = NULL, akey[256];
	
	memset(akey, 0, 256);
	
	if (!PyArg_ParseTuple(args, "isli", &expire_time, &fkey, &salt, &base))
		return (NULL);
	
	if ((err = fstorage_akey_create(expire_time, fkey, salt, base, (char *)&akey)) != ERR_OK) {
		PyErr_SetString(PyExc_Exception, fstorage_strerror(err));
		return (NULL);
	}
	
	return (Py_BuildValue("s", akey));
}

static PyObject *
pyfstorage_akey_check(PyObject *self, PyObject *args)
{
	uint64_t salt = 0;
	int base = 0;
	char *fkey = NULL, *akey = NULL;
	
	if (!PyArg_ParseTuple(args, "ssli", &akey, &fkey, &salt, &base))
		return (NULL);
	
	return (Py_BuildValue("b", fstorage_akey_check(akey, fkey, salt, base)));
}

static PyObject *
pyfstorage_uri(PyObject *self, PyObject *args)
{
	int err;
	char *filename, *fkey = NULL, *akey = NULL, uri[1024];
	
	memset(uri, 0, 1024);
	
	if (!PyArg_ParseTuple(args, "ss|s", &filename, &fkey, &akey))
		return (NULL);
	
	if ((err = fstorage_uri(filename, fkey, akey, (char *)&uri)) != ERR_OK) {
		PyErr_SetString(PyExc_Exception, fstorage_strerror(err));
		return (NULL);
	}
	
	return (Py_BuildValue("s", uri));
}

static PyObject *
pyfstorage_http_url(pyfs_obj_t *self, PyObject *args)
{
	int err;
	char *filename, *fkey = NULL, *akey = NULL, url[1024];
	
	if (!self) {
		PyErr_SetString(PyExc_Exception, "http_url is not static method");
		return (NULL);
	}
	
	memset(url, 0, 1024);
	
	if (!PyArg_ParseTuple(args, "ss|s", &filename, &fkey, &akey))
		return (NULL);
	
	if ((err = fstorage_http_url(self->ctx, filename, fkey, akey, (char *)&url)) != ERR_OK) {
		PyErr_SetString(PyExc_Exception, fstorage_strerror(err));
		return (NULL);
	}
	
	return (Py_BuildValue("s", url));
}

static PyObject *
pyfstorage_https_url(pyfs_obj_t *self, PyObject *args)
{
	int err;
	char *filename, *fkey = NULL, *akey = NULL, url[1024];
	
	if (!self) {
		PyErr_SetString(PyExc_Exception, "https_url is not static method");
		return (NULL);
	}
	
	memset(url, 0, 1024);
	
	if (!PyArg_ParseTuple(args, "ss|s", &filename, &fkey, &akey))
		return (NULL);
	
	if ((err = fstorage_https_url(self->ctx, filename, fkey, akey, (char *)&url)) != ERR_OK) {
		PyErr_SetString(PyExc_Exception, fstorage_strerror(err));
		return (NULL);
	}
	
	return (Py_BuildValue("s", url));
}

static PyMethodDef pyfstorage_methods[] = {
	{"file_send", (PyCFunction)pyfstorage_file_send, METH_VARARGS, "Sending file"},
	{"file_recv", (PyCFunction)pyfstorage_file_recv, METH_VARARGS, "Receiving file"},
    {"file_replace", (PyCFunction)pyfstorage_file_replace, METH_VARARGS, "Replacing file"},
    {"file_remove", (PyCFunction)pyfstorage_file_remove, METH_VARARGS, "Replacing file"},
	{"http_url", (PyCFunction)pyfstorage_http_url, METH_VARARGS, "Create HTTP URL"},
	{"https_url", (PyCFunction)pyfstorage_https_url, METH_VARARGS, "Create HTTPS URL"},
	{NULL, NULL, 0, NULL}
};

static PyMethodDef pyfstorage_functions[] = {
	{"ns_encode", pyfstorage_ns_encode, METH_VARARGS, "Encode numeral system"},
	{"ns_decode", pyfstorage_ns_decode, METH_VARARGS, "Decode numeral system"},
	{"akey_create", pyfstorage_akey_create, METH_VARARGS, "Create access key"},
	{"akey_check", pyfstorage_akey_check, METH_VARARGS, "Check access key"},
	{"uri", pyfstorage_uri, METH_VARARGS, "URI"},
	{NULL, NULL, 0, NULL}
};

static PyTypeObject pyfstorageType = {
	PyObject_HEAD_INIT(NULL)
	0,				/* ob_size */
	"pyfstorage", 			/* tp_name */
	sizeof(pyfs_obj_t),		/* tp_basicsize */
	0,				/* tp_itemsize */
	(destructor)pyfstorage_dealloc, /* tp_dealloc */
	0, 				/* tp_ print */
	0, 				/* tp_getattr */
	0, 				/* tp_setattr */
	0, 				/* tp_compare */
	0, 				/* tp_repr */
	0, 				/* tp_as_number */
	0, 				/* tp_as_sequence */
	0, 				/* tp_as_mapping */
	0, 				/* tp_hash */
	0, 				/* tp_call */
	0, 				/* tp_str */
	0, 				/* tp_getattro */
	0, 				/* tp_setattro */
	0, 				/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT, 		/* tp_flags */
	"Fstorage Object", 		/* tp_doc */
	0, 				/* tp_traverse */
	0, 				/* tp_clear */
	0, 				/* tp_richcompare */
	0, 				/* tp_weaklistoffset */
	0, 				/* tp_iter */
	0, 				/* tp_iternext */
	pyfstorage_methods, 		/* tp_methods */
	0, 				/* tp_members */
	0, 				/* tp_getset */
	0, 				/* tp_base */
	0, 				/* tp_dict */
	0, 				/* tp_descr_get */
	0, 				/* tp_descr_set */
	0, 				/* tp_dictoffset */
	(initproc)pyfstorage_init, 	/* tp_init */
	0, 				/* tp_alloc */
	pyfstorage_new, 		/* tp_new */
	0, 				/* tp_free */
};

PyMODINIT_FUNC
initpyfstorage(void)
{
	PyObject *m, *s;
	
	if (PyType_Ready(&pyfstorageType) < 0)
		return;
	
	m = Py_InitModule3("pyfstorage", pyfstorage_methods, "<FstorageClient>");
	s = Py_InitModule3("pyfstorage", pyfstorage_functions, "<static method>");
	
	Py_INCREF(&pyfstorageType);
	PyModule_AddObject(m, "Fstorage", (PyObject *)&pyfstorageType);
}
