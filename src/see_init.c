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

#include "MetaClass.h"
#include "see_init.h"
#include "Error.h"
#include "DynamicArray.h"

int see_init()
{
    // The meta class must be initialized first, otherwise the rest
    // will fail.
    int ret = see_meta_class_init();
    if (ret)
        return ret;

    // Initialize the other objects.

    ret = see_dynamic_array_init();
    if (ret)
        return ret;

	ret = see_error_init();
    if (ret)
        return ret;

    return ret;
}

void see_deinit()
{
    see_dynamic_array_deinit();
    see_error_deinit();

    see_meta_class_deinit();
}
