message timeoutMsg //超时消息
{
	bool finish; //结束
    int finishIndex; //结束的下标
	int dutyId; 
	int nexthop; //下一跳
	int nodefrom; 
	double beginTime;
	double endTime;
	
	int group[50]; //三个center不太明白
	int groupLength;
	
	int trace[50]; //轨迹记录
	int traceLength; //轨迹长度（跳数）
	
	int flag;
}