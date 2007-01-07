
%{
  static cClassDescriptor *findDescriptor(cPolymorphic *p)
  {
      cClassDescriptor *desc = p->getDescriptor();
      if (!desc)
          opp_error("no descriptor for class %s", p->className());
      return desc;
  }
  static int findField(cClassDescriptor *desc, const char *fieldName)
  {
      opp_error("no such field"); //XXX
      return -1;
  }
%}

%extend cPolymorphic {

  std::string getField(const char *fieldName)
  {
      cClassDescriptor *desc = findDescriptor(self);
      int fieldId = findField(desc, fieldName);
      char buf[200];
      desc->getFieldAsString(self, fieldId, 0, buf, 200);
      return buf;
  }

  std::string getArrayField(const char *fieldName, int index)
  {
      cClassDescriptor *desc = findDescriptor(self);
      int fieldId = findField(desc, fieldName);
      char buf[200];
      desc->getFieldAsString(self, fieldId, index, buf, 200); //XXX check out of bounds!!!
      return buf;
  }

  void setField(const char *fieldName, const char *value)
  {
      cClassDescriptor *desc = findDescriptor(self);
      int fieldId = findField(desc, fieldName);
      desc->setFieldAsString(self, fieldId, 0, value);
  }

  void setArrayField(const char *fieldName, int index, const char *value)
  {
      cClassDescriptor *desc = findDescriptor(self);
      int fieldId = findField(desc, fieldName);
      desc->setFieldAsString(self, fieldId, index, value); //XXX check out of bounds!!!
  }

}



