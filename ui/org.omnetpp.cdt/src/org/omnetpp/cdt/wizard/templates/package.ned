#if($rootpackage!="")// designate the NED package of this folder#end
#if($rootpackage!="")package ${rootpackage};#end

#if($namespace!="")// namespace of module C++ classes#end
#if($namespace!="")@namespace(${namespace});#end

#if($license!="")@license(${license});#end

