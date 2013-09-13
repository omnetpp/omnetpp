package org.omnetpp.common.image;

import org.eclipse.core.runtime.Assert;

public final class Padding {

    public static final Padding NULL = new Padding(0);

    public final int left;
    public final int top;
    public final int right;
    public final int bottom;

    public Padding(int padding) {
        this(padding, padding, padding, padding);
    }

    public Padding(int left, int top, int right, int bottom) {
        Assert.isLegal(left >= 0 && top >= 0 && right >= 0 && bottom >=0);
        this.left = left;
        this.top = top;
        this.right = right;
        this.bottom = bottom;
    }

    public int getWidth() {
        return left + right;
    }

    public int getHeight() {
        return top + bottom;
    }

    public boolean equals(Object o) {
        if (this == o)
            return true;
        if (!(o instanceof Padding))
            return false;

        Padding other = (Padding)o;
        return left == other.left &&
                top == other.top &&
                right == other.right &&
                bottom == other.bottom;
    }

    public int hashCode() {
        return left + top + right + bottom;
    }

    public String toString() {
        return "Padding("+left+","+top+","+right+","+bottom+")";
    }
}
