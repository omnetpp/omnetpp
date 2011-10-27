package org.omnetpp.animation.widgets;

public class EventLogAnimationParameters {
    public boolean enableHandleMessageAnimations;
    public double handleMessageAnimationTimeShift;

    public boolean enableCreateModuleAnimations;
    public double createModuleAnimationTimeShift;
    public boolean enableCreateConnectionAnimations;
    public double createConnectionAnimationTimeShift;

    public boolean enableSetModuleDisplayStringAnimations;
    public double setModuleDisplayStringAnimationTimeShift;
    public boolean enableSetConnectionDisplayStringAnimations;
    public double setConnectionDisplayStringAnimationTimeShift;

    public boolean enableBubbleAnimations;
    public double bubbleAnimationTimeDuration;
    public boolean enableModuleMethodCallAnimations;
    public double moduleMethodCallAnimationTimeDuration;

    public boolean enableScheduleSelfMessageAnimations;
    public double scheduleSelfMessageAnimationTimeDuration;
    public boolean enableScheduleMessageAnimations;
    public double scheduleMessageAnimationTimeDuration;
    public boolean enableScheduleDirectAnimations;
    public double scheduleDirectAnimationTimeDuration;

    public boolean enableSendSelfMessageAnimations;
    public boolean enableSendMessageAnimations;
    public boolean enableSendDirectAnimations;

    public EventLogAnimationParameters() {
        enableDefault();
    }

    public void enableDefault() {
        enableMessagesAndEvents();
    }

    public void enableMessages() {
        enableHandleMessageAnimations = false;
        handleMessageAnimationTimeShift = 0;

        enableCreateModuleAnimations = true;
        createModuleAnimationTimeShift = 0.25;
        enableCreateConnectionAnimations = true;
        createConnectionAnimationTimeShift = 0.25;

        enableSetModuleDisplayStringAnimations = true;
        setModuleDisplayStringAnimationTimeShift = 0.25;
        enableSetConnectionDisplayStringAnimations = true;
        setConnectionDisplayStringAnimationTimeShift = 0.25;

        enableBubbleAnimations = false;
        enableModuleMethodCallAnimations = false;

        enableScheduleSelfMessageAnimations = false;
        enableScheduleMessageAnimations = false;
        enableScheduleDirectAnimations = false;

        enableSendSelfMessageAnimations = true;
        enableSendMessageAnimations = true;
        enableSendDirectAnimations = true;
    }

    public void enableMessagesAndEvents() {
        enableMessages();
        enableHandleMessageAnimations = true;
        handleMessageAnimationTimeShift = 1;
    }

    public void enableDetail() {
        enableHandleMessageAnimations = true;
        handleMessageAnimationTimeShift = 1;

        enableCreateModuleAnimations = true;
        createModuleAnimationTimeShift = 1;
        enableCreateConnectionAnimations = true;
        createConnectionAnimationTimeShift = 1;

        enableSetModuleDisplayStringAnimations = true;
        setModuleDisplayStringAnimationTimeShift = 1;
        enableSetConnectionDisplayStringAnimations = true;
        setConnectionDisplayStringAnimationTimeShift = 1;

        enableBubbleAnimations = true;
        bubbleAnimationTimeDuration = 1;
        enableModuleMethodCallAnimations = true;
        moduleMethodCallAnimationTimeDuration = 1;

        enableScheduleSelfMessageAnimations = true;
        scheduleSelfMessageAnimationTimeDuration = 1;
        enableScheduleMessageAnimations = true;
        scheduleMessageAnimationTimeDuration = 1;
        enableScheduleDirectAnimations = true;
        scheduleDirectAnimationTimeDuration = 1;

        enableSendSelfMessageAnimations = true;
        enableSendMessageAnimations = true;
        enableSendDirectAnimations = true;
    }
}
