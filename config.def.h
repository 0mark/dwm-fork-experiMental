/* See LICENSE file for copyright and license details. */

/* appearance */
static const char font[]                  = "-*-terminus-*-*-*-*-12-*-*-*-*-*-*-*";
static const char normbordercolor[]       = "#777777";
static const char normbgcolor[]           = "#161616";
static const char normfgcolor[]           = "#ffffff";
static const char selbordercolor[]        = "#276CC2";
static const char selbgcolor[]            = "#222222";
static const char selfgcolor[]            = "#276CC2";
static const unsigned int borderpx        = 1;        /* border pixel of windows */
static const unsigned int snap            = 32;       /* snap pixel */
static const unsigned int systraypinning  = 0;      /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing  = 2;      /* systray spacing */
static const Bool systraypinningfailfirst = True;   /* True: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const Bool showsystray             = True;   /* False means no systray */
static const Bool monobar                 = True;

static const Rule rules[] = {
	/* 
	 * xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
     *
     * class            instance               title         tags mask     isfloating   monitor   scratch? */
	 */
// to tab
	{ "Firefox",        NULL,                  NULL,         1 << 8,       False,       -1,       False },

// float
	{ "Gimp",           NULL,                  NULL,         0,            True,        -1,       False },

// scratchpads
	{ NULL,             "dwm-scratchpad",      NULL,        ~0,            True,        -1,       1 },
};

/* layout(s) */
static const Bool resizehints = True; /* True means respect size hints in tiled resizals */

/* addons: layouts */
#include "layouts/nbstack.c"       /* bottom stack (tiling) */

static const Layout layouts[] = {
	/* symbol     arrange function   mfact   nmaster   showbar   topbar   addgaps*/
	{ "[]=",      tile,              0.55,   1,        True,     True,    6 },    /* first entry is default */
	{ "><>",      NULL,              0.55,   1,        True,     True,    6 },    /* no layout function means floating behavior */
	{ "[M]",      monocle,           0.55,   1,        True,     True,    0 },
	{ "[TTT]",    nbstack,           0.7,    1,        True,     True,    6 },
};

/* naive preload approach */
static const TagPreset tags[] = {
	{ "1", { 0, 2 } },
	{ "2", { 3, 0 } },
	{ "3", { 0, 2 } },
	{ "4", { 1, 1 } },
	{ "5", { 3, 2 } },
	{ "6", { 0, 2 } },
	{ "7", { 0, 2 } },
	{ "8", { 0, 2 } },
	{ "9", { 2, 0 } },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", font, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "st", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */

	/* spawn */
	{ MODKEY|ControlMask,           XK_x,       spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_k,       spawn,          {.v = keycmd } },
	//{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	/* -- */
	/* appereance */
	{ MODKEY,                       XK_Home,    togglebar,      {0} },
	{ MODKEY,                       XK_Left,    setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_Right,   setmfact,       {.f = +0.05} },
	/* -- */
	/* stack */
	{ MODKEY,                       XK_Down,    focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_Up,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Page_Up, togglemax,      {0} },
	{ MODKEY|ControlMask,           XK_Down,    pushdown,       {0} },
	{ MODKEY|ControlMask,           XK_Up,      pushup,         {0} },
	/* -- */
	/* tab */
	{ MODKEY,                       XK_Tab,    view,           {0} },
	//{ Mod1Mask,                     XK_Tab,     focustoggle,    {0} },
	/* -- */
	/* set layout */
	{ MODKEY,                       XK_space,   toggle_scratch, {.i = 1} },
	{ MODKEY,                       XK_n,       toggle_scratch, {.i = 2} },
	{ MODKEY,                       XK_l,       toggle_scratch, {.i = 3} },
	{ MODKEY|ShiftMask,             XK_space,   setlayout,      {0} },
	{ MODKEY|ControlMask,           XK_space,   togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_Delete,  killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_t,       setlayout,      {.v = &layouts[0]} }, /*tile*/
	{ MODKEY|ShiftMask,             XK_f,       setlayout,      {.v = &layouts[1]} }, /*float*/
	{ MODKEY|ShiftMask,             XK_m,       setlayout,      {.v = &layouts[2]} }, /*monocle*/
	{ MODKEY|ShiftMask,             XK_b,       setlayout,      {.v = &layouts[3]} }, /*nbstack*/
	//{ MODKEY|ShiftMask,             XK_g,       setlayout,      {.v = &layouts[4]} }, /*gaplessgrid*/
	//{ MODKEY|ShiftMask,             XK_r,       setlayout,      {.v = &layouts[5]} }, /*grid*/
	//{ MODKEY|ShiftMask,             XK_h,       setlayout,      {.v = &layouts[6]} }, /*bstackhoriz*/
	/* -- */
	/* nmaster */
	{ MODKEY|ShiftMask,             XK_Left,    incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_Right,   incnmaster,     {.i = -1 } },
	//{ MODKEY|ShiftMask,             XK_Up,      setnmaster,     {.i = 2 } },
	/* -- */
	/* tag keys */
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY,                       XK_0,       view,           {.ui = ~0 } },
	{ MODKEY|ControlMask,           XK_0,       tag,            {.ui = ~0 } },
	/* -- */
	/* quit */
	{ MODKEY|ControlMask,           XK_Escape,  quit,           {0} },
	//{ MODKEY,                       XK_Escape,  quit,           {.i = 23} },
	/* -- */
	/* monitor */
	{ MODKEY,                       XK_comma,   focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period,  focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,   tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,  tagmon,         {.i = +1 } },
	/* -- */

};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkWinTitle,          0,              Button1,        focusonclick,   {0} },
};

