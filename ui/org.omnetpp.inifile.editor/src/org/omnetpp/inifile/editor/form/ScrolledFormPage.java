package org.omnetpp.inifile.editor.form;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.inifile.editor.editors.InifileEditor;

public abstract class ScrolledFormPage extends FormPage {
	protected Composite form; // the composite being scrolled inside the page

	public ScrolledFormPage(Composite parent, InifileEditor inifileEditor) {
		super(parent, inifileEditor);

		// we are not ScrolledForm, so create a ScrolledForm child which fills the FormPage
		this.setLayout(new FillLayout());
		final ScrolledComposite scrolledComposite = new ScrolledComposite(this, SWT.H_SCROLL | SWT.V_SCROLL);
		scrolledComposite.getVerticalBar().setIncrement(10); // mouse wheel step
        scrolledComposite.getVerticalBar().setPageIncrement(100);
        scrolledComposite.getHorizontalBar().setPageIncrement(100);

		// create form which will hold the actual controls -- clients should use "form" as parent
		form = new Composite(scrolledComposite, SWT.NONE);
		scrolledComposite.setContent(form);

		// handle resizes
		scrolledComposite.addControlListener(new ControlAdapter() {
			public void controlResized(ControlEvent e) {
				layoutForm();
			}
		});
	}

	/**
	 * Must be called when form layout is to be updated (children get added/removed, etc)
	 */
	@Override
	public void layoutForm() {
		if (getSize().x!=0) {
			// reset size of content so children can be seen (see ScrolledComposite method 1)
		    Point formPreferredSize = form.computeSize(SWT.DEFAULT, SWT.DEFAULT);
			int availableWidth = getSize().x-RIGHT_MARGIN;
            form.setSize(Math.max(formPreferredSize.x, availableWidth), formPreferredSize.y);
			form.layout();
		}
	}
}
