package org.omnetpp.scave.editors.ui;

import org.apache.commons.lang.ObjectUtils;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

public class FileSelectionPanel extends Composite {
	
	public static final String PROP_FILENAME = "FileName";

	private String fileName;
	private String[] filterExtensions;
	private String labelText;
	private String dialogTitle;
	private int dialogStyle;	// SWT.OPEN / SWT.SAVE
	
	private Text fileNameText;
	private Button browseButton;
	
	private ListenerList listeners = new ListenerList();

	public FileSelectionPanel(Composite parent, int style,
								String label, int dialogStyle, String dialogTitle,
								String[] filterExtensions) {
		super(parent, style);
		this.labelText = label;
		this.dialogStyle = dialogStyle;
		this.dialogTitle = dialogTitle;
		this.filterExtensions = filterExtensions;
		initialize();
	}
	
	public String getFileName() {
		return fileName;
	}
	
	public void setFileName(String fileName) {
		fileNameText.setText(fileName);
		updateFileName(fileName);
	}
	
	protected void updateFileName(String fileName) {
		if (!ObjectUtils.equals(this.fileName, fileName)) {
			String oldFileName = this.fileName;
			this.fileName = fileName;
			firePropertyChange(PROP_FILENAME, oldFileName, fileName);
		}
	}
	
	private void initialize() {
		this.setLayout(new GridLayout(3, false));
		
		Label label = new Label(this, SWT.NONE);
		label.setText(labelText);
		fileNameText = new Text(this, SWT.SINGLE | SWT.BORDER);
		fileNameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		browseButton = new Button(this, SWT.NONE);
		browseButton.setText("Browse...");
		
		fileNameText.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				updateFileName(fileNameText.getText());
			}
		});
	
		browseButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				handleBrowseButtonPressed();
			}
		});
	}
	
	protected void handleBrowseButtonPressed() {
		FileDialog dialog = new FileDialog(getShell(), dialogStyle);
		dialog.setText(dialogTitle);
		dialog.setFilterPath(fileName);
		dialog.setFilterExtensions(filterExtensions);
		String selectedFileName = dialog.open();

		if (selectedFileName != null) {
			setFileName(selectedFileName);
		}
	}
	
	public void addPropertyChangeListener(IPropertyChangeListener listener) {
		listeners.add(listener);
	}
	
	public void removePropertyChangeListener(IPropertyChangeListener listener) {
		listeners.remove(listener);
	}
	
	protected void firePropertyChange(String property, Object oldValue, Object newValue) {
		PropertyChangeEvent event = new PropertyChangeEvent(this, property, oldValue, newValue);
		for (Object listener : listeners.getListeners()) {
			((IPropertyChangeListener)listener).propertyChange(event);
		}
	}
}
