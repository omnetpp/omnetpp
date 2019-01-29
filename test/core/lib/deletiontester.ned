package testlib;

simple DeletionTesterBase {
    parameters:
        string moduleToDelete = default("");
        double at @unit(s) = default(1s);
    gates:
        input in @loose;
        output out @loose;
}

simple DeletionTesterActivity extends DeletionTesterBase {
    @class(DeletionTesterActivity);
}

simple DeletionTesterHandleMessage extends DeletionTesterBase {
    @class(DeletionTesterHandleMessage);
}
