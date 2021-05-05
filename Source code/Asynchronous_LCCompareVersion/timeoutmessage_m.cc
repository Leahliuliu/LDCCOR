//
// Generated file, do not edit! Created by nedtool 4.6 from timeoutmessage.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "timeoutmessage_m.h"

USING_NAMESPACE


// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




// Template rule for outputting std::vector<T> types
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

Register_Class(timeoutMsg);

timeoutMsg::timeoutMsg(const char *name, int kind) : ::cMessage(name,kind)
{
    this->finish_var = 0;
    this->finishIndex_var = 0;
    this->dutyId_var = 0;
    this->nexthop_var = 0;
    this->nodefrom_var = 0;
    this->beginTime_var = 0;
    this->endTime_var = 0;
    for (unsigned int i=0; i<50; i++)
        this->group_var[i] = 0;
    this->groupLength_var = 0;
    for (unsigned int i=0; i<50; i++)
        this->trace_var[i] = 0;
    this->traceLength_var = 0;
    this->flag_var = 0;
}

timeoutMsg::timeoutMsg(const timeoutMsg& other) : ::cMessage(other)
{
    copy(other);
}

timeoutMsg::~timeoutMsg()
{
}

timeoutMsg& timeoutMsg::operator=(const timeoutMsg& other)
{
    if (this==&other) return *this;
    ::cMessage::operator=(other);
    copy(other);
    return *this;
}

void timeoutMsg::copy(const timeoutMsg& other)
{
    this->finish_var = other.finish_var;
    this->finishIndex_var = other.finishIndex_var;
    this->dutyId_var = other.dutyId_var;
    this->nexthop_var = other.nexthop_var;
    this->nodefrom_var = other.nodefrom_var;
    this->beginTime_var = other.beginTime_var;
    this->endTime_var = other.endTime_var;
    for (unsigned int i=0; i<50; i++)
        this->group_var[i] = other.group_var[i];
    this->groupLength_var = other.groupLength_var;
    for (unsigned int i=0; i<50; i++)
        this->trace_var[i] = other.trace_var[i];
    this->traceLength_var = other.traceLength_var;
    this->flag_var = other.flag_var;
}

void timeoutMsg::parsimPack(cCommBuffer *b)
{
    ::cMessage::parsimPack(b);
    doPacking(b,this->finish_var);
    doPacking(b,this->finishIndex_var);
    doPacking(b,this->dutyId_var);
    doPacking(b,this->nexthop_var);
    doPacking(b,this->nodefrom_var);
    doPacking(b,this->beginTime_var);
    doPacking(b,this->endTime_var);
    doPacking(b,this->group_var,50);
    doPacking(b,this->groupLength_var);
    doPacking(b,this->trace_var,50);
    doPacking(b,this->traceLength_var);
    doPacking(b,this->flag_var);
}

void timeoutMsg::parsimUnpack(cCommBuffer *b)
{
    ::cMessage::parsimUnpack(b);
    doUnpacking(b,this->finish_var);
    doUnpacking(b,this->finishIndex_var);
    doUnpacking(b,this->dutyId_var);
    doUnpacking(b,this->nexthop_var);
    doUnpacking(b,this->nodefrom_var);
    doUnpacking(b,this->beginTime_var);
    doUnpacking(b,this->endTime_var);
    doUnpacking(b,this->group_var,50);
    doUnpacking(b,this->groupLength_var);
    doUnpacking(b,this->trace_var,50);
    doUnpacking(b,this->traceLength_var);
    doUnpacking(b,this->flag_var);
}

bool timeoutMsg::getFinish() const
{
    return finish_var;
}

void timeoutMsg::setFinish(bool finish)
{
    this->finish_var = finish;
}

int timeoutMsg::getFinishIndex() const
{
    return finishIndex_var;
}

void timeoutMsg::setFinishIndex(int finishIndex)
{
    this->finishIndex_var = finishIndex;
}

int timeoutMsg::getDutyId() const
{
    return dutyId_var;
}

void timeoutMsg::setDutyId(int dutyId)
{
    this->dutyId_var = dutyId;
}

int timeoutMsg::getNexthop() const
{
    return nexthop_var;
}

void timeoutMsg::setNexthop(int nexthop)
{
    this->nexthop_var = nexthop;
}

int timeoutMsg::getNodefrom() const
{
    return nodefrom_var;
}

void timeoutMsg::setNodefrom(int nodefrom)
{
    this->nodefrom_var = nodefrom;
}

double timeoutMsg::getBeginTime() const
{
    return beginTime_var;
}

void timeoutMsg::setBeginTime(double beginTime)
{
    this->beginTime_var = beginTime;
}

double timeoutMsg::getEndTime() const
{
    return endTime_var;
}

void timeoutMsg::setEndTime(double endTime)
{
    this->endTime_var = endTime;
}

unsigned int timeoutMsg::getGroupArraySize() const
{
    return 50;
}

int timeoutMsg::getGroup(unsigned int k) const
{
    if (k>=50) throw cRuntimeError("Array of size 50 indexed by %lu", (unsigned long)k);
    return group_var[k];
}

void timeoutMsg::setGroup(unsigned int k, int group)
{
    if (k>=50) throw cRuntimeError("Array of size 50 indexed by %lu", (unsigned long)k);
    this->group_var[k] = group;
}

int timeoutMsg::getGroupLength() const
{
    return groupLength_var;
}

void timeoutMsg::setGroupLength(int groupLength)
{
    this->groupLength_var = groupLength;
}

unsigned int timeoutMsg::getTraceArraySize() const
{
    return 50;
}

int timeoutMsg::getTrace(unsigned int k) const
{
    if (k>=50) throw cRuntimeError("Array of size 50 indexed by %lu", (unsigned long)k);
    return trace_var[k];
}

void timeoutMsg::setTrace(unsigned int k, int trace)
{
    if (k>=50) throw cRuntimeError("Array of size 50 indexed by %lu", (unsigned long)k);
    this->trace_var[k] = trace;
}

int timeoutMsg::getTraceLength() const
{
    return traceLength_var;
}

void timeoutMsg::setTraceLength(int traceLength)
{
    this->traceLength_var = traceLength;
}

int timeoutMsg::getFlag() const
{
    return flag_var;
}

void timeoutMsg::setFlag(int flag)
{
    this->flag_var = flag;
}

class timeoutMsgDescriptor : public cClassDescriptor
{
  public:
    timeoutMsgDescriptor();
    virtual ~timeoutMsgDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(timeoutMsgDescriptor);

timeoutMsgDescriptor::timeoutMsgDescriptor() : cClassDescriptor("timeoutMsg", "cMessage")
{
}

timeoutMsgDescriptor::~timeoutMsgDescriptor()
{
}

bool timeoutMsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<timeoutMsg *>(obj)!=NULL;
}

const char *timeoutMsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int timeoutMsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 12+basedesc->getFieldCount(object) : 12;
}

unsigned int timeoutMsgDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<12) ? fieldTypeFlags[field] : 0;
}

const char *timeoutMsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "finish",
        "finishIndex",
        "dutyId",
        "nexthop",
        "nodefrom",
        "beginTime",
        "endTime",
        "group",
        "groupLength",
        "trace",
        "traceLength",
        "flag",
    };
    return (field>=0 && field<12) ? fieldNames[field] : NULL;
}

int timeoutMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='f' && strcmp(fieldName, "finish")==0) return base+0;
    if (fieldName[0]=='f' && strcmp(fieldName, "finishIndex")==0) return base+1;
    if (fieldName[0]=='d' && strcmp(fieldName, "dutyId")==0) return base+2;
    if (fieldName[0]=='n' && strcmp(fieldName, "nexthop")==0) return base+3;
    if (fieldName[0]=='n' && strcmp(fieldName, "nodefrom")==0) return base+4;
    if (fieldName[0]=='b' && strcmp(fieldName, "beginTime")==0) return base+5;
    if (fieldName[0]=='e' && strcmp(fieldName, "endTime")==0) return base+6;
    if (fieldName[0]=='g' && strcmp(fieldName, "group")==0) return base+7;
    if (fieldName[0]=='g' && strcmp(fieldName, "groupLength")==0) return base+8;
    if (fieldName[0]=='t' && strcmp(fieldName, "trace")==0) return base+9;
    if (fieldName[0]=='t' && strcmp(fieldName, "traceLength")==0) return base+10;
    if (fieldName[0]=='f' && strcmp(fieldName, "flag")==0) return base+11;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *timeoutMsgDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "bool",
        "int",
        "int",
        "int",
        "int",
        "double",
        "double",
        "int",
        "int",
        "int",
        "int",
        "int",
    };
    return (field>=0 && field<12) ? fieldTypeStrings[field] : NULL;
}

const char *timeoutMsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int timeoutMsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    timeoutMsg *pp = (timeoutMsg *)object; (void)pp;
    switch (field) {
        case 7: return 50;
        case 9: return 50;
        default: return 0;
    }
}

std::string timeoutMsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    timeoutMsg *pp = (timeoutMsg *)object; (void)pp;
    switch (field) {
        case 0: return bool2string(pp->getFinish());
        case 1: return long2string(pp->getFinishIndex());
        case 2: return long2string(pp->getDutyId());
        case 3: return long2string(pp->getNexthop());
        case 4: return long2string(pp->getNodefrom());
        case 5: return double2string(pp->getBeginTime());
        case 6: return double2string(pp->getEndTime());
        case 7: return long2string(pp->getGroup(i));
        case 8: return long2string(pp->getGroupLength());
        case 9: return long2string(pp->getTrace(i));
        case 10: return long2string(pp->getTraceLength());
        case 11: return long2string(pp->getFlag());
        default: return "";
    }
}

bool timeoutMsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    timeoutMsg *pp = (timeoutMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setFinish(string2bool(value)); return true;
        case 1: pp->setFinishIndex(string2long(value)); return true;
        case 2: pp->setDutyId(string2long(value)); return true;
        case 3: pp->setNexthop(string2long(value)); return true;
        case 4: pp->setNodefrom(string2long(value)); return true;
        case 5: pp->setBeginTime(string2double(value)); return true;
        case 6: pp->setEndTime(string2double(value)); return true;
        case 7: pp->setGroup(i,string2long(value)); return true;
        case 8: pp->setGroupLength(string2long(value)); return true;
        case 9: pp->setTrace(i,string2long(value)); return true;
        case 10: pp->setTraceLength(string2long(value)); return true;
        case 11: pp->setFlag(string2long(value)); return true;
        default: return false;
    }
}

const char *timeoutMsgDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *timeoutMsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    timeoutMsg *pp = (timeoutMsg *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


