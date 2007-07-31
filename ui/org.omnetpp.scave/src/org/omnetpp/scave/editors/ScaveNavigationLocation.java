package org.omnetpp.scave.editors;

import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.INavigationLocation;
import org.eclipse.ui.NavigationLocation;
import org.eclipse.ui.XMLMemento;
import org.omnetpp.scave.editors.ui.ScaveEditorPage;

public class ScaveNavigationLocation extends NavigationLocation {
	
	private static final String TAG_PAGE_ID = "PageID";
	private static final String TAG_PAGE_TITLE = "PageTitle";
	private static final String TAG_PAGE_MEMENTO = "PageMemento";
	
	long timestamp;
	String pageId;
	String pageTitle;
	IMemento locationMemento;
	
	public ScaveNavigationLocation(ScaveEditor editor, boolean empty) {
		super(editor);
		
		if (!empty) {
			timestamp = System.currentTimeMillis();
			ScaveEditorPage page = editor.getActiveEditorPage();
			if (page != null) {
				pageId = editor.getPageId(page);
				pageTitle = page.getPageTitle();
				locationMemento = XMLMemento.createWriteRoot(TAG_PAGE_MEMENTO);
				page.saveState(locationMemento);
			}
		}
	}
	
	@Override
	public String getText() {
		IEditorPart editor = getEditorPart();
		StringBuffer sb = new StringBuffer();
		if (editor != null)
			sb.append(editor.getTitle());
		if (pageTitle != null) {
			sb.append(' ').append(pageTitle);
		}
		return sb.toString();
	}

	public boolean mergeInto(INavigationLocation currentLocation) {
		if (currentLocation instanceof ScaveNavigationLocation) {
			ScaveNavigationLocation other = (ScaveNavigationLocation)currentLocation;
			if (timestamp > 0 && other.timestamp > 0 && timestamp - other.timestamp < 500 &&
					pageId != null && pageId.equals(other.pageId)) {
				other.timestamp = timestamp;
				other.locationMemento = locationMemento;
				return true;
			}
		}
		return false;
	}

	public void restoreLocation() {
		ScaveEditor editor = getScaveEditor();
		if (editor != null) {
			if (pageId != null) {
				ScaveEditorPage page = editor.restorePage(pageId);
				if (page != null && locationMemento != null) {
					page.restoreState(locationMemento);
				}
			}
		}
	}

	public void restoreState(IMemento memento) {
		pageId = memento.getString(TAG_PAGE_ID);
		pageTitle = memento.getString(TAG_PAGE_TITLE);
		locationMemento = memento.getChild(TAG_PAGE_MEMENTO);
	}

	public void saveState(IMemento memento) {
		if (pageId != null)
			memento.putString(TAG_PAGE_ID, pageId);
		if (pageTitle != null)
			memento.putString(TAG_PAGE_TITLE, pageTitle);
		if (locationMemento != null) {
			IMemento pageMemento = memento.createChild(TAG_PAGE_MEMENTO);
			pageMemento.putMemento(locationMemento);
		}
	}

	public void update() {
	}
	
	protected ScaveEditor getScaveEditor() {
		IEditorPart editor = getEditorPart();
		return editor instanceof ScaveEditor ? (ScaveEditor)editor : null;
	}
}
