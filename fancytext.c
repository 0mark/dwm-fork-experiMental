#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

#include "drw.h"
#include "fancytext.h"
#include "util.h"

void drw_bitmap(Drw *drw, int x, int y, unsigned int w, unsigned int h, Bitmap *bm) {
	w = MIN(bm->w, w);
	h = MIN(bm->h, h);

	XSetBackground(drw->dpy, drw->gc, drw->scheme->bg->rgb);
	XSetForeground(drw->dpy, drw->gc, drw->scheme->fg->rgb);
	XCopyPlane(drw->dpy, bm->pix, drw->drawable, drw->gc, 0, 0, w, h, x, y, 1);
}

void drw_bargraph(Drw *drw, int x, int y, unsigned int w, unsigned int h, Bool horizontal, int val) {
	int range, n;
	float v;
	Clr *ctmp = drw->scheme->bg;

	drw->scheme->bg = drw->scheme->border;
	drw->scheme->border = ctmp;

	range = horizontal ? h : w;

	v = MAX(MIN(val, 9), 0) / 9.0;
	n = range * v;

printf("%d, %d, %d, %d, h?: %d, val: %d, v: %f, n: %d, range: %d\n",x,y,w,h,horizontal,val,v,n, range);

	if(horizontal) {
		drw_rect2(drw, x, y,             w, range - n, True, False, True);
		drw_rect2(drw, x, y + range - n, w, n,         True, False, False);
	} else/*vertical*/ {
		drw_rect2(drw, x,     y, n,         h, True, False, False);
		drw_rect2(drw, x + n, y, range - n, h, True, False, True);
	}

	drw->scheme->border = drw->scheme->bg;
	drw->scheme->bg = ctmp;
}

void drw_bitmap_bargraph(Drw *drw, int x, int y, unsigned int w, unsigned int h, Bool horizontal, int val, Bitmap *bm) {
	int range, n;
	float v;

	range = horizontal ? w : h;

	v = MIN(MAX(val, 9), 0) / 9.0;
	n = range * v;

	if(horizontal) {
		drw_bitmap(drw, x,     y, n,         h, bm);
		drw_bitmap(drw, x + n, y, range - n, h, bm);
	} else {
		drw_bitmap(drw, x, y,             w, range - n, bm);
		drw_bitmap(drw, x, y + range - n, w, n,         bm);
	}
}

void ft_set_color(Drw *drw, int x, int y, unsigned int h, barItem *item) {
	static Clr bg_last, fg_last;

	if(item->data[0].c!=NULL) {
		if(item->k=='b') {
			memcpy(&bg_last, drw->scheme->bg, sizeof(Clr));
			drw_clr_free(drw->scheme->bg);
			drw->scheme->bg = item->data[0].c;
		} else {
			memcpy(&fg_last, drw->scheme->fg, sizeof(Clr));
			drw_clr_free(drw->scheme->fg);
			drw->scheme->fg = item->data[0].c;
		}
	} else {
		if(item->k=='b')
			memcpy(drw->scheme->bg, &bg_last, sizeof(Clr));
		else
			memcpy(drw->scheme->fg, &fg_last, sizeof(Clr));
	}
}

void ft_add_bargraph(Drw *drw, int x, int y, unsigned int h, barItem *item) {
	printf("%c, %d\n", item->k, item->data[0].i);
	if(item->k=='h') {
		drw_bargraph(drw, x + 1, y + 1, h/3, h - 2, True, item->data[0].i);
	} else {
		drw_bargraph(drw, x + 1, y + h/4 + 1, h*2, h/2 - 2, False, item->data[0].i);
	}
}

void ft_add_bitmap(Drw *drw, int x, int y, unsigned int h, barItem *item) {
	Bitmap *bm = item->data[0].b;

	drw_bitmap(drw, ALIGN(x, y, bm->w + 2, h, bm->w, bm->h), bm);
}

void ft_add_bitmap_bargraph(Drw *drw, int x, int y, unsigned int h, barItem *item) {
	Bitmap *bm = item->data[0].b;

	drw_bitmap_bargraph(drw, ALIGN(x, y, bm->w + 2, h, bm->w, bm->h), item->k=='G', item->data[1].i, bm);
}

void ft_add_text(Drw *drw, int x, int y, unsigned int h, barItem *item) {
	drw_textn(drw, x, y, item->w, h, item->data[0].s, item->len, False, False);
}

void drw_fancytext(Drw *drw, int x, int y, unsigned int w, unsigned int h, barItem *item) {
	Clr *bg, *fg;
	barItem *tmp;

	bg = drw->scheme->bg;
	fg = drw->scheme->fg;
	drw->scheme->bg = calloc(sizeof(Clr), 1);
	drw->scheme->fg = calloc(sizeof(Clr), 1);
	memcpy(drw->scheme->bg, bg, sizeof(Clr));
	memcpy(drw->scheme->fg, fg, sizeof(Clr));

	XSetForeground(drw->dpy, drw->gc, drw->scheme->bg->rgb);
	XFillRectangle(drw->dpy, drw->drawable, drw->gc, x, y, /*h/2*/w, h);
	x += h/2;

	while(item->next) {
		item->func(drw, x, y, h, item);
		x += item->w;
		tmp = item;
		item = item->next;
		free(tmp);
	}

	drw_clr_free(drw->scheme->bg);
	drw_clr_free(drw->scheme->fg);
	drw->scheme->bg = bg;
	drw->scheme->fg = fg;

	XSetForeground(drw->dpy, drw->gc, drw->scheme->bg->rgb);
	XFillRectangle(drw->dpy, drw->drawable, drw->gc, x, y, h/2, h);
}

int drw_fancytext_prepare(Drw *drw, BitmapSet *bitmaps, char *text, barItem *item) {
	char *ptr = text;
	int len, fh = drw->font->ascent + drw->font->descent + 2, i, w = drw->font->h;

	while(*ptr) {
		if(*ptr=='^' && ptr[1]=='[') {
			// seek for end of control sequence
			ptr += 2;
			for(len = 0; ptr[len] && (ptr[len]!=';'); len++);

			if(len) {
				item->k = *ptr;
				ptr[len] = '\0';
				switch(*ptr) {
					// * background & foreground color
					case 'b': case 'f':
						item->func = &ft_set_color;
						if(len>1) {
							*ptr = '#';
							item->data[0].c = drw_clr_create(drw, ptr);
							*ptr = item->k;
						}
						break;

					// * bargraph
					case 'v': case 'h':
						item->func = &ft_add_bargraph;
						w += item->w = (*ptr=='h' ? fh/3 : fh*2);
						//printf("%d\n", item->w);
						if(len==2) {
							item->data[0].i = atoi(ptr+1);
						}
						break;

					// * bitmap
					case 'i':
						item->func = &ft_add_bitmap;
						i = atoi(ptr+1);
						if(i>=0 && i<bitmaps->len) {
							item->data[0].b = bitmaps->items[i];
							w += item->w = bitmaps->items[i]->w;
						}
						break;

					// * bitmap as bargraph
					case 'G': case 'g':
						item->func = &ft_add_bitmap_bargraph;
						for(i = 1; ptr[i] && (ptr[i]!=','); i++);
						if(len>3 && i<len) {
							ptr[i] = '\0';
							i = atoi(ptr+1);
							if(i>0 && i<bitmaps->len) {
								item->data[0].b = bitmaps->items[i];
								w += item->w = bitmaps->items[i]->w;
								item->data[1].i = atoi(ptr+i+1);
							}
							ptr[i] = ',';
						}
						break;

					default:
						printf("fancytext: unknown operator '%c' in '%s'\n", *ptr, ptr + 1);
						break;
				}
				ptr[len++] = ';';
			} else {
				printf("fancytext: malformated sequence '%s'\n", ptr);
				continue;
			}
		} else {
			// seek for begin of control sequence (^[)
			// everything else is the current simple text string
			for(len = 0; ptr[len] && (ptr[len]!='^' || ptr[len+1]!='['); len++);

			// everything before the control sequence is plain text
			if(len) {
				item->func = &ft_add_text;
				//printf("%.*s\n", len, ptr);
				w += item->w = drw_font_getexts_width(drw->font, ptr, len);
				item->data[0].s = ptr;
				item->len = len;
			}
		}
		ptr += len;
		if(item->func!=NULL) {
			item->next = calloc(1, sizeof(barItem));
			item = item->next;
		} else {
			printf("fancytext: malformated operator '%c' in '%.*s'\n", *ptr, len, ptr + 1);
		}
	}
	printf("::::::::::::::::::::.\n");
	return w;
}
