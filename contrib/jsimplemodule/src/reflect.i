
%{
#if (OMNETPP_VERSION <= 0x0303)
# include <memory>
# define cClassDescriptor  cStructDescriptor
# define getDescriptor()   createDescriptor()
# define NEWARG(x)
# define AUTOPTR(x)        auto_ptr<cStructDescriptor *> _x(x)
#else
# define NEWARG(x)         x,
# define AUTOPTR(x)        (void)0
#endif

  static cClassDescriptor *findDescriptor(cPolymorphic *p)
  {
      cClassDescriptor *desc = p->getDescriptor();
      if (!desc)
          opp_error("no descriptor for class %s", p->className());
      return desc;
  }

  static int findField(cClassDescriptor *desc, void *object, const char *fieldName)
  {
      int n = desc->getFieldCount(NEWARG(object));
      for (int i=0; i<n; i++)
          if (!strcmp(desc->getFieldName(NEWARG(object) i), fieldName))
              return i;
      return -1;
  }

  static int getFieldID(cClassDescriptor *desc, void *object, const char *fieldName)
  {
      int id = findField(desc, object, fieldName);
      if (id==-1)
          opp_error("no `%s' field in class %s", fieldName, desc->name());
      return id;
  }
%}

%extend cPolymorphic {

  bool hasField(const char *fieldName)
  {
      cClassDescriptor *desc = findDescriptor(self);
      AUTOPTR(desc);
      return findField(desc, self, fieldName)!=-1;
  }

  std::string getField(const char *fieldName)
  {
      cClassDescriptor *desc = findDescriptor(self);
      AUTOPTR(desc);
      int fieldId = getFieldID(desc, self, fieldName);
      char buf[200];
      desc->getFieldAsString(NEWARG(self) fieldId, 0, buf, 200);
      return buf;
  }

  std::string getArrayField(const char *fieldName, int index)
  {
      cClassDescriptor *desc = findDescriptor(self);
      AUTOPTR(desc);
      int fieldId = getFieldID(desc, self, fieldName);
      char buf[200];
      desc->getFieldAsString(NEWARG(self) fieldId, index, buf, 200); //XXX check out of bounds!!!
      return buf;
  }

  void setField(const char *fieldName, const char *value)
  {
      cClassDescriptor *desc = findDescriptor(self);
      AUTOPTR(desc);
      int fieldId = getFieldID(desc, self, fieldName);
      desc->setFieldAsString(NEWARG(self) fieldId, 0, value);
  }

  void setArrayField(const char *fieldName, int index, const char *value)
  {
      cClassDescriptor *desc = findDescriptor(self);
      AUTOPTR(desc);
      int fieldId = getFieldID(desc, self, fieldName);
      desc->setFieldAsString(NEWARG(self) fieldId, index, value); //XXX check out of bounds!!!
  }

  bool isFieldArray(const char *fieldName)
  {
      cClassDescriptor *desc = findDescriptor(self);
      AUTOPTR(desc);
      int fieldId = getFieldID(desc, self, fieldName);
#if (OMNETPP_VERSION <= 0x0303)
      int type = desc->getFieldType(fieldId);
      return type==FT_BASIC_ARRAY || type==FT_SPECIAL_ARRAY || type==FT_STRUCT_ARRAY;
#else
      return desc->getFieldIsArray(NEWARG(self) fieldId);
#endif
  }

  bool isFieldCompound(const char *fieldName)
  {
      cClassDescriptor *desc = findDescriptor(self);
      AUTOPTR(desc);
      int fieldId = getFieldID(desc, self, fieldName);
#if (OMNETPP_VERSION <= 0x0303)
      int type = desc->getFieldType(fieldId);
      return type==FT_STRUCT || type==FT_STRUCT_ARRAY;
#else
      return desc->getFieldIsCompound(NEWARG(self) fieldId);
#endif
  }
}



