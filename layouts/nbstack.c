static void
nbstack(Monitor *m) {
	int x, y, h, w, mh, nm, nmax;
	unsigned int i, n;
	Client *c;

	/* override layout symbol */
	snprintf(m->ltsymbol, sizeof m->ltsymbol, "T%dT", LT(m)->nmaster);
	for(n = 0, c = nexttiled(cl->clients, m); c; c = nexttiled(c->next, m), n++);
	c = nexttiled(cl->clients, m);
	nmax = LT(m)->nmaster;
	nm = nmax == 1 ? 1 : MIN(n / 2, nmax);
	if(nm > n)
		nm = n;
	/* master */
	if(nm > 0) {
		mh = LT(m)->mfact * m->wh;
		w = m->ww / nm;
		if(w < bh)
			w = m->ww;
		x = m->wx;
		for(i = 0; i < nm; i++, c = nexttiled(c->next, m)) {
			resize(c, x, m->wy, ((i + 1 == nm) ? m->wx + m->ww - x : w) - 2 * c->bw,
			       (n == nm ? m->wh : mh) - 2 * c->bw, False);
			if(w != m->ww)
				x = c->x + WIDTH(c);
		}
		n -= nm;
	} else
		mh = 0;
	if(n == 0)
		return;
	/* tile stack */
	x = m->wx;
	y = m->wy + mh;
	w = m->ww / n;
	h = m->wh - mh;
	if(w < bh)
		w = m->ww;
	for(i = 0; c; c = nexttiled(c->next, m), i++) {
		resize(c, x, y, ((i + 1 == n) ? m->wx + m->ww - x : w) - 2 * c->bw,
		       h - 2 * c->bw, False);
		if(w != m->ww)
			x = c->x + WIDTH(c);
	}
}
