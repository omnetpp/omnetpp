package org.omnetpp.common.ui;

/*******************************************************************************
 * Copyright (c) 2000, 2007 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
import java.io.IOException;
import java.io.StringReader;
import java.util.Iterator;

import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.internal.text.html.HTML2TextReader;
import org.eclipse.jface.internal.text.html.HTMLPrinter;
import org.eclipse.jface.text.IInformationControl;
import org.eclipse.jface.text.IInformationControlExtension;
import org.eclipse.jface.text.IInformationControlExtension3;
import org.eclipse.jface.text.IInformationControlExtension4;
import org.eclipse.jface.text.TextPresentation;
import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTError;
import org.eclipse.swt.browser.Browser;
import org.eclipse.swt.browser.LocationAdapter;
import org.eclipse.swt.browser.LocationEvent;
import org.eclipse.swt.custom.StyleRange;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.graphics.TextLayout;
import org.eclipse.swt.graphics.TextStyle;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Shell;


// COPIED from org.eclipse.jface.internal.text.html.BrowserInformationControl
// removed only the some shell flags (look for CHANGE comments)
// must be synchronized with the platform if eclipse is updated
// SEE bug 23980 and 62652

/**
 * Displays textual information in a {@link org.eclipse.swt.browser.Browser} widget.
 * <p>
 * Moved into this package from <code>org.eclipse.jface.internal.text.revisions</code>.</p>
 * <p>
 * This class may be instantiated; it is not intended to be subclassed.</p>
 * <p>
 * Current problems:
 * <ul>
 * 	<li>the size computation is too small</li>
 * 	<li>focusLost event is not sent - see https://bugs.eclipse.org/bugs/show_bug.cgi?id=84532</li>
 * </ul>
 * </p>
 * 
 * @since 3.2
 */
public class BrowserInformationControl implements IInformationControl, IInformationControlExtension, IInformationControlExtension3, IInformationControlExtension4, DisposeListener {


	/**
	 * Tells whether the SWT Browser widget and hence this information
	 * control is available.
	 *
	 * @param parent the parent component used for checking or <code>null</code> if none
	 * @return <code>true</code> if this control is available
	 */
	public static boolean isAvailable(Composite parent) {
		if (!fgAvailabilityChecked) {
			try {
				Browser browser= new Browser(parent, SWT.NONE);
				browser.dispose();
				fgIsAvailable= true;
			} catch (SWTError er) {
				fgIsAvailable= false;
			} finally {
				fgAvailabilityChecked= true;
			}
		}

		return fgIsAvailable;
	}


	/** Border thickness in pixels. */
	private static final int BORDER= 1;
	
	/**
	 * Minimal size constraints.
	 * @since 3.2
	 */
	private static final int MIN_WIDTH= 80;
	private static final int MIN_HEIGHT= 80;

	
	/**
	 * Availability checking cache.
	 */
	private static boolean fgIsAvailable= false;
	private static boolean fgAvailabilityChecked= false;

	/** The control's shell */
	private Shell fShell;
	/** The control's browser widget */
	private Browser fBrowser;
	/** Tells whether the browser has content */
	private boolean fBrowserHasContent;
	/** The control width constraint */
	private int fMaxWidth= SWT.DEFAULT;
	/** The control height constraint */
	private int fMaxHeight= SWT.DEFAULT;
	private Font fStatusTextFont;
	private Label fStatusTextField;
	private String fStatusFieldText;
	private boolean fHideScrollBars;
	private Listener fDeactivateListener;
	private ListenerList fFocusListeners= new ListenerList();
	private Label fSeparator;
	private String fInputText;
	private TextLayout fTextLayout;

	private TextStyle fBoldStyle;

	/**
	 * Creates a default information control with the given shell as parent. The given
	 * information presenter is used to process the information to be displayed. The given
	 * styles are applied to the created styled text widget.
	 *
	 * @param parent the parent shell
	 * @param shellStyle the additional styles for the shell
	 * @param style the additional styles for the styled text widget
	 */
	public BrowserInformationControl(Shell parent, int shellStyle, int style) {
		this(parent, shellStyle, style, null);
	}

	/**
	 * Creates a default information control with the given shell as parent. The given
	 * information presenter is used to process the information to be displayed. The given
	 * styles are applied to the created styled text widget.
	 *
	 * @param parent the parent shell
	 * @param shellStyle the additional styles for the shell
	 * @param style the additional styles for the styled text widget
	 * @param statusFieldText the text to be used in the optional status field
	 *                         or <code>null</code> if the status field should be hidden
	 */
	public BrowserInformationControl(Shell parent, int shellStyle, int style, String statusFieldText) {
		fStatusFieldText= statusFieldText;
		
		// CHANGE removed SWT.ON_TOP so it is possible to create a resizable browser control on Linux too
		fShell= new Shell(parent, SWT.NO_FOCUS | shellStyle);
		Display display= fShell.getDisplay();
		fShell.setBackground(display.getSystemColor(SWT.COLOR_BLACK));
		fTextLayout= new TextLayout(display);

		Composite composite= fShell;
		GridLayout layout= new GridLayout(1, false);
		int border= ((shellStyle & SWT.NO_TRIM) == 0) ? 0 : BORDER;
		layout.marginHeight= border;
		layout.marginWidth= border;
		composite.setLayout(layout);
		
		if (statusFieldText != null) {
			composite= new Composite(composite, SWT.NONE);
			layout= new GridLayout(1, false);
			layout.marginHeight= 0;
			layout.marginWidth= 0;
			layout.verticalSpacing= 1;
			layout.horizontalSpacing= 1;
			composite.setLayout(layout);
			
			GridData  gd= new GridData(GridData.FILL_BOTH);
			composite.setLayoutData(gd);
			
			composite.setForeground(display.getSystemColor(SWT.COLOR_INFO_FOREGROUND));
			composite.setBackground(display.getSystemColor(SWT.COLOR_INFO_BACKGROUND));
		}

		// Browser field
		fBrowser= new Browser(composite, SWT.NONE);
		fHideScrollBars= (style & SWT.V_SCROLL) == 0 && (style & SWT.H_SCROLL) == 0;
		
		GridData gd= new GridData(GridData.BEGINNING | GridData.FILL_BOTH);
		fBrowser.setLayoutData(gd);
		
		fBrowser.setForeground(display.getSystemColor(SWT.COLOR_INFO_FOREGROUND));
		fBrowser.setBackground(display.getSystemColor(SWT.COLOR_INFO_BACKGROUND));
		fBrowser.addKeyListener(new KeyListener() {

			public void keyPressed(KeyEvent e)  {
				if (e.character == 0x1B) // ESC
					fShell.dispose();
			}

			public void keyReleased(KeyEvent e) {}
		});
		/*
		 * XXX revisit when the Browser support is better 
		 * See https://bugs.eclipse.org/bugs/show_bug.cgi?id=107629. Choosing a link to a
		 * non-available target will show an error dialog behind the ON_TOP shell that seemingly
		 * blocks the workbench. Disable links completely for now.
		 */
		fBrowser.addLocationListener(new LocationAdapter() {
			/*
			 * @see org.eclipse.swt.browser.LocationAdapter#changing(org.eclipse.swt.browser.LocationEvent)
			 */
			public void changing(LocationEvent event) {
				String location= event.location;
				/*
				 * Using the Browser.setText API triggers a location change to "about:blank" with
				 * the mozilla widget. The Browser on carbon uses yet another kind of special
				 * initialization URLs. 
				 * TODO remove this code once https://bugs.eclipse.org/bugs/show_bug.cgi?id=130314 is fixed
				 */
				if (!"about:blank".equals(location) && !("carbon".equals(SWT.getPlatform()) && location.startsWith("applewebdata:"))) //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$
					event.doit= false;
			}
		});

		// Replace browser's built-in context menu with none
		fBrowser.setMenu(new Menu(fShell, SWT.NONE));

		// Status field
		if (statusFieldText != null) {

			fSeparator= new Label(composite, SWT.SEPARATOR | SWT.HORIZONTAL | SWT.LINE_DOT);
			fSeparator.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));

			// Status field label
			fStatusTextField= new Label(composite, SWT.RIGHT);
			fStatusTextField.setText(statusFieldText);
			Font font= fStatusTextField.getFont();
			FontData[] fontDatas= font.getFontData();
			for (int i= 0; i < fontDatas.length; i++)
				fontDatas[i].setHeight(fontDatas[i].getHeight() * 9 / 10);
			fStatusTextFont= new Font(fStatusTextField.getDisplay(), fontDatas);
			fStatusTextField.setFont(fStatusTextFont);
			gd= new GridData(GridData.FILL_HORIZONTAL | GridData.HORIZONTAL_ALIGN_BEGINNING | GridData.VERTICAL_ALIGN_BEGINNING);
			fStatusTextField.setLayoutData(gd);

			fStatusTextField.setForeground(display.getSystemColor(SWT.COLOR_WIDGET_DARK_SHADOW));

			fStatusTextField.setBackground(display.getSystemColor(SWT.COLOR_INFO_BACKGROUND));
		}

		addDisposeListener(this);		
		createTextLayout();
	}

	/**
	 * Creates a default information control with the given shell as parent. The given
	 * information presenter is used to process the information to be displayed. The given
	 * styles are applied to the created styled text widget.
	 *
	 * @param parent the parent shell
	 * @param style the additional styles for the browser widget
	 */
	public BrowserInformationControl(Shell parent,int style) {
		this(parent, SWT.TOOL | SWT.NO_TRIM, style);
	}

	/**
	 * Creates a default information control with the given shell as parent.
	 * No information presenter is used to process the information
	 * to be displayed. No additional styles are applied to the styled text widget.
	 *
	 * @param parent the parent shell
	 */
	public BrowserInformationControl(Shell parent) {
		this(parent, SWT.NONE);
	}


	/*
	 * @see IInformationControl#setInformation(String)
	 */
	public void setInformation(String content) {
		fBrowserHasContent= content != null && content.length() > 0;

		if (!fBrowserHasContent)
			content= "<html><body ></html>"; //$NON-NLS-1$
		
		fInputText= content;

		int shellStyle= fShell.getStyle();
		boolean RTL= (shellStyle & SWT.RIGHT_TO_LEFT) != 0;

		String[] styles= null;
		if (RTL && !fHideScrollBars)
			styles= new String[] { "direction:rtl;", "word-wrap:break-word;" }; //$NON-NLS-1$ //$NON-NLS-2$
		else if (RTL && fHideScrollBars)
			styles= new String[] { "direction:rtl;", "overflow:hidden;", "word-wrap:break-word;" }; //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$
		else if (fHideScrollBars && true)
			styles= new String[] { "overflow:hidden;", "word-wrap: break-word;" }; //$NON-NLS-1$ //$NON-NLS-2$
		
		if (styles != null) {
			StringBuffer buffer= new StringBuffer(content);
			HTMLPrinter.insertStyles(buffer, styles);
			content= buffer.toString();
		}
		
		fBrowser.setText(content);
	
	}

	/*
	 * @see org.eclipse.jdt.internal.ui.text.IInformationControlExtension4#setStatusText(java.lang.String)
	 * @since 3.2
	 */
	public void setStatusText(String statusFieldText) {
		fStatusFieldText= statusFieldText;
	}

	/*
	 * @see IInformationControl#setVisible(boolean)
	 */
	public void setVisible(boolean visible) {
		if (fShell.isVisible() == visible)
			return;
		
		if (visible) {
			if (fStatusTextField != null) {
				boolean state= fStatusFieldText != null;
				if (state)
					fStatusTextField.setText(fStatusFieldText);
				fStatusTextField.setVisible(state);
				fSeparator.setVisible(state);
			}
		}

		fShell.setVisible(visible);
		if (!visible)
			setInformation(""); //$NON-NLS-1$
	}

	/**
	 * Creates and initializes the text layout used
	 * to compute the size hint.
	 * 
	 * @since 3.2
	 */
	private void createTextLayout() {
		fTextLayout= new TextLayout(fBrowser.getDisplay());
		
		// Initialize fonts
		Font font= fBrowser.getFont();
		fTextLayout.setFont(font);
		fTextLayout.setWidth(-1);
		FontData[] fontData= font.getFontData();
		for (int i= 0; i < fontData.length; i++)
			fontData[i].setStyle(SWT.BOLD);
		font= new Font(fShell.getDisplay(), fontData);
		fBoldStyle= new TextStyle(font, null, null);
		
		// Compute and set tab width
		fTextLayout.setText("    "); //$NON-NLS-1$
		int tabWidth = fTextLayout.getBounds().width;
		fTextLayout.setTabs(new int[] {tabWidth});

		fTextLayout.setText(""); //$NON-NLS-1$
	}

	/*
	 * @see IInformationControl#dispose()
	 */
	public void dispose() {
		fTextLayout.dispose();
		fTextLayout= null;
		fBoldStyle.font.dispose();
		fBoldStyle= null;
		if (fShell != null && !fShell.isDisposed())
			fShell.dispose();
		else
			widgetDisposed(null);
	}

	/*
	 * @see org.eclipse.swt.events.DisposeListener#widgetDisposed(org.eclipse.swt.events.DisposeEvent)
	 */
	public void widgetDisposed(DisposeEvent event) {
		if (fStatusTextFont != null && !fStatusTextFont.isDisposed())
			fStatusTextFont.dispose();

		fShell= null;
		fBrowser= null;
		fStatusTextFont= null;
	}

	/*
	 * @see IInformationControl#setSize(int, int)
	 */
	public void setSize(int width, int height) {
		fShell.setSize(Math.min(width, fMaxWidth), Math.min(height, fMaxHeight));
	}

	/*
	 * @see IInformationControl#setLocation(Point)
	 */
	public void setLocation(Point location) {
		fShell.setLocation(location);
	}

	/*
	 * @see IInformationControl#setSizeConstraints(int, int)
	 */
	public void setSizeConstraints(int maxWidth, int maxHeight) {
		fMaxWidth= maxWidth;
		fMaxHeight= maxHeight;
	}

	/*
	 * @see IInformationControl#computeSizeHint()
	 */
	public Point computeSizeHint() {
		TextPresentation presentation= new TextPresentation();
		HTML2TextReader reader= new HTML2TextReader(new StringReader(fInputText), presentation);
		String text;
		try {
			text= reader.getString();
		} catch (IOException e) {
			text= ""; //$NON-NLS-1$
		}

		fTextLayout.setText(text);
		Iterator iter= presentation.getAllStyleRangeIterator();
		while (iter.hasNext()) {
			StyleRange sr= (StyleRange)iter.next();
			if (sr.fontStyle == SWT.BOLD)
				fTextLayout.setStyle(fBoldStyle, sr.start, sr.start + sr.length - 1);
		}
		Rectangle bounds= fTextLayout.getBounds();
		int width= bounds.width;
		int height= bounds.height;
		
		width += 15;
		// CHANGE added the title bar height too
		height += 25 + 20; 

		if (fStatusFieldText != null && fSeparator != null) {
			fTextLayout.setText(fStatusFieldText);
			Rectangle statusBounds= fTextLayout.getBounds();
			Rectangle separatorBounds= fSeparator.getBounds();
			width= Math.max(width, statusBounds.width);
			height= height + statusBounds.height + separatorBounds.height;
		}

		// Apply size constraints
		if (fMaxWidth != SWT.DEFAULT)
			width= Math.min(fMaxWidth, width);
		if (fMaxHeight != SWT.DEFAULT)
			height= Math.min(fMaxHeight, height);

		// Ensure minimal size
		width= Math.max(MIN_WIDTH, width);
		height= Math.max(MIN_HEIGHT, height);
		
		return new Point(width, height);
	}

	/*
	 * @see org.eclipse.jface.text.IInformationControlExtension3#computeTrim()
	 */
	public Rectangle computeTrim() {
		return fShell.computeTrim(0, 0, 0, 0);
	}

	/*
	 * @see org.eclipse.jface.text.IInformationControlExtension3#getBounds()
	 */
	public Rectangle getBounds() {
		return fShell.getBounds();
	}

	/*
	 * @see org.eclipse.jface.text.IInformationControlExtension3#restoresLocation()
	 */
	public boolean restoresLocation() {
		return false;
	}

	/*
	 * @see org.eclipse.jface.text.IInformationControlExtension3#restoresSize()
	 */
	public boolean restoresSize() {
		return false;
	}

	/*
	 * @see IInformationControl#addDisposeListener(DisposeListener)
	 */
	public void addDisposeListener(DisposeListener listener) {
		fShell.addDisposeListener(listener);
	}

	/*
	 * @see IInformationControl#removeDisposeListener(DisposeListener)
	 */
	public void removeDisposeListener(DisposeListener listener) {
		fShell.removeDisposeListener(listener);
	}

	/*
	 * @see IInformationControl#setForegroundColor(Color)
	 */
	public void setForegroundColor(Color foreground) {
		fBrowser.setForeground(foreground);
	}

	/*
	 * @see IInformationControl#setBackgroundColor(Color)
	 */
	public void setBackgroundColor(Color background) {
		fBrowser.setBackground(background);
	}

	/*
	 * @see IInformationControl#isFocusControl()
	 */
	public boolean isFocusControl() {
		return fBrowser.isFocusControl();
	}

	/*
	 * @see IInformationControl#setFocus()
	 */
	public void setFocus() {
		fShell.forceFocus();
		fBrowser.setFocus();
	}

	/*
	 * @see IInformationControl#addFocusListener(FocusListener)
	 */
	public void addFocusListener(final FocusListener listener) {
		fBrowser.addFocusListener(listener);

		/*
		 * FIXME:	This is a workaround for bug https://bugs.eclipse.org/bugs/show_bug.cgi?id=84532
		 * 			(Browser widget does not send focusLost event)
		 */
		if (fFocusListeners.isEmpty()) {
			fDeactivateListener=  new Listener() {
				public void handleEvent(Event event) {
					Object[] listeners= fFocusListeners.getListeners();
					for (int i = 0; i < listeners.length; i++)
						((FocusListener)listeners[i]).focusLost(new FocusEvent(event));
				}
			};
			fBrowser.getShell().addListener(SWT.Deactivate, fDeactivateListener);
		}
		fFocusListeners.add(listener);
	}

	/*
	 * @see IInformationControl#removeFocusListener(FocusListener)
	 */
	public void removeFocusListener(FocusListener listener) {
		fBrowser.removeFocusListener(listener);

		/*
		 * FIXME:	This is a workaround for bug https://bugs.eclipse.org/bugs/show_bug.cgi?id=84532
		 * 			(Browser widget does not send focusLost event)
		 */
		fFocusListeners.remove(listener);
		if (fFocusListeners.isEmpty()) {
			fBrowser.getShell().removeListener(SWT.Deactivate, fDeactivateListener);
			fDeactivateListener= null;
		}
	}

	/*
	 * @see IInformationControlExtension#hasContents()
	 */
	public boolean hasContents() {
		return fBrowserHasContent;
	}
}
