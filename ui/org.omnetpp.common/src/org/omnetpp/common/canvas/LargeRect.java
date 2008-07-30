package org.omnetpp.common.canvas;

import org.eclipse.swt.graphics.Rectangle;

/**
 * A variant of SWT's Rectangle that stores coordinates in long.
 *
 * @author andras
 */
public final class LargeRect {
	public long x;
	public long y;
	public long width;
	public long height;

	/**
	 * Default constructor.
	 */
	public LargeRect () {
	}

	/**
	 * Construct a new instance of this class given the 
	 * x, y, width and height values.
	 */
	public LargeRect (long x, long y, long width, long height) {
	    setLocation(x, y);
	    setSize(width, height);
	}

	/**
	 * Construct a copy of the given rectangle.
	 */
	public LargeRect (LargeRect r) {
        this(r.x, r.y, r.width, r.height);
	}

	/**
	 * Construct a copy of the given rectangle.
	 */
	public LargeRect (Rectangle r) {
        this(r.x, r.y, r.width, r.height);
	}

    public LargeRect(org.eclipse.draw2d.geometry.Rectangle r) {
        this(r.x, r.y, r.width, r.height);
    }

    public void setLocation(long x, long y) {
        this.x = x;
        this.y = y;
    }

    public void setSize(long width, long height) {
        this.width = width;
        this.height = height;
    }

    public long left() {
        return x;
    }

	public long right() {
		return x + width;
	}

    public long top() {
        return y;
    }

	public long bottom() {
		return y + height;
	}

	/**
	 * Destructively replaces the x, y, width and height values
	 * in the receiver with ones which represent the union of the
	 * rectangles specified by the receiver and the given rectangle.
	 */
	public void add (LargeRect rect) {
		long left = x < rect.x ? x : rect.x;
		long top = y < rect.y ? y : rect.y;
		long lhs = x + width;
		long rhs = rect.x + rect.width;
		long right = lhs > rhs ? lhs : rhs;
		lhs = y + height;
		rhs = rect.y + rect.height;
		long bottom = lhs > rhs ? lhs : rhs;
		x = left;  y = top;  width = right - left;  height = bottom - top;
	}

	/**
	 * Returns <code>true</code> if the point specified by the
	 * arguments is inside the area specified by the receiver,
	 * and <code>false</code> otherwise.
	 */
	public boolean contains (long x, long y) {
		return (x >= this.x) && (y >= this.y) && ((x - this.x) < width) && ((y - this.y) < height);
	}

	/**
	 * Returns <code>true</code> if the given retangle is inside the area specified 
	 * by the receiver, and <code>false</code> otherwise.
	 */
	public boolean contains (LargeRect r) {
		return contains(r.x, r.y) && contains(r.x+r.width-1, r.y+r.height-1);
	}

	/**
	 * Compares the argument to the receiver, and returns true
	 * if they represent the <em>same</em> object using a class
	 * specific comparison.
	 */
	public boolean equals (Object object) {
		if (object == this) return true;
		if (!(object instanceof LargeRect)) return false;
		LargeRect r = (LargeRect)object;
		return (r.x == this.x) && (r.y == this.y) && (r.width == this.width) && (r.height == this.height);
	}

	/**
	 * Returns an integer hash code for the receiver. Any two 
	 * objects that return <code>true</code> when passed to 
	 * <code>equals</code> must return the same value for this
	 * method.
	 */
	public int hashCode () {
		return (int)(x ^ y ^ width ^ height);
	}

	/**
	 * Destructively replaces the x, y, width and height values
	 * in the receiver with ones which represent the intersection of the
	 * rectangles specified by the receiver and the given rectangle.
	 */
	public void intersect (LargeRect rect) {
		if (this == rect) return;
		long left = x > rect.x ? x : rect.x;
		long top = y > rect.y ? y : rect.y;
		long lhs = x + width;
		long rhs = rect.x + rect.width;
		long right = lhs < rhs ? lhs : rhs;
		lhs = y + height;
		rhs = rect.y + rect.height;
		long bottom = lhs < rhs ? lhs : rhs;
		x = right < left ? 0 : left;
		y = bottom < top ? 0 : top;
		width = right < left ? 0 : right - left;
		height = bottom < top ? 0 : bottom - top;
	}

	/**
	 * Returns a new rectangle which represents the intersection
	 * of the receiver and the given rectangle. 
	 * <p>
	 * The intersection of two rectangles is the rectangle that
	 * covers the area which is contained within both rectangles.
	 * </p>
	 */
	public LargeRect intersection (LargeRect rect) {
		if (this == rect) return new LargeRect (x, y, width, height);
		long left = x > rect.x ? x : rect.x;
		long top = y > rect.y ? y : rect.y;
		long lhs = x + width;
		long rhs = rect.x + rect.width;
		long right = lhs < rhs ? lhs : rhs;
		lhs = y + height;
		rhs = rect.y + rect.height;
		long bottom = lhs < rhs ? lhs : rhs;
		return new LargeRect (
				right < left ? 0 : left,
				bottom < top ? 0 : top,
				right < left ? 0 : right - left,
				bottom < top ? 0 : bottom - top);
	}

	/**
	 * Returns <code>true</code> if the rectangle described by the
	 * arguments intersects with the receiver and <code>false</code>
	 * otherwise.
	 * <p>
	 * Two rectangles intersect if the area of the rectangle
	 * representing their intersection is not empty.
	 * </p>
	 */
	public boolean intersects (long x, long y, long width, long height) {
		return (x < this.x + this.width) && (y < this.y + this.height) &&
		(x + width > this.x) && (y + height > this.y);
	}

	/**
	 * Returns <code>true</code> if the given rectangle intersects
	 * with the receiver and <code>false</code> otherwise.
	 * <p>
	 * Two rectangles intersect if the area of the rectangle
	 * representing their intersection is not empty.
	 * </p>
	 */
	public boolean intersects (LargeRect rect) {
		return rect == this || intersects (rect.x, rect.y, rect.width, rect.height);
	}

	/**
	 * Returns <code>true</code> if the receiver does not cover any
	 * area in the (x, y) coordinate plane, and <code>false</code> if
	 * the receiver does cover some area in the plane.
	 */
	public boolean isEmpty () {
		return (width <= 0) || (height <= 0);
	}

	/**
	 * Returns a string containing a concise, human-readable
	 * description of the receiver.
	 */
	public String toString () {
		return "LargeRect {" + x + ", " + y + ", " + width + ", " + height + "}"; //$NON-NLS-1$//$NON-NLS-2$ //$NON-NLS-3$ //$NON-NLS-4$ //$NON-NLS-5$
	}

	/**
	 * Returns a new rectangle which represents the union of
	 * the receiver and the given rectangle.
	 */
	public LargeRect union (LargeRect rect) {
		long left = x < rect.x ? x : rect.x;
		long top = y < rect.y ? y : rect.y;
		long lhs = x + width;
		long rhs = rect.x + rect.width;
		long right = lhs > rhs ? lhs : rhs;
		lhs = y + height;
		rhs = rect.y + rect.height;
		long bottom = lhs > rhs ? lhs : rhs;
		return new LargeRect (left, top, right - left, bottom - top);
	}

	/**
	 * Returns a new rectangle which represents the receiver minus the 
	 * given rectangle. Returns null if the difference is not a rectangular area.
	 * 
	 * @author andras
	 */
	public LargeRect minus (LargeRect rect) {
		LargeRect intersection = intersection(rect);
		if (intersection.height==height && intersection.x==x)
			return new LargeRect(intersection.right(), y, width-intersection.width, height);
		if (intersection.height==height && intersection.right()==right())
			return new LargeRect(x, y, width-intersection.width, height);
		if (intersection.width==width && intersection.y==y)
			return new LargeRect(x, intersection.bottom(), width, height-intersection.height);
		if (intersection.width==width && intersection.bottom()==bottom())
			return new LargeRect(x, y, width, height-intersection.height);
		return null;
	}

	/**
	 * Returns an array of new rectangles which represent the receiver minus the 
	 * given rectangle. This means at most 4 rectangles in the worst case (when
	 * rect is completely inside the receiver rect.) 
	 * 
	 * @author andras
	 */
	public LargeRect[] advancedMinus (LargeRect rect) {
		throw new RuntimeException("not implemented");
	}
}
