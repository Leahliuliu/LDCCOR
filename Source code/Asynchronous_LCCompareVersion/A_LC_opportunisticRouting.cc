/*
 *  A_LC_opportunisticRouting.cc
 *
 *  Created on: 2020��9��11��
 *      Author: Lili Liu
 */
#include<omnetpp.h>
#include<iostream>
#include<stdio.h>
#include<iomanip>
#include<fstream>
#include<math.h>
#include<vector>
#include<string>
#include<map>
#include<algorithm>
#include<string.h>

#include"initial_m.h"
#include"code_m.h"
#include"selfmessage_m.h"
#include"timeoutmessage_m.h"

#define K 3  //��ѡת������Ϊ?��
#define N 50 //��?���ڵ�
#define dataPackNum 10 //�������ݰ��ĸ���
#define prepareTime 60
#define experienceTime 2
#define LQControl 0.7
#define matrixLen 1000
#define neighborDistance 220
#define CONST 1e6   //10�����η�
using namespace std;

//��Ϊȫ��ʹ��
double Lq[N][N]; //��·����
bool received[N][N][dataPackNum]; //n���ڵ��໥����dataPackNum��hello����Ϣ�Ľ������,�ڵ�����֮�������λͼ���
int Suc_Rate_GROUP[K]; //ÿһ�������ڵĳɹ�������

vector<int> groups[K]; //�������
vector<int> neighbour; //�ھӽڵ����
vector<int> candidate; //��ѡ�ھӽڵ����
vector<int> simulationVal;

/*
 * �Ա�ʵ��, LDC-AOR(���ڽڵ��첽����, ��������·�����)
 */

class Job1: public cSimpleModule {
private:
    timeoutMsg *timeoutEvent; //ȫ�ֶ����ط��¼�
    //SelfMessage *prepareOK; //��·��Ϣ�ռ��ú������Ϣ
    SelfMessage *lowdutycycle;

    int n;
    int size;
    int flag; //�ж��Ƿ��Ѿ����ھӽڵ㷢��10��hello���ݰ�
    int flag2;

    int counter;
    int mindist; //��С����
    int nodeNum; //�ڵ����
    int reception;
    int destination;
    int centerLength;
    int neighbourNum;
    int neighborLength;

    int count[N] = { 0 }; //��¼��ĳ���ڵ���յ�����Ϣ����
    int center[K] = { 0 };
    int neighbor[N] = { 0 }; //�ھӽڵ�
    int receivedNum[N] = { 0 }; //��ǰ�ڵ��i�����յ������ݰ�����

public:
    Job1();
    virtual ~Job1();

protected:
    virtual void initialize(); //��ʼ��
    virtual void handleMessage(cMessage *msg); //������Ϣ
    virtual InitialMessage *generateHelloMessage(int msgid); //����hello��Ϣ
    virtual void sendCopyOfInitial(InitialMessage *message, int i); //������Ϣ����
    virtual void deleteValInVector(int k);
    virtual bool findValInVector(int k);
    virtual void divideGroup(int index, int size); //�ھӽڵ㲻ͬ���麯��
    virtual void divideGroup1(int index, int size);
    virtual void divideGroup2(int index, double threshold); //������ֵ������
    virtual double calProbability(vector<int> groupI,int from); //������ʧ��
    virtual double prfk(vector<int> groupI, int node,int from); //������·�����
    virtual void finish(int transmission, int hop, double sumEnergy); //����
};

Define_Module(Job1); //ע��ģ��

Job1::Job1() { //������ʼ��ʱ
    timeoutEvent = NULL;
    lowdutycycle = NULL;

    memset(Lq, 0, sizeof(Lq)); //ȫ�ֳ�ʼ��
    memset(received, 0, sizeof(received));
    candidate.clear();
    simulationVal.clear();
    for(int i = 0;i < K;i++) groups[i].clear();
}
Job1::~Job1() { //��������ʱ
    cancelAndDelete(timeoutEvent);
    cancelAndDelete(lowdutycycle);
}

/*
 * ��ʼ��
 * */
void Job1::initialize() {
    lowdutycycle = new SelfMessage("lowDutyCycle"); //��ռ�ձ���Ϣ
    cModule *neighbour; //�Զ���ģ��ָ�����ͣ������ھ�
    //�������ʹ��
    int xpos, n_xpos, ypos, n_ypos;
    int dist = 0;
    mindist = 60;
    neighborLength = 0; //�ھ���Ŀ
    centerLength = 0; //��ʼ�����Ľڵ㳤��
    destination = N - 1; //Ŀ�ĵ�
    counter = 0;  //��������ȷ��������Ҫ�İ�����
    flag = 0;
    flag2 = 0;
    size = 0; //ÿ�������ڵĽڵ�������ֵ
    int i = 0;

    srand((unsigned) time(0)); //������������ĳ�ʼ������
    nodeNum = (int) getParentModule()->par("n"); //���ɴ������ڵ�ĸ���

    if(simulationVal.size() == 0){
        for(int s = 1;s <= experienceTime;s++){
            simulationVal.push_back(s); //����1��2  ��ʾ����2��prepareOK��Ϣ
        }
    }

    xpos = par("x"); //��������λ��ȡֵ
    ypos = par("y");
    EV << "Start******Initial******, the node ID is " << getIndex() << "\n";

    //������̾��루�ȳ�ʼ�������еĽڵ㣬Ȼ����о��������
    while (1) {
        bool succeed = true;
        for (i = 0; i < nodeNum; i++) {
            if (i == getIndex()) //���iΪ��ǰ�Ľڵ㣬�Ͳ�����
                continue;
            neighbour = getParentModule()->getSubmodule("telosb", i); //ȡ���ھӽڵ�
            n_xpos = neighbour->par("x");
            n_ypos = neighbour->par("y");
            //�ھӽڵ��뵱ǰ�ڵ��ŷ�Ͼ����ƽ��
            dist = (n_xpos - xpos) * (n_xpos - xpos) + (n_ypos - ypos) * (n_ypos - ypos);
            if (dist < mindist * mindist) {
                succeed = false;
                EV << getIndex() << " and " << i << " are too close! Rearrange them! " << "\n";
                break; //����forѭ��
            }
        }
        if (succeed) { //���еĽڵ��뵱ǰ�ڵ�ľ��붼���㲻С����С����
            par("x").setLongValue(xpos); //Ϊx��y���¸�ֵ
            par("y").setLongValue(ypos);
            break; //����while��ѭ��
        } else {
            xpos = rand() % matrixLen; //�������ȷֲ�����������µ����ýڵ��λ��
            ypos = rand() % matrixLen;
        }
    }

    //�������
    xpos = par("x"); //��ǰ����Ľڵ��λ��ȡֵ
    ypos = par("y");
    for (i = 0; i < getIndex(); i++) { //����ȵ�ǰ�ڵ�С�Ľڵ�
        neighbour = getParentModule()->getSubmodule("telosb", i);
        n_xpos = neighbour->par("x");
        n_ypos = neighbour->par("y");
        dist = (n_xpos - xpos) * (n_xpos - xpos) + (n_ypos - ypos) * (n_ypos - ypos);
        if (dist <= neighborDistance * neighborDistance) { //����С��220m��Ϊ�ھ�(ģ����ʵ�����ķ������
            par("neighbourNum").setLongValue((int) par("neighbourNum") + 1); //��ǰ�ڵ��ھ���+1
            neighbour->par("neighbourNum").setLongValue((int) neighbour->par("neighbourNum") + 1); //�ھӽڵ��ھ���+1

            EV << getIndex() << " and " << i << " are neighbors! Connect them!" << "\n";

            cDatarateChannel *channel1 = cDatarateChannel::create("channel"); //ģ���ӳ١������ʺ������ʵ�ͨ��
            cDatarateChannel *channel2 = cDatarateChannel::create("channel");
            cGate *gate1 = NULL;
            cGate *gate2 = NULL;
            channel1->setDatarate(250); //����ͨ�����������ʣ���λ/��Ϊ��λ
            channel1->setDelay(0.00);   //����ͨ���Ĵ����ӳ٣�����Ϊ��λ
            channel2->setDatarate(250);
            channel2->setDelay(0.00);

            //����ǰ�ڵ����ھӽڵ�ͨ��channel����������
            gate1 = this->gate("gate$o", i); //���
            gate2 = neighbour->gate("gate$i", getIndex()); //����
            gate1->connectTo(gate2, channel1); //ʹ�ø�����ͨ�����������ָ�����������ӵ���һ��
            channel1->callInitialize(); //ʵ�ֶԴ�ͨ������ִ�ж�׶γ�ʼ��
            channel1->getDisplayString().setTagArg("ls", 0, "blue"); //������ʾ�ַ��������ַ��������ڸ���ģ��ͼ���н�ģ����ʾΪ��ģ��ʱ�����

            gate1 = this->gate("gate$i", i);
            gate2 = neighbour->gate("gate$o", getIndex());
            gate2->connectTo(gate1, channel2);
            channel2->callInitialize();
            channel2->getDisplayString().setTagArg("ls", 0, "red");
        }
    }
    InitialMessage *msg = new InitialMessage("initial"); //������ʼ��Ϣ
    msg->setNodefrom(getIndex()); //��Ϣ��Դ������ǰ�ڵ�
    msg->setMsgid(0); //��ϢID 0,��ʾ��ʼ��Ҫ��hello������·�����������
    scheduleAt(simTime() + exponential(0.1), msg); //���Լ��Ķ�ʱ��Ϣ
}

/*
 * ��Ϣ����
 * */
void Job1::handleMessage(cMessage *msg) {
    int i, j, n, a, b;
    cModule *node; //�����е�ģ��
    int nodefrom = -1; //��¼��Ϣ�����ĸ��ڵ�
    int msgid; //��ϢID
    InitialMessage *hello;
    neighbourNum = par("neighbourNum"); //�ڵ���ھӸ����Ѿ��ڳ�ʼ����ʱ��������
    EV << "Now, the node ID is " << getIndex() << "\n";

    if (msg->isSelfMessage()) { //���������Ϣ�Ļ�
        /*
         * ��ʼ���׶Σ�ÿ���ڵ㷢 dataPackNum��hello��
         * */
        if (strcmp(msg->getName(), "initial") == 0) {
            if (flag == 0) {
                hello = generateHelloMessage(counter++); //counter��ʼ��Ϊ0
                n = gateSize("gate");  //�ŵĸ���
                for (i = 0; i < n; i++) {
                    cGate *t = this->gate("gate$o", i);
                    if (t->isConnected()) { //ͨ�����hello��Ϣ�����п���ͨ�������
                        sendCopyOfInitial(hello, i); //�����е��ھ��Ƿ���hello��Ϣ
                    }
                }
                delete hello;
                if (counter < dataPackNum) { //��û����
                    InitialMessage *msg = new InitialMessage("initial"); //��Ϊinitial��Ϣ
                    msg->setNodefrom(getIndex());
                    msg->setMsgid(0); //��ϢID��Ϊ0
                    scheduleAt(simTime() + exponential(0.5), msg); //���Լ��ٷ�һ������Ϣ���Ӷ��ٸ���Χ�ھ��Ƿ�һ��hello��
                }
                if (counter == dataPackNum) {
                    /*
                     * ������㶨Ϊ0����0�ڵ㷢�����ݰ���Ŀ�ĵ�N-1�ڵ�
                     *  1. ���Ŀ�Ľڵ����0�ڵ���ھӣ���ֱ��һ�����;
                     *  2. ������ѡ����һ���ڵ㣬��������ת����֪�����ݰ����ɹ�ת����Ŀ�Ľڵ�Ϊֹ.
                     */
                    flag = 1;
                    if (getIndex() == 0) { //traceͳ�ƴ�0��ʼ��·������������[���0���յ�n-1]
                        int nodefrom = getIndex();
                        int traceLength = 1;
                        int trace[traceLength];
                        trace[0] = nodefrom; //��Դ�ڵ���ӵ��켣������ trace[0] = 0;

                        SelfMessage *prepareOK = new SelfMessage("prepareOK"); //new�µ��ط��¼�
                        prepareOK->setTraceLength(traceLength);
                        for (j = 0; j < traceLength; j++) {
                            prepareOK->setTrace(j, trace[j]);
                        }
                        prepareOK->setLowDutyCycleId(0);
                        EV << "Send prepareOK message" << "\n";
                        scheduleAt(prepareTime, prepareOK); //prepareTime����prepareOK��Ϣ ����������0�ڵ㿪ʼת��
                    }
                }
            }
        }

        /*
         * �鿴�����ÿ���ڵ����helloMessage�Ľ������, ��������ETX
         * ĳһ�ڵ�ȫ����Ϣ�������֮��Ż����ھ���Ϣ�ķ���
         * */
        else if (strcmp(msg->getName(), "neighInformation") == 0) {
            SelfMessage *message = check_and_cast<SelfMessage *>(msg); //ǿ������ת��
            nodefrom = message->getNodefrom();
            for (i = 0; i < dataPackNum; i++) {
                EV << "The dataBits from " << nodefrom << " to " << getIndex() << " = " << received[nodefrom][getIndex()][i] << "\n";
            }
            EV << "The linkQuality from " << nodefrom << " to " << getIndex() << " = " << Lq[nodefrom][getIndex()] << "\n";
        }

        /**
         * �õ��ڵ���ڽڵ㡢��ʼ���������ġ�������ڵ�, ���з������ ,prepare�ǳ�ʼ������һЩ׼�����������ڽڵ㣬���Ľڵ�Ĳ���
         */
        else if (strcmp(msg->getName(), "prepareOK") == 0) {
            SelfMessage *message = check_and_cast<SelfMessage *>(msg); //ǿ������ת��
                nodefrom = message->getNodefrom();
                int dutyId = message->getLowDutyCycleId();
                int traceLength = message->getTraceLength();
                EV << "Now, the traceLength: " << traceLength << "\n";
                int trace[traceLength];
                for (i = 0; i < traceLength; i++) {
                    trace[i] = message->getTrace(i); //·������  trace[0] = 0
                }
                EV << "In duty " << dutyId << ", in prepareOKMsg, the trace is: "; //��ǰtrace·���еĽڵ�
                for (i = 0; i < traceLength; i++) {
                     EV << trace[i] << "  ";
                }
                EV << "\n";

                //ȥ����·��������trace�Ľڵ�Ͳ��ٽ��������ھӽڵ������У���ֹ�ظ��㲥
                n = gateSize("gate");
                for (i = 0; i < n; i++) {
                    int flag2 = 0;
                    for (j = 0; j < traceLength; j++) {
                        if (i == trace[j]) {
                            flag2 = 1;
                            break;
                        }
                    }
                    if (flag2 == 1) {
                        continue;
                    }
                    cGate *t = this->gate("gate$o", i); //����trace�Ľڵ�Ͳ��ٽ��������ھӽڵ������У���ֹ�ظ��㲥
                    neighbour.clear();
                    if (t->isConnected()) {
                        neighbor[neighborLength++] = i; //��ȡ�ڽڵ�
                    }
                }
                if(neighborLength > 0){ //�ýڵ�û���ھ�ʱ���򲻽��д���
                    EV << "Then, for " << getIndex() << " the neighbor is: "; //�ھӽڵ����
                    for (i = 0; i < neighborLength; i++) {
                         EV << neighbor[i] << "  ";
                         neighbour.push_back(neighbor[i]); //�ھӽڵ�vector
                    }
                    EV << "\n";

                    if (neighborLength % K == 0) { //���ݷ��ͽڵ��һ���ڽڵ����ȷ��ÿ�����������ܰ��������ڵ����
                        size = neighborLength / K;
                    } else {
                        size = neighborLength / K + 1;
                    }
                    EV << "For " << getIndex() << ", the neighborLength is: " << neighborLength << "\n"; //���0�ڵ���ھӳ���
                    //EV << "For " << getIndex() << ", the number of nodes in each group is: " << size << "\n"; //���0�ڵ�ķ����ڵĽڵ����

                    for (i = 0; i < neighborLength; i++) { //ѡ�����򡪡����ڵ���ھӽڵ����Ȱ�����·�������дӴ�С������,��ȡǰK��Ϊ�ܵľ�������
                        for (j = i + 1; j < neighborLength; j++) {
                            double lqi = Lq[nodefrom][neighbor[i]];
                            double lqj = Lq[nodefrom][neighbor[j]];
                            if (lqi < lqj) {
                                int temp = neighbor[i];
                                neighbor[i] = neighbor[j];
                                neighbor[j] = temp;
                            }
                        }
                    }
                    for (i = 0; i < neighborLength; i++) {
                        EV << "After exchange: node " << getIndex() << ", the neighbor is: " << neighbor[i] << "\n";//���������·�����ź����ĵ�ǰ�ڵ�������ھ�
                    }

                    if (neighborLength >= K) { //����������
                        for (i = 0; i < K; i++) {
                            center[centerLength++] = neighbor[i]; //��·��������ǰK�Ľڵ�ֱ�ָ�K����
                        }
                    } else {
                        for (i = 0; i < neighborLength; i++) {
                            center[centerLength++] = neighbor[i]; //����������ֻ��ΪneighborLength���飬�����޺�������
                        }
                    }

                    if (neighborLength > K) { //�����ú�ѡת�����ϡ� ���ڽڵ�ĸ�������Kʱ��ȡ������ڵ�ļ��ϣ����ڽڵ�ĸ���С��Kʱ��û�д�����ڵ�
                        for (i = 0; i < neighborLength; i++) {
                            int tag = 0;
                            for (j = 0; j < K; j++) {
                                if (neighbor[i] == center[j]) {
                                    tag = 1;
                                    break;
                                }
                            }
                            if (tag == 0) { //�ų����Ѿ�����õ����ĵ㣬ͳ�ƺ����µĺ�ѡת������
                                candidate.push_back(neighbor[i]);
                            }
                        }
                        EV << "The candidate is: "; //���������ڵ�����
                        for (i = 0; i < candidate.size(); i++) {
                             EV << candidate[i] << "  ";
                        }
                        EV << "\n";
                    }

                    for (i = 0; i < K; i++) { //����ǰ�����
                        groups[i].clear();
                    }
                    for (i = 0; i < centerLength; i++) { //��ÿ��������ӳ�ʼ�ڵ�
                        groups[i].push_back(center[i]);
                        EV << "The node of group " << i << " is " << groups[i][0] << " ,length " << i << " is " << groups[i].size() << "\n";
                    }

                    /*����candidate�����еĽڵ���з��顿
                     * 1�����Ƚ�candidate�еĴ������ڵ���ӵ�K�������������������--�Ա����ýڵ�ͬK�������ETX
                     * 2��Ȼ�󽫸ýڵ��������ɾ��-- �ָ�ԭ��
                     * 3�����ýڵ���ӵ�ͬ�ýڵ������СETX��������
                     */
                    if (candidate.size() > 0) {
                        divideGroup1(getIndex(),size); //�������

                        EV << "End Group:" << "\n";
                        for (i = 0; i < K; i++) { //���ÿ�����������еĽ��
                            EV << "group " << i << ": ";
                            for (j = 0; j < groups[i].size(); j++) {
                                EV << groups[i][j] << "  ";
                            }
                            EV << "\n";
                        }
                    } else{
                        EV << "End Group:" << "\n";
                        for (i = 0; i < neighborLength; i++) { //���ÿ�����������еĽ��
                            EV << "group " << i << ": ";
                            for (j = 0; j < groups[i].size(); j++) {
                                EV << groups[i][j] << "  ";
                            }
                            EV << "\n";
                        }
                    }

                    //�����ж������ھӷ���ڵ����Ƿ���Ŀ��ڵ�
                    int lowIndex = -1; //��¼Ŀ��ڵ����ڵļ���
                    int has = false;
                    for(i = 0;i < K;i++){
                        if (groups[i].size() > 0) {
                            for(j = 0; j < groups[i].size();j++){
                                if (groups[i][j] == destination) {
                                    has = true; //�ж��ڽڵ����Ƿ���Ŀ��ڵ�
                                    lowIndex = i;
                                }
                            }
                        }
                    }

                    /*��ģ���ռ�ձ�ģ�͡� */
                    double beginTime = simTime().dbl() + 1; // ����ʱ��SimTimeת����˫����double����,��dbl()����ת��
                    EV << "BeginTime in spot [prepareOK] is " << beginTime << "\n";

                    if(has){ //��ĳһ�����麬��Ŀ��ڵ�,��ֻ������һ��Ľڵ���Ϣ����
                        for (i = 0; i < centerLength; i++) {
                            if(i == lowIndex){
                                EV << "has = true" << "\n";
                                SelfMessage *copy = lowdutycycle->dup(); //���������ش˶���ľ�ȷ����
                                copy->setLowDutyCycleId(i + 1);
                                copy->setBeginTime(beginTime);
                                copy->setNodefrom(getIndex());
                                copy->setGroupLength(groups[i].size());
                                for (j = 0; j < groups[i].size(); j++) {
                                    copy->setGroup(j, groups[i][j]);
                                    EV << "groups[i][j]" << groups[i][j] << "\n";
                                }
                                copy->setTraceLength(traceLength);
                                for (j = 0; j < traceLength; j++) {
                                    copy->setTrace(j, trace[j]);
                                    EV << "trace[j]" << trace[j] << "\n";
                                }
                                scheduleAt(simTime() + 1, copy); //��lowdutyCycle����Ϣ
                            }
                        }
                    }else{ //���򵼳����еĲ�Ϊ0�ķ���
                        for (i = 0; i < centerLength; i++) {
                              if (groups[i].size() > 0)
                                  EV << "has = false" << "\n";
                                  EV << "this is the " << i + 1 << " time spot" << "\n";
                                  SelfMessage *copy = lowdutycycle->dup(); //���������ش˶���ľ�ȷ����
                                  copy->setLowDutyCycleId(i + 1);
                                  copy->setBeginTime(beginTime);
                                  copy->setNodefrom(getIndex());
                                  copy->setGroupLength(groups[i].size());
                                  EV << "group: ";
                                  for (j = 0; j < groups[i].size(); j++) {
                                      copy->setGroup(j, groups[i][j]);
                                      EV << groups[i][j] << " ";
                                  }
                                  EV << "\n";
                                  EV << "trace: ";
                                  copy->setTraceLength(traceLength);
                                  for (j = 0; j < traceLength; j++) {
                                      copy->setTrace(j, trace[j]);
                                      EV << trace[j] << " ";
                                  }
                                  EV << "\n";
                                  scheduleAt(simTime() + (2 * (i + 1) - 1), copy); //��lowdutyCycle����Ϣ
                          }
                    }
                }
                // ��������
                neighborLength = 0;
                centerLength = 0;
                candidate.clear();
                neighbour.clear();
        }

        /*
         * ���յ���ռ�ձ���Ϣ��������Ϣ�ڷ��鼯�ϣ��ڵ�ǰʱ��ռ�ձ��¹㲥��Ϣ
         * */
        else if (strcmp(msg->getName(), "lowDutyCycle") == 0) {
            SelfMessage *message = check_and_cast<SelfMessage *>(msg); //ǿ��ת������Ϊ����Ϣ
                SelfMessage *data = message->dup(); //����
                data->setName("data"); //��������,�µ����ݰ�������������һ���ڵ�
                double beginTime = message->getBeginTime();
                int dutyId = message->getLowDutyCycleId();
                EV << "[[***********lowDutyCycle**********]] BeginTime = " << beginTime << " ,in " << dutyId << " TIME SPOT" << "\n";

                for (i = 0; i < K; i++) {
                    if (dutyId == (i + 1)) {
                        SelfMessage *copy = (SelfMessage *) data->dup(); //data��Ϣ�ĸ���

                        //ת������
                        int groupLength = message->getGroupLength(); //��ǰ������Ԫ�ؼ���
                        int group[groupLength];
                        for (j = 0; j < groupLength; j++) {
                            group[j] = message->getGroup(j);
                        }
                        EV << "Nodes in group " << i << ": ";
                        for (j = 0; j < groupLength; j++) {
                            EV << group[j] << "  ";
                        }
                        EV << "\n";

                        //�켣����
                        int traceLength = message->getTraceLength();
                        int trace[traceLength];
                        for (j = 0; j < traceLength; j++) {
                            trace[j] = message->getTrace(j);
                        }
                        EV << "Nodes in trace " << ": ";
                        for (j = 0; j < traceLength; j++) {
                            EV << trace[j] << "  ";
                        }
                        EV << "\n";

                        bool result = false; //�ж�Ŀ��ڵ��Ƿ��ڵ�ǰ������
                        for (j = 0; j < groupLength; j++) {
                            if (group[j] == destination) {
                                result = true;
                            }
                        }
                        EV << "has destination: " << result << "\n";

                        if (result) { //���Ŀ��ڵ��ڸü����У���ôһ�ξ��ܷ�����ϣ�ֱ�Ӹ�Ŀ��ڵ㷢����Ϣ
                            par("transmission").setLongValue((int) par("transmission") + 1);
                            par("send").setLongValue((int) par("send") + 1);
                            copy->setIsFinish(true); //�ڴ˽�������
                            send(copy, "gate$o", destination); //��SelfMessage��Ϣ���͸�destination

                        } else {
                            par("transmission").setLongValue((int) par("transmission") + 1);
                            par("send").setLongValue((int) par("send") + 1);
                            int wakeNodeNum; //�����Ľڵ���
                            wakeNodeNum = groupLength % 2 ? groupLength/2 + 1 : groupLength/2;
                            //wakeNodeNum = groupLength;

                            for (j = 0; j < wakeNodeNum; j++) { //����ѡת�����Ϲ㲥��Ϣ
                                SelfMessage *copy1 = (SelfMessage *) copy->dup();
                                copy1->setIsFinish(false); //��û�н�������
                                send(copy1, "gate$o", group[j]); //��ǰ����Ⱥ��SelfMessage������Ϣ
                            }
                        }
                    }
                }
        }

    } else {
        /*
         * �յ�hello��Ϣʱ�� ��¼�ھӽڵ�id������¼�հ�������Լ������нڵ����·��������·�����
         * */
        if (strcmp(msg->getName(), "hello") == 0) {
            InitialMessage *message = check_and_cast<InitialMessage *>(msg); //ǿ������ת��
            nodefrom = message->getNodefrom();
            count[nodefrom]++; //���Ե�ǰnodeFrom�����ݰ�����
            msgid = message->getMsgid();
            if (uniform(0, 1) < LQControl) {  //LQControl�ĸ������յ�hello��
                received[nodefrom][getIndex()][msgid] = 1;
                receivedNum[nodefrom] += 1;  //����nodeFrom�����ݰ��ɹ��յ��ĸ���
            } else {
                bubble("message lost"); //�������ʧ������
            }
            if (count[nodefrom] == dataPackNum) { //�յ������͵Ĵ����㹻
                EV << "Now, the index is " << getIndex() << " have enough dataPackages from " << nodefrom << "\n";
                Lq[nodefrom][getIndex()] = receivedNum[nodefrom] * 1.0/ dataPackNum; //��·��������
                SelfMessage *msg = new SelfMessage("neighInformation"); //�����ھ���Ϣ��Ϣ
                msg->setNodefrom(nodefrom);
                scheduleAt(simTime() + 0.1, msg);
            }
        }

        /*
         * ������鼯���ڵĽ����ʵ���Ϣ
         * */
        else if (strcmp(msg->getName(), "data") == 0) { //�����鷢����Ϣ,���ݱ������ھ��յ�
            SelfMessage *message = check_and_cast<SelfMessage *>(msg);
                SelfMessage *ack = new SelfMessage("ACK"); //�򸸽ڵ㷢��ȷ�ϰ�
                int dutyId = message->getLowDutyCycleId();
                bool res = message->getIsFinish(); //�Ƿ��͸��յ㣬��Ҫ�ж�
                int nodefrom = message->getNodefrom();
                double beginTime = message->getBeginTime(); //���׽ڵ�ķ���ʱ��

                int groupLength = message->getGroupLength();
                vector<int> tmp; //�ü�����ȫ���ڵ�
                for (j = 0; j < groupLength; j++) {
                    tmp.push_back(message->getGroup(j));
                }
                int traceLength = message->getTraceLength();
                int trace[traceLength];  //��ȡ·��
                for (j = 0; j < traceLength; j++) {
                    trace[j] = message->getTrace(j);
                }

                int temp[N]; //ͳ�Ƴɹ����յ����ݰ��Ľڵ�
                int k = 0;

                for (i = 0; i < K; i++) {
                    if (dutyId == (i + 1)) {
                        double probability = 0; // ������������һ���ڵ��յ����ݰ��ĸ���
                        EV << "This group: "; //�����ǰ����������нڵ���Ϣ
                        for (j = 0; j < groupLength; j++) {
                            EV << "Node: " << tmp[j] << " ";
                            EV << "LinkQuality: " << Lq[nodefrom][tmp[j]] << " ";
                        }
                        EV << "\n";
                        probability = calProbability(tmp,nodefrom); //���㵱ǰ��������һ���ڵ��յ����ݰ��ĸ���
                        EV << "probability: " << probability << "\n";
                        EV << "The dutyId is " << dutyId << " and the lost probability is " << (1 - probability) << "\n";
                        if (uniform(0, 1) < (1 - probability)) { //��Ϣ����ʧ��
                            EV << "Node " << getIndex() << "\"Losing\" DATA.\n"; //û���յ����ݰ�
                            bubble("Data lost");
                            delete msg;

                            //��ǰ�����е�Ԫ���Ƿ��Ѿ���������������ִ�г���
                            if (getIndex() == tmp[groupLength - 1]) {
                                EV << "\n" << "Now, last node in this group lose DATA" << "\n";
                                for (j = 0; j < groupLength; j++) {
                                    node = getParentModule()->getSubmodule("telosb", tmp[j]);
                                    bool result = node->par("isReceived");
                                    if (result) {
                                        temp[k++] = tmp[j];
                                    }
                                }
                                if (k > 0) {
                                    EV << "In duty " << (i + 1) << " , the receivedData node is ";
                                    for (j = 0; j < k; j++) {
                                        EV << temp[j] << "  ";
                                    }
                                    EV << "\n";

                                    int index = intuniform(0, k - 1); //���յ������ݰ�������ѡ��һ���ڵ���Ϊ��һ��
                                    EV << "In duty " << i + 1 << " We select the node " << temp[index] << " as next hop" << "\n";
                                    SelfMessage *ackCopy = ack->dup();
                                    ackCopy->setIsFinish(false); //δ����
                                    ackCopy->setLowDutyCycleId(i + 1);
                                    ackCopy->setBeginTime(beginTime); //��ʼ����ʱ��
                                    ackCopy->setNodefrom(getIndex());
                                    ackCopy->setNexthop(temp[index]);

                                    trace[traceLength++] = temp[index]; //trace���¸�ֵ
                                    ackCopy->setTraceLength(traceLength);
                                    for (j = 0; j < traceLength; j++) {
                                        ackCopy->setTrace(j, trace[j]);
                                    }
                                    send(ackCopy, "gate$o", nodefrom); //��Դ�ڵ㷢��ȷ�ϰ���ͨ��ACK��Ϣ֪ͨ���ڵ� ��һ����Ϣ
                                    for (j = 0; j < groupLength; j++) {
                                        node = getParentModule()->getSubmodule("telosb", tmp[j]);
                                        node->par("isReceived").setBoolValue(false);//�ָ�Ĭ��
                                    }
                                    k = 0; //�ָ�Ĭ��

                                } else{ //��ʾ���������нڵ㶼û���յ����ݰ�
                                   EV << "No node received the data in this group" << "\n";
                                   cancelAndDelete(timeoutEvent); //ɾ��֮ǰ�Ķ���
                                   timeoutEvent = new timeoutMsg("timeoutEvent"); //new�µ��ط��¼�
                                   timeoutEvent->setDutyId(i+1);
                                   timeoutEvent->setNodefrom(nodefrom);
                                   timeoutEvent->setBeginTime(beginTime);
                                   timeoutEvent->setGroupLength(groupLength);
                                   for (j = 0; j < groupLength; j++) {
                                       timeoutEvent->setGroup(j, tmp[j]);
                                   }
                                   timeoutEvent->setTraceLength(traceLength);
                                   for (j = 0; j < traceLength; j++) {
                                       timeoutEvent->setTrace(j, trace[j]);
                                   }
                                   while (!timeoutEvent->isScheduled()) { //��ǰʱ��Ƭ���ǹ���״̬
                                       EV << "*****************timeoutEvent --> Resent the dataMsg*****************" << "\n";
                                       EV << "nodeFrom:" << nodefrom << "\n";
                                       EV << "groupLength" << groupLength << "\n";
                                       EV <<"groupNode: ";
                                       for (j = 0; j < groupLength; j++) {
                                          EV << tmp[j] << "  ";
                                       }
                                       EV << "\n";
                                       send(timeoutEvent, "gate$o", nodefrom);
                                   }
                                }
                            }

                        } else { //��Ϣ���ճɹ�
                            par("isReceived").setBoolValue(true); //ȷ���յ����ݰ�
                            par("received").setLongValue((int) par("received") + 1); //���յ����ݰ��ĸ���

                            if (res) {//��Ϣ����Ŀ�ĵ�
                                EV << "The Destination node received packet!!!" << "\n";
                                delete msg;
                                // ��Ŀ��ڵ��յ����ݰ�
                                SelfMessage *ackCopy = ack->dup();
                                ackCopy->setIsFinish(true); //�ѽ���
                                ackCopy->setLowDutyCycleId(i + 1);
                                ackCopy->setBeginTime(beginTime);
                                ackCopy->setNodefrom(getIndex());
                                ackCopy->setNexthop(destination);

                                trace[traceLength++] = destination; //�����յ�
                                ackCopy->setTraceLength(traceLength);
                                for (j = 0; j < traceLength; j++) {
                                    ackCopy->setTrace(j, trace[j]);
                                }
                                send(ackCopy, "gate$o", nodefrom); //��Դ�ڵ㷢��ȷ�ϰ�������Ŀ�ĵ�
                                // �ָ�Ĭ��ֵ
                                for (j = 0; j < groupLength; j++) {
                                    node = getParentModule()->getSubmodule("telosb", tmp[j]);
                                    node->par("isReceived").setBoolValue(false);
                                }
                            }

                            else { //�յ���Ϣ��������Ϣδ���Ŀ�ĵء�����Ҫ��������
                                EV << "node " << getIndex() << " received DataMsg from " << nodefrom << "\n";
                                //��ǰ�����е�Ԫ���Ƿ��Ѿ���������������ִ�г���
                                if (getIndex() == tmp[groupLength - 1]) {
                                    EV << "\n" << "Now, handle the last node in this group:" << "\n";
                                    for (j = 0; j < groupLength; j++) {
                                        node = getParentModule()->getSubmodule("telosb", tmp[j]);
                                        bool result = node->par("isReceived");
                                        if (result) {
                                            temp[k++] = tmp[j];
                                        }
                                    }
                                    if (k > 0) {
                                        EV << "In duty " << (i + 1) << " , the receivedData node is ";
                                        for (j = 0; j < k; j++) {
                                            EV << temp[j] << "  ";
                                        }
                                        EV << "\n";
                                        int index = intuniform(0, k - 1); //���յ������ݰ�������ѡ��һ���ڵ���Ϊ��һ��
                                        EV << "In duty " << i + 1 << " We select the node " << temp[index] << " as next hop" << "\n";
                                        SelfMessage *ackCopy = ack->dup();
                                        ackCopy->setIsFinish(false);
                                        ackCopy->setLowDutyCycleId(i + 1);
                                        ackCopy->setBeginTime(beginTime); //��ʼ����ʱ��
                                        ackCopy->setNodefrom(getIndex());
                                        ackCopy->setNexthop(temp[index]);
                                        trace[traceLength++] = temp[index]; //trace���¸�ֵ
                                        ackCopy->setTraceLength(traceLength);
                                        for (j = 0; j < traceLength; j++) {
                                            ackCopy->setTrace(j, trace[j]);
                                        }
                                        send(ackCopy, "gate$o", nodefrom); //��Դ�ڵ㷢��ȷ�ϰ�

                                        for (j = 0; j < groupLength; j++) {
                                            node = getParentModule()->getSubmodule("telosb", tmp[j]);
                                            node->par("isReceived").setBoolValue(false); //�ָ�Ĭ��
                                        }
                                        k = 0; //�ָ�Ĭ��
                                    }
                                }
                            }
                        }
                    }
                }
        }

        /**
         * �����ط�����,��ĳһ����ȫû���յ����ݵķ��������·��ڵ�
         */
        else if (strcmp(msg->getName(), "timeoutEvent") == 0) {
            timeoutMsg *message = check_and_cast<timeoutMsg *>(msg);
                SelfMessage *data = new SelfMessage("data"); //����data��Ϣ
                bool res = false;
                int dutyId = message->getDutyId();
                for(i = 0;i < K;i++){
                    if(dutyId == (i+1)){
                        double beginTime = message->getBeginTime();
                        int nodefrom = message->getNodefrom();
                        data->setBeginTime(beginTime);
                        data->setLowDutyCycleId(i+1);
                        data->setNodefrom(nodefrom);
                        EV << "TimeoutEvent:  in spot "<< i+1 << ", reSend DATA message" << "\n";

                        int groupLength = message->getGroupLength();
                        data->setGroupLength(groupLength);
                        int group[groupLength];
                        for (j = 0;j < groupLength; j++) {
                            group[j] = message->getGroup(j);
                            data->setGroup(j, message->getGroup(j));
                        }
                        int traceLength = message->getTraceLength();
                        data->setTraceLength(traceLength);
                        int trace[traceLength];
                        for(j = 0;j < traceLength;j++){
                            trace[j] = message->getTrace(j);
                            data->setTrace(j, message->getTrace(j));
                        }

                        for (j = 0; j < groupLength; j++) { //�ж�Ŀ��ڵ��Ƿ�����һ��
                            if (group[j] == destination) {
                                res = true;
                            }
                        }
                        if (res) { //���Ŀ��ڵ��ڸü����У���ôһ�ξ��ܷ������
                            par("transmission").setLongValue((int) par("transmission") + 1);
                            par("send").setLongValue((int) par("send") + 1);
                            data->setIsFinish(true);
                            send(data, "gate$o", destination);
                        } else {
                            par("transmission").setLongValue((int) par("transmission") + 1);
                            par("send").setLongValue((int) par("send") + 1);
                            int wakeNodeNum; //�����Ľڵ���
                            wakeNodeNum = groupLength % 2 ? groupLength/2 + 1 : groupLength/2;
                            //wakeNodeNum = groupLength;

                            for (j = 0; j < wakeNodeNum; j++) { //����ѡת�����Ϲ㲥��Ϣ
                                SelfMessage *copy = (SelfMessage *) data->dup();
                                copy->setIsFinish(false);
                                send(copy, "gate$o", group[j]);
                            }
                        }
                    }
                }
        }

        /*
         * ���ڵ����ȷ����ϢACK����
         * */
        else if (strcmp(msg->getName(), "ACK") == 0) {
            SelfMessage *message = check_and_cast<SelfMessage *>(msg);
                int dutyId = message->getLowDutyCycleId();
                int traceLength = message->getTraceLength();
                int trace[traceLength];  //��ȡ·��
                for (j = 0; j < traceLength; j++) {
                    trace[j] = message->getTrace(j);
                }
                int nodefrom = message->getNodefrom();
                cModule *finalCount;
                cModule *temp;
                EV << "Now, " << getIndex() << " received ACKMsg from " << nodefrom << "\n";
                double beginTime = message->getBeginTime(); //����ʱ��
                EV << "The beginTime is " << beginTime << "\n";
                double endTime = simTime().dbl(); //��ǰʱ��,��Ϊ�յ�ACK���ݰ���ʱ��
                EV << "The endTime is " << endTime << "\n";
                double waitingTime = endTime - beginTime; //�ȴ�ʱ��----��Ϊʱ��Ƭ��ԭ��
                EV << "The waitingTime is " << waitingTime << "\n";
                if (waitingTime > 0) {
                    par("waitingTime").setDoubleValue((double) par("waitingTime") + waitingTime);
                }
                for (i = 0; i < K; i++) {
                    if (dutyId == (i + 1)) {
                        bool res = message->getIsFinish(); //�ж��Ƿ�������Ŀ��ڵ��ACK

                        if (res) { //Ŀ��ڵ��յ����ݰ�����������
                            EV << "finish program !" << "\n";
                            int all = 0;
                            for (j = 0; j < traceLength; j++) {
                                int lujing = message->getTrace(j);
                                EV << "The trace is " << lujing << "\n";
                                temp = getParentModule()->getSubmodule("telosb",lujing);
                                int total = temp->par("transmission");
                                all += total;
                            }
                            EV << "The truly ETX is " << all << "\n";
                            EV << "All need " << all << " transmission ! !" << "\n"; //ͳ���ܹ���ת������

                            double sumEnergy = 0;
                            for (j = 0; j < N; j++) { //���N���ڵ�
                                finalCount = getParentModule()->getSubmodule("telosb", j);
                                int send = finalCount->par("send");
                                int received = finalCount->par("received");
                                double waitingTime = finalCount->par("waitingTime");
                                if (send != 0 || received != 0 || waitingTime != 0) {
                                    EV << "Node " << finalCount->getIndex() << " send " << send << " packets, "
                                      "and received " << received << " packets, wait "<< waitingTime << " seconds" << "\n";
                                    sumEnergy = sumEnergy + send * 3 * 0.01 * 4.8; /*�������ļ���  ��̫����.���͡����ա�����*/
                                    sumEnergy = sumEnergy + received * 3 * 0.009 * 4.8;
                                    sumEnergy = sumEnergy + waitingTime * 0.00002;
                                }
                            }
                            EV << "The sumEnergy is " << sumEnergy << "\n";
                            cancelEvent(lowdutycycle);

                            finish(all, traceLength - 1, sumEnergy); //�����������������������������ǰִ�е�ʵ�鼸
                            endSimulation();
                        }
                        else { //����Ŀ��ڵ��ack,�Ͱ���ack��Ϣ�ϴ��е���һ����Ϣ����ת��
                            SelfMessage *nextHop = new SelfMessage("nexthop");
                            nextHop->setLowDutyCycleId(i + 1);
                            nextHop->setTraceLength(traceLength);
                            for (j = 0; j < traceLength; j++) {
                                nextHop->setTrace(j, message->getTrace(j));
                            }
                            send(nextHop, "gate$o", message->getNexthop());
                        }
                    }
                }
        }

        /*
         * nexthop,��ʼ������һ�����ھӣ��Ӷ��ó���һ���ھӵķ���ȵ�
         * */
        else if (strcmp(msg->getName(), "nexthop") == 0) {
            SelfMessage *message = check_and_cast<SelfMessage *>(msg);
                int traceLength = message->getTraceLength();
                int dutyId = message->getLowDutyCycleId();

                SelfMessage *newMsg = new SelfMessage("prepareOK");
                newMsg->setLowDutyCycleId(dutyId);
                newMsg->setTraceLength(traceLength);
                for (i = 0; i < traceLength; i++) {
                    newMsg->setTrace(i, message->getTrace(i));
                }
                scheduleAt(simTime() + exponential(0.1), newMsg); //����Ϣ
        }
    }
}

/* ���ھӽڵ������·����Եķ��麯���� */
void Job1::divideGroup(int index,int size){ //���ڽڵ����ƽ�����䣬�ھ�from index
    int a,b;
    for (a = 0; a < candidate.size(); a++) { //��ѡ�ڵ�
       double minETX = 99999999; //��СETX
       int centerIndex = -1; // �ڵ㽫Ҫ��ӵ���group���
       EV << "for candidate node:" << candidate[a] << "\n";
       for (b = 0; b < K; b++) { //����
           double currentETX = 0;
           if (groups[b].size() < size) { //�жϵ�ǰ�����еĳ����Ƿ񳬹�����޶�
               currentETX = prfk(groups[b], candidate[a], index);
               EV << "In group:" << b << ", the centerETX is " << (1 / currentETX) << "\n";
               int lValue = (int)(1 / currentETX * CONST); //�������Ƚ�
               int rValue = minETX >= 99999999?(int)minETX:(int)(minETX * CONST);
               if (lValue < rValue) {
                   minETX = (1 / currentETX); //������ETXΪ�䵹��
                   centerIndex = b;
                   EV << "Now, the minETX is " << minETX << ",and the centerIndex is " << centerIndex << "\n";
               }
           }
       }
       EV << candidate[a] << ", enter into group: " << centerIndex << "\n";
       groups[centerIndex].push_back(candidate[a]); //����centerIndex�ҵ�����ӽڵ㣨��ǰ��a���ھӽڵ㣩������
   }
}
/* ���ھӽڵ�����ķ��麯���� */
void Job1::divideGroup1(int index, int size){
    int a;
    int b = 0;
    for (int i = 0; i < K; i++) { //��գ����·���
        groups[i].clear();
    }
    for (a = 0; a < neighbour.size(); a++) { //��ѡ�ڵ�
       EV << "for neighbour: " << neighbour[a] << "\n";
       if (groups[b % K].size() < size) { //�жϵ�ǰ�����еĳ����Ƿ񳬹�����޶�
           groups[b % K].push_back(neighbour[a]);
           EV << neighbour[a] << ", enter into group: " << b % K << "\n";
           b++;
       }
   }
}
/*��������������������С��ֵ���ﵽ��Ͳ�������ӽڵ��ˡ�*/
void Job1::divideGroup2(int index, double threshold){
    int a, b;
    double currentETX;
    for(a = 0;a < candidate.size();a++){
        double minETX = 99999999;
        double sucRate = 0;
        int flag = 0;//��ʾ������Ľ����ʶ��ﵽ����ֵ
        int centerIndex = -1;
        EV << "for candidate node:" << candidate[a] << "\n";
        for(b = 0;b < K;b++){
            currentETX = 0;
            if (Suc_Rate_GROUP[b] < threshold) { //�жϵ�ǰ�����еĽ������Ƿ�С����ֵ
               flag = 1;
               currentETX = prfk(groups[b], candidate[a], index);
               EV << "In group:" << b << ", the centerETX is " << (1 / currentETX) << "\n";
               int lValue = (int)(1 / currentETX * CONST); //�������Ƚ�
               int rValue = minETX >= 99999999?(int)minETX:(int)(minETX * CONST);
               if (lValue < rValue) {
                   minETX = (1 / currentETX); //������ETXΪ�䵹��
                   sucRate = currentETX;
                   centerIndex = b;
                   EV << "Now, the minETX is " << minETX << ",and the centerIndex is " << centerIndex << "\n";
               }
            }
            if(b == K-1 && flag == 0){//���ÿ�����ڶ������Ѿ�������ֵ
                for(int ii = 0; ii < K; ii++){
                    currentETX = prfk(groups[ii], candidate[a], index);
                    EV << "In group:" << ii << ", the centerETX is " << (1 / currentETX) << "\n";
                    int lValue = (int)(1 / currentETX * CONST); //�������Ƚ�
                    int rValue = minETX >= 99999999?(int)minETX:(int)(minETX * CONST);
                    if (lValue < rValue) {
                        minETX = (1 / currentETX); //������ETXΪ�䵹��
                        sucRate = currentETX;
                        centerIndex = ii;
                        EV << "Now, the minETX is " << minETX << ",and the centerIndex is " << centerIndex << "\n";
                    }
                }
            }
        }
        Suc_Rate_GROUP[centerIndex] = sucRate;
        EV << candidate[a] << ", enter into group: " << centerIndex << "\n";
        groups[centerIndex].push_back(candidate[a]); //����centerIndex�ҵ�����ӽڵ㣨��ǰ��a���ھӽڵ㣩������
    }
}

InitialMessage *Job1::generateHelloMessage(int msgid) {
    int from = getIndex(); //��ǰ�ڵ�Ϊ���ͷ�
    InitialMessage *message = new InitialMessage("hello"); //������Ϣ����Ϊhello
    message->setNodefrom(from); //��ǰ�ڵ�
    message->setMsgid(msgid);
    return message;
}

void Job1::sendCopyOfInitial(InitialMessage *message, int i) {
    InitialMessage *copy = (InitialMessage *) message->dup(); //������Ϣ
    send(copy, "gate$o", i);
}

/*�����߷��� tools��*/
void Job1::deleteValInVector(int k){
    for (vector<int>::iterator iter = simulationVal.begin(); iter != simulationVal.end();iter++) {
        if (*iter == k){
            iter = simulationVal.erase(iter);
            iter--;  //erase�����ķ���ָ��ǰ��ɾ��Ԫ�ص���һ��Ԫ�صĵ�����
        }
    }
}

bool Job1::findValInVector(int k){
    for (vector<int>::iterator iter = simulationVal.begin(); iter != simulationVal.end();iter++) {
        if (*iter == k){
            return true;
        }
    }
    return false;
}

string bool2string(bool A[dataPackNum]) {
    string s = "";
    for (int i = 0; i < dataPackNum; i++) {
        s += ((char) (A[i] + '0'));
    }
    return s;
}

string calAnd(string A, string B) { //������������&�Ľ��
    int lenA = A.length();
    int lenB = B.length();
    if (lenA == lenB) {
        string res = "";
        for (int i = 0; i < lenA; i++) {
            res += (A[i] == B[i] && A[i] == '1' ? '1' : '0');
        }
        return res;
    }
    return "";
}

double calCor(string A) {
    int lenA = A.length();
    if (lenA == 0)
        return 0;
    int c = 0;
    for (int i = 0; i < lenA; i++) {
        c += (A[i] == '1' ? 1 : 0);
    }
    return 1.0 * c / lenA;
}

double calCorBetweenNode(vector<int> node, int from) {
    int len = node.size();
    string s = "";
    for (int i = 0; i < len; i++) {
        string b = bool2string(received[from][node[i]]);
        if (i == 0) {
            s = calAnd(b, b);
        } else {
            s = calAnd(s, b);
        }
    }
    return calCor(s);
}

//��ǰcur�����е�start��endλ�ã�
//target��ʾ����2������3��...��ETX��step��ʾ��ǰ������tmp��ʾ��ǰ�õ��ĽǱ�
double cal(int start, int end, int target, int step, vector<int> cur, vector<int> tmp, int from) {
    if (step >= target) {
        return calCorBetweenNode(tmp, from);
    }
    double res = 0;
    for (int i = start; i < end; i++) {
        tmp.push_back(cur[i]);
        res += cal(i + 1, end, target, step + 1, cur, tmp,from);
        tmp.pop_back();
    }
    return res;
}

/*��������ʧ�ʡ�*/
double Job1::calProbability(vector<int> groupI, int from) {
    double probability = 0;
    double res = 0;
    vector<int> tmp;
    int groupNum = groupI.size();
    for (int j = 1; j <= groupNum; j++) {
        res = (j & 1 ? 1 : -1) * cal(0, groupNum, j, 0, groupI, tmp, from);
        probability += res;
    }
    return probability;
}

/**
 *�����㽫�ڵ�node�Ž�groupI�е�ETX��
 * �����۵ļӷ���ʽ�߼��� p(a)+p(b)+p(c)-p(ab)-p(ac)-p(bc)+p(abc)
 * c=1��ʱ�������p(a)+p(b)+p(c); c=2��ʱ�������p(ab),p(ac),p(bc); c=3��p(abc),���������� ż����
 */
double Job1::prfk(vector<int> groupI, int node, int from) {
    double ETX = 0;
    double res = 0;
    groupI.push_back(node);
    vector<int> tmp;
    int groupNum = groupI.size();
    for (int j = 1; j <= groupNum; j++) {
        res = (j & 1 ? 1 : -1) * cal(0, groupNum, j, 0, groupI, tmp, from);
        ETX += res;
    }
    groupI.pop_back();
    return ETX;
}

void Job1::finish(int transmissions, int hop, double sumEnergy) {
    simtime_t delay; //����ʱ��
    delay = simTime() - prepareTime; //����ʱ��simTime()������һ��ʼͳ����·������ʱ��
    ofstream f("D:\\A-WorkSpace\\Omnet++\\data\\LDC_AOR.txt", ios::app);
    EV << "LDC-AOR: \n";

    if (!f) //���ļ�����д�����ļ������ھʹ����� .���ļ�ʧ�����������
        return;
    f << "LDC-AOR: \n";

    f << "transmissions:" << transmissions << ",    hop:" << hop << ",    delay:" << delay << ",    sumEnergy:" << sumEnergy << endl;
    f.close();
}




