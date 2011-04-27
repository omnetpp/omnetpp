package org.omnetpp.animation.widgets;

public class AnimationParameters {
    public boolean enableHandleMessageAnimations;
    public double handleMessageAnimationShift;

    public boolean enableCreateModuleAnimations;
    public double createModuleAnimationShift;
    public boolean enableCreateConnectionAnimations;
    public double createConnectionAnimationShift;

    public boolean enableSetModuleDisplayStringAnimations;
    public double setModuleDisplayStringAnimationShift;
    public boolean enableSetConnectionDisplayStringAnimations;
    public double setConnectionDisplayStringAnimationShift;

    public boolean enableBubbleAnimations;
    public double bubbleAnimationDuration;
    public boolean enableModuleMethodCallAnimations;
    public double moduleMethodCallAnimationDuration;

    public boolean enableScheduleSelfMessageAnimations;
    public double scheduleSelfMessageAnimationDuration;
    public boolean enableScheduleMessageAnimations;
    public double scheduleMessageAnimationDuration;
    public boolean enableScheduleDirectAnimations;
    public double scheduleDirectAnimationDuration;

    public boolean enableSendSelfMessageAnimations;
    public boolean enableSendMessageAnimations;
    public boolean enableSendDirectAnimations;

    public AnimationParameters() {
        enableDefault();
    }

    public void enableDefault() {
        enableMessagesAndEvents();
    }

    public void enableMessages() {
        enableHandleMessageAnimations = false;
        handleMessageAnimationShift = 0;

        enableCreateModuleAnimations = true;
        createModuleAnimationShift = 0.25;
        enableCreateConnectionAnimations = true;
        createConnectionAnimationShift = 0.25;

        enableSetModuleDisplayStringAnimations = true;
        setModuleDisplayStringAnimationShift = 0.25;
        enableSetConnectionDisplayStringAnimations = true;
        setConnectionDisplayStringAnimationShift = 0.25;

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
        handleMessageAnimationShift = 1;
    }

    public void enableDetail() {
        enableHandleMessageAnimations = true;
        handleMessageAnimationShift = 1;

        enableCreateModuleAnimations = true;
        createModuleAnimationShift = 1;
        enableCreateConnectionAnimations = true;
        createConnectionAnimationShift = 1;

        enableSetModuleDisplayStringAnimations = true;
        setModuleDisplayStringAnimationShift = 1;
        enableSetConnectionDisplayStringAnimations = true;
        setConnectionDisplayStringAnimationShift = 1;

        enableBubbleAnimations = true;
        bubbleAnimationDuration = 1;
        enableModuleMethodCallAnimations = true;
        moduleMethodCallAnimationDuration = 1;

        enableScheduleSelfMessageAnimations = true;
        scheduleSelfMessageAnimationDuration = 1;
        enableScheduleMessageAnimations = true;
        scheduleMessageAnimationDuration = 1;
        enableScheduleDirectAnimations = true;
        scheduleDirectAnimationDuration = 1;

        enableSendSelfMessageAnimations = true;
        enableSendMessageAnimations = true;
        enableSendDirectAnimations = true;
    }
}
