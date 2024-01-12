#ifndef NETWORK_INDEX_H_
#define NETWORK_INDEX_H_

#include <vector>
#include <utility>
#include <cmath>

using namespace std;

enum mess_type {
    EVG_, 
    ROUTER_, 
    WIRE_, 
    CREDIT_,
    //changed at 2022-4-20
    RECONFIGURATION
};
//changed at 2020-5-6
//添加了芯粒的路由算法枚举
enum routing_type {
    XY_ ,
    TXY_ ,
    CHIPLET_ROUTING_MESH,
    CHIPLET_STAR_TOPO_ROUTING,
    GRAPH_TOPO,
    RECONFIGURABLE_GRAPH_TOPO
};
enum VC_state_type {INIT_, ROUTING_, VC_AB_, SW_AB_, SW_TR_, HOME_};
enum flit_type {
    HEADER_, BODY_, TAIL_,SINGLE
};
enum vc_share_type {SHARE_, MONO_};
enum VC_usage_type {USED_, FREE_};
typedef double time_type;

//changed at 2022-4-4
typedef long TAddressNumber;

typedef vector<long> add_type;
typedef pair<long, long> VC_type;
typedef vector<unsigned long long> Data_type;
typedef unsigned long long Atom_type;
const VC_type VC_NULL = VC_type(-1, -1); 
#define BUFF_BOUND_ 100
#define WIRE_DELAY_ 0.9
//路由延迟
#define PIPE_DELAY_ 1.0
#define CREDIT_DELAY_ 1.0

//changed at 2022-4-5
//输出周期
#define REPORT_PERIOD_ 2000
//#define REPORT_PERIOD_ 1e10

#define S_ELPS_ 0.00000001
#define ATOM_WIDTH_ 64
#define ZERO_ 0
#define MAX_64_ 0xffffffffffffffffLL
#define CORR_EFF_ 0.8
#define POWER_NOM_ 1e9

//changed at 2022-4-16
//#define FILTERING
#define START_TIME (5.644e8-1)
#define START_RECONFIGURATION_PERIOD_NUMBER 2822

#endif
