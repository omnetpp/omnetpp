
%{
  static cClassDescriptor *findDescriptor(cPolymorphic *p)
  {
      cClassDescriptor *desc = p->getDescriptor();
      if (!desc)
          opp_error("no descriptor for class %s", p->className());
      return desc;
  }

  static int findField(cClassDescriptor *desc, void *object, const char *fieldName)
  {
      int n = desc->getFieldCount(object);
      for (int i=0; i<n; i++)
          if (!strcmp(desc->getFieldName(object, i), fieldName))
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
      return findField(desc, self, fieldName)!=-1;
  }

  std::string getField(const char *fieldName)
  {
      cClassDescriptor *desc = findDescriptor(self);
      int fieldId = getFieldID(desc, self, fieldName);
      char buf[200];
      desc->getFieldAsString(self, fieldId, 0, buf, 200);
      return buf;
  }

  std::string getArrayField(const char *fieldName, int index)
  {
      cClassDescriptor *desc = findDescriptor(self);
      int fieldId = getFieldID(desc, self, fieldName);
      char buf[200];
      desc->getFieldAsString(self, fieldId, index, buf, 200); //XXX check out of bounds!!!
      return buf;
  }

  void setField(const char *fieldName, const char *value)
  {
      cClassDescriptor *desc = findDescriptor(self);
      int fieldId = getFieldID(desc, self, fieldName);
      desc->setFieldAsString(self, fieldId, 0, value);
  }

  void setArrayField(const char *fieldName, int index, const char *value)
  {
      cClassDescriptor *desc = findDescriptor(self);
      int fieldId = getFieldID(desc, self, fieldName);
      desc->setFieldAsString(self, fieldId, index, value); //XXX check out of bounds!!!
  }

}



