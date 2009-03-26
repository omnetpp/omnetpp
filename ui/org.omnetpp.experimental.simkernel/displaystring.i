
%typemap(javainterfaces) cDisplayString "org.omnetpp.common.displaymodel.IDisplayString";

%typemap(javaimports) cDisplayString
  "import org.eclipse.draw2d.geometry.Dimension;\n"
  "import org.eclipse.draw2d.geometry.Point;\n"
  "import org.omnetpp.common.displaymodel.IDisplayString;\n";

%extend cDisplayString {
   /* A version of getTagArg() that returns NULL for missing values, not "". */
   const char *getTagArgNull(const char *tagname, int index) const {
       const char *s = self->getTagArg(tagname, index);
       return (!s || !*s) ? NULL : s;
   }
}

%typemap(javacode) cDisplayString %{
  public boolean containsProperty(Prop prop) {
    return getTagArgNull(prop.getTagName(), prop.getPos()) != null;
  }

  public boolean containsTag(Tag tagName) {
    return containsTag(tagName.name());
  }

  public float getAsFloat(Prop prop, float defaultValue) {
    try {
      String propValue = getTagArgNull(prop.getTagName(), prop.getPos());
      return propValue==null ? defaultValue : Float.valueOf(propValue);
    } catch (NumberFormatException e) { }
    return defaultValue;
  }

  public int getAsInt(Prop prop, int defaultValue) {
    try {
      String propValue = getTagArgNull(prop.getTagName(), prop.getPos());
      return propValue==null ? defaultValue : Integer.valueOf(propValue);
    } catch (NumberFormatException e) { }
    return defaultValue;
  }

  public String getAsString(Prop prop) {
    return getTagArg(prop.getTagName(), prop.getPos());
  }

  public Dimension getCompoundSize(Float scale) {
    int width = unit2pixel(getAsFloat(Prop.MODULE_WIDTH, -1.0f), scale);
    width = width > 0 ? width : -1;
    int height = unit2pixel(getAsFloat(Prop.MODULE_HEIGHT, -1.0f), scale);
    height = height > 0 ? height : -1;

    return new Dimension(width, height);
  }

  public Point getLocation(Float scale) {
    // return NaN to signal that the property is missing
    Float x = getAsFloat(Prop.X, Float.NaN);
    Float y = getAsFloat(Prop.Y, Float.NaN);
    // if it's unspecified in any direction, return a null constraint
    if (x.equals(Float.NaN) || y.equals(Float.NaN))
      return null;

    return new Point(unit2pixel(x, scale), unit2pixel(y, scale));
  }

  public int getRange(Float scale) {
    float floatValue = getAsFloat(IDisplayString.Prop.RANGE, -1.0f);
    return (floatValue <= 0) ? -1 : unit2pixel(floatValue, scale);
  }

  public float getScale() {
    return getAsFloat(Prop.MODULE_SCALE, 1.0f);
  }

  public Dimension getSize(Float scale) {
    int width = unit2pixel(getAsFloat(Prop.WIDTH, -1.0f), scale);
    width = width > 0 ? width : -1;
    int height = unit2pixel(getAsFloat(Prop.HEIGHT, -1.0f), scale);
    height = height > 0 ? height : -1;
    return new Dimension(width, height);
  }

  public final float pixel2unit(int pixel, Float overrideScale) {
    if (overrideScale != null)
      return pixel / overrideScale;
    return  pixel / getScale();
  }

  public final int unit2pixel(float unit, Float overrideScale) {
    if (overrideScale != null)
      return (int)(unit * overrideScale);
    return (int)(unit * getScale());
  }
%};


