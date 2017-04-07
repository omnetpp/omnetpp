/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.treeproviders;

import java.util.concurrent.Callable;

import org.omnetpp.common.ui.CachedTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.model2.ResultFilePayload;
import org.omnetpp.scave.model2.RunPayload;

/**
 * Content provider for the "Run/File" tree of the Inputs page.
 */
public class InputsRunFileViewContentProvider extends CachedTreeContentProvider  {

    public GenericTreeNode buildTree(Object element) {
        // Manager/Run/ResultFile
        final ResultFileManager manager = (ResultFileManager)element;
        return ResultFileManager.callWithReadLock(manager, new Callable<GenericTreeNode>() {
            public GenericTreeNode call() throws Exception {
                GenericTreeNode root = new GenericTreeNode(manager);
                for (Run run : Sorter.sort(manager.getRuns())) {
                    GenericTreeNode runNode = new GenericTreeNode(new RunPayload(run));
                    root.addChild(runNode);
                    ResultFileList filelist = manager.getFilesForRun(run);
                    for (ResultFile file : Sorter.sort(filelist)) {
                        GenericTreeNode fileNode = new GenericTreeNode(new ResultFilePayload(file));
                        runNode.addChild(fileNode);
                    }
                }
                return root;
            }
        });
    }
}
