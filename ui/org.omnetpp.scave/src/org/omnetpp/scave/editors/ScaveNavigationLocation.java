package org.omnetpp.scave.editors;

import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.INavigationLocation;
import org.eclipse.ui.NavigationLocation;
import org.eclipse.ui.XMLMemento;
import org.omnetpp.scave.editors.ui.ScaveEditorPage;

/**
 * INavigationLocation implementation for the ScaveEditor.
 * <p>
 * Instances store the identifier of the active page and
 * page specific state in IMemento form.
 *  
 * @author tomi
 */
public class ScaveNavigationLocation extends NavigationLocation {
	
	/* Tags used in the saved state (IMemento format) */
	public static final String TAG_TEXT = "Text";
	public static final String TAG_PAGE_ID = "PageID";
	public static final String TAG_PAGE_MEMENTO = "PageMemento";
	
	/** Timestamp of the creation of this location. */
	long timestamp;
	
	/** Text displayed on the gui. */
	String text;
	
	/** Identifier of the active page of the editor. */
	String pageId;
	
	/** State of the active page to be saved and restored. */
	IMemento pageMemento;
	
	public ScaveNavigationLocation(ScaveEditor editor, boolean empty) {
		super(editor);
		
		if (!empty) {
			ScaveEditorPage page = editor.getActiveEditorPage();
			timestamp = System.currentTimeMillis();
			text = editor.getTitle();
			if (page != null && page.getPageTitle() != null)
				text += " " + page.getPageTitle();
			if (page != null) {
				pageId = editor.getPageId(page);
				pageMemento = XMLMemento.createWriteRoot(TAG_PAGE_MEMENTO);
				page.saveState(pageMemento);
			}
		}
	}
	
	@Override
	public String getText() {
		return text;
	}

	/**
	 * Merge two navigation locations if they are on the same page and
	 * the time between them is less than 500ms.
	 */
	public boolean mergeInto(INavigationLocation currentLocation) {
		if (currentLocation instanceof ScaveNavigationLocation) {
			ScaveNavigationLocation other = (ScaveNavigationLocation)currentLocation;
			if (timestamp > 0 && other.timestamp > 0 && timestamp - other.timestamp < 500 &&
					pageId != null && pageId.equals(other.pageId)) {
				other.timestamp = timestamp;
				other.pageMemento = pageMemento;
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
				if (page != null && pageMemento != null) {
					page.restoreState(pageMemento);
				}
			}
		}
	}

	public void restoreState(IMemento memento) {
		text = memento.getString(TAG_TEXT);
		pageId = memento.getString(TAG_PAGE_ID);
		pageMemento = memento.getChild(TAG_PAGE_MEMENTO);
	}

	public void saveState(IMemento memento) {
		if (text != null)
			memento.putString(TAG_TEXT, text);
		if (pageId != null)
			memento.putString(TAG_PAGE_ID, pageId);
		if (pageMemento != null) {
			IMemento parentMemento = memento.createChild(TAG_PAGE_MEMENTO);
			parentMemento.putMemento(pageMemento);
		}
	}

	public void update() {
		// do nothing
	}
	
	protected ScaveEditor getScaveEditor() {
		IEditorPart editor = getEditorPart();
		return editor instanceof ScaveEditor ? (ScaveEditor)editor : null;
	}
}
