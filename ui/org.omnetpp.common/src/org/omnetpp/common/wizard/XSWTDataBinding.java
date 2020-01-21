package org.omnetpp.common.wizard;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Collection;
import java.util.Date;
import java.util.LinkedHashMap;
import java.util.Map;

import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.DateTime;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Link;
import org.eclipse.swt.widgets.List;
import org.eclipse.swt.widgets.Scale;
import org.eclipse.swt.widgets.Slider;
import org.eclipse.swt.widgets.Spinner;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Tree;
import org.omnetpp.common.util.StringUtils;

/**
 * Rudimentary data binding: get/set data from/to SWT controls. Support for
 * the basic widgets is built in. Other widget classes can be supported via
 * IWidgetAdapter, which must be implemented either by the control class or
 * by a class with the similar name with the suffix "Adapter" (i.e. for
 * org.foo.FancyTable, the adapter would be looked for as org.foo.FancyTableAdapter.)
 *
 * @author Andras
 */
public class XSWTDataBinding {

    public static Object getValueFromControl(Control control, CreationContext context) {
        if (control instanceof Button)
            return ((Button) control).getSelection(); // -> Boolean
        if (control instanceof CCombo)
            return ((CCombo) control).getText();
        if (control instanceof Combo)
            return ((Combo) control).getText();
        if (control instanceof DateTime) {
            DateTime d = (DateTime) control;
            return d.getYear()+"-"+d.getMonth()+"-"+d.getDay()+" "+d.getHours()+":"+d.getMinutes()+":"+d.getSeconds();
        }
        if (control instanceof Label)
            return ((Label) control).getText(); // not very useful
        if (control instanceof List)
            return ((List) control).getSelection(); // -> String[]
        if (control instanceof Link)
            return ((Link) control).getText(); // not very useful
        if (control instanceof Scale)
            return ((Scale) control).getSelection(); // -> Integer
        if (control instanceof Slider)
            return ((Slider) control).getSelection(); // -> Integer
        if (control instanceof Spinner) {
            Spinner spinner = ((Spinner) control);
            if (spinner.getDigits() == 0)
                return spinner.getSelection(); // -> Integer
            else
                return spinner.getSelection() / Math.pow(10, spinner.getDigits()); // -> Double
        }
        if (control instanceof StyledText)
            return ((StyledText) control).getText();
        if (control instanceof Text)
            return ((Text) control).getText();
        if (control instanceof Table) {
            //TODO checkbox table?
            Table t = (Table)control;
            int rows = t.getItemCount();
            int cols = t.getColumnCount();
            if (cols == 1) {
                String[] data = new String[rows];
                TableItem[] items = t.getItems();
                for (int i=0; i<rows; i++)
                    data[i] = items[i].getText();
                return data; // -> String[]
            }
            else {
                String[][] data = new String[rows][cols];
                TableItem[] items = t.getItems();
                for (int i=0; i<rows; i++)
                    for (int j=0; j<cols; j++)
                        data[i][j] = items[i].getText(j);
                return data;  // -> String[][]
            }
            //TODO TableItem
        }
        if (control instanceof Tree) {
            //TODO tree, checkbox tree, treeItem, etc
        }

        Object adapter = getWidgetAdapter(control);
        if (adapter==null)
            throw new RuntimeException("No adapter for widget " + control.getClass().toString());
        if (adapter instanceof IWidgetAdapterExt)
            return ((IWidgetAdapterExt)adapter).getValue(context);
        else
            return ((IWidgetAdapter)adapter).getValue();
    }


    /**
     * May throw exception: NumberFormatException or ParseException (for date/time), etc
     */
    @SuppressWarnings("deprecation")
    public static void putValueIntoControl(Control control, Object value, CreationContext context) {
        if (control instanceof Button)
            ((Button) control).setSelection(toBoolean(value));
        else if (control instanceof CCombo)
            ((CCombo) control).setText(toString(value));
        else if (control instanceof Combo)
            ((Combo) control).setText(toString(value));
        else if (control instanceof DateTime) {
            Date d;
            try {
                d = new SimpleDateFormat("y-M-d H:m:s").parse(toString(value));
            } catch (ParseException e) {
                throw new RuntimeException(e);
            }
            ((DateTime) control).setDate(d.getYear(), d.getMonth(), d.getDate());
            ((DateTime) control).setTime(d.getHours(), d.getMinutes(), d.getSeconds());
        }
        else if (control instanceof Label)
            ((Label) control).setText(toString(value));
        else if (control instanceof List)
            ((List) control).setSelection(toStringArray(value));
        else if (control instanceof Link)
            ((Link) control).setText(toString(value));
        else if (control instanceof Scale)
            ((Scale) control).setSelection(toInt(value));
        else if (control instanceof Slider)
            ((Slider) control).setSelection(toInt(value));
        else if (control instanceof Spinner) {
            Spinner spinner = ((Spinner) control);
            if (spinner.getDigits() == 0)
                spinner.setSelection(toInt(value));
            else {
                double e = Math.pow(10, spinner.getDigits());
                spinner.setSelection((int)(toDouble(value) * e));
            }
        }
        else if (control instanceof StyledText) {
            ((StyledText) control).setText(toString(value));
            ((StyledText) control).selectAll();
        }
        else if (control instanceof Text) {
            ((Text) control).setText(toString(value));
            ((Text) control).selectAll();
        }
        else if (control instanceof Table) {
            //TODO table, checkbox table, tableItem..
        }
        else if (control instanceof Tree) {
            //TODO tree, checkbox tree, treeItem, etc
        }
        else {
            Object adapter = getWidgetAdapter(control);
            if (adapter==null)
                throw new RuntimeException("No adapter for widget " + control.getClass().toString());
            if (adapter instanceof IWidgetAdapterExt)
                ((IWidgetAdapterExt)adapter).setValue(value, context);
            else
                ((IWidgetAdapter)adapter).setValue(value);
        }
    }

    public static Object getWidgetAdapter(Control control) {
        Object adapter = null;
        if (control instanceof IWidgetAdapter || control instanceof IWidgetAdapterExt)
            adapter = control;
        if (adapter == null) {
            for (IWidgetAdapterFactory f : getAdapterFactories()) {
                adapter = f.getAdapterFor(control);
                if (adapter != null)
                    break;
            }
        }
        return adapter;
    }

    public static IWidgetAdapterFactory[] getAdapterFactories() {
        return new IWidgetAdapterFactory[0]; //TODO implement some extension mechanism
    }


    public static boolean toBoolean(Object value) {
        return value.equals(true) || value.toString().equals("true");
    }

    public static String toString(Object value) {
        return value.toString().trim();
    }

    /**
     * May throw NumberFormatException.
     */
    public static int toInt(Object value) {
        if (value instanceof Number)
            return ((Number) value).intValue();
        else
            return Integer.parseInt(value.toString().trim());
    }

    /**
     * May throw NumberFormatException.
     */
    public static double toDouble(Object value) {
        if (value instanceof Number)
            return ((Number) value).doubleValue();
        else
            return Double.parseDouble(value.toString().trim());
    }


    /**
     * Note: this method CANNOT HANDLE QUOTES. Use JSON parsing for anything serious.
     */
    public static String[] toStringArray(Object value) {
        return toStringArray(value, null);
    }


    /**
     * Note: this method CANNOT HANDLE QUOTES. Use JSON parsing for anything serious.
     */
    @SuppressWarnings("rawtypes")
    public static String[] toStringArray(Object value, String splitRegex) {
        if (value instanceof String[])
            return (String[])value;
        else if (value instanceof Object[] || value instanceof Collection) {
            Object[] v = (value instanceof Collection) ? ((Collection)value).toArray() : (Object[])value;
            String[] sv = new String[v.length];
            for (int i=0; i<v.length; i++)
                sv[i] = v[i].toString();
            return sv;
        }
        else {
            String str = value.toString().trim();
            if (str.length() == 0)
                return new String[0];
            else if (splitRegex != null)
                return str.split(splitRegex);
            else
                return new String[] { str };
        }
    }

    /**
     * Note: this method CANNOT HANDLE QUOTES. Use JSON parsing for anything serious.
     */
    @SuppressWarnings("rawtypes")
    public static Map<String,String> toStringMap(Object value) {
        if (value instanceof Map) {
            Map<String,String> result = new LinkedHashMap<String, String>();
            for (Object key : ((Map) value).keySet())
                result.put(key.toString(), value.toString());
            return result;
        }
        else {
            String[] items = toStringArray(value, " *, *");
            Map<String,String> result = new LinkedHashMap<String, String>(); // use LinkedHashMap to preserve order (important in some cases)
            for (int i=0; i<items.length; i++) {
                String key = StringUtils.substringBefore(items[i], ":").trim();
                String val = StringUtils.substringAfter(items[i], ":").trim();
                result.put(key, val);
            }
            return result;
        }
    }

}
