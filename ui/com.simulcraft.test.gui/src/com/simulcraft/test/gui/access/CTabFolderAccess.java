package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.omnetpp.common.util.IPredicate;

public class CTabFolderAccess extends CompositeAccess {

	public CTabFolderAccess(CTabFolder tabfolder) {
		super(tabfolder);
	}

    @Override
	public CTabFolder getControl() {
		return (CTabFolder)super.getControl();
	}
    
    public CTabItemAccess[] getItems() {
    	return wrapItems(getControl().getItems());
    }
    
    public CTabItemAccess findItemByText(final String label) {
    	return wrapItem(
    			(CTabItem)findObject(
    						getControl().getItems(),
    						new IPredicate() {
    							public boolean matches(Object object) {
    								return ((CTabItem)object).getText().matches(label);
    							}
    						}));
    }
    
    public CTabItemAccess getItem(int index) {
		Assert.assertTrue(0 <= index && index < getControl().getItemCount());
		return wrapItem(getControl().getItem(index));
    }
    
    public CTabItemAccess getSelection() {
    	CTabItem selection = getControl().getSelection();
    	return selection == null ? null : wrapItem(selection);
    }
    
	public void selectItemWithMouseClick(String label) {
		findItemByText(label).reveal().click();
	}
	
	protected static CTabItemAccess[] wrapItems(CTabItem[] items) {
    	CTabItemAccess[] result = new CTabItemAccess[items.length];
    	for (int i = 0; i < result.length; ++i)
    		result[i] = wrapItem(items[i]);
    	return result;
	}
	
	protected static CTabItemAccess wrapItem(CTabItem item) {
		return (CTabItemAccess)createAccess(item);
	}
}
