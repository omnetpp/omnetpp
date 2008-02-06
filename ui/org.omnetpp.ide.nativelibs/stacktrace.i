%{

int printStackTrace(JNIEnv *env)
{
	jclass clazz = env->FindClass("java/lang/Thread");
	if (!clazz)
		return -1;

	jmethodID method = env->GetStaticMethodID(clazz, "dumpStack", "()V");
	if (!method)
		return -2;

	env->CallStaticVoidMethod(clazz, method);
	return 0;
}

%}