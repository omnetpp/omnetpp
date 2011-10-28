package org.omnetpp.animation.providers;


import java.util.ArrayList;

import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.animation.primitives.BubbleAnimation;
import org.omnetpp.animation.primitives.CreateConnectionAnimation;
import org.omnetpp.animation.primitives.CreateGateAnimation;
import org.omnetpp.animation.primitives.CreateModuleAnimation;
import org.omnetpp.animation.primitives.HandleMessageAnimation;
import org.omnetpp.animation.primitives.IAnimationPrimitive;
import org.omnetpp.animation.primitives.ModuleMethodCallAnimation;
import org.omnetpp.animation.primitives.ScheduleDirectMessageAnimation;
import org.omnetpp.animation.primitives.ScheduleMessageAnimation;
import org.omnetpp.animation.primitives.ScheduleSelfMessageAnimation;
import org.omnetpp.animation.primitives.SendBroadcastAnimation;
import org.omnetpp.animation.primitives.SendDirectAnimation;
import org.omnetpp.animation.primitives.SendMessageAnimation;
import org.omnetpp.animation.primitives.SendSelfMessageAnimation;
import org.omnetpp.animation.primitives.SetConnectionDisplayStringAnimation;
import org.omnetpp.animation.primitives.SetModuleDisplayStringAnimation;
import org.omnetpp.animation.widgets.EventLogAnimationParameters;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.simulation.ConnectionModel;
import org.omnetpp.common.simulation.GateModel;
import org.omnetpp.common.simulation.MessageModel;
import org.omnetpp.common.simulation.ModuleModel;
import org.omnetpp.common.simulation.SimulationModel;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.eventlog.engine.BeginSendEntry;
import org.omnetpp.eventlog.engine.BubbleEntry;
import org.omnetpp.eventlog.engine.ConnectionCreatedEntry;
import org.omnetpp.eventlog.engine.ConnectionDisplayStringChangedEntry;
import org.omnetpp.eventlog.engine.EndSendEntry;
import org.omnetpp.eventlog.engine.EventEntry;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.EventNumberKind;
import org.omnetpp.eventlog.engine.GateCreatedEntry;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.KeyframeEntry;
import org.omnetpp.eventlog.engine.MessageSendDependency;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;
import org.omnetpp.eventlog.engine.ModuleDisplayStringChangedEntry;
import org.omnetpp.eventlog.engine.ModuleMethodBeginEntry;
import org.omnetpp.eventlog.engine.SendDirectEntry;
import org.omnetpp.eventlog.engine.SendHopEntry;
import org.omnetpp.ned.model.DisplayString;

/**
 *
 * <p>
 * Animation primitives are loaded lazily based on the current animation
 * position. At any given moment animation primitives are loaded for a
 * continuous range of events plus the extra events marked in the corresponding
 * eventlog keyframes.
 * </p>
 * <p>
 * The animation primitive provider ensures that when the eventlog file is appended,
 * the animation that is already done by the previous version of the file will not
 * change. In other words it is not allowed to animate into the not completely known
 * future.
 * </p>
 */
public class EventLogAnimationPrimitiveProvider implements IAnimationPrimitiveProvider {
    private final static boolean debug = false;

    /**
     * The eventlog input that is used to construct the animation primitives.
     */
    private EventLogInput eventLogInput;

    /**
     * The animation controller that requests the animation primitives;
     */
    private AnimationController animationController;

    /**
     * The parameters that specifies what is animated and how.
     */
    private EventLogAnimationParameters eventLogAnimationParameters;

    public EventLogAnimationPrimitiveProvider(EventLogInput eventLogInput, EventLogAnimationParameters eventLogAnimationParameters) {
        this.eventLogInput = eventLogInput;
        this.eventLogAnimationParameters = eventLogAnimationParameters;
    }

    public void setAnimationController(AnimationController animationController) {
        this.animationController = animationController;
    }

    public EventLogAnimationParameters getAnimationParameters() {
        return eventLogAnimationParameters;
    }

    /**
     * Creates and stores all animation primitives for the given animation position.
     *
     * The first step is to generate the primitives and fill in the begin/end simulation times and simulation/animation
     * time durations that are known just by looking at the eventlog.
     *
     * In the second step the begin/end animation times are calculated by doing a topological sort on all the animation
     * positions of the collected animation primitives.
     */
     public ArrayList<IAnimationPrimitive> loadAnimationPrimitivesForAnimationPosition(AnimationPosition animationPosition) {
         long begin = System.currentTimeMillis();
         ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>();
         IEventLog eventLog = eventLogInput.getEventLog();
         if (eventLog != null && !eventLog.isEmpty()) {
             Long eventNumber = animationPosition.getEventNumber();
             if (eventNumber != null) {
                 if (animationController.getFrameRelativeEndAnimationTimeForEventNumber(eventNumber) == null) {
                     int keyframeBlockSize = eventLog.getKeyframeBlockSize();
                     int keyframeBlockIndex = (int)(eventNumber / keyframeBlockSize);
                     long firstEventNumber = keyframeBlockIndex * keyframeBlockSize;
                     long lastEventNumber = firstEventNumber + keyframeBlockSize - 1;
                     IEvent firstEvent = eventLog.getEventForEventNumber(firstEventNumber);
                     IEvent lastEvent = eventLog.getEventForEventNumber(lastEventNumber);
                     if (lastEvent == null)
                         lastEvent = eventLog.getLastEvent();
                     // TODO: KLUDGE: what about events that generate animation positions that fall into another event's animation frame???
                     // TODO: KLUDGE: how can we be still lazy by knowing this??? is it enough that we load those events too? does the order matter?
                     animationPrimitives.addAll(collectAnimationPrimitivesForKeyframeSimulationState(firstEvent));
                     animationPrimitives.addAll(collectAnimationPrimitivesForEvents(firstEvent, lastEvent));
                 }
             }
             else if (animationPosition.getOriginRelativeAnimationTime() != null) {
                 double currentAnimationTime = animationController.getCurrentAnimationTime();
                 double newAnimationTime = animationPosition.getOriginRelativeAnimationTime();
                 boolean forward = currentAnimationTime < newAnimationTime;
                 eventNumber = animationController.getCurrentEventNumber();
                 long lastEventNumber = eventLog.getLastEvent().getEventNumber();
                 while (eventNumber >= 0 && eventNumber <= lastEventNumber) {
                     double eventAnimationTime = animationController.getAnimationCoordinateSystem().getAnimationTime(eventNumber);
                     if (forward ? eventAnimationTime >= newAnimationTime : eventAnimationTime <= newAnimationTime)
                         break;
                     animationPrimitives.addAll(loadAnimationPrimitivesForAnimationPosition(new AnimationPosition(eventNumber, null, null, null)));
                     eventNumber += forward ? 1 : -1;
                 }
             }
         }
         if (debug)
             System.out.println("loadAnimationPrimitivesForAnimationPosition:" + (System.currentTimeMillis() - begin) + "ms");
         return animationPrimitives;
     }

     private ArrayList<IAnimationPrimitive> collectAnimationPrimitivesForKeyframeSimulationState(IEvent keyframeEvent) {
         KeyframeEntry keyframeEntry = null;
         for (int i = 0; i < keyframeEvent.getNumEventLogEntries(); i++) {
             EventLogEntry eventLogEntry = keyframeEvent.getEventLogEntry(i);
             if (eventLogEntry instanceof KeyframeEntry)
                 keyframeEntry = (KeyframeEntry)eventLogEntry;
         }
         String[] simulationStateEntries = keyframeEntry.getSimulationStateEntries().split(",");
         ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>();
         for (String simulationStateEntry : simulationStateEntries) {
             if (!StringUtils.isEmpty(simulationStateEntry)) {
                 long eventNumber = Long.parseLong(simulationStateEntry.split(":")[0]);
                 if (eventNumber != -1) {
                     IEvent event = keyframeEvent.getEventLog().getEventForEventNumber(eventNumber);
                     animationPrimitives.addAll(collectAnimationPrimitivesForEvents(event, event));
                 }
             }
         }
         return animationPrimitives;
     }

    /**
     * Collects animation primitives for all events between the two specified events.
     * The range is extended in both directions with the events having the same simulation time.
     */
    // TODO: KLUDGE: do we still need to extend the range now that we have the new animation frame concept?
    public ArrayList<IAnimationPrimitive> collectAnimationPrimitivesForEvents(IEvent beginEvent, IEvent endEvent) {
        AnimationPrimitiveContext animationPrimitiveContext = new AnimationPrimitiveContext();
        ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>();
        // we have to begin collecting after an event having a fresh simulation time to find out the proper relativeAnimationTime
        BigDecimal beginSimulationTime = beginEvent.getSimulationTime();
        while (true) {
            IEvent previousEvent = beginEvent.getPreviousEvent();
            if (previousEvent == null || !previousEvent.getSimulationTime().equals(beginSimulationTime))
                break;
            else
                beginEvent = previousEvent;
        }
        // we have to end collecting before an event having a fresh simulation time
        BigDecimal endSimulationTime = endEvent.getSimulationTime();
        while (true) {
            IEvent nextEvent = endEvent.getNextEvent();
            if (nextEvent == null || !nextEvent.getSimulationTime().equals(endSimulationTime))
                break;
            else
                endEvent = nextEvent;
        }
        IEvent event = beginEvent;
        while (true) {
            long eventNumber = event.getEventNumber();
            if (animationController.getFrameRelativeEndAnimationTimeForEventNumber(eventNumber) == null) {
                animationPrimitiveContext.handleMessageAnimation = null;
                animationPrimitiveContext.frameRelativeAnimationTime = 0;
                animationPrimitiveContext.isCollectingHandleMessageAnimations = true;
                for (int i = 0; i < event.getNumEventLogEntries(); i++)
                    collectAnimationPrimitivesForEventLogEntry(animationPrimitiveContext, event.getEventLogEntry(i), animationPrimitives);
                animationPrimitiveContext.isCollectingHandleMessageAnimations = false;
                for (int i = 0; i < event.getNumEventLogEntries(); i++)
                    collectAnimationPrimitivesForEventLogEntry(animationPrimitiveContext, event.getEventLogEntry(i), animationPrimitives);
                animationController.setFrameRelativeEndAnimationTimeForEventNumber(eventNumber, animationPrimitiveContext.frameRelativeAnimationTime);
            }
            if (eventNumber == endEvent.getEventNumber())
                break;
            else
                event = event.getNextEvent();
        }
        if (debug) // avoids dead code warning
            if (animationPrimitives.size() != 0)
                System.out.println("Loaded " + animationPrimitives.size() + " animation primitives for events " + beginEvent.getEventNumber() + " - " + endEvent.getEventNumber());
        return animationPrimitives;
    }

    /**
     * Adds new IAnimationPrimitives to the collection based on the EventLogEntry.
     */
    private void collectAnimationPrimitivesForEventLogEntry(AnimationPrimitiveContext animationPrimitiveContext, EventLogEntry eventLogEntry, ArrayList<IAnimationPrimitive> animationPrimitives) {
        IEvent event = eventLogEntry.getEvent();
        long eventNumber = event == null ? -1 : event.getEventNumber();
        BigDecimal simulationTime = event == null ? BigDecimal.getZero() : event.getSimulationTime();
        if (eventLogEntry instanceof EventEntry) {
            EventEntry eventEntry = (EventEntry)eventLogEntry;
            if (eventEntry.getModuleId() == 1 || isSubmoduleVisible(eventEntry.getModuleId())) {
                if (animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                    if (eventLogAnimationParameters.enableHandleMessageAnimations) {
                        HandleMessageAnimation handleMessageAnimation = new HandleMessageAnimation(animationController, eventNumber, simulationTime, event.getModuleId());
                        handleMessageAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                        animationPrimitives.add(handleMessageAnimation);
                        animationPrimitiveContext.handleMessageAnimation = handleMessageAnimation;
                        animationPrimitiveContext.frameRelativeAnimationTime += eventLogAnimationParameters.handleMessageAnimationTimeShift;
                    }
                }
                else {
                    if (animationPrimitiveContext.handleMessageAnimation != null)
                        animationPrimitiveContext.handleMessageAnimation.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                }
            }
        }
        else if (eventLogEntry instanceof ModuleCreatedEntry) {
            if (eventLogAnimationParameters.enableCreateModuleAnimations && animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                ModuleCreatedEntry moduleCreatedEntry = (ModuleCreatedEntry)eventLogEntry;
                int moduleId = moduleCreatedEntry.getModuleId();
                int parentModuleId = moduleCreatedEntry.getParentModuleId();
                ModuleModel parentModule = parentModuleId == -1 ? null : animationController.getSimulation().getModuleById(parentModuleId);
                ModuleModel module = animationController.getSimulation() == null ? null : animationController.getSimulation().getModuleById(moduleId);
                if (module == null) {
                    module = new ModuleModel(parentModule);
                    module.setId(moduleId);
                    module.setName(moduleCreatedEntry.getFullName());
                    module.setTypeName(moduleCreatedEntry.getNedTypeName());
                    module.setClassName(moduleCreatedEntry.getModuleClassName());
                    if (animationController.getSimulation() == null) {
                        animationController.setSimulation(new SimulationModel(module));
                        animationController.getAnimationCanvas().addShownCompoundModule(moduleId);
                    }
                    else
                        animationController.getSimulation().addModule(module);
                }
                if (isSubmoduleVisible(moduleId) || isCompoundModuleVisible(moduleId)) {
                    CreateModuleAnimation createModuleAnimation = new CreateModuleAnimation(animationController, eventNumber, simulationTime, module, parentModuleId);
                    createModuleAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitiveContext.frameRelativeAnimationTime += eventLogAnimationParameters.createModuleAnimationTimeShift;
                    animationPrimitives.add(createModuleAnimation);
                }
            }
        }
        else if (eventLogEntry instanceof ModuleDisplayStringChangedEntry) {
            if (eventLogAnimationParameters.enableSetModuleDisplayStringAnimations && animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                ModuleDisplayStringChangedEntry moduleDisplayStringChangedEntry = (ModuleDisplayStringChangedEntry)eventLogEntry;
                if (isSubmoduleVisible(moduleDisplayStringChangedEntry.getModuleId())) {
                    DisplayString displayString = new DisplayString(moduleDisplayStringChangedEntry.getDisplayString());
                    SetModuleDisplayStringAnimation setModuleDisplayStringAnimation = new SetModuleDisplayStringAnimation(animationController, eventNumber, simulationTime, moduleDisplayStringChangedEntry.getModuleId(), displayString);
                    setModuleDisplayStringAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitiveContext.frameRelativeAnimationTime += eventLogAnimationParameters.setModuleDisplayStringAnimationTimeShift;
                    animationPrimitives.add(setModuleDisplayStringAnimation);
                }
            }
        }
        else if (eventLogEntry instanceof GateCreatedEntry) {
            if (animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                GateCreatedEntry gateCreatedEntry = (GateCreatedEntry)eventLogEntry;
                int moduleId = gateCreatedEntry.getModuleId();
                if (isSubmoduleVisible(moduleId) || isCompoundModuleVisible(moduleId)) {
                    ModuleModel module = animationController.getSimulation().getModuleById(moduleId);
                    GateModel gate = new GateModel(module, gateCreatedEntry.getGateId());
                    gate.setName(gateCreatedEntry.getName());
                    gate.setIndex(gateCreatedEntry.getIndex());
                    module.addGate(gate);
                    CreateGateAnimation createGateAnimation = new CreateGateAnimation(animationController, eventNumber, simulationTime, gate);
                    createGateAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitives.add(createGateAnimation);
                }
            }
        }
        else if (eventLogEntry instanceof ConnectionCreatedEntry) {
            if (eventLogAnimationParameters.enableCreateConnectionAnimations && animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                ConnectionCreatedEntry connectionCreatedEntry = (ConnectionCreatedEntry)eventLogEntry;
                int sourceModuleId = connectionCreatedEntry.getSourceModuleId();
                int destinationModuleId = connectionCreatedEntry.getDestModuleId();
                if ((isSubmoduleVisible(sourceModuleId) || isCompoundModuleVisible(sourceModuleId)) && (isSubmoduleVisible(destinationModuleId) || isCompoundModuleVisible(destinationModuleId))) {
                    ModuleModel sourceModule = animationController.getSimulation().getModuleById(sourceModuleId);
                    GateModel sourceGate = sourceModule.getGateById(connectionCreatedEntry.getSourceGateId());
                    ModuleModel destinationModule = animationController.getSimulation().getModuleById(destinationModuleId);
                    GateModel destinationGate = destinationModule.getGateById(connectionCreatedEntry.getDestGateId());
                    CreateConnectionAnimation createConnectionAnimation = new CreateConnectionAnimation(animationController, eventNumber, simulationTime, new ConnectionModel(sourceModule, sourceGate, destinationModule, destinationGate));
                    createConnectionAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitiveContext.frameRelativeAnimationTime += eventLogAnimationParameters.createConnectionAnimationTimeShift;
                    animationPrimitives.add(createConnectionAnimation);
                }
            }
        }
        else if (eventLogEntry instanceof ConnectionDisplayStringChangedEntry) {
            if (eventLogAnimationParameters.enableSetConnectionDisplayStringAnimations && animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                ConnectionDisplayStringChangedEntry connectionDisplayStringChangedEntry = (ConnectionDisplayStringChangedEntry)eventLogEntry;
                int sourceModuleId = connectionDisplayStringChangedEntry.getSourceModuleId();
                if (isSubmoduleVisible(sourceModuleId) || isCompoundModuleVisible(sourceModuleId)) {
                    DisplayString displayString = new DisplayString(connectionDisplayStringChangedEntry.getDisplayString());
                    int sourceGateId = connectionDisplayStringChangedEntry.getSourceGateId();
                    ModuleModel ownerModule = animationController.getSimulation().getModuleById(sourceModuleId);
                    GateModel sourceGate = ownerModule.getGateById(sourceGateId);
                    if (sourceGate == null) {
                        sourceGate = new GateModel(ownerModule, sourceGateId);
                        ownerModule.addGate(sourceGate);
                    }
                    SetConnectionDisplayStringAnimation setConnectionDisplayStringAnimation = new SetConnectionDisplayStringAnimation(animationController, eventNumber, simulationTime, sourceGate, displayString);
                    setConnectionDisplayStringAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitiveContext.frameRelativeAnimationTime += eventLogAnimationParameters.setConnectionDisplayStringAnimationTimeShift;
                    animationPrimitives.add(setConnectionDisplayStringAnimation);
                }
            }
        }
        else if (eventLogEntry instanceof BubbleEntry) {
            if (eventLogAnimationParameters.enableBubbleAnimations && animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                BubbleEntry bubbleEntry = (BubbleEntry)eventLogEntry;
                if (isSubmoduleVisible(bubbleEntry.getModuleId())) {
                    BubbleAnimation bubbleAnimation = new BubbleAnimation(animationController, eventNumber, simulationTime, bubbleEntry.getText(), bubbleEntry.getModuleId());
                    bubbleAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitiveContext.frameRelativeAnimationTime += eventLogAnimationParameters.bubbleAnimationTimeDuration;
                    bubbleAnimation.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitives.add(bubbleAnimation);
                }
            }
        }
        else if (eventLogEntry instanceof ModuleMethodBeginEntry) {
            if (eventLogAnimationParameters.enableModuleMethodCallAnimations && animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                ModuleMethodBeginEntry moduleMethodBeginEntry = (ModuleMethodBeginEntry)eventLogEntry;
                if (isSubmoduleVisible(moduleMethodBeginEntry.getFromModuleId()) && isSubmoduleVisible(moduleMethodBeginEntry.getToModuleId())) {
                    ModuleMethodCallAnimation moduleMethodCallAnimation = new ModuleMethodCallAnimation(animationController, eventNumber, simulationTime, moduleMethodBeginEntry.getMethod(), moduleMethodBeginEntry.getFromModuleId(), moduleMethodBeginEntry.getToModuleId());
                    moduleMethodCallAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitiveContext.frameRelativeAnimationTime += eventLogAnimationParameters.moduleMethodCallAnimationTimeDuration;
                    moduleMethodCallAnimation.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitives.add(moduleMethodCallAnimation);
                }
            }
        }
        else if (eventLogEntry instanceof BeginSendEntry) {
            IEventLog eventLog = eventLogInput.getEventLog();
            IEvent lastEvent = eventLog.getLastEvent();
            BeginSendEntry beginSendEntry = (BeginSendEntry)eventLogEntry;
            // TODO: handle EndSendEntry's isReceptionStart flag
            EndSendEntry endSendEntry = event.getEndSendEntry(beginSendEntry);
            MessageModel message = createMessage(beginSendEntry, endSendEntry);
            int beginSendEntryIndex = beginSendEntry.getEntryIndex();
            int endSendEntryIndex = endSendEntry.getEntryIndex();
            BigDecimal arrivalTime = endSendEntry.getArrivalTime();
            long arrivalEventNumber = new MessageSendDependency(eventLog, eventNumber, beginSendEntryIndex).getConsequenceEventNumber();
            message.setArrivalEventNumber(arrivalEventNumber);
            message.setSenderEventNumber(eventNumber);
            if (beginSendEntryIndex == endSendEntryIndex - 1) {
                // self message send
                ModuleModel module = animationController.getSimulation().getModuleById(event.getModuleId());
                message.setSenderModule(module);
                message.setArrivalModule(module);
                if (isSubmoduleVisible(beginSendEntry.getContextModuleId())) {
                    if (animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                        if (eventLogAnimationParameters.enableScheduleSelfMessageAnimations) {
                            ScheduleSelfMessageAnimation scheduleSelfMessageAnimation = new ScheduleSelfMessageAnimation(animationController, eventNumber, simulationTime, beginSendEntry.getContextModuleId(), message);
                            scheduleSelfMessageAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            animationPrimitiveContext.frameRelativeAnimationTime += eventLogAnimationParameters.scheduleSelfMessageAnimationTimeDuration;
                            scheduleSelfMessageAnimation.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            animationPrimitives.add(scheduleSelfMessageAnimation);
                        }
                    }
                    else {
                        if (eventLogAnimationParameters.enableSendSelfMessageAnimations && arrivalEventNumber != EventNumberKind.NO_SUCH_EVENT) {
                            SendSelfMessageAnimation sendSelfMessageAnimation = new SendSelfMessageAnimation(animationController, beginSendEntry.getContextModuleId(), message, eventLogAnimationParameters.handleMessageAnimationTimeShift);
                            sendSelfMessageAnimation.setSourceEventNumber(eventNumber);
                            sendSelfMessageAnimation.setBeginEventNumber(eventNumber);
                            sendSelfMessageAnimation.setBeginSimulationTime(simulationTime);
                            sendSelfMessageAnimation.setEndSimulationTime(arrivalTime);
                            sendSelfMessageAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            // TODO: handle CancelEventEntry and stop animation if present!
                            // TODO: sum up the handle message animation time durations for entries before the cancel entry?
                            if (arrivalEventNumber < 0 || lastEvent.getSimulationTime().less(arrivalTime)) {
                                sendSelfMessageAnimation.setEndEventNumber(lastEvent.getEventNumber());
                                sendSelfMessageAnimation.setFrameRelativeEndAnimationTime(Double.POSITIVE_INFINITY);
                            }
                            else {
                                sendSelfMessageAnimation.setEndEventNumber(arrivalEventNumber);
                                sendSelfMessageAnimation.setFrameRelativeEndAnimationTime(eventLogAnimationParameters.handleMessageAnimationTimeShift);
                            }
                            animationPrimitives.add(sendSelfMessageAnimation);
                        }
                    }
                }
            }
            else if (beginSendEntryIndex <= endSendEntryIndex - 2 && event.getEventLogEntry(beginSendEntryIndex + 1) instanceof SendDirectEntry) {
                // direct message send
                SendDirectEntry sendDirectEntry = (SendDirectEntry)event.getEventLogEntry(beginSendEntryIndex + 1);
                int senderModuleId = sendDirectEntry.getSenderModuleId();
                int destinationModuleId = sendDirectEntry.getDestModuleId();
                ModuleModel senderModule = animationController.getSimulation().getModuleById(senderModuleId);
                ModuleModel destinationModule = animationController.getSimulation().getModuleById(destinationModuleId);
                message.setSenderModule(senderModule);
                message.setArrivalModule(destinationModule);
                message.setArrivalGate(destinationModule.getGateById(sendDirectEntry.getDestGateId()));
                if (isCompoundModuleVisible(senderModule.getCommonAncestorModule(destinationModule).getId())) {
                    if (animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                        if (eventLogAnimationParameters.enableScheduleDirectAnimations) {
                            ScheduleDirectMessageAnimation scheduleDirectMessageAnimation = new ScheduleDirectMessageAnimation(animationController, eventNumber, simulationTime, beginSendEntry.getContextModuleId(), message);
                            scheduleDirectMessageAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            animationPrimitiveContext.frameRelativeAnimationTime += eventLogAnimationParameters.scheduleDirectAnimationTimeDuration;
                            scheduleDirectMessageAnimation.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            animationPrimitives.add(scheduleDirectMessageAnimation);
                        }
                    }
                    else {
                        if (eventLogAnimationParameters.enableSendDirectAnimations) {
                            BigDecimal transmissionDelay = sendDirectEntry.getTransmissionDelay();
                            BigDecimal propagationDelay = sendDirectEntry.getPropagationDelay();
                            BigDecimal simulationTimeDuration = transmissionDelay.add(propagationDelay);
                            BigDecimal endSimulationTime = simulationTime.add(simulationTimeDuration);
                            long endEventNumber = arrivalEventNumber;
                            double frameRelativeEndAnimationTime = endSendEntry.getIsReceptionStart() && !transmissionDelay.equals(BigDecimal.getZero()) ? 0 : eventLogAnimationParameters.handleMessageAnimationTimeShift;
                            if (endEventNumber < 0 || lastEvent.getSimulationTime().less(endSimulationTime)) {
                                endEventNumber = lastEvent.getEventNumber();
                                frameRelativeEndAnimationTime = Double.POSITIVE_INFINITY;
                            }
                            SendDirectAnimation sendDirectAnimation = new SendDirectAnimation(animationController, propagationDelay, transmissionDelay, senderModuleId, destinationModuleId, message, eventLogAnimationParameters.handleMessageAnimationTimeShift);
                            sendDirectAnimation.setSourceEventNumber(eventNumber);
                            sendDirectAnimation.setBeginEventNumber(eventNumber);
                            sendDirectAnimation.setEndEventNumber(endEventNumber);
                            sendDirectAnimation.setBeginSimulationTime(simulationTime);
                            sendDirectAnimation.setEndSimulationTime(endSimulationTime);
                            sendDirectAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            sendDirectAnimation.setFrameRelativeEndAnimationTime(frameRelativeEndAnimationTime);
                            animationPrimitives.add(sendDirectAnimation);
                            // TODO: SendBroadcastAnimation should not be used by default
                            SendBroadcastAnimation sendBroadcastAnimation = new SendBroadcastAnimation(animationController, propagationDelay, transmissionDelay, senderModuleId, destinationModuleId, message, eventLogAnimationParameters.handleMessageAnimationTimeShift);
                            sendBroadcastAnimation.setSourceEventNumber(eventNumber);
                            sendBroadcastAnimation.setBeginEventNumber(eventNumber);
                            sendBroadcastAnimation.setEndEventNumber(endEventNumber);
                            sendBroadcastAnimation.setBeginSimulationTime(simulationTime);
                            sendBroadcastAnimation.setEndSimulationTime(endSimulationTime);
                            sendBroadcastAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            sendBroadcastAnimation.setFrameRelativeEndAnimationTime(frameRelativeEndAnimationTime);
                            animationPrimitives.add(sendBroadcastAnimation);
                        }
                    }
                }
            }
            else {
                // normal message send
                SendHopEntry firstSendHopEntry = (SendHopEntry)event.getEventLogEntry(eventLogEntry.getEntryIndex() + 1);
                ModuleModel senderModule = animationController.getSimulation().getModuleById(firstSendHopEntry.getSenderModuleId());
                int destinationModuleId = arrivalEventNumber < 0 ? -1 : eventLog.getEventForEventNumber(arrivalEventNumber).getModuleId();
                ModuleModel destinationModule = destinationModuleId == -1 ? null : animationController.getSimulation().getModuleById(destinationModuleId);
                message.setSenderModule(senderModule);
                message.setSenderGate(senderModule.getGateById(firstSendHopEntry.getSenderGateId()));
                if (animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                    if (eventLogAnimationParameters.enableScheduleMessageAnimations && isSubmoduleVisible(beginSendEntry.getContextModuleId())) {
                        ScheduleMessageAnimation scheduleMessageAnimation = new ScheduleMessageAnimation(animationController, eventNumber, simulationTime, beginSendEntry.getContextModuleId(), message);
                        scheduleMessageAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                        animationPrimitiveContext.frameRelativeAnimationTime += eventLogAnimationParameters.scheduleMessageAnimationTimeDuration;
                        scheduleMessageAnimation.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                        animationPrimitives.add(scheduleMessageAnimation);
                    }
                }
                else {
                    // a send message animation is a list of alternating constant and interpolation frames
                    if (eventLogAnimationParameters.enableSendMessageAnimations) {
                        AnimationPosition beginAnimationPosition = new AnimationPosition(eventNumber, simulationTime, animationPrimitiveContext.frameRelativeAnimationTime, null);
                        for (int i = beginSendEntryIndex + 1; i < endSendEntryIndex; i++) {
                            boolean isLastHop = i == endSendEntryIndex - 1;
                            SendHopEntry sendHopEntry = (SendHopEntry)event.getEventLogEntry(i);
                            BigDecimal propagationDelay = sendHopEntry.getPropagationDelay();
                            BigDecimal transmissionDelay = sendHopEntry.getTransmissionDelay();
                            BigDecimal simulationTimeDuration = transmissionDelay.add(propagationDelay);
                            long beginEventNumber = beginAnimationPosition.getEventNumber();
                            BigDecimal beginSimulationTime = beginAnimationPosition.getSimulationTime();
                            double frameRelativeBeginAnimationTime = beginAnimationPosition.getFrameRelativeAnimationTime();
                            // TODO: arrival of what? first or last bit or what?
                            BigDecimal endSimulationTime = beginSimulationTime.add(simulationTimeDuration);
                            long endEventNumber = isLastHop ? arrivalEventNumber : Math.max(beginEventNumber, eventLogInput.getEventLog().getLastEventNotAfterSimulationTime(endSimulationTime).getEventNumber());
                            double frameRelativeEndAnimationTime;
                            if (simulationTimeDuration.equals(BigDecimal.getZero())) {
                                if (isLastHop)
                                    frameRelativeEndAnimationTime = 0;
                                else
                                    frameRelativeEndAnimationTime = frameRelativeBeginAnimationTime + animationController.getAnimationCoordinateSystem().getAnimationTimeDelta(0);
                            }
                            else {
                                // to avoid overlapping with the end event's beginning
                                // we have to count the number of send hops with zero simulation time
                                double remainingAnimationTimeDuration = 0;
                                for (int j = i + 1; j < endSendEntryIndex; j++) {
                                    SendHopEntry remainingSendHopEntry = (SendHopEntry)event.getEventLogEntry(j);
                                    if (remainingSendHopEntry.getPropagationDelay().add(remainingSendHopEntry.getTransmissionDelay()).equals(BigDecimal.getZero()))
                                        remainingAnimationTimeDuration += animationController.getAnimationCoordinateSystem().getAnimationTimeDelta(0);
                                }
                                frameRelativeEndAnimationTime = -remainingAnimationTimeDuration;
                            }
                            if (endEventNumber < 0 || lastEvent.getSimulationTime().less(endSimulationTime)) {
                                endEventNumber = lastEvent.getEventNumber();
                                frameRelativeEndAnimationTime = Double.POSITIVE_INFINITY;
                            }
                            frameRelativeEndAnimationTime += eventLogAnimationParameters.handleMessageAnimationTimeShift;
                            ModuleModel sendHopSenderModule = animationController.getSimulation().getModuleById(sendHopEntry.getSenderModuleId());
                            ModuleModel sendHopDestinationModule = isLastHop ? destinationModule : animationController.getSimulation().getModuleById(((SendHopEntry)event.getEventLogEntry(i + 1)).getSenderModuleId());
                            boolean visible = sendHopSenderModule != null && sendHopDestinationModule != null && isCompoundModuleVisible(sendHopSenderModule.getCommonAncestorModule(sendHopDestinationModule).getId());
                            if (visible) {
                                GateModel gate = new GateModel(sendHopSenderModule, sendHopEntry.getSenderGateId());
                                SendMessageAnimation sendMessageAnimation = new SendMessageAnimation(animationController, propagationDelay, transmissionDelay, gate, message, eventLogAnimationParameters.handleMessageAnimationTimeShift);
                                sendMessageAnimation.setSourceEventNumber(eventNumber);
                                sendMessageAnimation.setBeginEventNumber(beginEventNumber);
                                sendMessageAnimation.setEndEventNumber(endEventNumber);
                                sendMessageAnimation.setBeginSimulationTime(beginSimulationTime);
                                sendMessageAnimation.setEndSimulationTime(endSimulationTime);
                                sendMessageAnimation.setFrameRelativeBeginAnimationTime(frameRelativeBeginAnimationTime);
                                sendMessageAnimation.setFrameRelativeEndAnimationTime(frameRelativeEndAnimationTime);
                                animationPrimitives.add(sendMessageAnimation);
                            }
                            // NOTE: avoid adding animation time periods where nothing happens
                            beginAnimationPosition = visible || beginEventNumber != endEventNumber ?
                                new AnimationPosition(endEventNumber, endSimulationTime, frameRelativeEndAnimationTime, null) :
                                new AnimationPosition(endEventNumber, endSimulationTime, frameRelativeBeginAnimationTime, null);
                        }
                    }
                }
            }
        }
    }

    private boolean isSubmoduleVisible(int moduleId) {
        ModuleModel module = animationController.getSimulation().getModuleById(moduleId);
        ModuleModel parentModule = module.getParentModule();
        return module != null && parentModule != null && isCompoundModuleVisible(parentModule.getId());
    }

    private boolean isCompoundModuleVisible(int moduleId) {
        return animationController.getAnimationCanvas().showsCompoundModule(moduleId);
    }

    private MessageModel createMessage(BeginSendEntry beginSendEntry, EndSendEntry endSendEntry) {
        MessageModel message = new MessageModel();
        message.setName(beginSendEntry.getMessageName());
        message.setClassName(beginSendEntry.getMessageClassName());
        message.setId(beginSendEntry.getMessageId());
        message.setTreeId(beginSendEntry.getMessageTreeId());
        message.setEncapsulationId(beginSendEntry.getMessageEncapsulationId());
        message.setEncapsulationTreeId(beginSendEntry.getMessageEncapsulationTreeId());
        message.setSendingTime(beginSendEntry.getEvent().getSimulationTime());
        message.setArrivalTime(endSendEntry.getArrivalTime());
        message.setBitLength(beginSendEntry.getMessageLength());
        return message;
    }

    private static class AnimationPrimitiveContext {
        public HandleMessageAnimation handleMessageAnimation;
        public double frameRelativeAnimationTime;
        public boolean isCollectingHandleMessageAnimations;
    }
}
