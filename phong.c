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
#define	SPEED		10
/* 1 / ANGLE_STEP == 2 MUST BY INTEGER */
#define CIRCLE_SIZE	(360 * 2 * 180 * 2)
#define Ia		0.0

bool	pkeys[512];
coord_t	coords[CIRCLE_SIZE];
coord_t light;

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
phong(coord_t *c)
{
	float r, Idiff, Ispec;
	coord_t L, E, R, N;

	normalize(c, &N);

	sub(&light, c, &L);
	normalize(&L, &L);

	neg(c, &E);
	normalize(&E, &E);

	reflect(&L, &N, &R);
	neg(&R, &R);
	normalize(&R, &R);

	Idiff = clamp(fmax(dot(&N, &L), 0.0), 0.0, 1.0);

	Ispec = clamp(pow(fmax(dot(&R, &E), 0.0), 0.3), 0.0, 1.0);

	r = Ia + Idiff + Ispec;

	glColor3f(r, 0.0, 0.0);
}

static void
draw_scene(void)
{
	unsigned int i;

	for (i = 0; i < CIRCLE_SIZE; i++) {
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

	light.x = 0.0;
	light.y = 0.0;
	light.z = 120.0;

	printf("%i %i\n", i, CIRCLE_SIZE);
}

int
main(void)
{

	init_circle();
	init_window(600, 480, "gk", false);
	while (1) {
		SDL_Event ev;

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
			light.y += 1.0 * SPEED;
		if (pkeys[SDLK_DOWN])
			light.y -= 1.0 * SPEED;
		if (pkeys[SDLK_RIGHT])
			light.x += 1.0 * SPEED;
		if (pkeys[SDLK_LEFT])
			light.x -= 1.0 * SPEED;
		if (pkeys[SDLK_PAGEUP])
			light.z += 1.0 * SPEED;
		if (pkeys[SDLK_PAGEDOWN])
			light.z -= 1.0 * SPEED;

		SDL_GL_SwapBuffers();
	}

	/* NOT REACHED */
	return (0);
}
