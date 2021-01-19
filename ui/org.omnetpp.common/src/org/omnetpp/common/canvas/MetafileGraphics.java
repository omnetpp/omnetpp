package org.omnetpp.common.canvas;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.PointList;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontMetrics;
import org.eclipse.swt.graphics.Image;

/**
 * A draw2d.Graphics subclass that records operations instead of painting,
 * and can replay them later to an actual Graphics.
 *
 * Restrictions:
 * - Getter operations are naturally not available.
 * - drawImage() remembers images by pointer, so do not modify an image after
 *   passing it to a drawImage() call. (Note that Rectangles and PointLists
 *   are copied, so they can be freely reused. Color and Font are already
 *   immutable.)
 * - not-abstract Graphics methods like setLineJoin() are missing due to oversight
 *
 * @author andras
 */
public class MetafileGraphics extends Graphics {
    public static final int ID_ClipRect = 0;
    public static final int ID_DrawArc = 1;
    public static final int ID_DrawFocus = 2;
    public static final int ID_DrawImage = 3;
    public static final int ID_DrawImage2 = 4;
    public static final int ID_DrawLine = 5;
    public static final int ID_DrawOval = 6;
    public static final int ID_DrawPolygon = 7;
    public static final int ID_DrawPolyline = 8;
    public static final int ID_DrawRectangle = 9;
    public static final int ID_DrawRoundRectangle = 10;
    public static final int ID_DrawString = 11;
    public static final int ID_DrawText = 12;
    public static final int ID_FillArc = 13;
    public static final int ID_FillGradient = 14;
    public static final int ID_FillOval = 15;
    public static final int ID_FillPolygon = 16;
    public static final int ID_FillRectangle = 17;
    public static final int ID_FillRoundRectangle = 18;
    public static final int ID_FillString = 19;
    public static final int ID_FillText = 20;
    public static final int ID_PopState = 21;
    public static final int ID_PushState = 22;
    public static final int ID_RestoreState = 23;
    public static final int ID_Scale = 24;
    public static final int ID_SetBackgroundColor = 25;
    public static final int ID_SetClip = 26;
    public static final int ID_SetFont = 27;
    public static final int ID_SetForegroundColor = 28;
    public static final int ID_SetLineStyle = 29;
    public static final int ID_SetLineWidth = 30;
    public static final int ID_SetLineWidthFloat = 31;
    public static final int ID_SetLineMiterLimit = 32;
    public static final int ID_SetXORMode = 33;
    public static final int ID_Translate = 34;

    public class Item { int code; }
    public class ClipRect extends Item { {code=ID_ClipRect;}  Rectangle r; }
    public class DrawArc extends Item { {code=ID_DrawArc;}  int x; int y; int w; int h; int offset; int length; }
    public class DrawFocus extends Item { {code=ID_DrawFocus;}  int x; int y; int w; int h; }
    public class DrawImage extends Item { {code=ID_DrawImage;}  Image srcImage; int x; int y; }
    public class DrawImage2 extends Item { Image srcImage; int x1; int y1; int w1; int h1; int x2; int y2; int w2; int h2; }
    public class DrawLine extends Item { {code=ID_DrawLine;} int x1; int y1; int x2; int y2; }
    public class DrawOval extends Item { {code=ID_DrawOval;} int x; int y; int w; int h; }
    public class DrawPolygon extends Item { {code=ID_DrawPolygon;} PointList points; }
    public class DrawPolyline extends Item { {code=ID_DrawPolyline;} PointList points; }
    public class DrawRectangle extends Item { {code=ID_DrawRectangle;} int x; int y; int width; int height; }
    public class DrawRoundRectangle extends Item { {code=ID_DrawRoundRectangle;} Rectangle r; int arcWidth; int arcHeight; }
    public class DrawString extends Item { {code=ID_DrawString;} String s; int x; int y; }
    public class DrawText extends Item { {code=ID_DrawText;} String s; int x; int y; }
    public class FillArc extends Item { {code=ID_FillArc;} int x; int y; int w; int h; int offset; int length; }
    public class FillGradient extends Item { {code=ID_FillGradient;} int x; int y; int w; int h; boolean vertical; }
    public class FillOval extends Item { {code=ID_FillOval;} int x; int y; int w; int h; }
    public class FillPolygon extends Item { {code=ID_FillPolygon;} PointList points; }
    public class FillRectangle extends Item { {code=ID_FillRectangle;} int x; int y; int width; int height; }
    public class FillRoundRectangle extends Item { {code=ID_FillRoundRectangle;} Rectangle r; int arcWidth; int arcHeight; }
    public class FillString extends Item { {code=ID_FillString;} String s; int x; int y; }
    public class FillText extends Item { {code=ID_FillText;} String s; int x; int y; }
    public class PopState extends Item { {code=ID_PopState;} };
    public class PushState extends Item { {code=ID_PushState;} };
    public class RestoreState extends Item { {code=ID_RestoreState;} };
    public class Scale extends Item { {code=ID_Scale;} double amount; }
    public class SetBackgroundColor extends Item { {code=ID_SetBackgroundColor;} Color rgb; }
    public class SetClip extends Item { {code=ID_SetClip;} Rectangle r; }
    public class SetFont extends Item { {code=ID_SetFont;} Font f; }
    public class SetForegroundColor extends Item { {code=ID_SetForegroundColor;} Color rgb; }
    public class SetLineStyle extends Item { {code=ID_SetLineStyle;} int style; }
    public class SetLineWidth extends Item { {code=ID_SetLineWidth;} int width; }
    public class SetLineWidthFloat extends Item { {code=ID_SetLineWidthFloat;} float width; }
    public class SetLineMiterLimit extends Item { {code=ID_SetLineMiterLimit;} float miterLimit; }
    public class SetXORMode extends Item { {code=ID_SetXORMode;} boolean b; }
    public class Translate extends Item { {code=ID_Translate;} int dx; int dy; }

    private List<Item> items = new ArrayList<>();

    @Override
    public void clipRect(Rectangle r) {
        ClipRect o  =  new ClipRect();
        o.r = r.getCopy();
        items.add(o);
    }

    @Override
    public void dispose() {
        items = null;
    }

    @Override
    public void drawArc(int x, int y, int w, int h, int offset, int length) {
        DrawArc o  =  new DrawArc();
        o.x = x; o.y = y; o.w = w; o.h = h; o.offset = offset; o.length = length;
        items.add(o);
    }

    @Override
    public void drawFocus(int x, int y, int w, int h) {
        DrawFocus o  =  new DrawFocus();
        o.x = x; o.y = y; o.w = w; o.h = h;
        items.add(o);
    }

    @Override
    public void drawImage(Image srcImage, int x, int y) {
        DrawImage o  =  new DrawImage();
        o.srcImage = srcImage; o.x = x; o.y = y;
        items.add(o);
    }

    @Override
    public void drawImage(Image srcImage, int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
        DrawImage2 o  =  new DrawImage2();
        o.srcImage = srcImage; o.x1 = x1; o.y1 = y1; o.w1 = w1; o.h1 = h1; o.x2 = x2; o.y2 = y2; o.w2 = w2; o.h2 = h2;
        items.add(o);
    }

    @Override
    public void drawLine(int x1, int y1, int x2, int y2) {
        DrawLine o  =  new DrawLine();
        o.x1 = x1; o.y1 = y1; o.x2 = x2; o.y2 = y2;
        items.add(o);
    }

    @Override
    public void drawOval(int x, int y, int w, int h) {
        DrawOval o  =  new DrawOval();
        o.x = x; o.y = y; o.w = w; o.h = h;
        items.add(o);
    }

    @Override
    public void drawPolygon(PointList points) {
        DrawPolygon o  =  new DrawPolygon();
        o.points = points.getCopy();
        items.add(o);
    }

    @Override
    public void drawPolyline(PointList points) {
        DrawPolyline o  =  new DrawPolyline();
        o.points = points.getCopy();
        items.add(o);
    }

    @Override
    public void drawRectangle(int x, int y, int width, int height) {
        DrawRectangle o  =  new DrawRectangle();
        o.x = x; o.y = y; o.width = width; o.height = height;
        items.add(o);
    }

    @Override
    public void drawRoundRectangle(Rectangle r, int arcWidth, int arcHeight) {
        DrawRoundRectangle o  =  new DrawRoundRectangle();
        o.r = r.getCopy(); o.arcWidth = arcWidth; o.arcHeight = arcHeight;
        items.add(o);
    }

    @Override
    public void drawString(String s, int x, int y) {
        DrawString o  =  new DrawString();
        o.s = s; o.x = x; o.y = y;
        items.add(o);
    }

    @Override
    public void drawText(String s, int x, int y) {
        DrawText o  =  new DrawText();
        o.s = s; o.x = x; o.y = y;
        items.add(o);
    }

    @Override
    public void fillArc(int x, int y, int w, int h, int offset, int length) {
        FillArc o  =  new FillArc();
        o.x = x; o.y = y; o.w = w; o.h = h; o.offset = offset; o.length = length;
        items.add(o);
    }

    @Override
    public void fillGradient(int x, int y, int w, int h, boolean vertical) {
        FillGradient o  =  new FillGradient();
        o.x = x; o.y = y; o.w = w; o.h = h; o.vertical = vertical;
        items.add(o);
    }

    @Override
    public void fillOval(int x, int y, int w, int h) {
        FillOval o  =  new FillOval();
        o.x = x; o.y = y; o.w = w; o.h = h;
        items.add(o);
    }

    @Override
    public void fillPolygon(PointList points) {
        FillPolygon o  =  new FillPolygon();
        o.points = points.getCopy();
        items.add(o);
    }

    @Override
    public void fillRectangle(int x, int y, int width, int height) {
        FillRectangle o  =  new FillRectangle();
        o.x = x; o.y = y; o.width = width; o.height = height;
        items.add(o);
    }

    @Override
    public void fillRoundRectangle(Rectangle r, int arcWidth, int arcHeight) {
        FillRoundRectangle o  =  new FillRoundRectangle();
        o.r = r.getCopy(); o.arcWidth = arcWidth; o.arcHeight = arcHeight;
        items.add(o);
    }

    @Override
    public void fillString(String s, int x, int y) {
        FillString o  =  new FillString();
        o.s = s; o.x = x; o.y = y;
        items.add(o);
    }

    @Override
    public void fillText(String s, int x, int y) {
        FillText o  =  new FillText();
        o.s = s; o.x = x; o.y = y;
        items.add(o);
    }

    @Override
    public Color getBackgroundColor() {
        throw new IllegalAccessError();
    }

    @Override
    public Rectangle getClip(Rectangle rect) {
        throw new IllegalAccessError();
    }

    @Override
    public Font getFont() {
        throw new IllegalAccessError();
    }

    @Override
    public FontMetrics getFontMetrics() {
        throw new IllegalAccessError();
    }

    @Override
    public Color getForegroundColor() {
        throw new IllegalAccessError();
    }

    @Override
    public int getLineStyle() {
        throw new IllegalAccessError();
    }

    @Override
    public int getLineWidth() {
        throw new IllegalAccessError();
    }

    @Override
    public float getLineWidthFloat() {
        throw new IllegalAccessError();
    }

    @Override
    public boolean getXORMode() {
        throw new IllegalAccessError();
    }

    @Override
    public void popState() {
        PopState o  =  new PopState();
        items.add(o);
    }

    @Override
    public void pushState() {
        PushState o  =  new PushState();
        items.add(o);
    }

    @Override
    public void restoreState() {
        RestoreState o  =  new RestoreState();
        items.add(o);
    }

    @Override
    public void scale(double amount) {
        Scale o  =  new Scale();
        o.amount = amount;
        items.add(o);
    }

    @Override
    public void setBackgroundColor(Color rgb) {
        SetBackgroundColor o  =  new SetBackgroundColor();
        o.rgb = rgb;
        items.add(o);
    }

    @Override
    public void setClip(Rectangle r) {
        SetClip o  =  new SetClip();
        o.r = r.getCopy();
        items.add(o);
    }

    @Override
    public void setFont(Font f) {
        SetFont o  =  new SetFont();
        o.f = f;
        items.add(o);
    }

    @Override
    public void setForegroundColor(Color rgb) {
        SetForegroundColor o  =  new SetForegroundColor();
        o.rgb = rgb;
        items.add(o);
    }

    @Override
    public void setLineStyle(int style) {
        SetLineStyle o  =  new SetLineStyle();
        o.style = style;
        items.add(o);
    }

    @Override
    public void setLineWidth(int width) {
        SetLineWidth o  =  new SetLineWidth();
        o.width = width;
    }

    @Override
    public void setLineWidthFloat(float width) {
        SetLineWidthFloat o  =  new SetLineWidthFloat();
        o.width = width;
        items.add(o);
    }

    @Override
    public void setLineMiterLimit(float miterLimit) {
        SetLineMiterLimit o  =  new SetLineMiterLimit();
        o.miterLimit = miterLimit;
        items.add(o);
    }

    @Override
    public void setXORMode(boolean b) {
        SetXORMode o  =  new SetXORMode();
        o.b = b;
        items.add(o);
    }

    @Override
    public void translate(int dx, int dy) {
        Translate o  =  new Translate();
        o.dx = dx; o.dy = dy;
        items.add(o);
    }

    public List<Item> getItems() {
        return items;
    }

    public void replay(Graphics graphics) {
        for (Item i: items) {
            switch(i.code) {
            case ID_ClipRect: { ClipRect o = (ClipRect)i; graphics.clipRect(o.r); break; }
            case ID_DrawArc: { DrawArc o = (DrawArc)i; graphics.drawArc(o.x, o.y, o.w, o.h, o.offset, o.length); break; }
            case ID_DrawFocus: { DrawFocus o = (DrawFocus)i; graphics.drawFocus(o.x, o.y, o.w, o.h); break; }
            case ID_DrawImage: { DrawImage o = (DrawImage)i; graphics.drawImage(o.srcImage, o.x, o.y); break; }
            case ID_DrawImage2: { DrawImage2 o = (DrawImage2)i; graphics.drawImage(o.srcImage, o.x1, o.y1, o.w1, o.h1, o.x2, o.y2, o.w2, o.h2); break; }
            case ID_DrawLine: { DrawLine o = (DrawLine)i; graphics.drawLine(o.x1, o.y1, o.x2, o.y2); break; }
            case ID_DrawOval: { DrawOval o = (DrawOval)i; graphics.drawOval(o.x, o.y, o.w, o.h); break; }
            case ID_DrawPolygon: { DrawPolygon o = (DrawPolygon)i; graphics.drawPolygon(o.points); break; }
            case ID_DrawPolyline: { DrawPolyline o = (DrawPolyline)i; graphics.drawPolyline(o.points); break; }
            case ID_DrawRectangle: { DrawRectangle o = (DrawRectangle)i; graphics.drawRectangle(o.x, o.y, o.width, o.height); break; }
            case ID_DrawRoundRectangle: { DrawRoundRectangle o = (DrawRoundRectangle)i; graphics.drawRoundRectangle(o.r, o.arcWidth, o.arcHeight); break; }
            case ID_DrawString: { DrawString o = (DrawString)i; graphics.drawString(o.s, o.x, o.y); break; }
            case ID_DrawText: { DrawText o = (DrawText)i; graphics.drawText(o.s, o.x, o.y); break; }
            case ID_FillArc: { FillArc o = (FillArc)i; graphics.fillArc(o.x, o.y, o.w, o.h, o.offset, o.length); break; }
            case ID_FillGradient: { FillGradient o = (FillGradient)i; graphics.fillGradient(o.x, o.y, o.w, o.h, o.vertical); break; }
            case ID_FillOval: { FillOval o = (FillOval)i; graphics.fillOval(o.x, o.y, o.w, o.h); break; }
            case ID_FillPolygon: { FillPolygon o = (FillPolygon)i; graphics.fillPolygon(o.points); break; }
            case ID_FillRectangle: { FillRectangle o = (FillRectangle)i; graphics.fillRectangle(o.x, o.y, o.width, o.height); break; }
            case ID_FillRoundRectangle: { FillRoundRectangle o = (FillRoundRectangle)i; graphics.fillRoundRectangle(o.r, o.arcWidth, o.arcHeight); break; }
            case ID_FillString: { FillString o = (FillString)i; graphics.fillString(o.s, o.x, o.y); break; }
            case ID_FillText: { FillText o = (FillText)i; graphics.fillText(o.s, o.x, o.y); break; }
            case ID_PopState: { graphics.popState(); break; }
            case ID_PushState: { graphics.pushState(); break; }
            case ID_RestoreState: { graphics.restoreState(); break; }
            case ID_Scale: { Scale o = (Scale)i; graphics.scale(o.amount); break; }
            case ID_SetBackgroundColor: { SetBackgroundColor o = (SetBackgroundColor)i; graphics.setBackgroundColor(o.rgb); break; }
            case ID_SetClip: { SetClip o = (SetClip)i; graphics.setClip(o.r); break; }
            case ID_SetFont: { SetFont o = (SetFont)i; graphics.setFont(o.f); break; }
            case ID_SetForegroundColor: { SetForegroundColor o = (SetForegroundColor)i; graphics.setForegroundColor(o.rgb); break; }
            case ID_SetLineStyle: { SetLineStyle o = (SetLineStyle)i; graphics.setLineStyle(o.style); break; }
            case ID_SetLineWidth: { SetLineWidth o = (SetLineWidth)i; graphics.setLineWidth(o.width); break; }
            case ID_SetLineWidthFloat: { SetLineWidthFloat o = (SetLineWidthFloat)i; graphics.setLineWidthFloat(o.width); break; }
            case ID_SetLineMiterLimit: { SetLineMiterLimit o = (SetLineMiterLimit)i; graphics.setLineMiterLimit(o.miterLimit); break; }
            case ID_SetXORMode: { SetXORMode o = (SetXORMode)i; graphics.setXORMode(o.b); break; }
            case ID_Translate: { Translate o = (Translate)i; graphics.translate(o.dx, o.dy); break; }
            default: throw new IllegalArgumentException("unrecognized item object: " + i.toString());
            }
        }

    }
}

