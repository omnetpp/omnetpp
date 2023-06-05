from multiprocessing import shared_memory as mp_shm, resource_tracker as mp_rt

def shm_ctor_wrapper(name, create=False, size=0):
    """
    Wraps the standard SharedMemory constructor, dealing with
    all of its platform-dependent peculiarities in one place.

    `name` may or may not start with a single slash, the result
    will be the same. The first element of the returned tuple
    is the actual SHM name that is used towards the OS. This
    always has a single leading slash on all platforms.

    See:
    https://github.com/python/cpython/issues/105380
    https://github.com/python/cpython/issues/105383
    """

    if name.startswith("//"):
        raise RuntimeError("Shared memory names starting with more than one '/' are disallowed")

    if name.startswith("/"):
        withslash = name
        noslash = name[1:]
    else:
        withslash = "/" + name
        noslash = name

    # On POSIX, the constructor prepends it implicitly.
    nameparam = noslash if mp_shm._USE_POSIX else withslash
    shm = mp_shm.SharedMemory(name=nameparam, create=create, size=size)

    # The following HACK is a workaround for https://github.com/python/cpython/issues/82300,
    # so the ResourceTracker doesn't prematurely unlink the SHM object - it's up to the IDE.
    if mp_shm._USE_POSIX and not create:
        # Can't use `name` as that is the name without the leading slash
        mp_rt.unregister(shm._name, "shared_memory")

    return withslash, shm
