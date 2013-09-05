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

	range = horizontal ? w : h;

	v = MIN(MAX(val, 9), 0) / 9.0;
	n = range * v;

	if(horizontal) {
		drw_rect(drw, x,     y, n,         h, True, False, False);
		drw_rect(drw, x + n, y, range - n, h, True, False, True);
	} else {
		drw_rect(drw, x, y,             w, range - n, True, False, True);
		drw_rect(drw, x, y + range - n, w, n,         True, False, False);
	}
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

void ft_set_color(Drw *drw, char *seq, unsigned int len) {
	static Clr bg_last, fg_last;
	char c;

	c = *seq;
	if(len>1) {
		*seq = '#';
		if(c=='b') {
			memcpy(&bg_last, drw->scheme->bg, sizeof(Clr));
			drw_clr_free(drw->scheme->bg);
			drw->scheme->bg = drw_clr_create(drw, seq);
		} else {
			memcpy(&fg_last, drw->scheme->fg, sizeof(Clr));
			drw_clr_free(drw->scheme->fg);
			drw->scheme->fg = drw_clr_create(drw, seq);
		}
	} else {
		if(c=='b')
			memcpy(drw->scheme->bg, &bg_last, sizeof(Clr));
		else
			memcpy(drw->scheme->fg, &fg_last, sizeof(Clr));
	}
	*seq = c;
}

void ft_add_bargraph(Drw *drw, int *x, int y, unsigned int h, char *seq, unsigned int len) {
	int num;

	if(len==2) {
		num = atoi(seq + 1);
		if(*seq=='h') {
			drw_bargraph(drw, *x + 1, y + h/4 + 1, h*2, h/2 - 2, True, num);
			*x += h*2 + 2;
		} else {
			drw_bargraph(drw, *x + 1, y + 1, h/3, h - 2, False, num);
			*x += h/3 + 2;
		}
	} else {
		printf("Bad bargraph sequence '%s'!", &seq[1]);
	}
}

void ft_add_bitmap(Drw *drw, BitmapSet *bitmaps, int *x, int y, unsigned int h, char *seq, unsigned int len) {
	Bitmap *bm;
	int idx;

	if(len>1) {
		idx = atoi(seq+1);
		if(idx>=0 && idx<bitmaps->len) {
			bm = bitmaps->items[idx];
			drw_bitmap(drw, ALIGN(*x, y, bm->w + 2, h, bm->w, bm->h), bm);
			*x += bm->w + 2;
		} else {
			printf("Image %d not found!", idx);
		}
	} else {
		printf("Bad image sequence '%s'!", &seq[1]);
	}

}

void ft_add_bitmap_bargraph(Drw *drw, BitmapSet *bitmaps, int *x, int y, unsigned int h, char *seq, unsigned int len) {
	Bitmap *bm;
	int idx, val, i;

	for(i = 1; seq[i] && (seq[i]!=','); i++);
	if(len>3 && i<len) {
		seq[i] = '\0';
		idx = atoi(seq + 1);
		seq[i] = ',';
		if(idx>=0 && idx<bitmaps->len) {
			bm = bitmaps->items[idx];
			val = atoi(seq + i + 1);
			drw_bitmap_bargraph(drw, ALIGN(*x, y, bm->w + 2, h, bm->w, bm->h), *seq=='G', val, bm);
			x += bm->w + 2;
		} else {
			printf("Image %d not found!", idx);
		}
	} else {
		printf("Bad image sequence '%s'!", &seq[1]);
	}
}

int drw_fancytext_prepare(Drw *drw, BitmapSet *bitmaps, char *text, &) {

}

void drw_fancytext(Drw *drw, BitmapSet *bitmaps, int x, int y, unsigned int w, unsigned int h, char *text) {
	char *ptr = text;
	int len, fh = drw->font->ascent + drw->font->descent + 2;
	unsigned int iw;
	Bool first = True;

	while(*ptr) {
		// seek for begin of conrol sequence (^[)
		// everything else is the current simple text string
		for(len = 0; *ptr && (*ptr!='^' || ptr[1]!='['); len++, ptr++);

		if(len) {
			// output normal text
			iw = TEXTNW(text, len);
			if(first) iw += ( drw->font->ascent + drw->font->descent ) / 2;
			drw_textn(drw, x, y, iw, h, text, len, False);
			x += iw;
			first = False;
		}

		if(!*ptr) break;

		if(*ptr=='^' && ptr[1]=='[') {
			ptr += 2;
			// seek for end of control sequence
			for(len = 0; ptr[len] && (ptr[len]!=';'); len++);
			if(len) {
				ptr[len] = '\0';
				switch(*ptr) {
					// * background & foreground color
					case 'b':
					case 'f':
						ft_set_color(drw, ptr, len);
						break;
					// * bargraph
					case 'v':
					case 'h':
						ft_add_bargraph(drw, &x, y, fh, ptr, len);
						break;
					// * bitmap
					case 'i':
						ft_add_bitmap(drw, bitmaps, &x, y, fh, ptr, len);
						break;
					// * bargraph with bitmap
					case 'G':
					case 'g':
						ft_add_bitmap_bargraph(drw, bitmaps, &x, y, fh, ptr, len);
						break;
					// * delimiter
					// case 'd':
					// 	ptr[i] = ';';
					// 	rptr = ptr + i + 1;
					// 	ptr = buf = sstrings[atoi(ptr+1)];
					// 	i=-1;
					// 	drw_fancytext(drw, x, y, w, h, )
					// 	break;
					default:
						printf("fancytext: unknown operator '%c' in '%s'\n", *ptr, ptr + 1);
						break;
				}
				ptr[len] = ';';
				text = ptr = ptr + len + 1;
			} else {
				printf("fancytext: malformated sequence '%s'\n", ptr);
			}
			
		}
	}
}

void drw_prepare(Drw *drw, BitmapSet *bitmaps, unsigned int h, char *text, barItem *item) {
	char *ptr = text;
	int len, fh = drw->font->ascent + drw->font->descent + 2;
	unsigned int iw;
	Bool first = True;

	while(*ptr) {
		// seek for begin of conrol sequence (^[)
		// everything else is the current simple text string
		for(len = 0; *ptr && (*ptr!='^' || ptr[1]!='['); len++, ptr++);

		if(len) {
			ptr[len] = '\0';
			// output normal text
			item->func = ft_drw_text;
			item->w = TEXTNW(text, len);
			//if(first) iw += ( drw->font->ascent + drw->font->descent ) / 2;
			//drw_textn(drw, x, y, iw, h, text, len, False);
			//x += iw;
			//first = False;
			item->data[0].s = text;
		}

		if(!*ptr) break;

		if(*ptr=='^' && ptr[1]=='[') {
			ptr += 2;
			// seek for end of control sequence
			for(len = 0; ptr[len] && (ptr[len]!=';'); len++);
			if(len) {
				ptr[len] = '\0';
				switch(*ptr) {
					// * background & foreground color
					case 'b':
					case 'f':
						ft_set_color(drw, ptr, len);
						break;
					// * bargraph
					case 'v':
					case 'h':
						ft_add_bargraph(drw, &x, y, fh, ptr, len);
						break;
					// * bitmap
					case 'i':
						ft_add_bitmap(drw, bitmaps, &x, y, fh, ptr, len);
						break;
					// * bargraph with bitmap
					case 'G':
					case 'g':
						ft_add_bitmap_bargraph(drw, bitmaps, &x, y, fh, ptr, len);
						break;
					// * delimiter
					// case 'd':
					// 	ptr[i] = ';';
					// 	rptr = ptr + i + 1;
					// 	ptr = buf = sstrings[atoi(ptr+1)];
					// 	i=-1;
					// 	drw_fancytext(drw, x, y, w, h, )
					// 	break;
					default:
						printf("fancytext: unknown operator '%c' in '%s'\n", *ptr, ptr + 1);
						break;
				}
				ptr[len] = ';';
				text = ptr = ptr + len + 1;
			} else {
				printf("fancytext: malformated sequence '%s'\n", ptr);
			}
			
		}
	}
}
