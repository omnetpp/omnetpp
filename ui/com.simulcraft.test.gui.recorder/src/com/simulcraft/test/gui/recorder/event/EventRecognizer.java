/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package com.simulcraft.test.gui.recorder.event;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.IEventRecognizer;
import com.simulcraft.test.gui.recorder.RecognizerBase;

public abstract class EventRecognizer extends RecognizerBase implements IEventRecognizer {
    public EventRecognizer(GUIRecorder recorder) {
        super(recorder);
    }
}