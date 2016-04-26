/*
 * Copyright (c) 2015-2016 Martin Mitas
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "misc.h"
#include "backend-d2d.h"
#include "backend-gdix.h"
#include "lock.h"


void
wdFillCircle(WD_HCANVAS hCanvas, WD_HBRUSH hBrush, float cx, float cy, float r)
{
    if(d2d_enabled()) {
        d2d_canvas_t* c = (d2d_canvas_t*) hCanvas;
        ID2D1Brush* b = (ID2D1Brush*) hBrush;
        D2D1_ELLIPSE e = { { cx, cy }, r, r };

        ID2D1RenderTarget_FillEllipse(c->target, &e, b);
    } else {
        gdix_canvas_t* c = (gdix_canvas_t*) hCanvas;
        float d = 2.0f * r;

        gdix_vtable->fn_FillEllipse(c->graphics, (void*) hBrush, cx - r, cy - r, d, d);
    }
}

void
wdFillPath(WD_HCANVAS hCanvas, WD_HBRUSH hBrush, const WD_HPATH hPath)
{
    if(d2d_enabled()) {
        d2d_canvas_t* c = (d2d_canvas_t*) hCanvas;
        ID2D1Geometry* g = (ID2D1Geometry*) hPath;
        ID2D1Brush* b = (ID2D1Brush*) hBrush;

        ID2D1RenderTarget_FillGeometry(c->target, g, b, NULL);
    } else {
        gdix_canvas_t* c = (gdix_canvas_t*) hCanvas;

        gdix_vtable->fn_FillPath(c->graphics, (void*) hBrush, (void*) hPath);
    }
}

void
wdFillPie(WD_HCANVAS hCanvas, WD_HBRUSH hBrush, float cx, float cy, float r,
          float fBaseAngle, float fSweepAngle)
{
    if(d2d_enabled()) {
        d2d_canvas_t* c = (d2d_canvas_t*) hCanvas;
        ID2D1Brush* b = (ID2D1Brush*) hBrush;
        ID2D1Geometry* g;

        g = d2d_create_arc_geometry(cx, cy, r, fBaseAngle, fSweepAngle, TRUE);
        if(g == NULL) {
            WD_TRACE("wdFillPie: d2d_create_arc_geometry() failed.");
            return;
        }

        ID2D1RenderTarget_FillGeometry(c->target, g, b, NULL);
        ID2D1Geometry_Release(g);
    } else {
        gdix_canvas_t* c = (gdix_canvas_t*) hCanvas;
        float d = 2.0f * r;

        gdix_vtable->fn_FillPie(c->graphics, (void*) hBrush,
                cx - r, cy - r, d, d, fBaseAngle, fSweepAngle);
    }
}

void
wdFillRect(WD_HCANVAS hCanvas, WD_HBRUSH hBrush,
           float x0, float y0, float x1, float y1)
{
    if(d2d_enabled()) {
        d2d_canvas_t* c = (d2d_canvas_t*) hCanvas;
        ID2D1Brush* b = (ID2D1Brush*) hBrush;
        D2D1_RECT_F r = { x0, y0, x1, y1 };

        ID2D1RenderTarget_FillRectangle(c->target, &r, b);
    } else {
        gdix_canvas_t* c = (gdix_canvas_t*) hCanvas;
        float tmp;

        /* Make sure x0 <= x1 and y0 <= y1. */
        if(x0 > x1) { tmp = x0; x0 = x1; x1 = tmp; }
        if(y0 > y1) { tmp = y0; y0 = y1; y1 = tmp; }

        gdix_vtable->fn_FillRectangle(c->graphics, (void*) hBrush,
                x0, y0, x1 - x0, y1 - y0);
    }
}
