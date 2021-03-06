/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_os.c,v 1.3 2006/01/26 15:13:37 synap Exp $
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
#include "lv_os.h"
#include "lv_common.h"
#include <sys/types.h>
#include <unistd.h>

#ifdef HAVE_SCHED_H
# include <sched.h>
# include <sys/types.h>
# define HAVE_SCHED 1
#elif defined HAVE_SYS_SCHED_H
# include <sys/sched.h>
# include <sys/types.h>
# define HAVE_SCHED 1
#endif

#include "lv_os.h"

/* FIXME: Lock all memory in realtime mode ? */

int visual_os_scheduler_realtime_start ()
{
#ifdef HAVE_SCHED
	int ret;

	/* FIXME: Do we want RR or FIFO here ? */
#ifndef VISUAL_WITH_MINGW
	struct sched_param attr;
	attr.sched_priority = 99;
	ret = sched_setscheduler (getpid (), SCHED_FIFO, &attr);
#else
	ret = sched_setscheduler (getpid (), SCHED_FIFO);
#endif /* VISUAL_WITH_MINGW */

	return ret >= 0 ? VISUAL_OK : -VISUAL_ERROR_OS_SCHED;
#else
	return -VISUAL_ERROR_OS_SCHED_NOT_SUPPORTED;
#endif /* HAVE_SCHED */
}

int visual_os_scheduler_realtime_stop ()
{
#ifdef HAVE_SCHED
	int ret;

#ifndef VISUAL_WITH_MINGW
	struct sched_param attr;
	attr.sched_priority = 0;
	ret = sched_setscheduler (getpid (), SCHED_OTHER, &attr);
#else
	ret = sched_setscheduler (getpid (), SCHED_OTHER);
#endif /* VISUAL_WITH_MINGW */

	return ret >= 0 ? VISUAL_OK : -VISUAL_ERROR_OS_SCHED;
#else
	return -VISUAL_ERROR_OS_SCHED_NOT_SUPPORTED;
#endif /* HAVE_SCHED */
}

int visual_os_scheduler_yield ()
{
#ifdef HAVE_SCHED
	sched_yield ();

	return VISUAL_OK;
#else
	return -VISUAL_ERROR_OS_SCHED_NOT_SUPPORTED;
#endif
}


