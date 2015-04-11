/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.panel;

import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.views.contentoutline.ContentOutlinePage;

import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.FileList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.model.IDListModel;
import org.omnetpp.scave.plugin.ScavePlugin;

public class DatasetContentOutlinePage extends ContentOutlinePage {

    private IDListModel dataset;

    class ViewContentProvider implements IStructuredContentProvider,
    ITreeContentProvider {

        public void inputChanged(Viewer v, Object oldInput, Object newInput) {
        }

        public void dispose() {
        }

        public Object[] getElements(Object parent) {
            return getChildren(parent);
        }

        public Object getParent(Object child) {
            if (child instanceof Run)
                return ((Run)child).getFile();
            if (child instanceof File)
                return ((File)child).getResultFileManager();
            if (child instanceof ResultFileManager)
                return null;
            return null;
        }

        public Object[] getChildren(Object parent) {
            if (parent instanceof Run)
                return new Object[0];
            if (parent instanceof File) {
                File file = (File)parent;
                RunList runList = file.getResultFileManager().getRunsInFile(file);
                return runList.toArray();
            }
            if (parent instanceof ResultFileManager) {
                FileList fileList = ((ResultFileManager)parent).getFiles();
                return fileList.toArray();
            }
            return new Object[0];
        }

        public boolean hasChildren(Object parent) {
            if (parent instanceof Run)
                return false;
            return getChildren(parent).length>0;  // TBD not very efficient
        }
    }

    class ViewLabelProvider extends LabelProvider {

        public String getText(Object obj) {
            if (obj instanceof Run)
                return "run " + ((Run)obj).getRunName();
            if (obj instanceof File)
                return ((File)obj).getFilePath();
            return "(" + obj.getClass().getSimpleName() + ")" + obj.toString(); // fallback -- never used
        }
        public Image getImage(Object obj) {
            String imageKey = ISharedImages.IMG_OBJ_ELEMENT;
            if (obj instanceof File)
                imageKey = ISharedImages.IMG_OBJ_FOLDER;
            return PlatformUI.getWorkbench().getSharedImages().getImage(imageKey);
        }
    }

    public DatasetContentOutlinePage(IDListModel dataset) {
        this.dataset = dataset;
        //TODO add listener, in dispose: remove listener
    }

    public void createControl(Composite parent) {
        super.createControl(parent);

        // add content provider and label provider to viewer
        TreeViewer viewer = getTreeViewer();
        viewer.setContentProvider(new ViewContentProvider());
        viewer.setLabelProvider(new ViewLabelProvider());
        viewer.setInput(ScavePlugin.getDefault().resultFileManager); // FIXME it's not possible to use IDListModel?

    }

}
