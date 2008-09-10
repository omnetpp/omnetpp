package org.omnetpp.common.util;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.PointList;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontMetrics;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.TextLayout;
import org.omnetpp.common.color.ColorFactory;

public class NullGraphics extends Graphics
{
    private Rectangle clipping;

    private Color foregroundColor = ColorFactory.BLACK;

    private Color backgroundColor = ColorFactory.WHITE;

    private FontMetrics fontMetrics = new TextLayout(null).getLineMetrics(0);

    @Override
    public void clipRect(Rectangle r) {
    }

    @Override
    public void dispose() {
    }

    @Override
    public void drawArc(int x, int y, int w, int h, int offset, int length) {
    }

    @Override
    public void drawFocus(int x, int y, int w, int h) {
    }

    @Override
    public void drawImage(Image srcImage, int x, int y) {
    }

    @Override
    public void drawImage(Image srcImage, int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    }

    @Override
    public void drawLine(int x1, int y1, int x2, int y2) {
    }

    @Override
    public void drawOval(int x, int y, int w, int h) {
    }

    @Override
    public void drawPolygon(PointList points) {
    }

    @Override
    public void drawPolyline(PointList points) {
    }

    @Override
    public void drawRectangle(int x, int y, int width, int height) {
    }

    @Override
    public void drawRoundRectangle(Rectangle r, int arcWidth, int arcHeight) {
    }

    @Override
    public void drawString(String s, int x, int y) {
    }

    @Override
    public void drawText(String s, int x, int y) {
    }

    @Override
    public void fillArc(int x, int y, int w, int h, int offset, int length) {
    }

    @Override
    public void fillGradient(int x, int y, int w, int h, boolean vertical) {
    }

    @Override
    public void fillOval(int x, int y, int w, int h) {
    }

    @Override
    public void fillPolygon(PointList points) {
    }

    @Override
    public void fillRectangle(int x, int y, int width, int height) {
    }

    @Override
    public void fillRoundRectangle(Rectangle r, int arcWidth, int arcHeight) {
    }

    @Override
    public void fillString(String s, int x, int y) {
    }

    @Override
    public void fillText(String s, int x, int y) {
    }

    @Override
    public Color getBackgroundColor() {
        return backgroundColor;
    }

    @Override
    public Color getForegroundColor() {
        return foregroundColor;
    }

    @Override
    public Rectangle getClip(Rectangle rect) {
        rect.setBounds(clipping);
        return rect;
    }

    @Override
    public Font getFont() {
        throw new UnsupportedOperationException();
    }

    @Override
    public FontMetrics getFontMetrics() {
        return fontMetrics ;
    }

    @Override
    public int getLineStyle() {
        throw new UnsupportedOperationException();
    }

    @Override
    public int getLineWidth() {
        throw new UnsupportedOperationException();
    }

    @Override
    public boolean getXORMode() {
        throw new UnsupportedOperationException();
    }

    @Override
    public void popState() {

    }

    @Override
    public void pushState() {

    }

    @Override
    public void restoreState() {

    }

    @Override
    public void scale(double amount) {
    }

    @Override
    public void setBackgroundColor(Color color) {
        this.backgroundColor = color;
    }

    @Override
    public void setForegroundColor(Color color) {
        this.foregroundColor = color;
    }

    @Override
    public void setClip(Rectangle r) {
        this.clipping = r;
    }

    @Override
    public void setFont(Font f) {
    }

    @Override
    public void setLineStyle(int style) {
    }

    @Override
    public void setLineWidth(int width) {
    }

    @Override
    public void setXORMode(boolean b) {
    }
    
    @Override
    public void setAlpha(int alpha) {
    }
    
    @Override
    public void setLineDash(int[] dash) {
    }

    @Override
    public void translate(int dx, int dy) {
    }
}
