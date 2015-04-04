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

#include <math.h>
#include <stdio.h>

#include "coord.h"

coord_t*
normalize(const coord_t *in, coord_t *out)
{
	float s;

	if (in == NULL || out == NULL)
		return (NULL);

	s = sqrt(in->x * in->x + in->y * in->y + in->z * in->z);
	out->x = in->x / s;
	out->y = in->y / s;
	out->z = in->z / s;

	return (out);
}

coord_t*
sub(const coord_t *ain, const coord_t *bin, coord_t *out)
{

	if(ain == NULL || bin == NULL || out == NULL)
		return (NULL);

	out->x = ain->x - bin->x;
	out->y = ain->y - bin->y;
	out->z = ain->z - bin->z;

	return (out);
}

coord_t*
add(const coord_t *ain, const coord_t *bin, coord_t *out)
{

	if(ain == NULL || bin == NULL || out == NULL)
		return (NULL);

	out->x = ain->x + bin->x;
	out->y = ain->y + bin->y;
	out->z = ain->z + bin->z;

	return (out);
}

coord_t*
neg(const coord_t *in, coord_t *out)
{

	if (in == NULL || out == NULL)
		return (NULL);

	out->x = -in->x;
	out->y = -in->y;
	out->z = -in->z;

	return (out);
}

float
dot(const coord_t *ain, const coord_t *bin)
{

	if (ain == NULL || bin == NULL)
		return (0.0);

	return (ain->x * bin->x + ain->y * bin->y + ain->z * bin->z);
}

float
clamp(float a, float min, float max)
{

	if (a > max)
		a = max;
	else if (a < min)
		a = min;

	return (a);
}

coord_t*
clamp_v(const coord_t *in, float min, float max, coord_t *out)
{

	if (in == NULL || out == NULL)
		return (NULL);

	out->x = clamp(in->x, min, max);
	out->y = clamp(in->y, min, max);
	out->z = clamp(in->z, min, max);

	return (out);
}

/*static void
mul(const coord_t *va, const coord_t *vb, coord_t *out)
{
	float i, j, k;

	i = va->y * vb->z - va->z * vb->y;
	j = va->z * vb->x - va->x * vb->z;
	k = va->x * vb->y - av->y * vb->x;

	out->x = i;
	out->y = j;
	out->z = k;
}*/

coord_t*
mul_by_const(const coord_t *v, float a, coord_t *out)
{

	if (v == NULL || out == NULL)
		return (NULL);

	out->x = v->x * a;
	out->y = v->y * a;
	out->z = v->z * a;

	return (out);
}

coord_t*
add_const(const coord_t *v, float a, coord_t *out)
{

	if (v == NULL || out == NULL)
		return (NULL);

	out->x = v->x + a;
	out->y = v->y + a;
	out->z = v->z + a;

	return (out);
}

coord_t*
reflect(const coord_t *v, const coord_t *n, coord_t *out)
{
	float d;

	if (v == NULL || n == NULL || out == NULL)
		return (NULL);

	d = dot(v, n) * 2.0f;
	mul_by_const(n, d, out);
	sub(out, v, out);

	return (out);
}

