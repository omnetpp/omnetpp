
network InspectorSafety
{
    @class(InspectorSafety);

    bool throwingGetterCalled = false @mutable;
    bool throwingSetterCalled = false @mutable;

    bool throwingStructToStringCalled = false @mutable;

    bool throwingSizeGetterCalled = false @mutable;

    bool throwingElementGetterCalled = false @mutable;
    bool throwingStructElementPointerGetterCalled = false @mutable;
    bool throwingClassElementPointerGetterCalled = false @mutable;

    @display("bgb=700,500");
}
