package org.omnetpp.figures.layout;

import java.util.ArrayList;
import java.util.List;

class Pt {
	public double x;

	public double y;
	
	public Pt(double x, double y) {
		this.x = x;
		this.y = y;
	}

	public Pt(Pt pt) {
		x = pt.x;
		y = pt.y;
	}
	
	static Pt newRadialPt(double radius, double angle) {
		return new Pt(Math.cos(angle) * radius, Math.sin(angle) * radius);
	}

	public Pt copy() {
		return new Pt(x, y);
	}

	public Pt assign(Pt pt) {
		x = pt.x;
		y = pt.y;

		return this;
	}

	public Pt assign(double x, double y) {
		this.x = x;
		this.y = y;
		
		return this;
	}
	
	public void setZero() {
		x = 0;
		y = 0;
	}

	public double getLength() {
		return Math.sqrt(x * x + y * y);
	}

	public double getNaNLength() {
		if (Double.isNaN(x))
			return y;
		else if (Double.isNaN(y))
			return x;
		else
			return getLength();
	}

	public double getDistance(Pt other) {
		double dx = x - other.x;
		double dy = y - other.y;

		return Math.sqrt(dx * dx + dy * dy);
	}
	
	public double getAngle() {
		return Math.atan2(y, x);
	}
	
	public void rotate(double rotateAngle) {
		double angle = getAngle();
		
		if (!Double.isNaN(angle)) {
			double length = getLength();
			angle += rotateAngle;
			x = Math.cos(angle) * length;
			y = Math.sin(angle) * length;
		}
	}

	public Pt normalize() {
		double length = getLength();
		x /= length;
		y /= length;

		return this;
	}

	public Pt add(Pt pt) {
		x += pt.x;
		y += pt.y;

		return this;
	}

	public Pt add(double x, double y) {
		this.x += x;
		this.y += y;

		return this;
	}

	public Pt subtract(Pt pt) {
		x -= pt.x;
		y -= pt.y;

		return this;
	}

	public Pt multiply(double d) {
		x *= d;
		y *= d;

		return this;
	}

	public Pt reverse() {
		x *= -1;
		y *= -1;

		return this;
	}

	public Pt divide(double d) {
		x /= d;
		y /= d;

		return this;
	}

	public double crossProduct(Pt pt) {
		return x * pt.y - y * pt.x;
	}

	public Pt transpose() {
		return new Pt(y, -x);
	}
	
	public void setNaNToZero() {
		if (Double.isNaN(x))
			x = 0;
		if (Double.isNaN(y))
			y = 0;
	}

	public boolean isZero() {
		return x == 0 && y == 0;
	}

	public static Pt newNil() {
		return new Pt(Double.NaN, Double.NaN);
	}

	public static Pt newZero() {
		return new Pt(0, 0);
	}

	public boolean isNil() {
		return Double.isNaN(x) && Double.isNaN(y);
	}

	public boolean isFullySpecified() {
		return !Double.isNaN(x) && !Double.isNaN(y);
	}

	@Override
	public int hashCode() {
		final int PRIME = 31;
		int result = 1;
		long temp;
		temp = Double.doubleToLongBits(x);
		result = PRIME * result + (int) (temp ^ (temp >>> 32));
		temp = Double.doubleToLongBits(y);
		result = PRIME * result + (int) (temp ^ (temp >>> 32));
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		final Pt other = (Pt) obj;
		if (Double.doubleToLongBits(x) != Double.doubleToLongBits(other.x))
			return false;
		if (Double.doubleToLongBits(y) != Double.doubleToLongBits(other.y))
			return false;
		return true;
	}

	@Override
	public String toString() {
		return "x: " + x + " y: " + y;
	}
}

class Ln {
	public Pt begin;

	public Pt end;

	public Ln(double x1, double y1, double x2, double y2) {
		this(new Pt(x1, y1), new Pt(x2, y2));
	}
	
	public Ln(Pt begin, Pt end) {
		this.begin = begin;
		this.end = end;
	}

	public Pt getClosestPoint(Pt pt) {
		Pt n = getDirectionVector().transpose();

		return intersect(new Ln(pt, n.add(pt)));
	}

	public Pt getDirectionVector() {
		Pt v = end.copy().subtract(begin);
		v.normalize();

		return v;
	}

	public Pt intersect(Ln ln) {
		double x1 = begin.x;
		double y1 = begin.y;
		double x2 = end.x;
		double y2 = end.y;
		double x3 = ln.begin.x;
		double y3 = ln.begin.y;
		double x4 = ln.end.x;
		double y4 = ln.end.y;
		double a = determinant(x1, y1, x2, y2);
		double b = determinant(x3, y3, x4, y4);
		double c = determinant(x1 - x2, y1 - y2, x3 - x4, y3 - y4);
		double x = determinant(a, x1 - x2, b, x3 - x4) / c;
		double y = determinant(a, y1 - y2, b, y3 - y4) / c;

		return new Pt(x, y);
	}

	private double determinant(double a, double b, double c, double d) {
		return a * d - b * c;
	}

	@Override
	public int hashCode() {
		final int PRIME = 31;
		int result = 1;
		result = PRIME * result + ((begin == null) ? 0 : begin.hashCode());
		result = PRIME * result + ((end == null) ? 0 : end.hashCode());
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		final Ln other = (Ln) obj;
		if (begin == null) {
			if (other.begin != null)
				return false;
		} else if (!begin.equals(other.begin))
			return false;
		if (end == null) {
			if (other.end != null)
				return false;
		} else if (!end.equals(other.end))
			return false;
		return true;
	}
}

class Rs {
	public double width;

	public double height;

	public Rs(double width, double height) {
		this.width = width;
		this.height = height;
	}

	public static Rs newNil() {
		return new Rs(Double.NaN, Double.NaN);
	}

	public boolean isNil() {
		return Double.isNaN(width) && Double.isNaN(height);
	}
	
	public double getDiagonalLength()
	{
		return Math.sqrt(width * width + height * height);
	}
	
	public double getArea()
	{
		return width * height;
	}

	@Override
	public int hashCode() {
		final int PRIME = 31;
		int result = 1;
		long temp;
		temp = Double.doubleToLongBits(height);
		result = PRIME * result + (int) (temp ^ (temp >>> 32));
		temp = Double.doubleToLongBits(width);
		result = PRIME * result + (int) (temp ^ (temp >>> 32));
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		final Rs other = (Rs) obj;
		if (Double.doubleToLongBits(height) != Double.doubleToLongBits(other.height))
			return false;
		if (Double.doubleToLongBits(width) != Double.doubleToLongBits(other.width))
			return false;
		return true;
	}

	@Override
	public String toString() {
		return "width: " + width + " height: " + height;
	}
}

class Rc {
	public Pt pt;

	public Rs rs;

	public Rc(double x, double y, double width, double height) {
		this.pt = new Pt(x, y);
		this.rs = new Rs(width, height);
	}

	public Rc(Pt pt, Rs rs) {
		this.pt = pt;
		this.rs = rs;
	}

	public void assign(Pt pt, Rs rs) {
		this.pt = pt;
		this.rs = rs;
	}

	public boolean intersects(Rc rc2) {
		return
			rc2.contains(getTopLeft()) ||
			rc2.contains(getTopRight()) ||
			rc2.contains(getBottomLeft()) ||
			rc2.contains(getBottomRight());
	}

	public boolean contains(Pt p) {
		return pt.x <= p.x && p.x <= pt.x + rs.width && pt.y <= p.y && p.y <= pt.y + rs.height;
	}
	
	public double getLeft() {
		return pt.x;
	}

	public double getRight() {
		return pt.x + rs.width;
	}
	
	public double getTop() {
		return pt.y;
	}

	public double getBottom() {
		return pt.y + rs.height;
	}
	public Pt getTopLeft() {
		return new Pt(pt.x, pt.y);
	}

	public Pt getTopRight() {
		return new Pt(pt.x + rs.width, pt.y);
	}

	public Pt getBottomLeft() {
		return new Pt(pt.x, pt.y + rs.height);
	}

	public Pt getBottomRight() {
		return new Pt(pt.x + rs.width, pt.y + rs.height);
	}

	public static Rc getNil() {
		return new Rc(Pt.newNil(), Rs.newNil());
	}

	public boolean isNil() {
		return pt.isNil() && rs.isNil();
	}

	@Override
	public int hashCode() {
		final int PRIME = 31;
		int result = 1;
		result = PRIME * result + ((pt == null) ? 0 : pt.hashCode());
		result = PRIME * result + ((rs == null) ? 0 : rs.hashCode());
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		final Rc other = (Rc) obj;
		if (pt == null) {
			if (other.pt != null)
				return false;
		} else if (!pt.equals(other.pt))
			return false;
		if (rs == null) {
			if (other.rs != null)
				return false;
		} else if (!rs.equals(other.rs))
			return false;
		return true;
	}
}

class Cc {
	public Pt origin;

	public double radius;
	
	public Cc(double x, double y, double radius) {
		this(new Pt(x, y), radius);
	}

	public Cc(Pt origin, double radius) {
		this.origin = origin;
		this.radius = radius;
	}
	
	public boolean contains(Pt pt) {
		return origin.getDistance(pt) <= radius;
	}
	
	public List<Pt> intersect(Cc other) {
		double R2 = radius * radius;
		double r2 = other.radius * other.radius;
		double d = origin.getDistance(other.origin);
		double d2 = d * d;
		double a = (d2 - r2 + R2);
		
		if (d2 == 0)
			return new ArrayList<Pt>();

		double y2 = (4 * d2 * R2 - a * a) / (4 * d2);
		
		if (y2 < 0)
			return new ArrayList<Pt>();
		
		double y = Math.sqrt(y2);
		double x = (d2 - r2 + R2) / (2 * d);
		Pt pt1 = new Pt(x, y);
		Pt pt2 = new Pt(x, -y);
		double angle = other.origin.copy().subtract(origin).getAngle();
		pt1.rotate(angle);
		pt2.rotate(angle);
		pt1.add(origin);
		pt2.add(origin);

		ArrayList<Pt> pts = new ArrayList<Pt>(); 
		pts.add(pt1);
		pts.add(pt2);

		return pts;
	}

	public static Cc getEnclosingCircle(List<Cc> circles) {
		Cc cc = circles.get(0);
		
		for (Cc ccOther : circles)
			cc = cc.getEnclosingCircle(ccOther);

		return cc;
	}
	
	public Cc getEnclosingCircle(Cc other) {
		double distance = origin.getDistance(other.origin);
		double d = distance + Math.max(radius, other.radius - distance) + Math.max(other.radius, radius - distance);
		Pt pt = new Pt(d / 2 - Math.max(radius, other.radius - distance), 0);
		double angle = other.origin.copy().subtract(origin).getAngle();
		pt.rotate(angle);
		pt.add(origin);

		return new Cc(pt, d / 2);
	}

	public Pt getCenterTop() {
		return new Pt(origin.x, origin.y - radius);
	}

	public Pt getCenterBottom() {
		return new Pt(origin.x, origin.y + radius);
	}

	public Pt getLeftCenter() {
		return new Pt(origin.x - radius, origin.y);
	}

	public Pt getRightCenter() {
		return new Pt(origin.x + radius, origin.y);
	}
}