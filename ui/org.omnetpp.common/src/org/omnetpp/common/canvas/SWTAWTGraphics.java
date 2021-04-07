package org.omnetpp.common.canvas;

import java.awt.Graphics2D;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.PointList;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontMetrics;
import org.eclipse.swt.graphics.Image;

public class SWTAWTGraphics extends Graphics {
    private Graphics2D graphics2D;

    public SWTAWTGraphics(Graphics2D graphics2D) {
        this.graphics2D = graphics2D;
    }

    @Override
    public void clipRect(Rectangle r) {
        // TODO Auto-generated method stub

    }

    @Override
    public void dispose() {
        graphics2D.dispose();
    }

    @Override
    public void drawArc(int x, int y, int w, int h, int offset, int length) {
        // TODO Auto-generated method stub

    }

    @Override
    public void drawFocus(int x, int y, int w, int h) {
        // TODO Auto-generated method stub

    }

    @Override
    public void drawImage(Image srcImage, int x, int y) {
        // TODO Auto-generated method stub

    }

    @Override
    public void drawImage(Image srcImage, int x1, int y1, int w1, int h1,
            int x2, int y2, int w2, int h2) {
        // TODO Auto-generated method stub

    }

    @Override
    public void drawLine(int x1, int y1, int x2, int y2) {
        // TODO Auto-generated method stub

    }

    @Override
    public void drawOval(int x, int y, int w, int h) {
        // TODO Auto-generated method stub

    }

    @Override
    public void drawPolygon(PointList points) {
        // TODO Auto-generated method stub

    }

    @Override
    public void drawPolyline(PointList points) {
        // TODO Auto-generated method stub

    }

    @Override
    public void drawRectangle(int x, int y, int width, int height) {
        // TODO Auto-generated method stub

    }

    @Override
    public void drawRoundRectangle(Rectangle r, int arcWidth, int arcHeight) {
        // TODO Auto-generated method stub

    }

    @Override
    public void drawString(String s, int x, int y) {
        // TODO Auto-generated method stub

    }

    @Override
    public void drawText(String s, int x, int y) {
        // TODO Auto-generated method stub

    }

    @Override
    public void fillArc(int x, int y, int w, int h, int offset, int length) {
        // TODO Auto-generated method stub

    }

    @Override
    public void fillGradient(int x, int y, int w, int h, boolean vertical) {
        // TODO Auto-generated method stub

    }

    @Override
    public void fillOval(int x, int y, int w, int h) {
        // TODO Auto-generated method stub

    }

    @Override
    public void fillPolygon(PointList points) {
        // TODO Auto-generated method stub

    }

    @Override
    public void fillRectangle(int x, int y, int width, int height) {
        graphics2D.fillRect(x, y, width, height);
    }

    @Override
    public void fillRoundRectangle(Rectangle r, int arcWidth, int arcHeight) {
        // TODO Auto-generated method stub

    }

    @Override
    public void fillString(String s, int x, int y) {
        // TODO Auto-generated method stub

    }

    @Override
    public void fillText(String s, int x, int y) {
        // TODO Auto-generated method stub

    }

    @Override
    public Color getBackgroundColor() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public Rectangle getClip(Rectangle rect) {
        java.awt.Rectangle clip = graphics2D.getClipBounds();
        rect.x = clip.x;
        rect.y = clip.y;
        rect.width = clip.width;
        rect.height = clip.height;
        return rect;
    }

    @Override
    public Font getFont() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public FontMetrics getFontMetrics() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public Color getForegroundColor() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public int getLineStyle() {
        // TODO Auto-generated method stub
        return 0;
    }

    @Override
    public int getLineWidth() {
        // TODO Auto-generated method stub
        return 0;
    }

    @Override
    public float getLineWidthFloat() {
        // TODO Auto-generated method stub
        return 0;
    }

    @Override
    public boolean getXORMode() {
        // TODO Auto-generated method stub
        return false;
    }

    @Override
    public void popState() {
        // TODO Auto-generated method stub

    }

    @Override
    public void pushState() {
        // TODO Auto-generated method stub

    }

    @Override
    public void restoreState() {
        // TODO Auto-generated method stub

    }

    @Override
    public void scale(double amount) {
        // TODO Auto-generated method stub

    }

    @Override
    public void setBackgroundColor(Color rgb) {
        graphics2D.setBackground(new java.awt.Color(rgb.getRed(), rgb.getGreen(), rgb.getBlue()));
        graphics2D.setPaint(new java.awt.Color(rgb.getRed(), rgb.getGreen(), rgb.getBlue()));
    }

    @Override
    public void setClip(Rectangle r) {
        graphics2D.setClip(r.x, r.y, r.width, r.height);
    }

    @Override
    public void setFont(Font f) {
        // TODO Auto-generated method stub

    }

    @Override
    public void setForegroundColor(Color rgb) {
        // TODO Auto-generated method stub

    }

    @Override
    public void setLineStyle(int style) {
        // TODO Auto-generated method stub

    }

    @Override
    public void setLineWidth(int width) {
        // TODO Auto-generated method stub

    }

    @Override
    public void setLineWidthFloat(float width) {
        // TODO Auto-generated method stub

    }

    @Override
    public void setLineMiterLimit(float miterLimit) {
        // TODO Auto-generated method stub

    }

    @Override
    public void setXORMode(boolean b) {
        // TODO Auto-generated method stub

    }

    @Override
    public void translate(int dx, int dy) {
        graphics2D.translate(dx, dy);
    }
}
