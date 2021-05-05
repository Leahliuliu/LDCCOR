/*
 *  A_LC_opportunisticRouting.cc
 *
 *  Created on: 2020年9月11日
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

#define K 3  //候选转发集合为?个
#define N 50 //共?个节点
#define dataPackNum 10 //发送数据包的个数
#define prepareTime 60
#define experienceTime 2
#define LQControl 0.7
#define matrixLen 1000
#define neighborDistance 220
#define CONST 1e6   //10的六次方
using namespace std;

//作为全局使用
double Lq[N][N]; //链路质量
bool received[N][N][dataPackNum]; //n个节点相互发送dataPackNum个hello的消息的接收情况,节点两两之间的数据位图情况
int Suc_Rate_GROUP[K]; //每一个分组内的成功接收率

vector<int> groups[K]; //分组个数
vector<int> neighbour; //邻居节点个数
vector<int> candidate; //候选邻居节点个数
vector<int> simulationVal;

/*
 * 对比实验, LDC-AOR(组内节点异步唤醒, 不考虑链路相关性)
 */

class Job1: public cSimpleModule {
private:
    timeoutMsg *timeoutEvent; //全局定义重发事件
    //SelfMessage *prepareOK; //链路信息收集好后的自消息
    SelfMessage *lowdutycycle;

    int n;
    int size;
    int flag; //判断是否已经向邻居节点发送10个hello数据包
    int flag2;

    int counter;
    int mindist; //最小距离
    int nodeNum; //节点个数
    int reception;
    int destination;
    int centerLength;
    int neighbourNum;
    int neighborLength;

    int count[N] = { 0 }; //记录从某个节点接收到的消息个数
    int center[K] = { 0 };
    int neighbor[N] = { 0 }; //邻居节点
    int receivedNum[N] = { 0 }; //当前节点从i处接收到的数据包个数

public:
    Job1();
    virtual ~Job1();

protected:
    virtual void initialize(); //初始化
    virtual void handleMessage(cMessage *msg); //处理消息
    virtual InitialMessage *generateHelloMessage(int msgid); //生成hello消息
    virtual void sendCopyOfInitial(InitialMessage *message, int i); //发送消息副本
    virtual void deleteValInVector(int k);
    virtual bool findValInVector(int k);
    virtual void divideGroup(int index, int size); //邻居节点不同分组函数
    virtual void divideGroup1(int index, int size);
    virtual void divideGroup2(int index, double threshold); //根据阈值来计算
    virtual double calProbability(vector<int> groupI,int from); //计算损失率
    virtual double prfk(vector<int> groupI, int node,int from); //计算链路相关性
    virtual void finish(int transmission, int hop, double sumEnergy); //结束
};

Define_Module(Job1); //注入模块

Job1::Job1() { //函数初始化时
    timeoutEvent = NULL;
    lowdutycycle = NULL;

    memset(Lq, 0, sizeof(Lq)); //全局初始化
    memset(received, 0, sizeof(received));
    candidate.clear();
    simulationVal.clear();
    for(int i = 0;i < K;i++) groups[i].clear();
}
Job1::~Job1() { //函数结束时
    cancelAndDelete(timeoutEvent);
    cancelAndDelete(lowdutycycle);
}

/*
 * 初始化
 * */
void Job1::initialize() {
    lowdutycycle = new SelfMessage("lowDutyCycle"); //低占空比消息
    cModule *neighbour; //自定义模型指针类型，定义邻居
    //距离计算使用
    int xpos, n_xpos, ypos, n_ypos;
    int dist = 0;
    mindist = 60;
    neighborLength = 0; //邻居数目
    centerLength = 0; //初始化中心节点长度
    destination = N - 1; //目的地
    counter = 0;  //计数器（确定发送需要的包个数
    flag = 0;
    flag2 = 0;
    size = 0; //每个集合内的节点个数最大值
    int i = 0;

    srand((unsigned) time(0)); //随机数发生器的初始化函数
    nodeNum = (int) getParentModule()->par("n"); //生成传感器节点的个数

    if(simulationVal.size() == 0){
        for(int s = 1;s <= experienceTime;s++){
            simulationVal.push_back(s); //存入1、2  表示发送2次prepareOK消息
        }
    }

    xpos = par("x"); //横纵坐标位置取值
    ypos = par("y");
    EV << "Start******Initial******, the node ID is " << getIndex() << "\n";

    //限制最短距离（先初始化出所有的节点，然后进行距离调整）
    while (1) {
        bool succeed = true;
        for (i = 0; i < nodeNum; i++) {
            if (i == getIndex()) //如果i为当前的节点，就不处理
                continue;
            neighbour = getParentModule()->getSubmodule("telosb", i); //取出邻居节点
            n_xpos = neighbour->par("x");
            n_ypos = neighbour->par("y");
            //邻居节点与当前节点的欧氏距离的平方
            dist = (n_xpos - xpos) * (n_xpos - xpos) + (n_ypos - ypos) * (n_ypos - ypos);
            if (dist < mindist * mindist) {
                succeed = false;
                EV << getIndex() << " and " << i << " are too close! Rearrange them! " << "\n";
                break; //结束for循环
            }
        }
        if (succeed) { //所有的节点与当前节点的距离都满足不小于最小距离
            par("x").setLongValue(xpos); //为x、y重新赋值
            par("y").setLongValue(ypos);
            break; //结束while大循环
        } else {
            xpos = rand() % matrixLen; //产生均匀分布随机数，重新调整该节点的位置
            ypos = rand() % matrixLen;
        }
    }

    //添加连线
    xpos = par("x"); //当前处理的节点的位置取值
    ypos = par("y");
    for (i = 0; i < getIndex(); i++) { //处理比当前节点小的节点
        neighbour = getParentModule()->getSubmodule("telosb", i);
        n_xpos = neighbour->par("x");
        n_ypos = neighbour->par("y");
        dist = (n_xpos - xpos) * (n_xpos - xpos) + (n_ypos - ypos) * (n_ypos - ypos);
        if (dist <= neighborDistance * neighborDistance) { //距离小于220m即为邻居(模拟真实环境的发射距离
            par("neighbourNum").setLongValue((int) par("neighbourNum") + 1); //当前节点邻居数+1
            neighbour->par("neighbourNum").setLongValue((int) neighbour->par("neighbourNum") + 1); //邻居节点邻居数+1

            EV << getIndex() << " and " << i << " are neighbors! Connect them!" << "\n";

            cDatarateChannel *channel1 = cDatarateChannel::create("channel"); //模拟延迟、误码率和数据率的通道
            cDatarateChannel *channel2 = cDatarateChannel::create("channel");
            cGate *gate1 = NULL;
            cGate *gate2 = NULL;
            channel1->setDatarate(250); //设置通道的数据速率，以位/秒为单位
            channel1->setDelay(0.00);   //设置通道的传播延迟，以秒为单位
            channel2->setDatarate(250);
            channel2->setDelay(0.00);

            //将当前节点与邻居节点通过channel进行连接线
            gate1 = this->gate("gate$o", i); //输出
            gate2 = neighbour->gate("gate$i", getIndex()); //输入
            gate1->connectTo(gate2, channel1); //使用给定的通道对象（如果已指定）将门连接到另一门
            channel1->callInitialize(); //实现对此通道对象执行多阶段初始化
            channel1->getDisplayString().setTagArg("ls", 0, "blue"); //返回显示字符串，该字符串定义在复合模块图形中将模块显示为子模块时的外观

            gate1 = this->gate("gate$i", i);
            gate2 = neighbour->gate("gate$o", getIndex());
            gate2->connectTo(gate1, channel2);
            channel2->callInitialize();
            channel2->getDisplayString().setTagArg("ls", 0, "red");
        }
    }
    InitialMessage *msg = new InitialMessage("initial"); //声明初始消息
    msg->setNodefrom(getIndex()); //消息来源——当前节点
    msg->setMsgid(0); //消息ID 0,表示初始化要发hello包测链路质量和相关性
    scheduleAt(simTime() + exponential(0.1), msg); //给自己的定时消息
}

/*
 * 消息处理
 * */
void Job1::handleMessage(cMessage *msg) {
    int i, j, n, a, b;
    cModule *node; //仿真中的模块
    int nodefrom = -1; //记录消息来自哪个节点
    int msgid; //消息ID
    InitialMessage *hello;
    neighbourNum = par("neighbourNum"); //节点的邻居个数已经在初始化的时候计算好了
    EV << "Now, the node ID is " << getIndex() << "\n";

    if (msg->isSelfMessage()) { //如果是自消息的话
        /*
         * 初始化阶段，每个节点发 dataPackNum个hello包
         * */
        if (strcmp(msg->getName(), "initial") == 0) {
            if (flag == 0) {
                hello = generateHelloMessage(counter++); //counter初始化为0
                n = gateSize("gate");  //门的个数
                for (i = 0; i < n; i++) {
                    cGate *t = this->gate("gate$o", i);
                    if (t->isConnected()) { //通过输出hello消息给所有可连通的输出门
                        sendCopyOfInitial(hello, i); //向所有的邻居们发送hello消息
                    }
                }
                delete hello;
                if (counter < dataPackNum) { //还没发够
                    InitialMessage *msg = new InitialMessage("initial"); //仍为initial消息
                    msg->setNodefrom(getIndex());
                    msg->setMsgid(0); //消息ID仍为0
                    scheduleAt(simTime() + exponential(0.5), msg); //给自己再发一个自消息（从而再给周围邻居们发一个hello包
                }
                if (counter == dataPackNum) {
                    /*
                     * 首先起点定为0，由0节点发送数据包给目的地N-1节点
                     *  1. 如果目的节点就是0节点的邻居，则直接一跳完成;
                     *  2. 否则，再选择下一跳节点，继续进行转发，知道数据包被成功转发给目的节点为止.
                     */
                    flag = 1;
                    if (getIndex() == 0) { //trace统计从0开始的路径即跳数——[起点0、终点n-1]
                        int nodefrom = getIndex();
                        int traceLength = 1;
                        int trace[traceLength];
                        trace[0] = nodefrom; //将源节点添加到轨迹数组中 trace[0] = 0;

                        SelfMessage *prepareOK = new SelfMessage("prepareOK"); //new新的重发事件
                        prepareOK->setTraceLength(traceLength);
                        for (j = 0; j < traceLength; j++) {
                            prepareOK->setTrace(j, trace[j]);
                        }
                        prepareOK->setLowDutyCycleId(0);
                        EV << "Send prepareOK message" << "\n";
                        scheduleAt(prepareTime, prepareOK); //prepareTime后发送prepareOK消息 ————从0节点开始转发
                    }
                }
            }
        }

        /*
         * 查看保存的每个节点对于helloMessage的接收情况, 用来计算ETX
         * 某一节点全部信息发送完毕之后才会有邻居信息的发送
         * */
        else if (strcmp(msg->getName(), "neighInformation") == 0) {
            SelfMessage *message = check_and_cast<SelfMessage *>(msg); //强制类型转换
            nodefrom = message->getNodefrom();
            for (i = 0; i < dataPackNum; i++) {
                EV << "The dataBits from " << nodefrom << " to " << getIndex() << " = " << received[nodefrom][getIndex()][i] << "\n";
            }
            EV << "The linkQuality from " << nodefrom << " to " << getIndex() << " = " << Lq[nodefrom][getIndex()] << "\n";
        }

        /**
         * 得到节点的邻节点、初始化聚类中心、待分配节点, 进行分组操作 ,prepare是初始化后做一些准备操作，算邻节点，中心节点的操作
         */
        else if (strcmp(msg->getName(), "prepareOK") == 0) {
            SelfMessage *message = check_and_cast<SelfMessage *>(msg); //强制类型转换
                nodefrom = message->getNodefrom();
                int dutyId = message->getLowDutyCycleId();
                int traceLength = message->getTraceLength();
                EV << "Now, the traceLength: " << traceLength << "\n";
                int trace[traceLength];
                for (i = 0; i < traceLength; i++) {
                    trace[i] = message->getTrace(i); //路径拷贝  trace[0] = 0
                }
                EV << "In duty " << dutyId << ", in prepareOKMsg, the trace is: "; //当前trace路径中的节点
                for (i = 0; i < traceLength; i++) {
                     EV << trace[i] << "  ";
                }
                EV << "\n";

                //去除环路现象，属于trace的节点就不再将其算作邻居节点数组中，防止重复广播
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
                    cGate *t = this->gate("gate$o", i); //属于trace的节点就不再将其算作邻居节点数组中，防止重复广播
                    neighbour.clear();
                    if (t->isConnected()) {
                        neighbor[neighborLength++] = i; //获取邻节点
                    }
                }
                if(neighborLength > 0){ //该节点没有邻居时，则不进行处理
                    EV << "Then, for " << getIndex() << " the neighbor is: "; //邻居节点情况
                    for (i = 0; i < neighborLength; i++) {
                         EV << neighbor[i] << "  ";
                         neighbour.push_back(neighbor[i]); //邻居节点vector
                    }
                    EV << "\n";

                    if (neighborLength % K == 0) { //根据发送节点的一跳邻节点个数确定每个数组中所能包含的最大节点个数
                        size = neighborLength / K;
                    } else {
                        size = neighborLength / K + 1;
                    }
                    EV << "For " << getIndex() << ", the neighborLength is: " << neighborLength << "\n"; //输出0节点的邻居长度
                    //EV << "For " << getIndex() << ", the number of nodes in each group is: " << size << "\n"; //输出0节点的分组内的节点个数

                    for (i = 0; i < neighborLength; i++) { //选择排序——将节点的邻居节点首先按照链路质量进行从大到小的排序,获取前K个为总的聚类中心
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
                        EV << "After exchange: node " << getIndex() << ", the neighbor is: " << neighbor[i] << "\n";//输出按照链路质量排好序后的当前节点的所有邻居
                    }

                    if (neighborLength >= K) { //设置中心组
                        for (i = 0; i < K; i++) {
                            center[centerLength++] = neighbor[i]; //链路质量排在前K的节点分别分给K个组
                        }
                    } else {
                        for (i = 0; i < neighborLength; i++) {
                            center[centerLength++] = neighbor[i]; //个数不够则只分为neighborLength个组，并且无后续工作
                        }
                    }

                    if (neighborLength > K) { //【设置候选转发集合】 当邻节点的个数大于K时获取待分配节点的集合，当邻节点的个数小于K时则没有待分配节点
                        for (i = 0; i < neighborLength; i++) {
                            int tag = 0;
                            for (j = 0; j < K; j++) {
                                if (neighbor[i] == center[j]) {
                                    tag = 1;
                                    break;
                                }
                            }
                            if (tag == 0) { //排除掉已经分配好的中心点，统计好余下的候选转发集合
                                candidate.push_back(neighbor[i]);
                            }
                        }
                        EV << "The candidate is: "; //输出待分配节点数组
                        for (i = 0; i < candidate.size(); i++) {
                             EV << candidate[i] << "  ";
                        }
                        EV << "\n";
                    }

                    for (i = 0; i < K; i++) { //分组前的清空
                        groups[i].clear();
                    }
                    for (i = 0; i < centerLength; i++) { //给每个聚类添加初始节点
                        groups[i].push_back(center[i]);
                        EV << "The node of group " << i << " is " << groups[i][0] << " ,length " << i << " is " << groups[i].size() << "\n";
                    }

                    /*【对candidate集合中的节点进行分组】
                     * 1、首先将candidate中的待遍历节点添加到K个保存聚类结果的数组中--以便计算该节点同K个数组的ETX
                     * 2、然后将该节点从数组中删除-- 恢复原样
                     * 3、将该节点添加到同该节点具有最小ETX的数组中
                     */
                    if (candidate.size() > 0) {
                        divideGroup1(getIndex(),size); //随机分组

                        EV << "End Group:" << "\n";
                        for (i = 0; i < K; i++) { //输出每个聚类数组中的结果
                            EV << "group " << i << ": ";
                            for (j = 0; j < groups[i].size(); j++) {
                                EV << groups[i][j] << "  ";
                            }
                            EV << "\n";
                        }
                    } else{
                        EV << "End Group:" << "\n";
                        for (i = 0; i < neighborLength; i++) { //输出每个聚类数组中的结果
                            EV << "group " << i << ": ";
                            for (j = 0; j < groups[i].size(); j++) {
                                EV << groups[i][j] << "  ";
                            }
                            EV << "\n";
                        }
                    }

                    //首先判断所有邻居分组节点中是否有目标节点
                    int lowIndex = -1; //记录目标节点所在的集合
                    int has = false;
                    for(i = 0;i < K;i++){
                        if (groups[i].size() > 0) {
                            for(j = 0; j < groups[i].size();j++){
                                if (groups[i][j] == destination) {
                                    has = true; //判断邻节点中是否有目标节点
                                    lowIndex = i;
                                }
                            }
                        }
                    }

                    /*【模拟低占空比模型】 */
                    double beginTime = simTime().dbl() + 1; // 仿真时间SimTime转换成双精度double类型,用dbl()函数转换
                    EV << "BeginTime in spot [prepareOK] is " << beginTime << "\n";

                    if(has){ //若某一个分组含有目标节点,则只给将这一组的节点信息导出
                        for (i = 0; i < centerLength; i++) {
                            if(i == lowIndex){
                                EV << "has = true" << "\n";
                                SelfMessage *copy = lowdutycycle->dup(); //创建并返回此对象的精确副本
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
                                scheduleAt(simTime() + 1, copy); //发lowdutyCycle自消息
                            }
                        }
                    }else{ //否则导出所有的不为0的分组
                        for (i = 0; i < centerLength; i++) {
                              if (groups[i].size() > 0)
                                  EV << "has = false" << "\n";
                                  EV << "this is the " << i + 1 << " time spot" << "\n";
                                  SelfMessage *copy = lowdutycycle->dup(); //创建并返回此对象的精确副本
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
                                  scheduleAt(simTime() + (2 * (i + 1) - 1), copy); //发lowdutyCycle自消息
                          }
                    }
                }
                // 重置数组
                neighborLength = 0;
                centerLength = 0;
                candidate.clear();
                neighbour.clear();
        }

        /*
         * 接收到低占空比消息，根据消息内分组集合，在当前时间占空比下广播消息
         * */
        else if (strcmp(msg->getName(), "lowDutyCycle") == 0) {
            SelfMessage *message = check_and_cast<SelfMessage *>(msg); //强制转换类型为自消息
                SelfMessage *data = message->dup(); //复制
                data->setName("data"); //重新命名,新的数据包，用来决定下一跳节点
                double beginTime = message->getBeginTime();
                int dutyId = message->getLowDutyCycleId();
                EV << "[[***********lowDutyCycle**********]] BeginTime = " << beginTime << " ,in " << dutyId << " TIME SPOT" << "\n";

                for (i = 0; i < K; i++) {
                    if (dutyId == (i + 1)) {
                        SelfMessage *copy = (SelfMessage *) data->dup(); //data消息的复制

                        //转发集合
                        int groupLength = message->getGroupLength(); //当前这个组的元素集合
                        int group[groupLength];
                        for (j = 0; j < groupLength; j++) {
                            group[j] = message->getGroup(j);
                        }
                        EV << "Nodes in group " << i << ": ";
                        for (j = 0; j < groupLength; j++) {
                            EV << group[j] << "  ";
                        }
                        EV << "\n";

                        //轨迹集合
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

                        bool result = false; //判断目标节点是否在当前集合中
                        for (j = 0; j < groupLength; j++) {
                            if (group[j] == destination) {
                                result = true;
                            }
                        }
                        EV << "has destination: " << result << "\n";

                        if (result) { //如果目标节点在该集合中，那么一次就能发送完毕，直接给目标节点发送消息
                            par("transmission").setLongValue((int) par("transmission") + 1);
                            par("send").setLongValue((int) par("send") + 1);
                            copy->setIsFinish(true); //在此结束发送
                            send(copy, "gate$o", destination); //把SelfMessage消息发送给destination

                        } else {
                            par("transmission").setLongValue((int) par("transmission") + 1);
                            par("send").setLongValue((int) par("send") + 1);
                            int wakeNodeNum; //醒来的节点数
                            wakeNodeNum = groupLength % 2 ? groupLength/2 + 1 : groupLength/2;
                            //wakeNodeNum = groupLength;

                            for (j = 0; j < wakeNodeNum; j++) { //给候选转发集合广播消息
                                SelfMessage *copy1 = (SelfMessage *) copy->dup();
                                copy1->setIsFinish(false); //还没有结束发送
                                send(copy1, "gate$o", group[j]); //向当前分组群发SelfMessage副本消息
                            }
                        }
                    }
                }
        }

    } else {
        /*
         * 收到hello消息时， 记录邻居节点id，并记录收包情况，以计算所有节点的链路质量和链路相关性
         * */
        if (strcmp(msg->getName(), "hello") == 0) {
            InitialMessage *message = check_and_cast<InitialMessage *>(msg); //强制类型转换
            nodefrom = message->getNodefrom();
            count[nodefrom]++; //来自当前nodeFrom的数据包个数
            msgid = message->getMsgid();
            if (uniform(0, 1) < LQControl) {  //LQControl的概率能收到hello包
                received[nodefrom][getIndex()][msgid] = 1;
                receivedNum[nodefrom] += 1;  //来自nodeFrom的数据包成功收到的个数
            } else {
                bubble("message lost"); //否则接收失败气泡
            }
            if (count[nodefrom] == dataPackNum) { //收到过发送的次数足够
                EV << "Now, the index is " << getIndex() << " have enough dataPackages from " << nodefrom << "\n";
                Lq[nodefrom][getIndex()] = receivedNum[nodefrom] * 1.0/ dataPackNum; //链路质量计算
                SelfMessage *msg = new SelfMessage("neighInformation"); //生成邻居信息消息
                msg->setNodefrom(nodefrom);
                scheduleAt(simTime() + 0.1, msg);
            }
        }

        /*
         * 计算分组集合内的接收率等信息
         * */
        else if (strcmp(msg->getName(), "data") == 0) { //给分组发送消息,数据被组内邻居收到
            SelfMessage *message = check_and_cast<SelfMessage *>(msg);
                SelfMessage *ack = new SelfMessage("ACK"); //向父节点发送确认包
                int dutyId = message->getLowDutyCycleId();
                bool res = message->getIsFinish(); //是否发送给终点，需要判断
                int nodefrom = message->getNodefrom();
                double beginTime = message->getBeginTime(); //父亲节点的发送时间

                int groupLength = message->getGroupLength();
                vector<int> tmp; //该集合内全部节点
                for (j = 0; j < groupLength; j++) {
                    tmp.push_back(message->getGroup(j));
                }
                int traceLength = message->getTraceLength();
                int trace[traceLength];  //获取路径
                for (j = 0; j < traceLength; j++) {
                    trace[j] = message->getTrace(j);
                }

                int temp[N]; //统计成功接收到数据包的节点
                int k = 0;

                for (i = 0; i < K; i++) {
                    if (dutyId == (i + 1)) {
                        double probability = 0; // 集合中至少有一个节点收到数据包的概率
                        EV << "This group: "; //输出当前组里面的所有节点信息
                        for (j = 0; j < groupLength; j++) {
                            EV << "Node: " << tmp[j] << " ";
                            EV << "LinkQuality: " << Lq[nodefrom][tmp[j]] << " ";
                        }
                        EV << "\n";
                        probability = calProbability(tmp,nodefrom); //计算当前集合至少一个节点收到数据包的概率
                        EV << "probability: " << probability << "\n";
                        EV << "The dutyId is " << dutyId << " and the lost probability is " << (1 - probability) << "\n";
                        if (uniform(0, 1) < (1 - probability)) { //消息接收失败
                            EV << "Node " << getIndex() << "\"Losing\" DATA.\n"; //没有收到数据包
                            bubble("Data lost");
                            delete msg;

                            //当前组所有的元素是否都已经处理过，是则继续执行程序
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

                                    int index = intuniform(0, k - 1); //在收到的数据包中任意选择一个节点作为下一跳
                                    EV << "In duty " << i + 1 << " We select the node " << temp[index] << " as next hop" << "\n";
                                    SelfMessage *ackCopy = ack->dup();
                                    ackCopy->setIsFinish(false); //未结束
                                    ackCopy->setLowDutyCycleId(i + 1);
                                    ackCopy->setBeginTime(beginTime); //开始发送时间
                                    ackCopy->setNodefrom(getIndex());
                                    ackCopy->setNexthop(temp[index]);

                                    trace[traceLength++] = temp[index]; //trace重新赋值
                                    ackCopy->setTraceLength(traceLength);
                                    for (j = 0; j < traceLength; j++) {
                                        ackCopy->setTrace(j, trace[j]);
                                    }
                                    send(ackCopy, "gate$o", nodefrom); //向源节点发送确认包，通过ACK消息通知父节点 下一跳信息
                                    for (j = 0; j < groupLength; j++) {
                                        node = getParentModule()->getSubmodule("telosb", tmp[j]);
                                        node->par("isReceived").setBoolValue(false);//恢复默认
                                    }
                                    k = 0; //恢复默认

                                } else{ //表示该组内所有节点都没有收到数据包
                                   EV << "No node received the data in this group" << "\n";
                                   cancelAndDelete(timeoutEvent); //删掉之前的定义
                                   timeoutEvent = new timeoutMsg("timeoutEvent"); //new新的重发事件
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
                                   while (!timeoutEvent->isScheduled()) { //当前时间片不是工作状态
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

                        } else { //消息接收成功
                            par("isReceived").setBoolValue(true); //确认收到数据包
                            par("received").setLongValue((int) par("received") + 1); //接收到数据包的个数

                            if (res) {//消息到达目的地
                                EV << "The Destination node received packet!!!" << "\n";
                                delete msg;
                                // 由目标节点收到数据包
                                SelfMessage *ackCopy = ack->dup();
                                ackCopy->setIsFinish(true); //已结束
                                ackCopy->setLowDutyCycleId(i + 1);
                                ackCopy->setBeginTime(beginTime);
                                ackCopy->setNodefrom(getIndex());
                                ackCopy->setNexthop(destination);

                                trace[traceLength++] = destination; //加入终点
                                ackCopy->setTraceLength(traceLength);
                                for (j = 0; j < traceLength; j++) {
                                    ackCopy->setTrace(j, trace[j]);
                                }
                                send(ackCopy, "gate$o", nodefrom); //向源节点发送确认包，到达目的地
                                // 恢复默认值
                                for (j = 0; j < groupLength; j++) {
                                    node = getParentModule()->getSubmodule("telosb", tmp[j]);
                                    node->par("isReceived").setBoolValue(false);
                                }
                            }

                            else { //收到消息，但是消息未到达【目的地】，需要继续传播
                                EV << "node " << getIndex() << " received DataMsg from " << nodefrom << "\n";
                                //当前组所有的元素是否都已经处理过，是则继续执行程序
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
                                        int index = intuniform(0, k - 1); //在收到的数据包中任意选择一个节点作为下一跳
                                        EV << "In duty " << i + 1 << " We select the node " << temp[index] << " as next hop" << "\n";
                                        SelfMessage *ackCopy = ack->dup();
                                        ackCopy->setIsFinish(false);
                                        ackCopy->setLowDutyCycleId(i + 1);
                                        ackCopy->setBeginTime(beginTime); //开始发送时间
                                        ackCopy->setNodefrom(getIndex());
                                        ackCopy->setNexthop(temp[index]);
                                        trace[traceLength++] = temp[index]; //trace重新赋值
                                        ackCopy->setTraceLength(traceLength);
                                        for (j = 0; j < traceLength; j++) {
                                            ackCopy->setTrace(j, trace[j]);
                                        }
                                        send(ackCopy, "gate$o", nodefrom); //向源节点发送确认包

                                        for (j = 0; j < groupLength; j++) {
                                            node = getParentModule()->getSubmodule("telosb", tmp[j]);
                                            node->par("isReceived").setBoolValue(false); //恢复默认
                                        }
                                        k = 0; //恢复默认
                                    }
                                }
                            }
                        }
                    }
                }
        }

        /**
         * 处理重发操作,向某一个完全没有收到数据的分组内重新发节点
         */
        else if (strcmp(msg->getName(), "timeoutEvent") == 0) {
            timeoutMsg *message = check_and_cast<timeoutMsg *>(msg);
                SelfMessage *data = new SelfMessage("data"); //创建data消息
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

                        for (j = 0; j < groupLength; j++) { //判断目标节点是否在下一跳
                            if (group[j] == destination) {
                                res = true;
                            }
                        }
                        if (res) { //如果目标节点在该集合中，那么一次就能发送完毕
                            par("transmission").setLongValue((int) par("transmission") + 1);
                            par("send").setLongValue((int) par("send") + 1);
                            data->setIsFinish(true);
                            send(data, "gate$o", destination);
                        } else {
                            par("transmission").setLongValue((int) par("transmission") + 1);
                            par("send").setLongValue((int) par("send") + 1);
                            int wakeNodeNum; //醒来的节点数
                            wakeNodeNum = groupLength % 2 ? groupLength/2 + 1 : groupLength/2;
                            //wakeNodeNum = groupLength;

                            for (j = 0; j < wakeNodeNum; j++) { //给候选转发集合广播消息
                                SelfMessage *copy = (SelfMessage *) data->dup();
                                copy->setIsFinish(false);
                                send(copy, "gate$o", group[j]);
                            }
                        }
                    }
                }
        }

        /*
         * 父节点接收确认消息ACK处理
         * */
        else if (strcmp(msg->getName(), "ACK") == 0) {
            SelfMessage *message = check_and_cast<SelfMessage *>(msg);
                int dutyId = message->getLowDutyCycleId();
                int traceLength = message->getTraceLength();
                int trace[traceLength];  //获取路径
                for (j = 0; j < traceLength; j++) {
                    trace[j] = message->getTrace(j);
                }
                int nodefrom = message->getNodefrom();
                cModule *finalCount;
                cModule *temp;
                EV << "Now, " << getIndex() << " received ACKMsg from " << nodefrom << "\n";
                double beginTime = message->getBeginTime(); //发送时间
                EV << "The beginTime is " << beginTime << "\n";
                double endTime = simTime().dbl(); //当前时间,作为收到ACK数据包的时间
                EV << "The endTime is " << endTime << "\n";
                double waitingTime = endTime - beginTime; //等待时间----因为时间片的原因
                EV << "The waitingTime is " << waitingTime << "\n";
                if (waitingTime > 0) {
                    par("waitingTime").setDoubleValue((double) par("waitingTime") + waitingTime);
                }
                for (i = 0; i < K; i++) {
                    if (dutyId == (i + 1)) {
                        bool res = message->getIsFinish(); //判断是否是来自目标节点的ACK

                        if (res) { //目标节点收到数据包，结束程序
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
                            EV << "All need " << all << " transmission ! !" << "\n"; //统计总共的转发次数

                            double sumEnergy = 0;
                            for (j = 0; j < N; j++) { //针对N个节点
                                finalCount = getParentModule()->getSubmodule("telosb", j);
                                int send = finalCount->par("send");
                                int received = finalCount->par("received");
                                double waitingTime = finalCount->par("waitingTime");
                                if (send != 0 || received != 0 || waitingTime != 0) {
                                    EV << "Node " << finalCount->getIndex() << " send " << send << " packets, "
                                      "and received " << received << " packets, wait "<< waitingTime << " seconds" << "\n";
                                    sumEnergy = sumEnergy + send * 3 * 0.01 * 4.8; /*能量消耗计算  不太明白.发送、接收、空闲*/
                                    sumEnergy = sumEnergy + received * 3 * 0.009 * 4.8;
                                    sumEnergy = sumEnergy + waitingTime * 0.00002;
                                }
                            }
                            EV << "The sumEnergy is " << sumEnergy << "\n";
                            cancelEvent(lowdutycycle);

                            finish(all, traceLength - 1, sumEnergy); //参数：传输次数、跳数、能量、当前执行的实验几
                            endSimulation();
                        }
                        else { //不是目标节点的ack,就按照ack消息上带有的下一跳信息进行转发
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
         * nexthop,开始计算下一跳的邻居，从而得出下一跳邻居的分组等等
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
                scheduleAt(simTime() + exponential(0.1), newMsg); //自消息
        }
    }
}

/* 【邻居节点基于链路相关性的分组函数】 */
void Job1::divideGroup(int index,int size){ //组内节点个数平均分配，邻居from index
    int a,b;
    for (a = 0; a < candidate.size(); a++) { //候选节点
       double minETX = 99999999; //最小ETX
       int centerIndex = -1; // 节点将要添加到的group编号
       EV << "for candidate node:" << candidate[a] << "\n";
       for (b = 0; b < K; b++) { //组数
           double currentETX = 0;
           if (groups[b].size() < size) { //判断当前数组中的长度是否超过最大限度
               currentETX = prfk(groups[b], candidate[a], index);
               EV << "In group:" << b << ", the centerETX is " << (1 / currentETX) << "\n";
               int lValue = (int)(1 / currentETX * CONST); //浮点数比较
               int rValue = minETX >= 99999999?(int)minETX:(int)(minETX * CONST);
               if (lValue < rValue) {
                   minETX = (1 / currentETX); //真正的ETX为其倒数
                   centerIndex = b;
                   EV << "Now, the minETX is " << minETX << ",and the centerIndex is " << centerIndex << "\n";
               }
           }
       }
       EV << candidate[a] << ", enter into group: " << centerIndex << "\n";
       groups[centerIndex].push_back(candidate[a]); //根据centerIndex找到待添加节点（当前第a个邻居节点）的数组
   }
}
/* 【邻居节点随机的分组函数】 */
void Job1::divideGroup1(int index, int size){
    int a;
    int b = 0;
    for (int i = 0; i < K; i++) { //清空，重新分组
        groups[i].clear();
    }
    for (a = 0; a < neighbour.size(); a++) { //候选节点
       EV << "for neighbour: " << neighbour[a] << "\n";
       if (groups[b % K].size() < size) { //判断当前数组中的长度是否超过最大限度
           groups[b % K].push_back(neighbour[a]);
           EV << neighbour[a] << ", enter into group: " << b % K << "\n";
           b++;
       }
   }
}
/*【组内最大接收率限制最小阈值，达到后就不往里面加节点了】*/
void Job1::divideGroup2(int index, double threshold){
    int a, b;
    double currentETX;
    for(a = 0;a < candidate.size();a++){
        double minETX = 99999999;
        double sucRate = 0;
        int flag = 0;//表示所有组的接收率都达到了阈值
        int centerIndex = -1;
        EV << "for candidate node:" << candidate[a] << "\n";
        for(b = 0;b < K;b++){
            currentETX = 0;
            if (Suc_Rate_GROUP[b] < threshold) { //判断当前数组中的接收率是否小于阈值
               flag = 1;
               currentETX = prfk(groups[b], candidate[a], index);
               EV << "In group:" << b << ", the centerETX is " << (1 / currentETX) << "\n";
               int lValue = (int)(1 / currentETX * CONST); //浮点数比较
               int rValue = minETX >= 99999999?(int)minETX:(int)(minETX * CONST);
               if (lValue < rValue) {
                   minETX = (1 / currentETX); //真正的ETX为其倒数
                   sucRate = currentETX;
                   centerIndex = b;
                   EV << "Now, the minETX is " << minETX << ",and the centerIndex is " << centerIndex << "\n";
               }
            }
            if(b == K-1 && flag == 0){//如果每个组内都满足已经大于阈值
                for(int ii = 0; ii < K; ii++){
                    currentETX = prfk(groups[ii], candidate[a], index);
                    EV << "In group:" << ii << ", the centerETX is " << (1 / currentETX) << "\n";
                    int lValue = (int)(1 / currentETX * CONST); //浮点数比较
                    int rValue = minETX >= 99999999?(int)minETX:(int)(minETX * CONST);
                    if (lValue < rValue) {
                        minETX = (1 / currentETX); //真正的ETX为其倒数
                        sucRate = currentETX;
                        centerIndex = ii;
                        EV << "Now, the minETX is " << minETX << ",and the centerIndex is " << centerIndex << "\n";
                    }
                }
            }
        }
        Suc_Rate_GROUP[centerIndex] = sucRate;
        EV << candidate[a] << ", enter into group: " << centerIndex << "\n";
        groups[centerIndex].push_back(candidate[a]); //根据centerIndex找到待添加节点（当前第a个邻居节点）的数组
    }
}

InitialMessage *Job1::generateHelloMessage(int msgid) {
    int from = getIndex(); //当前节点为发送方
    InitialMessage *message = new InitialMessage("hello"); //发送消息内容为hello
    message->setNodefrom(from); //当前节点
    message->setMsgid(msgid);
    return message;
}

void Job1::sendCopyOfInitial(InitialMessage *message, int i) {
    InitialMessage *copy = (InitialMessage *) message->dup(); //复制消息
    send(copy, "gate$o", i);
}

/*【工具方法 tools】*/
void Job1::deleteValInVector(int k){
    for (vector<int>::iterator iter = simulationVal.begin(); iter != simulationVal.end();iter++) {
        if (*iter == k){
            iter = simulationVal.erase(iter);
            iter--;  //erase函数的返回指向当前被删除元素的下一个元素的迭代器
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

string calAnd(string A, string B) { //计算两两进行&的结果
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

//当前cur数组中的start和end位置，
//target表示计算2个还是3个...的ETX，step表示当前步数，tmp表示当前得到的角标
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

/*【计算损失率】*/
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
 *【计算将节点node放进groupI中的ETX】
 * 概率论的加法公式逻辑： p(a)+p(b)+p(c)-p(ab)-p(ac)-p(bc)+p(abc)
 * c=1的时候算的是p(a)+p(b)+p(c); c=2的时候算的是p(ab),p(ac),p(bc); c=3是p(abc),所以奇数加 偶数减
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
    simtime_t delay; //运行时间
    delay = simTime() - prepareTime; //仿真时间simTime()。减掉一开始统计链路质量的时长
    ofstream f("D:\\A-WorkSpace\\Omnet++\\data\\LDC_AOR.txt", ios::app);
    EV << "LDC-AOR: \n";

    if (!f) //打开文件用于写，若文件不存在就创建它 .打开文件失败则结束运行
        return;
    f << "LDC-AOR: \n";

    f << "transmissions:" << transmissions << ",    hop:" << hop << ",    delay:" << delay << ",    sumEnergy:" << sumEnergy << endl;
    f.close();
}




