#define TEXTNW(X, N)             (drw_font_getexts_width(drw->font, X, N))
#define ALIGN(X, Y, W, H, W2, H2)             X + (W / 2) - (W2 / 2), Y + (H / 2) - (H2 / 2), W2, H2

typedef struct {
        Pixmap pix;
        unsigned int w;
        unsigned int h;
} Bitmap;

typedef struct {
		Bitmap **items;
		int len;
} BitmapSet;

typedef union {
	char *s;
	unsigned int i;
} ft_data;

typedef struct {
	void (*func)(void *data);
	int w;
	ft_data data[3];
	barItem *next;
} barItem;

void drw_bitmap(Drw *drw, int x, int y, unsigned int w, unsigned int h, Bitmap *bm);
void drw_bargraph(Drw *drw, int x, int y, unsigned int w, unsigned int h, Bool horizontal, int val);
void drw_bitmap_bargraph(Drw *drw, int x, int y, unsigned int w, unsigned int h, Bool horizontal, int val, Bitmap *bm);
void ft_set_color(Drw *drw, char *seq, unsigned int len);
void ft_add_bargraph(Drw *drw, int *x, int y, unsigned int h, char *seq, unsigned int len);
void ft_add_bitmap(Drw *drw, BitmapSet *bitmaps, int *x, int y, unsigned int h, char *seq, unsigned int len);
void ft_add_bitmap_bargraph(Drw *drw, BitmapSet *bitmaps, int *x, int y, unsigned int h, char *seq, unsigned int len);
void drw_fancytext(Drw *drw, BitmapSet *bitmaps, int x, int y, unsigned int w, unsigned int h, char *text);
