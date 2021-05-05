message SelfMessage //自消息
{
    int nexthop;
    int nodefrom;
    bool isFinish; //是否不再需要继续传播
    int lowDutyCycleId;
    
    int ETX;// 统计发送次数
    double beginTime;
    double endTime;
    
    int group[50];
    int groupLength;
    
    int trace[50]; //轨迹
    int traceLength; //轨迹长度
    
	int flag;
}