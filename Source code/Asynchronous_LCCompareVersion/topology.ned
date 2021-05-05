
//模块定义   描述仿真模型的拓扑结构
simple Job1  //模型中的所有具体行为均封装在simple模块中
{
    parameters:
        int x = intuniform(0,1000); //随机生成的0-1000之间的整数
        int y = intuniform(0,1000);

        @display("i=block/routing"); //节点显示的样子

        //定义模块自身的相关参数
        //default() 表示被重写默认值，值可以被修改
        int nodefrom = default(-1);
        int neighbourNum = default(0); //邻居个数
        int neighbourCount = default(0); //记录每个节点的邻居结点个数

        bool nexthop = default(false);  //是否有下一跳
        
        int transmission = default(0);  //传输次数
        int send = default(0);  //发送出去的数据包个数
        int received = default(0);  //接收到的数据包个数
        
 		bool isReceived = default(false);  //确定每一轮是否接收到数据
        double waitingTime = default(0); //节点从发送到接受ack的等待时间

    gates:
        inout gate[]; //输入输出门
}

network net  //网络名：net
{
    parameters:
        int n = default(50);  //生成?个Job1样式的节点
        @display("bgb=1000,1000;is=vs"); //仿真空间
        
    submodules: //复合模块中的子模块
        telosb[n]: Job1 {
            parameters:
                @display("p=$x,$y;is=vl;i=old/card1");
            gates:
                gate[n];
        }
    connections allowunconnected:  //关掉复合模块所有门对象的连接性检查
}