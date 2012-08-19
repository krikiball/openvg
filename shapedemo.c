//
// shapedemo: testbed for OpenVG APIs
// Anthony Starks (ajstarks@gmail.com)
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"

// randcolor returns a random number 0..255
unsigned int randcolor() {
	return (unsigned int)(drand48() * 255.0);
}

// randf returns a floating point number bounded by n
VGfloat randf(int n) {
	return drand48() * n;
}

// coordpoint marks a coordinate, preserving a previous color
void coordpoint(VGfloat x, VGfloat y, VGfloat size, VGfloat pcolor[4]) {
	Fill(128, 0, 0, 0.3);
	Circle(x, y, size);
	setfill(pcolor);
}

// grid draws a grid
void grid(VGfloat x, VGfloat y, int n, int w, int h) {
	VGfloat ix, iy;
	Stroke(128, 128, 128, 0.5);
	strokeWidth(2);
	for (ix = x; ix <= x + w; ix += n) {
		Line(ix, y, ix, y + h);
	}

	for (iy = y; iy <= y + h; iy += n) {
		Line(x, iy, x + w, iy);
	}
}

typedef struct {
	Fontinfo font;
	VGfloat tw;
	int fontsize;
} FW;

// adjust the font to fit within a width
void fitwidth(int width, int adj, char *s, FW * f) {
	f->tw = textwidth(s, f->font, f->fontsize);
	while (f->tw > width) {
		f->fontsize -= adj;
		f->tw = textwidth(s, f->font, f->fontsize);
	}
}

// testpattern shows a test pattern 
void testpattern(int width, int height, char *s) {

	VGfloat midx1, midx2, midx3, midy1, midy2, midy3, bgcolor[4], tc[4];
	int fontsize = 256, w2 = width / 2, h2 = height / 2;
	FW tw1 = { MonoTypeface, 0, fontsize }, tw2 = {
	SansTypeface, 0, fontsize}, tw3 = {
	SerifTypeface, 0, fontsize};

	RGB(255, 255, 255, bgcolor);
	Start(width, height, bgcolor);

	// colored squares in the corners
	Fill(255, 0, 0, 1);
	Rect(0, 0, 100, 100);
	Fill(0, 255, 0, 1);
	Rect(0, height - 100, 100, 100);
	Fill(0, 0, 255, 1);
	Rect(width - 100, 0, 100, 100);
	Fill(128, 128, 128, 1);
	Rect(width - 100, height - 100, 100, 100);

	// for each font, (Sans, Serif, Mono), adjust the string to the width
	fitwidth(width, 20, s, &tw1);
	fitwidth(width, 20, s, &tw2);
	fitwidth(width, 20, s, &tw3);

	// Determine the midpoint
	midx1 = w2 - (tw1.tw / 2);
	midx2 = w2 - (tw2.tw / 2);
	midx3 = w2 - (tw3.tw / 2);

	// Adjust the baselines to be medial
	midy1 = h2 + 20 + (tw1.fontsize) / 2;
	midy2 = h2 - (tw2.fontsize) / 2;
	midy3 = h2 - 20 - tw2.fontsize - (tw3.fontsize) / 2;

	RGB(128, 128, 128, tc);
	Text(midx1, midy1, s, tw1.font, tw1.fontsize, tc);
	RGB(128, 0, 0, tc);
	Text(midx2, midy2, s, tw2.font, tw2.fontsize, tc);
	RGB(0, 0, 128, tc);
	Text(midx3, midy3, s, tw3.font, tw3.fontsize, tc);
	//SaveRaw("testpattern.raw");
	End();
}

// textlines writes lines of text
void textlines(VGfloat x, VGfloat y, char *s[], Fontinfo f, int fontsize, VGfloat leading, VGfloat fill[4]) {

	int i;
	for (i = 0;; i++) {
		if (s[i] == NULL) {
			break;
		}
		Text(x, y, s[i], f, fontsize, fill);
		y -= leading;
	}
}

// tb draws a block of text
void tb(int w, int h) {

	char *para[] = {
		"For lo, the winter is past,",
		"the rain is over and gone",
		"the flowers appear on the earth;",
		"the time for the singing of birds is come,",
		"and the voice of the turtle is heard in our land",
		NULL
	};

	char *labels[] = {
		"Serif",
		"Sans",
		"Mono",
		NULL
	};

	VGfloat bgcolor[4], textcolor[4], tmargin = w * 0.33, lmargin = w * 0.10;

	RGB(240, 240, 240, bgcolor);
	RGB(49, 79, 79, textcolor);
	Start(w, h, bgcolor);
	textlines(tmargin, h - 100, para, SerifTypeface, 24, 40, textcolor);
	textlines(tmargin, h - 400, para, SansTypeface, 24, 40, textcolor);
	textlines(tmargin, h - 700, para, MonoTypeface, 24, 40, textcolor);
	textlines(lmargin, h - 180, labels, SansTypeface, 48, 300, textcolor);
	//SaveRaw("tb.raw");
	End();
}

// cookie draws a cookie
void cookie(int w, int h) {
	int ew = 200, eh = 60, h2 = h / 2, w2 = w / 2;
	VGfloat gray[4] = { .5, .5, .5, 1 }, white[4] = {
	1, 1, 1, 1}, black[4] = {
	0, 0, 0, 1}, bgcolor[4] = {
	.75, .75, .75, 1};

	Start(w, h, bgcolor);
	setfill(gray);
	Ellipse(w2, h2, ew, eh);
	Translate(0, 10);

	setfill(white);
	Ellipse(w2, h2, ew, eh);
	Translate(0, 20);

	setfill(black);
	Ellipse(w2, h2, ew, eh);
	End();
}

// imagtest displays four JPEG images, centered on the display
void imagetest(int w, int h) {
	VGfloat bgcolor[4] = { 0, 0, 0, 1 };
	int imgw = 400, imgh = 400;
	VGfloat cx = (w / 2) - (imgw / 2), cy = (h / 2) - (imgh / 2);
	VGfloat ulx = 0, uly = h - imgh;
	VGfloat urx = w - imgw, ury = uly;
	VGfloat llx = 0, lly = 0;
	VGfloat lrx = urx, lry = lly;
	Start(w, h, bgcolor);
	Image(cx, cy, imgw, imgh, "test_img_violin.jpg");
	Image(ulx, uly, imgw, imgh, "test_img_piano.jpg");
	Image(urx, ury, imgw, imgh, "test_img_sax.jpg");
	Image(llx, lly, imgw, imgh, "test_img_guitar.jpg");
	Image(lrx, lry, imgw, imgh, "test_img_flute.jpg");
	//SaveRaw("image.raw");
	End();
}

// refcard shows a reference card of shapes
void refcard(int width, int height) {
	char *shapenames[] = {
		"Circle",
		"Ellipse",
		"Rectangle",
		"Rounded Rectangle",
		"Line",
		"Polyline",
		"Polygon",
		"Arc",
		"Quadratic Bezier",
		"Cubic Bezier",
		"Image"
	};
	VGfloat shapecolor[4], textcolor[4], bgcolor[4];
	RGB(202, 225, 255, shapecolor);
	RGB(0, 0, 0, textcolor);
	RGB(255, 255, 255, bgcolor);
	VGfloat top = height - 100, sx = 500, sy = top, sw = 100, sh = 45, dotsize = 7, spacing = 2.0;

	int i, ns = sizeof(shapenames) / sizeof(char *), fontsize = 36;
	Start(width, height, bgcolor);
	setfill(textcolor);
	sx = width * 0.10;
	textcolor[0] = 0.5;
	Text(width * .45, height / 2, "OpenVG on the Raspberry Pi", SansTypeface, 48, textcolor);
	textcolor[0] = 0;
	for (i = 0; i < ns; i++) {
		Text(sx + sw + sw / 2, sy, shapenames[i], SansTypeface, fontsize, textcolor);
		sy -= sh * spacing;
	}
	sy = top;
	VGfloat cx = sx + (sw / 2), ex = sx + sw;
	setfill(shapecolor);
	Circle(cx, sy, sw);
	coordpoint(cx, sy, dotsize, shapecolor);
	sy -= sh * spacing;
	Ellipse(cx, sy, sw, sh);
	coordpoint(cx, sy, dotsize, shapecolor);
	sy -= sh * spacing;
	Rect(sx, sy, sw, sh);
	coordpoint(sx, sy, dotsize, shapecolor);
	sy -= sh * spacing;
	Roundrect(sx, sy, sw, sh, 20, 20);
	coordpoint(sx, sy, dotsize, shapecolor);
	sy -= sh * spacing;

	strokeWidth(1);
	Stroke(204, 204, 204, 1);
	Line(sx, sy, ex, sy);
	coordpoint(sx, sy, dotsize, shapecolor);
	coordpoint(ex, sy, dotsize, shapecolor);
	sy -= sh;

	VGfloat px[5] = { sx, sx + (sw / 4), sx + (sw / 2), sx + ((sw * 3) / 4), sx + sw };
	VGfloat py[5] = { sy, sy - sh, sy, sy - sh, sy };

	Polyline(px, py, 5);
	coordpoint(px[0], py[0], dotsize, shapecolor);
	coordpoint(px[1], py[1], dotsize, shapecolor);
	coordpoint(px[2], py[2], dotsize, shapecolor);
	coordpoint(px[3], py[3], dotsize, shapecolor);
	coordpoint(px[4], py[4], dotsize, shapecolor);
	sy -= sh * spacing;

	py[0] = sy;
	py[1] = sy - sh;
	py[2] = sy - (sh / 2);
	py[3] = py[1] - (sh / 4);
	py[4] = sy;
	Polygon(px, py, 5);
	sy -= (sh * spacing) + sh;

	Arc(sx + (sw / 2), sy, sw, sh, 0, 180);
	coordpoint(sx + (sw / 2), sy, dotsize, shapecolor);
	sy -= sh * spacing;

	VGfloat cy = sy + (sh / 2), ey = sy;
	Qbezier(sx, sy, cx, cy, ex, ey);
	coordpoint(sx, sy, dotsize, shapecolor);
	coordpoint(cx, cy, dotsize, shapecolor);
	coordpoint(ex, ey, dotsize, shapecolor);
	sy -= sh * spacing;

	ey = sy;
	cy = sy + sh;
	Cbezier(sx, sy, cx, cy, cx, sy, ex, ey);
	coordpoint(sx, sy, dotsize, shapecolor);
	coordpoint(cx, cy, dotsize, shapecolor);
	coordpoint(cx, sy, dotsize, shapecolor);
	coordpoint(ex, ey, dotsize, shapecolor);

	sy -= (sh * spacing * 1.5);
	Image(sx, sy, 100, 100, "starx0.jpg");

	//SaveRaw("refcard.raw");
	End();
}

// rotext draws text, rotated around the center of the screen, progressively faded
void rotext(int w, int h, int n, char *s) {
	int i;
	VGfloat textcolor[4], bgcolor[4];
	VGfloat fade = (100.0 / (VGfloat) n) / 100.0;
	VGfloat deg = 360.0 / n;
	VGfloat x = w / 2, y = h / 2;

	RGBA(255, 255, 255, 1, textcolor);
	RGB(0, 0, 0, bgcolor);
	Start(w, h, bgcolor);
	Translate(x, y);
	int size = 256;
	for (i = 0; i < n; i++) {
		Text(0, 0, s, SerifTypeface, size, textcolor);
		textcolor[3] -= fade;
		size += n;
		Rotate(deg);
	}
	//  SaveRaw("rotext.raw");
	End();
}

// rseed seeds the random number generator from the random device
void rseed(void) {
	unsigned char d[sizeof(long int)];
	long int s;
	int fd;

	// read bytes from the random device,
	// pack them into a long int.
	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0) {
		srand48(1);
		return;
	}
	read(fd, (void *)d, (size_t) sizeof(long int));
	s = d[3] | (d[2] << 8) | (d[1] << 16) | (d[0] << 24);
	srand48(s);
	close(fd);
}

// rshapes draws shapes with random colors, strokes, and sizes. 
void rshapes(int width, int height, int n) {
	int np = 10;
	VGfloat bgcolor[4], textcolor[4];
	VGfloat sx, sy, cx, cy, px, py, ex, ey, pox, poy;
	VGfloat polyx[np], polyy[np];
	int i, j;
	RGB(255, 255, 255, bgcolor);
	RGB(128, 0, 0, textcolor);
	rseed();
	Start(width, height, bgcolor);
	for (i = 0; i < n; i++) {
		Fill(randcolor(), randcolor(), randcolor(), drand48());
		Ellipse(randf(width), randf(height), randf(200), randf(100));
		Circle(randf(width), randf(height), randf(100));
		Rect(randf(width), randf(height), randf(200), randf(100));
		Arc(randf(width), randf(height), randf(200), randf(200), randf(360), randf(360));

		sx = randf(width);
		sy = randf(height);
		Stroke(randcolor(), randcolor(), randcolor(), 1);
		strokeWidth(randf(5));
		Line(sx, sy, sx + randf(200), sy + randf(100));
		strokeWidth(0);

		sx = randf(width);
		sy = randf(height);
		ex = sx + randf(200);
		ey = sy;
		cx = sx + ((ex - sx) / 2.0);
		cy = sy + randf(100);
		Qbezier(sx, sy, cx, cy, ex, ey);

		sx = randf(width);
		sy = randf(height);
		ex = sx + randf(200);
		ey = sy;
		cx = sx + ((ex - sx) / 2.0);
		cy = sy + randf(100);
		px = cx;
		py = sy - randf(100);
		Cbezier(sx, sy, cx, cy, px, py, ex, ey);

		pox = randf(width);
		poy = randf(height);
		for (j = 0; j < np; j++) {
			polyx[j] = pox + randf(200);
			polyy[j] = poy + randf(100);
		}
		Polygon(polyx, polyy, np);

		pox = randf(width);
		poy = randf(height);
		for (j = 0; j < np; j++) {
			polyx[j] = pox + randf(200);
			polyy[j] = poy + randf(100);
		}
		Polyline(polyx, polyy, np);
	}
	Text(20, 20, "OpenVG on the Raspberry Pi", SansTypeface, 32, textcolor);
	//SaveRaw("rand.raw");
	End();
}

// play is a place for experimental code
void play(int w, int h) {

	VGfloat y = (6 * h) / 10;
	int fontsize = 84;
	char *s = "github.com/ajstarks/openvg";
	char *a = "ajstarks@gmail.com";
	int imw = 110, imh = 110;
	VGfloat tw = textwidth(s, SansTypeface, fontsize);
	VGfloat textcolor[4] = { 0.5, 0, 0, 1 }, bgcolor[4] = {
	1, 1, 1, 1};

	Start(w, h, bgcolor);
	Text(w / 2 - (tw / 2), y - (fontsize / 4), s, SansTypeface, fontsize, textcolor);
	y -= 150;
	tw = textwidth(a, SansTypeface, fontsize / 3);
	textcolor[1] = 0.5;
	textcolor[2] = 0.5;
	Text(w / 2 - (tw / 2), y, a, SansTypeface, fontsize / 3, textcolor);
	Image((w / 2) - (imw / 2), y - (imh * 2), imw, imh, "starx.jpg");
	End();
}

// main initializes the system and shows the picture. 
// Exit and clean up when you hit [RETURN].
int main(int argc, char **argv) {
	int w, h, nr;
	char *usage = "%s [command]\n\ttest ...\n\trand n\n\trotate n ...\n\timage\n\ttext\n";
	char *progname = argv[0];
	init(&w, &h);
	switch (argc) {
	case 2:
		if (strncmp(argv[1], "image", 5) == 0) {
			imagetest(w, h);
		} else if (strncmp(argv[1], "text", 4) == 0) {
			tb(w, h);
		} else if (strncmp(argv[1], "play", 5) == 0) {
			play(w, h);
		} else {
			fprintf(stderr, usage, progname);
			return 1;
		}
		break;
	case 3:
		if (strncmp(argv[1], "test", 4) == 0) {
			testpattern(w, h, argv[2]);
		} else if (strncmp(argv[1], "demo", 4) == 0) {
			nr = atoi(argv[2]);
			if (nr < 1 || nr > 30) {
				nr = 5;
			}
			refcard(w, h);
			sleep(nr);
			rshapes(w, h, 50);
			sleep(nr);
			testpattern(w, h, "abc");
			sleep(nr);
			imagetest(w, h);
			sleep(nr);
			rotext(w, h, 30, "Raspi");
			sleep(nr);
			tb(w, h);
			sleep(nr);
			play(w, h);
		} else if (strncmp(argv[1], "rand", 4) == 0) {
			nr = atoi(argv[2]);
			if (nr < 1 || nr > 1000) {
				nr = 100;
			}
			rshapes(w, h, nr);
		} else {
			fprintf(stderr, usage, progname);
			return 1;
		}
		break;

	case 4:
		if (strncmp(argv[1], "rotate", 6) == 0) {
			rotext(w, h, atoi(argv[2]), argv[3]);
		} else {
			fprintf(stderr, usage, progname);
			return 1;
		}
		break;

	default:
		refcard(w, h);
	}
	while (getchar() != '\n') {
		;
	}
	finish();
	return 0;
}
