//
// Generated file, do not edit! Created by nedtool 4.6 from selfmessage.msg.
//

#ifndef _SELFMESSAGE_M_H_
#define _SELFMESSAGE_M_H_

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0406
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>selfmessage.msg:2</tt> by nedtool.
 * <pre>
 * message SelfMessage //����Ϣ
 * {
 *     int nexthop;
 *     int nodefrom;
 *     bool isFinish; //�Ƿ�����Ҫ��������
 *     int lowDutyCycleId;
 * 
 *     int ETX;// ͳ�Ʒ��ʹ���
 *     double beginTime;
 *     double endTime;
 * 
 *     int group[50];
 *     int groupLength;
 * 
 *     int trace[50]; //�켣
 *     int traceLength; //�켣����
 * 
 *     int flag;
 * }
 * </pre>
 */
class SelfMessage : public ::cMessage
{
  protected:
    int nexthop_var;
    int nodefrom_var;
    bool isFinish_var;
    int lowDutyCycleId_var;
    int ETX_var;
    double beginTime_var;
    double endTime_var;
    int group_var[50];
    int groupLength_var;
    int trace_var[50];
    int traceLength_var;
    int flag_var;

  private:
    void copy(const SelfMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const SelfMessage&);

  public:
    SelfMessage(const char *name=NULL, int kind=0);
    SelfMessage(const SelfMessage& other);
    virtual ~SelfMessage();
    SelfMessage& operator=(const SelfMessage& other);
    virtual SelfMessage *dup() const {return new SelfMessage(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getNexthop() const;
    virtual void setNexthop(int nexthop);
    virtual int getNodefrom() const;
    virtual void setNodefrom(int nodefrom);
    virtual bool getIsFinish() const;
    virtual void setIsFinish(bool isFinish);
    virtual int getLowDutyCycleId() const;
    virtual void setLowDutyCycleId(int lowDutyCycleId);
    virtual int getETX() const;
    virtual void setETX(int ETX);
    virtual double getBeginTime() const;
    virtual void setBeginTime(double beginTime);
    virtual double getEndTime() const;
    virtual void setEndTime(double endTime);
    virtual unsigned int getGroupArraySize() const;
    virtual int getGroup(unsigned int k) const;
    virtual void setGroup(unsigned int k, int group);
    virtual int getGroupLength() const;
    virtual void setGroupLength(int groupLength);
    virtual unsigned int getTraceArraySize() const;
    virtual int getTrace(unsigned int k) const;
    virtual void setTrace(unsigned int k, int trace);
    virtual int getTraceLength() const;
    virtual void setTraceLength(int traceLength);
    virtual int getFlag() const;
    virtual void setFlag(int flag);
};

inline void doPacking(cCommBuffer *b, SelfMessage& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, SelfMessage& obj) {obj.parsimUnpack(b);}


#endif // ifndef _SELFMESSAGE_M_H_
