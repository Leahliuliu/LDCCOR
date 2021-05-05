//
// Generated file, do not edit! Created by nedtool 4.6 from initial.msg.
//

#ifndef _INITIAL_M_H_
#define _INITIAL_M_H_

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0406
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>initial.msg:2</tt> by nedtool.
 * <pre>
 * message InitialMessage //初始信息和hello消息，自己发给自己的，来开启整个程序
 * {
 *     int nodefrom;
 *     int msgid;
 * }
 * </pre>
 */
class InitialMessage : public ::cMessage
{
  protected:
    int nodefrom_var;
    int msgid_var;

  private:
    void copy(const InitialMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const InitialMessage&);

  public:
    InitialMessage(const char *name=NULL, int kind=0);
    InitialMessage(const InitialMessage& other);
    virtual ~InitialMessage();
    InitialMessage& operator=(const InitialMessage& other);
    virtual InitialMessage *dup() const {return new InitialMessage(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getNodefrom() const;
    virtual void setNodefrom(int nodefrom);
    virtual int getMsgid() const;
    virtual void setMsgid(int msgid);
};

inline void doPacking(cCommBuffer *b, InitialMessage& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, InitialMessage& obj) {obj.parsimUnpack(b);}


#endif // ifndef _INITIAL_M_H_

