
simple Queue_1
    parameters:
        repCount: numeric,
        qLevel: numeric;
endsimple

network queue_1 : Queue_1
endnetwork


simple SelectNextModule_1
    parameters:
        repCount: numeric;
endsimple

network selectNextModule_1 : SelectNextModule_1
endnetwork


simple Schedule_1
    parameters:
        repCount: numeric;
endsimple

network schedule_1 : Schedule_1
endnetwork


simple Send_1
    parameters:
        repCount: numeric;
    gates:
        in: in;
        out: out;
endsimple

module SendTest_1
    submodules:
        send: Send_1;
    connections:
        send.out --> send.in;
endmodule

module SendTest_2
    submodules:
        send: Send_1;
    connections:
        send.out --> delay 1.0 --> send.in;
endmodule

network sendTest_1 : SendTest_1
endnetwork

network sendTest_2 : SendTest_2
endnetwork

simple ScheduledEvents_1
    parameters:
        iaTime: numeric,
        numScheduledMsgs: numeric;
endsimple

network scheduledEvents_1 : ScheduledEvents_1
endnetwork

simple ScheduleAndCancel_1
    parameters:
        iaTime: numeric,
        numScheduledMsgs: numeric,
        cancelsPerEvent;
endsimple

network scheduleAndCancel_1 : ScheduleAndCancel_1
endnetwork

