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

#include <stdio.h>
#include <CUnit/CUnit.h>
#include "../src/MetaClass.h"
#include "test_macros.h"

static const char* SUITE_NAME = "SeeMetaClass suite";

typedef struct _SeeCustomRepr SeeCustomRepr;
typedef struct _SeeCustomReprClass SeeCustomReprClass;

struct _SeeCustomRepr {
    SeeObject   obj;
    const char* repr;
};

typedef void(*set_repr_func)(SeeCustomRepr* obj, const char* repr);

struct _SeeCustomReprClass {
    SeeObjectClass  parent;
    set_repr_func   set_repr;
};

SeeCustomReprClass* g_custom_class_instance = NULL;

int see_custom_repr_new(SeeCustomRepr** obj_out) {
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(g_custom_class_instance);
    if (!cls)
        return SEE_NOT_INITIALIZED;

    return cls->new_obj(cls, 0, SEE_OBJECT_REF(obj_out));
}

static int custom_repr(const SeeObject* obj, char** out)
{
    char *ret;
    const char* format = "Custom Wrapper message = %s";
    struct _SeeCustomRepr* custom = (struct _SeeCustomRepr*) obj;

    int size = snprintf(NULL, 0, format, custom->repr);
    ret = malloc(size + 1);

    sprintf(ret, format, custom->repr);
    *out = ret;
    return SEE_SUCCESS;
}

static void
set_repr(SeeCustomRepr* obj, const char* repr)
{
    obj->repr = repr;
}

static int post_init_class(SeeObjectClass* cls)
{
    // overwrite repr of SeeObjectClass
    cls->repr = custom_repr;
    cls->name = "TestCustomRepr";

    // Initialize our own derivable function.
    SeeCustomReprClass* custom_cls = (SeeCustomReprClass*) cls;
    custom_cls->set_repr = set_repr;
    return SEE_SUCCESS;
}

void set_custom_repr_set_repr(SeeCustomRepr* obj, const char* repr)
{
    const SeeCustomReprClass* cls =
        (const SeeCustomReprClass*)see_object_get_class((SeeObject*) obj);

    cls->set_repr(obj, repr);
}

static void meta_create(void)
{
    int ret = see_meta_class_new_class(
        see_meta_class_class(),
        (SeeObjectClass**) &g_custom_class_instance,
        sizeof(SeeCustomReprClass),
        sizeof(SeeCustomRepr),
        see_object_class(),
        sizeof(SeeObjectClass),
        post_init_class
        );
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    CU_ASSERT_NOT_EQUAL(g_custom_class_instance, NULL);

    SeeObjectClass* cls = (SeeObjectClass*) g_custom_class_instance;

    CU_ASSERT_EQUAL(g_custom_class_instance->set_repr, &set_repr);
    CU_ASSERT_EQUAL(cls->repr, &custom_repr);

    CU_ASSERT_EQUAL(cls->inst_size, sizeof(SeeCustomRepr));
}

static void meta_use(void)
{
    SeeCustomRepr* obj = NULL;
    char* ret_msg = NULL;
    int ret;
    const char* msg = "Hi from a custom class instance.";
    const char* expected_out =
        "Custom Wrapper message = Hi from a custom class instance.";
    ret = see_custom_repr_new(&obj);
    CU_ASSERT(ret == 0);
    if (ret)
        goto fail;

    // Can we successfully create a new instance.
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    // Is it an instance of the right class.
    CU_ASSERT_EQUAL((const SeeCustomReprClass*) obj->obj.cls, g_custom_class_instance);

    // Does the instance do what we think it does.
    g_custom_class_instance->set_repr(obj, msg);
    ret = g_custom_class_instance->parent.repr((SeeObject*) obj, &ret_msg);

    CU_ASSERT_STRING_EQUAL(expected_out, ret_msg);
    free(ret_msg);
fail:
    see_object_decref((SeeObject*) obj);
}

static void meta_destroy(void)
{
    // this doesn't test/assert anything but does free memory, hence
    // it is still important.
    see_object_decref((SeeObject*) g_custom_class_instance);
    g_custom_class_instance = NULL;
}

int add_meta_suite(void)
{
    CU_pSuite suite = CU_add_suite(SUITE_NAME, NULL, NULL);
    if (!suite) {
        fprintf(
            stderr, "Unable to create %s:%s\n",
            SUITE_NAME,
            CU_get_error_msg()
            );
        return CU_get_error();
    }

    CU_pTest test = CU_add_test(suite, "create", meta_create);
    if (!test) {
        fprintf(stderr, "Unable to create test %s:%s:%s",
            SUITE_NAME,
            "create",
            CU_get_error_msg()
            );
        return CU_get_error();
    }

    test = CU_add_test(suite, "use", meta_use);
    if (!test) {
        fprintf(stderr, "Unable to create test %s:%s:%s",
            SUITE_NAME,
            "use",
            CU_get_error_msg()
            );
        return CU_get_error();
    }

    test = CU_add_test(suite, "use", meta_destroy);
    if (!test) {
        fprintf(stderr, "Unable to create test %s:%s:%s",
            SUITE_NAME,
            "destroy",
            CU_get_error_msg()
            );
        return CU_get_error();
    }

    return 0;
}
