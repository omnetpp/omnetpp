package org.omnetpp.experimental.seqchart.editors;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LightweightSystem;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;

public class SequenceChartToolEditor extends EditorPart {

	private ColorManager colorManager;
	private SashForm sashForm;
	private Text text;
	private Figure rootFigure;
	private XYLayout rootLayout;
	
	public SequenceChartToolEditor() {
		super();
		colorManager = new ColorManager();
	}

	@Override
	public void init(IEditorSite site, IEditorInput input) throws PartInitException {
		setSite(site);
		setInput(input);
		//TODO site.setSelectionProvider(new WhateverSelectionProvider(this));
	}
	
	@Override
	public void createPartControl(Composite parent) {
		sashForm = new SashForm(parent, SWT.VERTICAL | SWT.BORDER | SWT.SMOOTH);

		// add canvas into the upper half
		Composite upper = new Composite(sashForm, SWT.NONE);
		upper.setLayout(new FillLayout());
		
		Canvas canvas = new Canvas(upper, SWT.NONE);
		LightweightSystem lws = new LightweightSystem(canvas);
		rootFigure = new Figure();
		rootLayout = new XYLayout();
		rootFigure.setLayoutManager(rootLayout);
		lws.setContents(rootFigure);

		drawSomething();
		
		// add text box into lower half
		Composite lower = new Composite(sashForm, SWT.NONE);
		lower.setLayout(new FillLayout());
		text = new Text(lower, SWT.MULTI | SWT.READ_ONLY | SWT.H_SCROLL | SWT.V_SCROLL);
		text.setBackground(colorManager.getColor(ISeqChartColorConstants.TEXT_BG));

		text.setText("Multi\nline\nText\nTest\n...\n\nMulti\nline\nText\nTest\n...\n\n");
	}

	private void drawSomething() {
		Font someFont = new Font(null, "Arial", 12, SWT.BOLD);
		Label label1 = new Label("Blughghgd", null);
		label1.setFont(someFont);
		
		rootFigure.add(label1);
		rootLayout.setConstraint(label1, new Rectangle(10,10,-1,-1));

	}

	public void dispose() {
		colorManager.dispose();
		super.dispose();
	}

	@Override
	public void setFocus() {
		// TODO Auto-generated method stub
	}

	@Override
	public void doSave(IProgressMonitor monitor) {
		// TODO Auto-generated method stub
	}

	@Override
	public void doSaveAs() {
		// TODO Auto-generated method stub
	}
	
	@Override
	public boolean isDirty() {
		return false;
	}

	@Override
	public boolean isSaveAsAllowed() {
		// TODO Auto-generated method stub
		return false;
	}
}
