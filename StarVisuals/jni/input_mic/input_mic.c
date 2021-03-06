/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2012
 *
 * Authors: Scott Sibley <sisibley@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <gettext.h>
#include <math.h>
#include <jni.h>
#include <libvisual/libvisual.h>

// This is just a dummy plugin. You need to provide a callback feature.

static int inp_mic_init (VisPluginData *plugin);
static int inp_mic_cleanup (VisPluginData *plugin);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static VisInputPlugin input[] = {{
            // empty.
    }};

    static VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_INPUT,

        .plugname = "mic",
        .name = "Microphone Input",
        .author = "Scott Sibley <sisibley@gmail.com>",
        .version = "0.1",
        .about = ("Mic input plugin for libvisual"),
        .help = ("You sing into the mic, and LibVisual goes nuts. Sing loud. With feeling."),
        .license = VISUAL_PLUGIN_LICENSE_LGPL,

        .init = inp_mic_init,
        .cleanup = inp_mic_cleanup,

        .plugin = VISUAL_OBJECT (&input[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int inp_mic_init (VisPluginData *plugin)
{
    visual_return_val_if_fail(plugin != NULL, -1);

    return 0;
}

int inp_mic_cleanup (VisPluginData *plugin)
{
    visual_return_val_if_fail(plugin != NULL, -1);

    return 0;
}


