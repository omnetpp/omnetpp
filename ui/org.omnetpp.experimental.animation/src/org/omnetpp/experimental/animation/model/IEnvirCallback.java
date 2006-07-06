package org.omnetpp.experimental.animation.model;

public interface IEnvirCallback {
    /**
     * Notifies the environment that the object no longer exists. The
     * user interface should close all inspector windows for the object
     * and remove it from object lists currently displayed. cObject's
     * destructor automatically calls this function.
     */
    public void objectDeleted(IObject object);

    /**
     * Notifies the environment that a message was sent. Details can be
     * extracted from the message object itself. The user interface
     * implementation may use the notification to animate the message on a
     * network diagram, to write a log entry, etc.
     *
     * The second argument is non-NULL only when sendDirect() was used, and
     * identifies the target gate that was passed to the sendDirect() call.
     * (This information is necessary for proper animation: the target gate
     * might belong to a compound module and be further connected, and then
     * the message will additionally travel through a series of connections
     * before it arrives in a simple module.)
     */
    public void messageSent(IRuntimeMessage msg, IRuntimeGate directToGate);

    /**
     * Notifies the environment that a module changed parent.
     */
    public void moduleReparented(IRuntimeModule module, IRuntimeModule oldparent);

    /**
     * Notifies the environment that a message was delivered to its destination
     * module, that is, a message arrival event occurred. Details can be
     * extracted from the message object itself. The user interface
     * implementation may use the notification to animate the message on a
     * network diagram, to write a log entry, etc.
     */
    public void messageDelivered(IRuntimeMessage msg);

    /**
     * Notifies the environment that a simple module executed a
     * breakpoint() call.
     */
    public void breakpointHit(String lbl, IRuntimeModule mod);

    /**
     * Notifies the environment that one module called a member function
     * of another module object. This hook enables a graphical user
     * interface animate the method call in the network diagram.
     */
    public void moduleMethodCalled(IRuntimeModule from, IRuntimeModule to, String method);

    /**
     * Notifies the environment that a module was created. This method is called
     * from cModuleType::create(), when the module has already been created
     * but buildInside() has not been invoked yet.
     */
    public void moduleCreated(IRuntimeModule newmodule);

    /**
     * Notifies the environment that a module was (more precisely: is being)
     * deleted. This method is called from cModule destructor, so the
     * "real" type (className() and everything from the actual subclass)
     * is already lost at this point, however name(), fullName(), fullPath(),
     * gates, parameters (everything that comes from cModule) are still valid.
     *
     * If a compound module (or a module with dynamically created submodules)
     * is deleted, one should not assume anything about the relative order
     * moduleDeleted() is called for the module and its submodules.
     */
    public void moduleDeleted(IRuntimeModule module);

    /**
     * Notifies the environment that a connection has been created using
     * srcgate->connectTo().
     */
    public void connectionCreated(IRuntimeGate srcgate);

    /**
     * Notifies the environment that a connection has been removed using
     * srcgate->disconnect().
     */
    public void connectionRemoved(IRuntimeGate srcgate);

    /**
     * Notifies the environment that a connection display string (stored in
     * the source gate) has been changed.
     */
    public void displayStringChanged(IRuntimeGate gate);

    /**
     * Notifies the environment that a module display string has been changed.
     */
    public void displayStringChanged(IRuntimeModule module);

    /**
     * In Tkenv it pops up a "bubble" over the module icon.
     */
    public void bubble(IRuntimeModule mod, String text);
}
