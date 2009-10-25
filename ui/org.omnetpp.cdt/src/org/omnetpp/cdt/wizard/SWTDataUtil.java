package org.omnetpp.cdt.wizard;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Collection;
import java.util.Date;

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

/**
 * Rudimentary data binding: get/set data from/to SWT controls
 * 
 * @author Andras
 */
public class SWTDataUtil {

	public static Object getValueFromControl(Control control) {
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
		if (control instanceof Spinner)
			return ((Spinner) control).getSelection(); // -> Integer 
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
		//TODO implement some adapterproviderfactoryvisitorproxy so that custom widgets can be supported
		return null;
	}

	/**
	 * May throw exception: NumberFormatException or ParseException (for date/time), etc 
	 */
	@SuppressWarnings("deprecation")
	public static void writeValueIntoControl(Control control, Object value) {
		if (control instanceof Button)
			((Button) control).setSelection(toBoolean(value));
		if (control instanceof CCombo)
			((CCombo) control).setText(toString(value));
		if (control instanceof Combo)
			((Combo) control).setText(toString(value));
		if (control instanceof DateTime) {
			Date d;
			try {
				d = new SimpleDateFormat("y-M-d H:m:s").parse(toString(value));
			} catch (ParseException e) {
				throw new RuntimeException(e);
			}
			((DateTime) control).setDate(d.getYear(), d.getMonth(), d.getDate());
			((DateTime) control).setTime(d.getHours(), d.getMinutes(), d.getSeconds());
		}
		if (control instanceof Label)
			((Label) control).setText(toString(value));
		if (control instanceof List)
			((List) control).setSelection(toStringList(value));
		if (control instanceof Link)
			((Link) control).setText(toString(value));
		if (control instanceof Scale)
			((Scale) control).setSelection(toInt(value));
		if (control instanceof Slider)
			((Slider) control).setSelection(toInt(value));
		if (control instanceof Spinner)
			((Spinner) control).setSelection(toInt(value));
		if (control instanceof StyledText)
			((StyledText) control).setText(toString(value));
		if (control instanceof Text) {
			((Text) control).setText(toString(value));
		}
		if (control instanceof Table) {
			//TODO table, checkbox table, tableItem..
		}
		if (control instanceof Tree) {
			//TODO tree, checkbox tree, treeItem, etc
		}
		//TODO implement some adapterproviderfactoryvisitorproxy so that custom widgets can be supported
	}

	private static boolean toBoolean(Object value) {
		return value.equals(true) || value.equals("true");
	}

	private static String toString(Object value) {
		return value.toString().trim();
	}

	/**
	 * May throw NumberFormatException.
	 */
	private static int toInt(Object value) {
		if (value instanceof Number)
			return ((Number) value).intValue();
		else 
			return Integer.parseInt(value.toString().trim());
	}

	@SuppressWarnings("unchecked")
	private static String[] toStringList(Object value) {
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
			return value.toString().split(" +");
		}
	}


}
