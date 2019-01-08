/*
 * This file is part of see-object.
 *
 * see-object is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * see-object is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with see-object.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>

#include "MetaClass.h"
#include "Error.h"

/* **** functions that implement SeeError or override SeeObject **** */

static void
error_init(SeeError* obj, const SeeErrorClass* cls, const char* msg)
{
    const SeeObjectClass* obj_cls = (const SeeObjectClass*) cls;

    obj_cls->object_init((SeeObject*)obj, obj_cls);

    if (msg == NULL)
        msg = "No error";

    // Do some extra initialization here (on demand).
    SeeError* error = (SeeError*)obj;

    cls->set_msg(error, msg);
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list list)
{
    const SeeErrorClass* error_cls = (const SeeErrorClass*) cls;
    const char* msg = va_arg(list, const char*);

    error_cls->error_init((SeeError*)obj, (const SeeErrorClass*) cls, msg);

    return SEE_SUCCESS;
}

static const char*
error_msg(const SeeError* error)
{
    assert(error);
    return error->msg;
}

static void
error_set_msg(SeeError* error, const char* msg)
{
    assert(error);
    assert(msg);

    error->msg = msg;
}

/* **** implementation of the public API **** */

int see_error_new(SeeError** out)
{
    int ret;
    if (!out || *out)
        return SEE_INVALID_ARGUMENT;

    const SeeErrorClass* cls = see_error_class();

    if (!cls)
        return SEE_NOT_INITIALIZED;

    const SeeObjectClass* obj_cls = (const SeeObjectClass*) cls;
    ret = obj_cls->new(obj_cls, 0, (SeeObject**) out, NULL);

    return ret;
}

int see_error_new_msg(SeeError** out, const char* msg)
{
    int ret;
    if (!out || *out)
        return SEE_INVALID_ARGUMENT;
    if (!msg)
        return SEE_INVALID_ARGUMENT;

    const SeeErrorClass* cls = see_error_class();

    if (!cls)
        return SEE_NOT_INITIALIZED;

    const SeeObjectClass* obj_cls = (const SeeObjectClass*) cls;
    ret = obj_cls->new(obj_cls, 0, (SeeObject**) out, msg);

    return ret;
}

const char*
see_error_msg(const SeeError* error)
{
    if (!error)
        return NULL;

    return see_error_class()->msg(error);
}

void
see_error_set_msg(SeeError* error, const char* msg)
{
    if (!error)
        return;

    see_error_class()->set_msg(error, msg);
}

/* **** initialization of the class **** */

SeeErrorClass* g_SeeErrorClass = NULL;

static int
see_error_class_init(SeeObjectClass* new_cls) {
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;
    
    /* Set the function pointers of the own class here */
    SeeErrorClass* cls = (SeeErrorClass*) new_cls;
    cls->error_init     = error_init;
    cls->msg            = error_msg;
    cls->set_msg        = error_set_msg;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeError(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_error_init() {
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeErrorClass,
        sizeof(SeeErrorClass),
        sizeof(SeeError),
        see_object_class(),
        sizeof(SeeObjectClass),
        see_error_class_init
        );

    return ret;
}

void
see_error_deinit()
{
    if(!g_SeeErrorClass)
        return;

    see_object_decref((SeeObject*) g_SeeErrorClass);
    g_SeeErrorClass = NULL;
}

const SeeErrorClass*
see_error_class()
{
    return g_SeeErrorClass;
}
