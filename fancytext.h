//#define TEXTNW(X, N)             (drw_font_getexts_width(drw->font, X, N))
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
	int i;
    Clr *c;
    Bitmap *b;
} ft_data;

typedef struct barItem barItem;

struct barItem {
	void (*func)(Drw *drw, int x, int y, unsigned int h, barItem *item);
	int w;
    int len;
    char k;
	ft_data data[2];
	barItem *next;
};

void drw_bitmap(Drw *drw, int x, int y, unsigned int w, unsigned int h, Bitmap *bm);
void drw_bargraph(Drw *drw, int x, int y, unsigned int w, unsigned int h, Bool horizontal, int val);
void drw_bitmap_bargraph(Drw *drw, int x, int y, unsigned int w, unsigned int h, Bool horizontal, int val, Bitmap *bm);
void ft_set_color(Drw *drw, int x, int y, unsigned int h, barItem *item);
void ft_add_bargraph(Drw *drw, int x, int y, unsigned int h, barItem *item);
void ft_add_bitmap(Drw *drw, int x, int y, unsigned int h, barItem *item);
void ft_add_bitmap_bargraph(Drw *drw, int x, int y, unsigned int h, barItem *item);
void ft_add_text(Drw *drw, int x, int y, unsigned int h, barItem *item);
void drw_fancytext(Drw *drw, int x, int y, unsigned int w, unsigned int h, barItem *item);
int drw_fancytext_prepare(Drw *drw, BitmapSet *bitmaps, char *text, barItem *item);
