#include "lv_video_convert.h"
#include "lv_common.h"

#pragma pack(1)

typedef struct {
#ifdef VISUAL_LITTLE_ENDIAN
	uint16_t b:5, g:6, r:5;
#else
	uint16_t r:5, g:6, b:5;
#endif
} rgb16_t;

#pragma pack()

void visual_video_convert_get_smallest (VisVideo *dest, VisVideo *src, int *width, int *height)
{
	*width = dest->width > src->width ? src->width : dest->width;
	*height = dest->height > src->height ? src->height : dest->height;
}

void visual_video_index8_to_rgb16 (VisVideo *dest, VisVideo *src)
{
	int x, y, i;
	int w;
	int h;

	int ddiff;
	int sdiff;

	rgb16_t* dbuf = visual_video_get_pixels (dest);
	uint8_t* sbuf = visual_video_get_pixels (src);

	rgb16_t colors[256];

	for(i = 0; i < 256; i++) {
		colors[i].r = src->pal->colors[i].r >> 3;
		colors[i].g = src->pal->colors[i].g >> 2;
		colors[i].b = src->pal->colors[i].b >> 3;
	}

	visual_video_convert_get_smallest (dest, src, &w, &h);

	ddiff = (dest->pitch / dest->bpp) - w;
	sdiff = src->pitch - (w * src->bpp);

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			*(dbuf++) = colors[*(sbuf++)];
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}
}

void visual_video_index8_to_rgb24 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	int ddiff;
	int sdiff;

	uint8_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);

	visual_video_convert_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = src->pitch - (w * src->bpp);

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
#ifdef VISUAL_LITTLE_ENDIAN
			*(dbuf++) = src->pal->colors[*(sbuf)].b;
			*(dbuf++) = src->pal->colors[*(sbuf)].g;
			*(dbuf++) = src->pal->colors[*(sbuf)].r;
#else
			*(dbuf++) = src->pal->colors[*(sbuf)].r;
			*(dbuf++) = src->pal->colors[*(sbuf)].g;
			*(dbuf++) = src->pal->colors[*(sbuf)].b;
#endif /* VISUAL_LITTLE_ENDIAN */
			sbuf++;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}
}

void visual_video_index8_to_argb32 (VisVideo *dest, VisVideo *src)
{
	int x, y, i;
	int w;
	int h;

	uint32_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	uint32_t colors[256];

	for (i = 0; i < 256; ++i) {
		colors[i] =
			255 << 24 |
			src->pal->colors[i].r << 16 |
			src->pal->colors[i].g << 8 |
			src->pal->colors[i].b;
	}

	visual_video_convert_get_smallest (dest, src, &w, &h);

	ddiff = (dest->pitch / dest->bpp) - w;
	sdiff = src->pitch - (w * src->bpp);

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			*(dbuf++) = colors[*(sbuf++)];
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}
}

void visual_video_rgb16_to_index8 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	uint8_t *dbuf = visual_video_get_pixels (dest);
	rgb16_t *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	uint8_t r, g, b, col;

	visual_video_convert_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = (src->pitch  / src->bpp) - w;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			r = sbuf->r << 3;
			g = sbuf->g << 2;
			b = sbuf->b << 3;
			sbuf++;

			col = (r + g + b) / 3;

			dest->pal->colors[col].r = r;
			dest->pal->colors[col].g = g;
			dest->pal->colors[col].b = b;

			*(dbuf++) = col;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}
}

void visual_video_rgb16_to_rgb24 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	uint8_t *dbuf = visual_video_get_pixels (dest);
	rgb16_t *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	visual_video_convert_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = (src->pitch  / src->bpp) - w;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
#ifdef VISUAL_LITTLE_ENDIAN
			*(dbuf++) = sbuf->b << 3;
			*(dbuf++) = sbuf->g << 2;
			*(dbuf++) = sbuf->r << 3;
#else
			*(dbuf++) = sbuf->r << 3;
			*(dbuf++) = sbuf->g << 2;
			*(dbuf++) = sbuf->b << 3;
#endif /* VISUAL_LITTLE_ENDIAN */
			sbuf++;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}
}

void visual_video_rgb16_to_argb32 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	uint8_t *dbuf = visual_video_get_pixels (dest);
	rgb16_t *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	visual_video_convert_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = (src->pitch  / src->bpp) - w;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
#ifdef VISUAL_LITTLE_ENDIAN
			*(dbuf++) = sbuf->b << 3;
			*(dbuf++) = sbuf->g << 2;
			*(dbuf++) = sbuf->r << 3;
			*(dbuf++) = 255;
#else
			*(dbuf++) = 255;
			*(dbuf++) = sbuf->r << 3;
			*(dbuf++) = sbuf->g << 2;
			*(dbuf++) = sbuf->b << 3;
#endif /* VISUAL_LITTLE_ENDIAN */

			sbuf++;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}
}

void visual_video_rgb24_to_index8 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;
	uint8_t r, g, b, col;

	uint8_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	visual_video_convert_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = src->pitch - (w * src->bpp);

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
#ifdef VISUAL_LITTLE_ENDIAN
			b = *(sbuf++);
			g = *(sbuf++);
			r = *(sbuf++);
#else
			r = *(sbuf++);
			g = *(sbuf++);
			b = *(sbuf++);
#endif /* VISUAL_LITTLE_ENDIAN */

			col = (b + g + r) / 3;

			dest->pal->colors[col].r = r;
			dest->pal->colors[col].g = g;
			dest->pal->colors[col].b = b;

			*(dbuf++) = col;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}
}

void visual_video_rgb24_to_rgb16 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	rgb16_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	visual_video_convert_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = (src->pitch / src->bpp) - w;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
#ifdef VISUAL_LITTLE_ENDIAN
			dbuf->b = *(sbuf++) >> 3;
			dbuf->g = *(sbuf++) >> 2;
			dbuf->r = *(sbuf++) >> 3;
#else
			dbuf->r = *(sbuf++) >> 3;
			dbuf->g = *(sbuf++) >> 2;
			dbuf->b = *(sbuf++) >> 3;
#endif
			dbuf++;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}
}

void visual_video_rgb24_to_argb32 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	uint8_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	visual_video_convert_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = src->pitch - (w * src->bpp);

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
#ifdef VISUAL_LITTLE_ENDIAN
			*(dbuf++) = *(sbuf++);
			*(dbuf++) = *(sbuf++);
			*(dbuf++) = *(sbuf++);
			*(dbuf++) = 255;
#else
			*(dbuf++) = 255;
			*(dbuf++) = *(sbuf++);
			*(dbuf++) = *(sbuf++);
			*(dbuf++) = *(sbuf++);
#endif /* VISUAL_LITTLE_ENDIAN */
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}
}

void visual_video_argb32_to_index8 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	uint8_t r, g, b, col;

	uint8_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	visual_video_convert_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = src->pitch - (w * src->bpp);

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
#ifdef VISUAL_LITTLE_ENDIAN
			b = *(sbuf++);
			g = *(sbuf++);
			r = *(sbuf++);
			sbuf++;
#else
			sbuf++;
			r = *(sbuf++);
			g = *(sbuf++);
			b = *(sbuf++);
#endif /* VISUAL_LITTLE_ENDIAN */

			col = (r + g + b) / 3;

			dest->pal->colors[col].r = r;
			dest->pal->colors[col].g = g;
			dest->pal->colors[col].b = b;

			*(dbuf++) = col;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}
}

void visual_video_argb32_to_rgb16 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	rgb16_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	visual_video_convert_get_smallest (dest, src, &w, &h);

	ddiff = (dest->pitch / dest->bpp) - w;
	sdiff = src->pitch - (w * src->bpp);

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
#ifdef VISUAL_LITTLE_ENDIAN
			dbuf->b = *(sbuf++) >> 3;
			dbuf->g = *(sbuf++) >> 2;
			dbuf->r = *(sbuf++) >> 3;
			sbuf++;
#else
			sbuf++;
			dbuf->r = *(sbuf++) >> 3;
			dbuf->g = *(sbuf++) >> 2;
			dbuf->b = *(sbuf++) >> 3;
#endif /* VISUAL_LITTLE_ENDIAN */

			dbuf++;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}
}

void visual_video_argb32_to_rgb24 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	uint8_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	visual_video_convert_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = src->pitch - (w * src->bpp);

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
#ifdef VISUAL_LITTLE_ENDIAN
			*(dbuf++) = *(sbuf++);
			*(dbuf++) = *(sbuf++);
			*(dbuf++) = *(sbuf++);
			sbuf++;
#else
			sbuf++;
			*(dbuf++) = *(sbuf++);
			*(dbuf++) = *(sbuf++);
			*(dbuf++) = *(sbuf++);
#endif /* VISUAL_LITTLE_ENDIAN */
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}
}

void visual_video_flip_bytes_color16 (VisVideo *dest, VisVideo *src)
{
	rgb16_t *destbuf, *srcbuf;
	int x, y;
	int pitchdiff = (dest->pitch - (dest->width * dest->bpp)) >> 1;

	destbuf = (rgb16_t *) visual_video_get_pixels (dest);
	srcbuf  = (rgb16_t *) visual_video_get_pixels (src);

	for (y = 0; y < dest->height; y++) {
		for (x = 0; x < dest->width; x++) {
			destbuf->b = srcbuf->r;
			destbuf->g = srcbuf->g;
			destbuf->r = srcbuf->b;
			destbuf++;
			srcbuf++;
		}

		destbuf += pitchdiff;
	}
}

void visual_video_flip_bytes_color24 (VisVideo *dest, VisVideo *src)
{
	uint8_t *destbuf, *srcbuf;
	int x, y;
	int pitchdiff = dest->pitch - (dest->width * dest->bpp);

	destbuf = visual_video_get_pixels (dest);
	srcbuf = visual_video_get_pixels (src);

	for (y = 0; y < dest->height; y++) {
		for (x = 0; x < dest->width; x++) {
			*(destbuf + 2) = *(srcbuf);
			*(destbuf + 1) = *(srcbuf + 1);
			*(destbuf) = *(srcbuf + 2);

			destbuf += dest->bpp;
			srcbuf += src->bpp;
		}

		destbuf += pitchdiff;
	}
}

void visual_video_flip_bytes_color32 (VisVideo *dest, VisVideo *src)
{
	uint8_t *destbuf, *srcbuf;
	int x, y;
	int pitchdiff = dest->pitch - (dest->width * dest->bpp);

	destbuf = visual_video_get_pixels (dest);
	srcbuf = visual_video_get_pixels (src);

	for (y = 0; y < dest->height; y++) {
		for (x = 0; x < dest->width; x++) {
			destbuf[0] = srcbuf[3];
			destbuf[1] = srcbuf[2];
			destbuf[2] = srcbuf[1];
			destbuf[3] = srcbuf[0];

			destbuf += dest->bpp;
			srcbuf += src->bpp;
		}

		destbuf += pitchdiff;
	}
}
