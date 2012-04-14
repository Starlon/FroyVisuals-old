/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_defines.h,v 1.7 2006/01/22 13:23:37 synap Exp $
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

#ifndef _LV_DEFINES_H
#define _LV_DEFINES_H

#ifdef __cplusplus
# define VISUAL_C_LINKAGE extern "C"
#else
# define VISUAL_C_LINKAGE
#endif /* __cplusplus */

#ifdef __cplusplus
# define VISUAL_BEGIN_DECLS	VISUAL_C_LINKAGE {
# define VISUAL_END_DECLS	}
#else
# define VISUAL_BEGIN_DECLS
# define VISUAL_END_DECLS
#endif /* __cplusplus */

#ifndef NULL
# ifndef __cplusplus
#   define NULL ((void *) 0)
# else
#   define NULL 0
# endif
#endif /* NULL */

#ifndef FALSE
#define FALSE	(0)
#endif

#ifndef TRUE
#define TRUE	(1)
#endif

/* Compiler specific optimalization macros */
#if __GNUC__ >= 3
# define VIS_ATTR_MALLOC    __attribute__ ((malloc))
# define VIS_ATTR_PACKED    __attribute__ ((packed))
# define VIS_LIKELY(x)      __builtin_expect (!!(x), 1)
# define VIS_UNLIKELY(x)    __builtin_expect (!!(x), 0)
#else
# define VIS_ATTR_MALLOC    /* no malloc */
# define VIS_ATTR_PACKED    /* no packed */
# define VIS_LIKELY(x)      (x)
# define VIS_UNLIKELY(x)    (x)
#endif /* __GNUC__ >= 3 */

#if defined __GNUC__
#  define VIS_CHECK_PRINTF_FORMAT(a, b) __attribute__ ((__format__ (__printf__, a, b)))
#else
#  define VIS_CHECK_PRINTF_FORMAT(a, b) /* no compile-time format string check */
#endif /* __GNUC__ */

#endif /* _LV_DEFINES_H */
