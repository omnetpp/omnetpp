%{
#if (OMNETPP_VERSION <= 0x0303)
# include <memory>
# define cClassDescriptor  cStructDescriptor
# define getDescriptor()   createDescriptor()
# define NEWARG(x)
# define NEWARG2(x)
# define AUTOPTR(x)        std::auto_ptr<cStructDescriptor> _x(x)
# define RETURN_FIELD_IS_ARRAY(fieldName) \
      int type = desc->getFieldType(fieldId); \
      return type==cStructDescriptor::FT_BASIC_ARRAY || \
             type==cStructDescriptor::FT_SPECIAL_ARRAY || \
             type==cStructDescriptor::FT_STRUCT_ARRAY
# define RETURN_FIELD_IS_COMPOUND(fieldName) \
      int type = desc->getFieldType(fieldId); \
      return type==cStructDescriptor::FT_STRUCT || \
             type==cStructDescriptor::FT_STRUCT_ARRAY
#else
# define NEWARG(x)         x,
# define NEWARG2(x)        x
# define AUTOPTR(x)        (void)0
# define RETURN_FIELD_IS_ARRAY(fieldName)    return desc->getFieldIsArray(self, fieldId)
# define RETURN_FIELD_IS_COMPOUND(fieldName) return desc->getFieldIsCompound(self, fieldId)
#endif

  static cClassDescriptor *findDescriptor(cObject *p)
  {
      cClassDescriptor *desc = p->getDescriptor();
      if (!desc)
          opp_error("no descriptor for class %s", p->getClassName());
      return desc;
  }

  static int findField(cClassDescriptor *desc, void *object, const char *fieldName)
  {
      int n = desc->getFieldCount(NEWARG2(object));
      for (int i=0; i<n; i++)
          if (!strcmp(desc->getFieldName(NEWARG(object) i), fieldName))
              return i;
      return -1;
  }

  static int getFieldID(cClassDescriptor *desc, void *object, const char *fieldName)
  {
      int id = findField(desc, object, fieldName);
      if (id==-1)
          opp_error("no `%s' field in class %s", fieldName, desc->getName());
      return id;
  }
%}

%extend cObject {

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
      RETURN_FIELD_IS_ARRAY(fieldName);
  }

  bool isFieldCompound(const char *fieldName)
  {
      cClassDescriptor *desc = findDescriptor(self);
      AUTOPTR(desc);
      int fieldId = getFieldID(desc, self, fieldName);
      RETURN_FIELD_IS_COMPOUND(fieldName);
  }
}
