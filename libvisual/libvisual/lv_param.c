/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_param.c,v 1.50 2006/01/22 13:23:37 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "config.h"
#include "lv_param.h"
#include "lv_common.h"
#include "lv_util.h"
#include "gettext.h"
#include <string.h>

static int param_container_dtor (VisObject *object);
static int param_entry_dtor (VisObject *object);

static int get_next_pcall_id (VisList *callbacks);

static int param_container_dtor (VisObject *object)
{
    VisParamContainer *paramcontainer = VISUAL_PARAMCONTAINER (object);

    visual_collection_destroy (VISUAL_COLLECTION (&paramcontainer->entries));

    return VISUAL_OK;
}

static int param_entry_dtor (VisObject *object)
{
    VisParamEntry *param = VISUAL_PARAMENTRY (object);

    if (param->string != NULL)
        visual_mem_free (param->string);

    if (param->name != NULL)
        visual_mem_free (param->name);

    if (param->objdata != NULL)
        visual_object_unref (param->objdata);

    if (param->annotation != NULL)
        visual_mem_free (param->annotation);

    visual_palette_free_colors (&param->pal);

    visual_collection_destroy (VISUAL_COLLECTION (&param->callbacks));

    param->string = NULL;
    param->name = NULL;
    param->objdata = NULL;
    param->annotation = NULL;

    return VISUAL_OK;
}

static int get_next_pcall_id (VisList *callbacks)
{
    VisListEntry *le = NULL;
    VisParamEntryCallback *pcall;
    int found = FALSE;
    int i;

    /* Walk through all possible ids */
    for (i = 0; i < VISUAL_PARAM_CALLBACK_ID_MAX; i++) {

        found = FALSE;
        /* Check all the callbacks if the id is used */
        while ((pcall = visual_list_next (callbacks, &le)) != NULL) {

            /* Found the ID, break and get ready for the next iterate */
            if (pcall->id == i) {
                found = TRUE;

                break;
            }
        }

        /* The id has NOT been found, thus is an original, and we return this as the next id */
        if (found == FALSE)
            return i;
    }

    /* This is virtually impossible, or something very wrong is going ok, but no id seems to be left */
    return -1;
}

VisParamContainer *visual_param_container_new ()
{
    VisParamContainer *paramcontainer;

    paramcontainer = visual_mem_new0 (VisParamContainer, 1);

    visual_list_init(&paramcontainer->entries, NULL);

    /* Do the VisObject initialization */
    visual_object_initialize (VISUAL_OBJECT (paramcontainer), TRUE, param_container_dtor);

    visual_collection_set_destroyer (VISUAL_COLLECTION (&paramcontainer->entries), visual_object_collection_destroyer);

    return paramcontainer;
}

int visual_param_container_set_eventqueue (VisParamContainer *paramcontainer, VisEventQueue *eventqueue)
{
    visual_return_val_if_fail (paramcontainer != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);

    paramcontainer->eventqueue = eventqueue;

    return VISUAL_OK;
}

VisEventQueue *visual_param_container_get_eventqueue (VisParamContainer *paramcontainer)
{
    visual_return_val_if_fail (paramcontainer != NULL, NULL);

    return paramcontainer->eventqueue;
}

int visual_param_container_add (VisParamContainer *paramcontainer, VisParamEntry *param)
{
    visual_return_val_if_fail (paramcontainer != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);
    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->parent = paramcontainer;

    /* On container add, we always set changed once, so vars can be synchronised in the plugin
     * it's event loop */
    visual_param_entry_changed (param);

    return visual_list_add (&paramcontainer->entries, param);
}

int visual_param_container_add_with_defaults (VisParamContainer *paramcontainer, VisParamEntry *param)
{
    visual_return_val_if_fail (paramcontainer != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);
    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->parent = paramcontainer;

    visual_mem_copy(&param->defaultnum, &param->numeric, sizeof(param->defaultnum));

    if(param->type == VISUAL_PARAM_ENTRY_TYPE_STRING)
        param->defaultstring = visual_strdup(param->string);

/*
    if(param->type == VISUAL_PARAM_ENTRY_TYPE_COLOR)
        visual_color_copy(&param->defaultcolor, &param->color);
*/

    /* On container add, we always set changed once, so vars can be synchronised in the plugin
     * it's event loop */
    visual_param_entry_changed (param);

    return visual_list_add (&paramcontainer->entries, param);
}

int visual_param_container_add_many (VisParamContainer *paramcontainer, VisParamEntry *params)
{
    VisParamEntry *pnew;
    int i = 0;

    visual_return_val_if_fail (paramcontainer != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);
    visual_return_val_if_fail (params != NULL, -VISUAL_ERROR_PARAM_NULL);

    while (params[i].type != VISUAL_PARAM_ENTRY_TYPE_END) {
        pnew = visual_param_entry_new (visual_param_entry_get_name (&params[i]));
        visual_param_entry_set_from_param (pnew, &params[i]);

        visual_param_container_add_with_defaults (paramcontainer, pnew);

        i++;
    }

    return VISUAL_OK;
}

int visual_param_container_remove (VisParamContainer *paramcontainer, const char *name)
{
    VisListEntry *le = NULL;
    VisParamEntry *param;

    visual_return_val_if_fail (paramcontainer != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);
    visual_return_val_if_fail (name != NULL, -VISUAL_ERROR_NULL);

    while ((param = visual_list_next (&paramcontainer->entries, &le)) != NULL) {

        if (strcmp (param->name, name) == 0) {
            visual_list_delete (&paramcontainer->entries, &le);

            return VISUAL_OK;
        }
    }

    return -VISUAL_ERROR_PARAM_NOT_FOUND;
}

int visual_param_container_copy (VisParamContainer *destcont, VisParamContainer *srccont)
{
    VisListEntry *le = NULL;
    VisParamEntry *destparam;
    VisParamEntry *srcparam;
    VisParamEntry *tempparam;

    visual_return_val_if_fail (destcont != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);
    visual_return_val_if_fail (srccont != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);

    while ((srcparam = visual_list_next (&srccont->entries, &le)) != NULL) {
        tempparam = visual_param_container_get (destcont, visual_param_entry_get_name (srcparam));

        /* Already exists, overwrite */
        if (tempparam != NULL) {
            visual_param_entry_set_from_param (tempparam, srcparam);

            continue;
        }

        /* Does not yet exist, create a new entry */
        destparam = visual_param_entry_new (visual_param_entry_get_name (srcparam));
        visual_param_entry_set_from_param (destparam, srcparam);

        visual_param_container_add (destcont, destparam);
    }

    return VISUAL_OK;
}

int visual_param_container_copy_match (VisParamContainer *destcont, VisParamContainer *srccont)
{
    VisListEntry *le = NULL;
    VisParamEntry *destparam;
    VisParamEntry *srcparam;

    visual_return_val_if_fail (destcont != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);
    visual_return_val_if_fail (srccont != NULL, -VISUAL_ERROR_PARAM_CONTAINER_NULL);

    while ((destparam = visual_list_next (&destcont->entries, &le)) != NULL) {
        srcparam = visual_param_container_get (srccont, visual_param_entry_get_name (destparam));

        if (srcparam != NULL)
            visual_param_entry_set_from_param (destparam, srcparam);
    }

    return VISUAL_OK;
}

VisParamEntry *visual_param_container_get (VisParamContainer *paramcontainer, const char *name)
{
    VisListEntry *le = NULL;
    VisParamEntry *param;

    visual_return_val_if_fail (paramcontainer != NULL, NULL);
    visual_return_val_if_fail (name != NULL, NULL);

    while ((param = visual_list_next (&paramcontainer->entries, &le)) != NULL) {
        param = le->data;

        if (strcmp (param->name, name) == 0)
            return param;
    }

    return NULL;
}

VisParamEntry *visual_param_entry_new (char *name)
{
    VisParamEntry *param;

    param = visual_mem_new0 (VisParamEntry, 1);

    /* Do the VisObject initialization */
    visual_object_initialize (VISUAL_OBJECT (param), TRUE, param_entry_dtor);

    visual_param_entry_set_name (param, name);

    visual_collection_set_destroyer (VISUAL_COLLECTION (&param->callbacks), visual_object_collection_destroyer);

    return param;
}

int visual_param_entry_add_callback (VisParamEntry *param, VisParamChangedCallbackFunc callback, void *priv)
{
    VisParamEntryCallback *pcall;
    int id;

    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);
    visual_return_val_if_fail (callback != NULL, -VISUAL_ERROR_PARAM_CALLBACK_NULL);

    id = get_next_pcall_id (&param->callbacks);

    visual_return_val_if_fail (id >= 0, -VISUAL_ERROR_PARAM_CALLBACK_TOO_MANY);

    pcall = visual_mem_new0 (VisParamEntryCallback, 1);

    /* Do the VisObject initialization for the VisParamEntryCallback */
    visual_object_initialize (VISUAL_OBJECT (pcall), TRUE, NULL);

    pcall->id = id;
    pcall->callback = callback;
    visual_object_set_private (VISUAL_OBJECT (pcall), priv);

    visual_list_add (&param->callbacks, pcall);

    return id;
}


int visual_param_entry_remove_callback (VisParamEntry *param, int id)
{
    VisListEntry *le = NULL;
    VisParamEntryCallback *pcall;

    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    while ((pcall = visual_list_next (&param->callbacks, &le)) != NULL) {

        if (id == pcall->id) {
            visual_list_delete (&param->callbacks, &le);

            visual_object_unref (VISUAL_OBJECT (pcall));

            return VISUAL_OK;
        }
    }

    return VISUAL_OK;
}

int visual_param_entry_notify_callbacks (VisParamEntry *param)
{
    VisListEntry *le = NULL;
    VisParamEntryCallback *pcall;

    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    while ((pcall = visual_list_next (&param->callbacks, &le)) != NULL)
        pcall->callback (param, visual_object_get_private (VISUAL_OBJECT (pcall)));

    return VISUAL_OK;
}

int visual_param_entry_is (VisParamEntry *param, const char *name)
{
    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    if (strcmp (param->name, name) == 0)
        return TRUE;

    return FALSE;
}

int visual_param_entry_changed (VisParamEntry *param)
{
    VisEventQueue *eventqueue;

    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    if (param->parent == NULL)
        return VISUAL_OK;

    eventqueue = param->parent->eventqueue;

    if (eventqueue != NULL)
        visual_event_queue_add_param (eventqueue, param);

    visual_param_entry_notify_callbacks (param);

    return VISUAL_OK;
}

VisParamEntryType visual_param_entry_get_type (VisParamEntry *param)
{
    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    return param->type;
}

int visual_param_entry_compare (VisParamEntry *src1, VisParamEntry *src2)
{
    visual_return_val_if_fail (src1 != NULL, -VISUAL_ERROR_PARAM_NULL);
    visual_return_val_if_fail (src2 != NULL, -VISUAL_ERROR_PARAM_NULL);

    if (src1->type != src2->type)
        return FALSE;

    switch (src1->type) {
        case VISUAL_PARAM_ENTRY_TYPE_NULL:
            return TRUE;

            break;

        case VISUAL_PARAM_ENTRY_TYPE_STRING:
            if (!strcmp (src1->string, src2->string))
                return TRUE;

            break;

        case VISUAL_PARAM_ENTRY_TYPE_INTEGER:
            if (src1->numeric.integer == src2->numeric.integer)
                return TRUE;

            break;

        case VISUAL_PARAM_ENTRY_TYPE_FLOAT:
            if (src1->numeric.floating == src2->numeric.floating)
                return TRUE;

            break;

        case VISUAL_PARAM_ENTRY_TYPE_DOUBLE:
            if (src1->numeric.doubleflt == src2->numeric.doubleflt)
                return TRUE;

            break;

        case VISUAL_PARAM_ENTRY_TYPE_COLOR:
            return visual_color_compare (&src1->color, &src2->color);

            break;

        case VISUAL_PARAM_ENTRY_TYPE_PALETTE:
            return FALSE;

            break;

        case VISUAL_PARAM_ENTRY_TYPE_OBJECT:
            return FALSE;

            break;

        default:
            visual_log (VISUAL_LOG_ERROR, _("param type is not valid"));

            return -VISUAL_ERROR_PARAM_INVALID_TYPE;

            break;
    }

    return -VISUAL_ERROR_IMPOSSIBLE;
}

int visual_param_entry_set_from_param (VisParamEntry *param, VisParamEntry *src)
{
    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);
    visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_PARAM_NULL);

    switch (src->type) {
        case VISUAL_PARAM_ENTRY_TYPE_NULL:

            break;

        case VISUAL_PARAM_ENTRY_TYPE_STRING:
            visual_param_entry_set_string (param, visual_param_entry_get_string (src));
            break;

        case VISUAL_PARAM_ENTRY_TYPE_INTEGER:
            visual_param_entry_set_integer (param, visual_param_entry_get_integer (src));

            break;

        case VISUAL_PARAM_ENTRY_TYPE_FLOAT:
            visual_param_entry_set_float (param, visual_param_entry_get_float (src));

            break;

        case VISUAL_PARAM_ENTRY_TYPE_DOUBLE:
            visual_param_entry_set_double (param, visual_param_entry_get_double (src));

            break;

        case VISUAL_PARAM_ENTRY_TYPE_COLOR:
            visual_param_entry_set_color_by_color (param, visual_param_entry_get_color (src));

            break;

        case VISUAL_PARAM_ENTRY_TYPE_PALETTE:
            visual_param_entry_set_palette (param, visual_param_entry_get_palette (src));

            break;

        case VISUAL_PARAM_ENTRY_TYPE_OBJECT:
            visual_param_entry_set_object (param, visual_param_entry_get_object (src));

            break;

        default:
            visual_log (VISUAL_LOG_ERROR, _("param type is not valid"));

            return -VISUAL_ERROR_PARAM_INVALID_TYPE;

            break;
    }

    return VISUAL_OK;
}

int visual_param_entry_set_name (VisParamEntry *param, char *name)
{
    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    if (param->name != NULL)
        visual_mem_free (param->name);

    param->name = NULL;

    if (name != NULL)
        param->name = visual_strdup (name);

    return VISUAL_OK;
}

int visual_param_entry_set_string (VisParamEntry *param, char *string)
{
    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->type = VISUAL_PARAM_ENTRY_TYPE_STRING;

    if (string == NULL && param->string == NULL)
        return VISUAL_OK;

    if (string == NULL && param->string != NULL) {
        visual_mem_free (param->string);
        param->string = NULL;

        visual_param_entry_changed (param);

    } else if (param->string == NULL && string != NULL) {
        param->string = visual_strdup (string);

        visual_param_entry_changed (param);

    } else if (strcmp (string, param->string) != 0) {
        visual_mem_free (param->string);

        param->string = visual_strdup (string);

        visual_param_entry_changed (param);
    }

    return VISUAL_OK;
}

int visual_param_entry_set_integer (VisParamEntry *param, int integer)
{
    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->type = VISUAL_PARAM_ENTRY_TYPE_INTEGER;

    if (param->numeric.integer != integer) {
        param->numeric.integer = integer;

        visual_param_entry_changed (param);
    }

    return VISUAL_OK;
}

int visual_param_entry_set_float (VisParamEntry *param, float floating)
{
    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->type = VISUAL_PARAM_ENTRY_TYPE_FLOAT;

    if (param->numeric.floating != floating) {
        param->numeric.floating = floating;

        visual_param_entry_changed (param);
    }

    return VISUAL_OK;
}

int visual_param_entry_set_double (VisParamEntry *param, double doubleflt)
{
    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->type = VISUAL_PARAM_ENTRY_TYPE_DOUBLE;

    if (param->numeric.doubleflt != doubleflt) {
        param->numeric.doubleflt = doubleflt;

        visual_param_entry_changed (param);
    }

    return VISUAL_OK;
}

int visual_param_entry_set_color (VisParamEntry *param, uint8_t r, uint8_t g, uint8_t b)
{
    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->type = VISUAL_PARAM_ENTRY_TYPE_COLOR;

    if (param->color.r != r || param->color.g != g || param->color.b != b) {
        visual_color_set (&param->color, r, g, b);

        visual_param_entry_changed (param);
    }

    return VISUAL_OK;
}

int visual_param_entry_set_color_by_color (VisParamEntry *param, VisColor *color)
{
    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->type = VISUAL_PARAM_ENTRY_TYPE_COLOR;

    if (visual_color_compare (&param->color, color) == FALSE) {
        visual_color_copy (&param->color, color);

        visual_param_entry_changed (param);
    }

    return VISUAL_OK;
}

int visual_param_entry_set_palette (VisParamEntry *param, VisPalette *pal)
{
    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->type = VISUAL_PARAM_ENTRY_TYPE_PALETTE;

    visual_palette_free_colors (&param->pal);

    if (pal != NULL) {
        visual_palette_allocate_colors (&param->pal, pal->ncolors);

        visual_palette_copy (&param->pal, pal);
    }

    visual_param_entry_changed (param);

    return VISUAL_OK;
}

int visual_param_entry_set_object (VisParamEntry *param, VisObject *object)
{
    visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->type = VISUAL_PARAM_ENTRY_TYPE_OBJECT;

    if (param->objdata != NULL)
        visual_object_unref (param->objdata);

    param->objdata = object;

    if (param->objdata != NULL)
        visual_object_ref (param->objdata);

    visual_param_entry_changed (param);

    return VISUAL_OK;
}

int visual_param_entry_set_collection (VisParamEntry *param, VisCollection *collection)
{
    visual_return_val_if_fail(param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->type = VISUAL_PARAM_ENTRY_TYPE_COLLECTION;

    if (param->collection != NULL)
        visual_object_unref (VISUAL_OBJECT(param->collection));

    param->collection = collection;

    if (param->collection != NULL)
        visual_object_ref(VISUAL_OBJECT(param->collection));

    return VISUAL_OK;
}

int visual_param_entry_set_annotation (VisParamEntry *param, char *anno)
{
    visual_return_val_if_fail(param != NULL, -VISUAL_ERROR_PARAM_NULL);
;
    visual_return_val_if_fail(anno != NULL, -VISUAL_ERROR_PARAM_ANNO_NULL);

    if (param->annotation != NULL)
        visual_mem_free(param->annotation);

    param->annotation = visual_strdup(anno);

    return VISUAL_OK;
}

char *visual_param_entry_get_name (VisParamEntry *param)
{
    visual_return_val_if_fail (param != NULL, NULL);

    return param->name;
}

char *visual_param_entry_get_string (VisParamEntry *param)
{
    visual_return_val_if_fail (param != NULL, NULL);

    if (param->type != VISUAL_PARAM_ENTRY_TYPE_STRING) {
        visual_log (VISUAL_LOG_WARNING, _("Requesting string from a non string param"));

        return NULL;
    }

    return param->string;
}

int visual_param_entry_get_integer (VisParamEntry *param)
{
    visual_return_val_if_fail (param != NULL, 0);

    if (param->type != VISUAL_PARAM_ENTRY_TYPE_INTEGER)
        visual_log (VISUAL_LOG_WARNING, _("Requesting integer from a non integer param"));

    return param->numeric.integer;
}

float visual_param_entry_get_float (VisParamEntry *param)
{
    visual_return_val_if_fail (param != NULL, 0);

    if (param->type != VISUAL_PARAM_ENTRY_TYPE_FLOAT)
        visual_log (VISUAL_LOG_WARNING, _("Requesting float from a non float param"));

    return param->numeric.floating;
}

double visual_param_entry_get_double (VisParamEntry *param)
{
    visual_return_val_if_fail (param != NULL, 0);

    if (param->type != VISUAL_PARAM_ENTRY_TYPE_DOUBLE)
        visual_log (VISUAL_LOG_WARNING, _("Requesting double from a non double param"));

    return param->numeric.doubleflt;
}

VisColor *visual_param_entry_get_color (VisParamEntry *param)
{
    visual_return_val_if_fail (param != NULL, NULL);

    if (param->type != VISUAL_PARAM_ENTRY_TYPE_COLOR) {
        visual_log (VISUAL_LOG_WARNING, _("Requesting color from a non color param"));

        return NULL;
    }

    return &param->color;
}

VisPalette *visual_param_entry_get_palette (VisParamEntry *param)
{
    visual_return_val_if_fail (param != NULL, NULL);

    if (param->type != VISUAL_PARAM_ENTRY_TYPE_PALETTE) {
        visual_log (VISUAL_LOG_WARNING, _("Requested palette from a non palette param"));

        return NULL;
    }

    return &param->pal;
}

VisObject *visual_param_entry_get_object (VisParamEntry *param)
{
    visual_return_val_if_fail (param != NULL, NULL);

    if (param->type != VISUAL_PARAM_ENTRY_TYPE_OBJECT) {
        visual_log (VISUAL_LOG_WARNING, _("Requested object from a non object param"));

        return NULL;
    }

    return param->objdata;
}

VisCollection *visual_param_entry_get_collection (VisParamEntry *param)
{
    visual_return_val_if_fail(param != NULL, NULL)

    if (param->type != VISUAL_PARAM_ENTRY_TYPE_COLLECTION) {
        visual_log (VISUAL_LOG_WARNING, _("Requested collection from non collection param"));
        return NULL;
    }

    return param->collection;
}

char *visual_param_entry_get_annotation (VisParamEntry *param)
{
    visual_return_val_if_fail(param != NULL, NULL)

    return param->annotation;
}

int visual_param_entry_min_set_integer (VisParamEntry *param, int integer)
{
    visual_return_val_if_fail(param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->min.integer = integer;

    return VISUAL_OK;
}

int visual_param_entry_min_set_float (VisParamEntry *param, float floating)
{
    visual_return_val_if_fail(param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->min.floating= floating;

    return VISUAL_OK;
}

int visual_param_entry_min_set_double (VisParamEntry *param, double doubleflt)
{
    visual_return_val_if_fail(param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->min.doubleflt = doubleflt;

    return VISUAL_OK;

}

int visual_param_entry_min_get_integer (VisParamEntry *param)
{
    visual_return_val_if_fail(param != NULL, 0);

    return param->min.integer;

}

float visual_param_entry_min_get_float(VisParamEntry *param)
{
    visual_return_val_if_fail(param != NULL, 0.0);

    return param->min.floating;

}

double visual_param_entry_min_get_double (VisParamEntry *param)
{
    visual_return_val_if_fail(param != NULL, 0.0);

    return param->min.doubleflt;

}

int visual_param_entry_max_set_integer (VisParamEntry *param, int integer)
{
    visual_return_val_if_fail(param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->max.integer = integer;

    return VISUAL_OK;

}

int visual_param_entry_max_set_float (VisParamEntry *param, float floating)
{
    visual_return_val_if_fail(param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->max.floating = floating;

    return VISUAL_OK;

}

int visual_param_entry_max_set_double (VisParamEntry *param, double doubleflt)
{
    visual_return_val_if_fail(param != NULL, -VISUAL_ERROR_PARAM_NULL);

    param->max.doubleflt = doubleflt;

    return VISUAL_OK;
}

int visual_param_entry_max_get_integer (VisParamEntry *param)
{
    visual_return_val_if_fail(param != NULL, 0);

    return param->max.integer;

}

float visual_param_entry_max_get_float(VisParamEntry *param)
{
    visual_return_val_if_fail(param != NULL, 0.0);

    return param->max.floating;

}

double visual_param_entry_max_get_double (VisParamEntry *param)
{
    visual_return_val_if_fail(param != NULL, 0.0);

    return param->max.doubleflt;

}

int visual_param_entry_default_set_string(VisParamEntry *param, char *str)
{
    visual_return_val_if_fail(param != NULL, -VISUAL_ERROR_PARAM_NULL);

    if(param->defaultstring)
        visual_mem_free(param->defaultstring);

    param->defaultstring = visual_strdup(str);

    return VISUAL_OK;
}

int visual_param_entry_default_set_integer(VisParamEntry *param, int integer)
{
    visual_return_val_if_fail(param != NULL, -VISUAL_ERROR_PARAM_NULL);
    param->defaultnum.integer = integer;
    return VISUAL_OK;
}

int visual_param_entry_default_set_float (VisParamEntry *param, float floating)
{
    visual_return_val_if_fail(param != NULL, -VISUAL_ERROR_PARAM_NULL);
    param->defaultnum.floating = floating;
    return VISUAL_OK;
}

int visual_param_entry_default_set_double (VisParamEntry *param, double doubleflt)
{
    visual_return_val_if_fail(param != NULL, -VISUAL_ERROR_PARAM_NULL);
    param->defaultnum.doubleflt = doubleflt;
    return VISUAL_OK;
}

int visual_param_entry_default_set_color (VisParamEntry *param, VisColor *color)
{
    visual_return_val_if_fail(param != NULL, -VISUAL_ERROR_PARAM_NULL);
    visual_color_copy(&param->defaultcolor, color);
    return VISUAL_OK;
}

char * visual_param_entry_default_get_string (VisParamEntry *param)
{
    visual_return_val_if_fail(param != NULL, NULL);
    return param->defaultstring;
}

int visual_param_entry_default_get_integer (VisParamEntry *param)
{
    visual_return_val_if_fail(param != NULL, 0.0);
    return param->defaultnum.integer;
}

float visual_param_entry_default_get_float(VisParamEntry *param)
{
    visual_return_val_if_fail(param != NULL, 0.0);
    return param->defaultnum.floating;
}

double visual_param_entry_default_get_double (VisParamEntry *param)
{
    visual_return_val_if_fail(param != NULL, 0.0);
    return param->defaultnum.doubleflt;
}

VisColor *visual_param_entry_default_get_color (VisParamEntry *param)
{
    visual_return_val_if_fail(param != NULL, NULL);
    return &param->defaultcolor;
}

