package org.omnetpp.animation.providers;

import java.util.ArrayList;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Path;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.animation.primitives.AbstractAnimationPrimitive;
import org.omnetpp.animation.primitives.CreateQueueAnimation;
import org.omnetpp.animation.primitives.CreateVectorAnimation;
import org.omnetpp.animation.primitives.IAnimationPrimitive;
import org.omnetpp.animation.primitives.SetQueueCountAnimation;
import org.omnetpp.animation.primitives.SetVectorValueAnimation;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.simulation.QueueModel;
import org.omnetpp.common.util.VectorFileUtil;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.XYArray;

public class VectorFileAnimationPrimitiveProvider implements IAnimationPrimitiveProvider {
    private EventLogInput eventLogInput;

    private AnimationController animationController;

    public VectorFileAnimationPrimitiveProvider(EventLogInput eventLogInput) {
        this.eventLogInput = eventLogInput;
    }

    public void setAnimationController(AnimationController animationController) {
        this.animationController = animationController;
    }

    public ArrayList<IAnimationPrimitive> loadAnimationPrimitivesForAnimationPosition(AnimationPosition animationPosition) {
        // TODO: KLUDGE: this provider is not lazy
        if (animationController.getAnimationCoordinateSystem().getFirstEventNumber() != -1)
            return new ArrayList<IAnimationPrimitive>();
        else {
            try {
                IFile inputFile = eventLogInput.getFile();
                String inputFileName = inputFile.getName();
                IFile vectorFile = inputFile.getParent().getFile(new Path(inputFileName.substring(0, inputFileName.indexOf(".")) + ".vec"));
                ResultFileManager resultFileManager = new ResultFileManager();
                ResultFile resultFile = resultFileManager.loadFile(vectorFile.getLocation().toOSString());
                RunList runList = resultFileManager.getRunsInFile(resultFile);
                ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>();
                for (int i = 0; i < runList.size(); i++) {
                    Run run = runList.get(i);
                    IDList idList = resultFileManager.getVectorsInFileRun(resultFileManager.getFileRun(resultFile, run));
                    for (int j = 0; j < idList.size(); j++) {
                        long id = idList.get(j);
                        XYArray xyArray = VectorFileUtil.getDataOfVector(resultFileManager, id, true);
                        String moduleName = resultFileManager.getItem(id).getModuleName();
                        String vectorName = resultFileManager.getItem(id).getName();
                        String name = moduleName + "/" + vectorName;
                        // KLUDGE: TODO: qlen?, owner module and size
                        QueueModel queue = vectorName.contains("qlen") ? new QueueModel(null, name, 10) : null;
                        for (int k = 0; k < xyArray.length(); k++) {
                            long eventNumber = xyArray.getEventNumber(k);
                            BigDecimal simulationTime = xyArray.getPreciseX(k);
                            if (k == 0) {
                                AbstractAnimationPrimitive animationPrimitive;
                                if (queue != null)
                                    animationPrimitive = new CreateQueueAnimation(animationController, eventNumber, simulationTime, queue);
                                else
                                    animationPrimitive = new CreateVectorAnimation(animationController, eventNumber, simulationTime, name);
                                animationPrimitive.setFrameRelativeBeginAnimationTime(0);
                                animationPrimitives.add(animationPrimitive);
                            }
                            else {
                                long previousEventNumber = xyArray.getEventNumber(k - 1);
                                BigDecimal previousSimulationTime = xyArray.getPreciseX(k - 1);
                                double value = xyArray.getY(k);
                                AbstractAnimationPrimitive animationPrimitive;
                                if (queue != null)
                                    animationPrimitive = new SetQueueCountAnimation(animationController, previousEventNumber, eventNumber, previousSimulationTime, simulationTime, queue, (int)value);
                                else
                                    animationPrimitive = new SetVectorValueAnimation(animationController, previousEventNumber, eventNumber, previousSimulationTime, simulationTime, name, value);
                                animationPrimitives.add(animationPrimitive);
                            }
                        }
                    }
                }
                return animationPrimitives;
            }
            catch (Exception e) {
                throw new RuntimeException(e);
            }
        }
    }
}
