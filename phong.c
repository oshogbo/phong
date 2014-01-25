/*-
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Michał Knieć <michalkniec91@gmail.com>
 * Copyright (c) 2014 Mariusz Zaborski <oshogbo@vexillium.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include <SDL/SDL.h>
#include <GL/gl.h>

#include "coord.h"

#define PIXEL_WIDTH	2.0
#define PIXEL_HEIGHT	PIXEL_WIDTH
#define ANGLE_STEP	0.5
#define RADIUS		100
#define SETTINGS_SPEED	0.01
/* 1 / ANGLE_STEP == 2 */
#define CIRCLE_SIZE	(360 * 2 * 180 * 2)
#define Ia		0.0

bool	pkeys[512];
bool	clampon, scale;
coord_t	coords[CIRCLE_SIZE];
coord_t scenecolor, ambientcolor, specularcolor, diffusecolor;
coord_t light;
coord_t *vcolor, rcolor, gcolor, bcolor, acolor;
void	(*addfunc)(coord_t*);
void	(*subfunc)(coord_t*);
void	(*infofunc)(void);
float	SPEED = 10;
float	Kspec = 0.3, Kamp = 0.25, Kdiff = 0.3, alpha = 200.0;

static void
init_window(int width, int height, const char *name, bool fs)
{
	Uint32 flags;

	flags = SDL_OPENGL;
	if (fs)
		flags |= SDL_FULLSCREEN;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetVideoMode(width, height, 0, flags);
	SDL_WM_SetCaption(name, NULL);

	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);

	glLoadIdentity();
	glOrtho(-width / 2, width / 2.0, - height / 2.0, height / 2.0, 1, -1);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_TEXTURE_2D);
}

static void
draw_pixel(float x, float y)
{

	glBegin(GL_QUADS);
	glVertex2f(x - PIXEL_WIDTH / 2, y - PIXEL_HEIGHT / 2);
	glVertex2f(x - PIXEL_WIDTH / 2, y + PIXEL_HEIGHT / 2);
	glVertex2f(x + PIXEL_WIDTH / 2, y + PIXEL_HEIGHT / 2);
	glVertex2f(x + PIXEL_WIDTH / 2, y - PIXEL_HEIGHT / 2);
	glEnd();
}

static void
info(const char *name, coord_t *c)
{

	printf("Changing: %s [%g %g %g]\n", name, c->x, c->y, c->z);
}

static void
sub_speed(coord_t __attribute__ ((unused)) *c)
{

	SPEED /= 10;
}

static void
add_speed(coord_t __attribute__ ((unused)) *c)
{

	SPEED *= 10;
}

static void
info_speed(void)
{

	printf("Changing: speed.\n");
}

static void
sub_kspec(coord_t __attribute__ ((unused)) *c)
{

	Kspec -= SETTINGS_SPEED * SPEED;
	if (Kspec < 0)
		Kspec = 0;
}

static void
add_kspec(coord_t __attribute__ ((unused)) *c)
{

	Kspec += SETTINGS_SPEED * SPEED;
}

static void
info_kspec(void)
{

	printf("Changing: kspec.\n");
}

static void
sub_kamp(coord_t __attribute__ ((unused)) *c)
{

	Kamp -= SETTINGS_SPEED * SPEED;
	if (Kamp < 0)
		Kamp = 0;
}

static void
add_kamp(coord_t __attribute__ ((unused)) *c)
{

	Kamp += SETTINGS_SPEED * SPEED;
}

static void
info_kamp(void)
{

	printf("Changing: kamp.\n");
}

static void
sub_kdiff(coord_t __attribute__ ((unused)) *c)
{

	Kdiff -= SETTINGS_SPEED * SPEED;
	if (Kdiff < 0)
		Kdiff = 0;
}

static void
add_kdiff(coord_t __attribute__ ((unused)) *c)
{

	Kdiff += SETTINGS_SPEED * SPEED;
}

static void
info_kdiff(void)
{

	printf("Changing kdiff.\n");
}

static void
add_alpha(coord_t __attribute__ ((unused)) *c)
{

	alpha += SETTINGS_SPEED * SPEED;
}

static void
sub_alpha(coord_t __attribute__ ((unused)) *c)
{

	alpha -= SETTINGS_SPEED * SPEED;
	if (alpha < 0)
		alpha = 0;
}

static void
info_alpha(void)
{

	printf("Changing alpha.\n");
}

static void
sub_scenecolor(coord_t *c)
{
	coord_t cspeed;

	mul_by_const(c, -SETTINGS_SPEED * SPEED, &cspeed);
	add(&scenecolor, &cspeed, &scenecolor);
}

static void
add_scenecolor(coord_t *c)
{
	coord_t cspeed;

	mul_by_const(c, SETTINGS_SPEED * SPEED, &cspeed);
	add(&scenecolor, &cspeed, &scenecolor);
}

static void
info_scenecolor(void)
{

	info("scenecolor", &scenecolor);
}

static void
sub_ambientcolor(coord_t *c)
{
	coord_t cspeed;

	mul_by_const(c, -SETTINGS_SPEED * SPEED, &cspeed);
	add(&ambientcolor, &cspeed, &ambientcolor);
}

static void
add_ambientcolor(coord_t *c)
{
	coord_t cspeed;

	mul_by_const(c, SETTINGS_SPEED * SPEED, &cspeed);
	add(&ambientcolor, &cspeed, &ambientcolor);
}

static void
info_ambientcolor(void)
{

	info("ambientcolor", &ambientcolor);
}

static void
sub_specularcolor(coord_t *c)
{
	coord_t cspeed;

	mul_by_const(c, -SETTINGS_SPEED * SPEED, &cspeed);
	add(&specularcolor, &cspeed, &specularcolor);
}

static void
add_specularcolor(coord_t *c)
{
	coord_t cspeed;

	mul_by_const(c, SETTINGS_SPEED * SPEED, &cspeed);
	add(&specularcolor, &cspeed, &specularcolor);
}

static void
info_specularcolor(void)
{

	info("specularcolor", &specularcolor);
}

static void
sub_diffiusecolor(coord_t *c)
{
	coord_t cspeed;

	mul_by_const(c, -SETTINGS_SPEED * SPEED, &cspeed);
	add(&diffusecolor, &cspeed, &diffusecolor);
}

static void
add_diffiusecolor(coord_t *c)
{
	coord_t cspeed;

	mul_by_const(c, SETTINGS_SPEED * SPEED, &cspeed);
	add(&diffusecolor, &cspeed, &diffusecolor);
}

static void
info_diffusecolor(void)
{

	info("diffusecolor", &diffusecolor);
}

static void
phong(coord_t *c)
{
	coord_t Idiff, Ispec, Iamb;
	coord_t L, E, R, N, result;
	float s;

	memcpy(&Iamb, &ambientcolor, sizeof(coord_t));

	normalize(c, &N);

	/* Count L vector */
	sub(&light, c, &L);
	normalize(&L, &L);

	/* Count E vector */
	neg(c, &E);
	normalize(&E, &E);

	/* Count R vector */
	reflect(&L, &N, &R);
	neg(&R, &R);
	normalize(&R, &R);

	mul_by_const(&diffusecolor, fmax(dot(&N, neg(&L, &L)), 0.0), &Idiff);
	mul_by_const(&specularcolor,
	    pow(fmax(dot(&R, &E), 0.0), alpha), &Ispec);
	if (clampon == true) {
		clamp_v(&Idiff, 0.0, 1.0, &Idiff);
		clamp_v(&Ispec, 0.0, 1.0, &Ispec);
	}

	mul_by_const(&Ispec, Kspec, &Ispec);
	mul_by_const(&Iamb, Kamp, &Iamb);
	mul_by_const(&Idiff, Kdiff, &Idiff);

	add(&Iamb, &Idiff, &result);
	add(&result, &Ispec, &result);
	add(&result, &scenecolor, &result);

	if (scale) {
		s = (Kspec + Kamp + Kdiff);
		glColor3f(result.x / (s + scenecolor.x),
		    result.y / (s + scenecolor.y),
		    result.z / (s + scenecolor.z));
	} else {
		glColor3f(result.x, result.y, result.z);
	}

}

static void
draw_scene(void)
{
	unsigned int i;

	for (i = 0; i < CIRCLE_SIZE; i++) {
		if (coords[i].z > 0.0)
			continue;
		phong(&coords[i]);
		draw_pixel(coords[i].x, coords[i].y);
	}
}

static void
init_circle(void)
{
	float a, b, rsb, rcb;
	unsigned int i;

	i = 0;
	for (b = 0; b < 180; b += ANGLE_STEP) {
		rsb = RADIUS * sinf(b * 180 / M_PI);
		rcb = RADIUS * cosf(b * 180 / M_PI);
		for (a = 0; a < 360; a += ANGLE_STEP) {
			coords[i].x = rsb * cosf(a * 180 / M_PI);
			coords[i].y = rsb * sinf(a * 180 / M_PI);
			coords[i].z = rcb;
			i++;
		}
	}

	light.x = -10.0;
	light.y = -10.0;
	light.z = 30.0;
	clampon = true;
	scale	= true;
	scenecolor.x = 0.0;
	scenecolor.y = 0.0;
	scenecolor.z = 0.0;

	diffusecolor.x = 0.1;
	diffusecolor.y = 0.1;
	diffusecolor.z = 0.1;

	specularcolor.x = 0.2;
	specularcolor.y = 0.2;
	specularcolor.z = 0.2;

	ambientcolor.x = 0.1;
	ambientcolor.y = 0.5;
	ambientcolor.z = 0.1;

	rcolor.x = 1.0;
	gcolor.y = 1.0;
	bcolor.z = 1.0;
	acolor.x = 1.0;
	acolor.y = 1.0;
	acolor.z = 1.0;
	vcolor = &acolor;

	printf("%i %i\n", i, CIRCLE_SIZE);
}

static void
help(void)
{

	printf("UP/DWON/LEFT/RIGHT/PgDn/PgUp - move light\n");
	printf("a - ambient color\n");
	printf("s - specular color\n");
	printf("d - diffuse color\n");
	printf("b - background color\n");
	printf("p - speed\n");
	printf("- - sb color\n");
	/* XXX: WTF plus dont work for me ??? */
	printf("0 - add color\n");
	printf("x - turn on clamp\n");
	printf("c - turn off clamp\n");
	printf("h - show this color\n");
	printf("1 - set changing red color\n");
	printf("2 - set changing green color\n");
	printf("3 - set changing blue color\n");
	printf("4 - set changing all color\n");
	printf("5 - scale mode on\n");
	printf("6 - scale mode off\n");
	printf("o - Kspec\n");
	printf("j - Kamp\n");
	printf("k - Kdiff\n");
	printf("l - alpha\n");
}

int
main(void)
{

	init_circle();
	init_window(600, 480, "phong", false);
	help();
	while (1) {
		SDL_Event ev;

		glClearColor(scenecolor.x, scenecolor.y, scenecolor.z, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		draw_scene();

		memset(&ev, 0, sizeof(ev));
		while(SDL_PollEvent(&ev))
		{
			switch(ev.type)
			{
				case SDL_KEYDOWN:
					pkeys[ev.key.keysym.sym] = true;
					break;
				case SDL_KEYUP:
					pkeys[ev.key.keysym.sym] = false;
					break;
				case SDL_QUIT:
					SDL_Quit();
					exit(1);
			}
		}

		if (pkeys[SDLK_UP])
			light.y -= 1.0 * SPEED;
		if (pkeys[SDLK_DOWN])
			light.y += 1.0 * SPEED;
		if (pkeys[SDLK_RIGHT])
			light.x -= 1.0 * SPEED;
		if (pkeys[SDLK_LEFT])
			light.x += 1.0 * SPEED;
		if (pkeys[SDLK_PAGEUP])
			light.z += 1.0 * SPEED;
		if (pkeys[SDLK_PAGEDOWN])
			light.z -= 1.0 * SPEED;
		if (pkeys[SDLK_i]) {
			printf("light: (%g %g %g); move speed = %g "
			    "seetings speed = %g\n"
			    "scale = %i, clamp = %i\n"
			    "Kspec = %g, Kamp = %g, Kdiff = %g, alpha = %g\n",
			    light.x, light.y, light.z, SPEED,
			    SETTINGS_SPEED * SPEED, scale, clampon,
			    Kspec, Kamp, Kdiff, alpha);
			printf("Changing color: ");
			if (vcolor == &rcolor)
				printf("red");
			else if (vcolor == &gcolor)
				printf("green");
			else if (vcolor == &bcolor)
				printf("blue");
			else if (vcolor == &acolor)
				printf("all");
			printf("\n");
			if (infofunc != NULL)
				infofunc();
		}
		if (pkeys[SDLK_a]) {
			addfunc = add_ambientcolor;
			subfunc = sub_ambientcolor;
			infofunc = info_ambientcolor;
		}
		if (pkeys[SDLK_b]) {
			addfunc = add_scenecolor;
			subfunc = sub_scenecolor;
			infofunc = info_scenecolor;
		}
		if (pkeys[SDLK_s]) {
			addfunc = add_specularcolor;
			subfunc = sub_specularcolor;
			infofunc = info_specularcolor;
		}
		if (pkeys[SDLK_p]) {
			addfunc = add_speed;
			subfunc = sub_speed;
			infofunc = info_speed;
		}
		if (pkeys[SDLK_o]) {
			addfunc = add_kspec;
			subfunc = sub_kspec;
			infofunc = info_kspec;
		}
		if (pkeys[SDLK_j]) {
			addfunc = add_kamp;
			subfunc = sub_kamp;
			infofunc = info_kamp;
		}
		if (pkeys[SDLK_k]) {
			addfunc = add_kdiff;
			subfunc = sub_kdiff;
			infofunc = info_kdiff;
		}
		if (pkeys[SDLK_l]) {
			addfunc = add_alpha;
			subfunc = sub_alpha;
			infofunc = info_alpha;
		}
		if (pkeys[SDLK_d]) {
			addfunc = add_diffiusecolor;
			subfunc = sub_diffiusecolor;
			infofunc = info_diffusecolor;
		}
		if (pkeys[SDLK_5])
			scale = true;
		if (pkeys[SDLK_6])
			scale = false;
		if (pkeys[SDLK_c])
			clampon = false;
		if (pkeys[SDLK_x])
			clampon = true;
		if (pkeys[SDLK_0] && addfunc != NULL)
			addfunc(vcolor);
		if (pkeys[SDLK_MINUS] && subfunc != NULL)
			subfunc(vcolor);
		if (pkeys[SDLK_h])
			help();
		if (pkeys[SDLK_1])
			vcolor = &rcolor;
		if (pkeys[SDLK_2])
			vcolor = &gcolor;
		if (pkeys[SDLK_3])
			vcolor = &bcolor;
		if (pkeys[SDLK_4])
			vcolor = &acolor;

		SDL_GL_SwapBuffers();
	}

	/* NOT REACHED */
	return (0);
}
