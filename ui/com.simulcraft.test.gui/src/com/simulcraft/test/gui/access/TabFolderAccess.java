package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.ReflectionUtils;

public class TabFolderAccess extends CompositeAccess {

	public TabFolderAccess(TabFolder tabfolder) {
		super(tabfolder);
	}

    @Override
	public TabFolder getControl() {
		return (TabFolder)super.getControl();
	}
    
    public TabItemAccess[] getItems() {
    	return wrapItems(getControl().getItems());
    }
    
    public TabItemAccess getItem(final String label) {
    	return wrapItem(
    			(TabItem)findObject(
    						getControl().getItems(),
    						new IPredicate() {
    							public boolean matches(Object object) {
    								return ((TabItem)object).getText().matches(label);
    							}
    						}));
    }
    
    public TabItemAccess getItem(int index) {
		Assert.assertTrue(0 <= index && index < getControl().getItemCount());
		return wrapItem(getControl().getItem(index));
    }
    
    public TabItemAccess getSelection() {
    	TabItem[] selection = getControl().getSelection();
    	return selection.length == 0 ? null : wrapItem(selection[0]);
    }
    
    public boolean isSelected(String label) {
    	TabItem[] selection = getControl().getSelection();
    	return selection.length == 1 && selection[0].getText().matches(label);
    }
    
    public void setSelection(TabItemAccess item) {
    	setSelection(item.getWidget());
    }

	public void setSelection(TabItem item) {
    	setSelection(getControl().indexOf(item));
    }
	
	public void setSelection(String label) {
		setSelection(getItem(label));
	}
	
	public void setSelection(int index) {
		Assert.assertTrue(0 <= index && index < getControl().getItemCount());
    	ReflectionUtils.invokeMethod(getControl(), "setSelection", index, true);
	}
	
	public static TabItemAccess[] wrapItems(TabItem[] items) {
    	TabItemAccess[] result = new TabItemAccess[items.length];
    	for (int i = 0; i < result.length; ++i)
    		result[i] = wrapItem(items[i]);
    	return result;
	}
	
	public static TabItemAccess wrapItem(TabItem item) {
		return (TabItemAccess)createAccess(item);
	}
}
