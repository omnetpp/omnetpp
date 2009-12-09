package org.omnetpp.common.ui;


import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.preference.FieldEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.FocusAdapter;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.KeyAdapter;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

/**
 * MultiLineTextFieldEditor.
 * Field editor that is same as string field editor but
 * will have multi-line text field for user input.
 *
 * Note: copied from CDT (org.eclipse.cdt.ui.newui.MultiLineTextFieldEditor).
 * Except that the "width" constructor arg didn't work, removed; also changed
 * hardcoded width to 300. Still bogus, i.e. does not resize with the dialog.
 * --Andras
 */
public class MultiLineTextFieldEditor extends FieldEditor {

	/**
	 * Validation strategy constant (value <code>0</code>) indicating that
	 * the editor should perform validation after every key stroke.
	 *
	 * @see #setValidateStrategy
	 */
	public static final int VALIDATE_ON_KEY_STROKE = 0;

	/**
	 * Validation strategy constant (value <code>1</code>) indicating that
	 * the editor should perform validation only when the text widget
	 * loses focus.
	 *
	 * @see #setValidateStrategy
	 */
	public static final int VALIDATE_ON_FOCUS_LOST = 1;

	/**
	 * Text limit constant (value <code>-1</code>) indicating unlimited
	 * text limit and width.
	 */
	public static int UNLIMITED = -1;

	/**
	 * Cached valid state.
	 */
	private boolean isValid;

	/**
	 * Old text value.
	 */
	private String oldValue;
	private String compTitle;
	private Label title;

	/**
	 * The text field, or <code>null</code> if none.
	 */
	private Text textField;

	/**
	 * Text limit of text field in characters; initially unlimited.
	 */
	private int textLimit = UNLIMITED;

	/**
	 * The error message, or <code>null</code> if none.
	 */
	private String errorMessage;

	/**
	 * Indicates whether the empty string is legal;
	 * <code>true</code> by default.
	 */
	private boolean emptyStringAllowed = true;

	/**
	 * The validation strategy;
	 * <code>VALIDATE_ON_KEY_STROKE</code> by default.
	 */
	private int validateStrategy = VALIDATE_ON_KEY_STROKE;
	/**
	 * Creates a new string field editor
	 */
	protected MultiLineTextFieldEditor() {
	}
	/**
	 * Creates a string field editor.
	 * Use the method <code>setTextLimit</code> to limit the text.
	 *
	 * @param name the name of the preference this field editor works on
	 * @param labelText the label text of the field editor
	 * @param strategy either <code>VALIDATE_ON_KEY_STROKE</code> to perform
	 *  on the fly checking (the default), or <code>VALIDATE_ON_FOCUS_LOST</code> to
	 *  perform validation only after the text has been typed in
	 * @param parent the parent of the field editor's control
	 * @since 2.0
	 */
	public MultiLineTextFieldEditor(String name, String labelText, int strategy, Composite parent) {
		init(name, labelText);
		setValidateStrategy(strategy);
		isValid = false;
		errorMessage = "Validation error";
		createControl(parent);
	}

	/**
	 * Creates a string field editor of unlimited width.
	 * Use the method <code>setTextLimit</code> to limit the text.
	 *
	 * @param name the name of the preference this field editor works on
	 * @param labelText the label text of the field editor
	 * @param parent the parent of the field editor's control
	 */
	public MultiLineTextFieldEditor(String name, String labelText, Composite parent) {
		this(name, labelText, VALIDATE_ON_KEY_STROKE, parent);
	}

	/**
	 * Adjusts the horizontal span of this field editor's basic controls
	 * <p>
	 * Subclasses must implement this method to adjust the horizontal span
	 * of controls so they appear correct in the given number of columns.
	 * </p>
	 * <p>
	 * The number of columns will always be equal to or greater than the
	 * value returned by this editor's <code>getNumberOfControls</code> method.
	 *
	 * @param numColumns the number of columns
	 */
	@Override
	protected void adjustForNumColumns(int numColumns) {
		GridData gd = (GridData) textField.getLayoutData();
		gd.horizontalSpan = numColumns - 1;
		// We only grab excess space if we have to
		// If another field editor has more columns then
		// we assume it is setting the width.
		gd.grabExcessHorizontalSpace = gd.horizontalSpan == 1;
	}
	/**
	 * Checks whether the text input field contains a valid value or not.
	 *
	 * @return <code>true</code> if the field value is valid,
	 *   and <code>false</code> if invalid
	 */
	protected boolean checkState() {
		boolean result = false;
		if (emptyStringAllowed)
			result = true;

		if (textField == null)
			result = false;

		String txt = textField.getText();

		if (txt == null) {
			result = false;
		}
		else {
			result = (txt.trim().length() > 0) || emptyStringAllowed;
		}

		// call hook for subclasses
		result = result && doCheckState();

		if (result)
			clearErrorMessage();
		else
			showErrorMessage(errorMessage);

		return result;
	}
	/**
	 * Hook for subclasses to do specific state checks.
	 * <p>
	 * The default implementation of this framework method does
	 * nothing and returns <code>true</code>.  Subclasses should
	 * override this method to specific state checks.
	 * </p>
	 *
	 * @return <code>true</code> if the field value is valid,
	 *   and <code>false</code> if invalid
	 */
	protected boolean doCheckState() {
		return true;
	}
	/**
	 * Fills this field editor's basic controls into the given parent.
	 * <p>
	 * The string field implementation of this <code>FieldEditor</code>
	 * framework method contributes the text field. Subclasses may override
	 * but must call <code>super.doFillIntoGrid</code>.
	 * </p>
	 */
	@Override
	protected void doFillIntoGrid(Composite parent, int numColumns) {

		title = new Label(parent, SWT.UP);
		title.setFont(parent.getFont());
		this.compTitle = getLabelText();
		title.setText(this.compTitle);
		title.setLayoutData(new GridData(GridData.VERTICAL_ALIGN_BEGINNING));

		textField = getTextControl(parent);
		GridData gd = new GridData(GridData.FILL_HORIZONTAL);
		gd.heightHint = 80;
		gd.widthHint = 300;
		textField.setLayoutData(gd);

	}

	/**
	 * Initializes this field editor with the preference value from
	 * the preference store.
	 * <p>
	 * Subclasses must implement this method to properly initialize
	 * the field editor.
	 * </p>
	 */
	@Override
	protected void doLoad() {
		if (textField != null) {
			String value = getPreferenceStore().getString(getPreferenceName());
			textField.setText(value);
			oldValue = value;
		}
	}

	/**
	 * Initializes this field editor with the default preference value from
	 * the preference store.
	 * <p>
	 * Subclasses must implement this method to properly initialize
	 * the field editor.
	 * </p>
	 */
	@Override
	protected void doLoadDefault() {
		if (textField != null) {
			String value =
				getPreferenceStore().getDefaultString(getPreferenceName());
			textField.setText(value);
		}
		valueChanged();
	}

	/* (non-Javadoc)
	 * @see org.eclipse.jface.preference.FieldEditor#doStore()
	 */
	@Override
	protected void doStore() {
		getPreferenceStore().setValue(getPreferenceName(), textField.getText());
	}

	/**
	 * Returns the error message that will be displayed when and if
	 * an error occurs.
	 *
	 * @return the error message, or <code>null</code> if none
	 */
	public String getErrorMessage() {
		return errorMessage;
	}
	/**
		* Returns the number of basic controls this field editor consists of.
		*
		* @return the number of controls
		*/
	@Override
	public int getNumberOfControls() {
		return 2;
	}
	/**
	 * Returns the field editor's value.
	 *
	 * @return the current value
	 */
	public String getStringValue() {
		if (textField != null)
			return textField.getText();
		return getPreferenceStore().getString(getPreferenceName());
	}

	/**
	 * Returns this field editor's text control.
	 * @return the text control, or <code>null</code> if no
	 * text field is created yet
	 */
	protected Text getTextControl() {
		return textField;
	}

	/**
	 * Returns this field editor's text control.
	 * <p>
	 * The control is created if it does not yet exist
	 * </p>
	 *
	 * @param parent the parent
	 * @return the text control
	 */
	public Text getTextControl(Composite parent) {
		if (textField == null) {
			textField =
				new Text(
					parent,
					SWT.MULTI | SWT.V_SCROLL | SWT.BORDER | SWT.WRAP);
			textField.setFont(parent.getFont());
			switch (validateStrategy) {
				case VALIDATE_ON_KEY_STROKE :
					textField.addKeyListener(new KeyAdapter() {
						@Override
						public void keyPressed(KeyEvent e) {
							valueChanged();
						}
					});

					textField.addFocusListener(new FocusAdapter() {
						@Override
						public void focusGained(FocusEvent e) {
							refreshValidState();
						}
						@Override
						public void focusLost(FocusEvent e) {
							clearErrorMessage();
						}
					});
					break;
				case VALIDATE_ON_FOCUS_LOST :
					textField.addKeyListener(new KeyAdapter() {
						@Override
						public void keyPressed(KeyEvent e) {
							clearErrorMessage();
						}
					});
					textField.addFocusListener(new FocusAdapter() {
						@Override
						public void focusGained(FocusEvent e) {
							refreshValidState();
						}
						@Override
						public void focusLost(FocusEvent e) {
							valueChanged();
							clearErrorMessage();
						}
					});
					break;
				default :
			}
			textField.addDisposeListener(new DisposeListener() {
				public void widgetDisposed(DisposeEvent event) {
					textField = null;
				}
			});
			if (textLimit > 0) { //Only set limits above 0 - see SWT spec
				textField.setTextLimit(textLimit);
			}
		} else {
			checkParent(textField, parent);
		}
		return textField;
	}

	/**
	 * Returns whether an empty string is a valid value.
	 *
	 * @return <code>true</code> if an empty string is a valid value, and
	 *  <code>false</code> if an empty string is invalid
	 * @see #setEmptyStringAllowed
	 */
	public boolean isEmptyStringAllowed() {
		return emptyStringAllowed;
	}

	/**
	 * Returns whether this field editor contains a valid value.
	 * <p>
	 * The default implementation of this framework method
	 * returns <code>true</code>. Subclasses wishing to perform
	 * validation should override both this method and
	 * <code>refreshValidState</code>.
	 * </p>
	 *
	 * @return <code>true</code> if the field value is valid,
	 * and <code>false</code> if invalid
	 * @see #refreshValidState
	 */
	@Override
	public boolean isValid() {
		return isValid;
	}

	/**
	 * Refreshes this field editor's valid state after a value change
	 * and fires an <code>IS_VALID</code> property change event if
	 * warranted.
	 * <p>
	 * The default implementation of this framework method does
	 * nothing. Subclasses wishing to perform validation should override
	 * both this method and <code>isValid</code>.
	 * </p>
	 */
	@Override
	protected void refreshValidState() {
		isValid = checkState();
	}

	/**
	 * Sets whether the empty string is a valid value or not.
	 *
	 * @param b <code>true</code> if the empty string is allowed,
	 *  and <code>false</code> if it is considered invalid
	 */
	public void setEmptyStringAllowed(boolean b) {
		emptyStringAllowed = b;
	}

	/**
	 * Sets the error message that will be displayed when and if
	 * an error occurs.
	 *
	 * @param message the error message
	 */
	public void setErrorMessage(String message) {
		errorMessage = message;
	}

	/**
	 * Sets the focus to this field editor.
	 * <p>
	 * The default implementation of this framework method
	 * does nothing. Subclasses may reimplement.
	 * </p>
	*/
	@Override
	public void setFocus() {
		if (textField != null) {
			textField.setFocus();
		}
	}

	/**
	 * Sets this field editor's value.
	 *
	 * @param value the new value, or <code>null</code> meaning the empty string
	 */
	public void setStringValue(String value) {
		if (textField != null) {
			if (value == null)
				value = ""; //$NON-NLS-1$
			oldValue = textField.getText();
			if (!oldValue.equals(value)) {
				textField.setText(value);
				valueChanged();
			}
		}
	}

	/**
	 * Sets this text field's text limit.
	 *
	 * @param limit the limit on the number of character in the text
	 *  input field, or <code>UNLIMITED</code> for no limit
	 */
	public void setTextLimit(int limit) {
		textLimit = limit;
		if (textField != null)
			textField.setTextLimit(limit);
	}

	/**
	 * Sets the strategy for validating the text.
	 * <p>
	 * Calling this method has no effect after <code>createPartControl</code>
	 * is called. Thus this method is really only useful for subclasses to call
	 * in their constructor. However, it has public visibility for backward
	 * compatibility.
	 * </p>
	 *
	 * @param value either <code>VALIDATE_ON_KEY_STROKE</code> to perform
	 *  on the fly checking (the default), or <code>VALIDATE_ON_FOCUS_LOST</code> to
	 *  perform validation only after the text has been typed in
	 */
	public void setValidateStrategy(int value) {
		Assert.isTrue(value == VALIDATE_ON_FOCUS_LOST || value == VALIDATE_ON_KEY_STROKE);
		validateStrategy = value;
	}

	/**
	 * Shows the error message set via <code>setErrorMessage</code>.
	 */
	public void showErrorMessage() {
		showErrorMessage(errorMessage);
	}

	/**
	 * Informs this field editor's listener, if it has one, about a change
	 * to the value (<code>VALUE</code> property) provided that the old and
	 * new values are different.
	 * <p>
	 * This hook is <em>not</em> called when the text is initialized
	 * (or reset to the default value) from the preference store.
	 * </p>
	 */
	protected void valueChanged() {
		setPresentsDefaultValue(false);
		boolean oldState = isValid;
		refreshValidState();

		if (isValid != oldState)
			fireStateChanged(IS_VALID, oldState, isValid);

		String newValue = textField.getText();
		if (!newValue.equals(oldValue)) {
			fireValueChanged(VALUE, oldValue, newValue);
			oldValue = newValue;
		}
	}
}
