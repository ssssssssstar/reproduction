/***********************************************************/
/*                      configuration parse                */
/***********************************************************/
#include "configuration.h"
#include <vector>
#include "SRGen.h"
#include <string>
#include "SString.h"
#include <ctime>
#include <unistd.h>
#include <iostream>
#include <strstream>

//changed at 2020-5-8
//为了便于调试改成一百万
//实际使用时要改回0
#ifdef FILTERING
const time_type TIME_0=START_TIME;
#else
//const time_type TIME_0=1000000;
const time_type TIME_0=0;
#endif

configuration * configuration::ap_ = 0;

configuration::configuration(int argc, char * const argv []):
	ary_number_(8),
	cube_number_(2),
	virtual_channel_number_(2),
	buffer_size_(64),
	outbuffer_size_(4),
	ran_seed_(1),
	sim_length_(1000000000),
	trace_fname_(),
	routing_alg_(0),
	vc_share_(MONO_)
{
	ap_ = this;
	const char * opt_str = "h:?:A:c:V:B:F:T:r:I:O:R:L:G:m:C:l";
	string usage = string("usage: ") + argv[0] + " [" +
		opt_str + "] \n";

	if(argc <= 1) {
		throw init_error(help_);
	}

	while (1) {
		long ch = getopt(argc, argv, opt_str);

		if(ch == -1) {
			break;
		}

		vector<string> vec;

		switch (ch) {

			case 'h':
				throw init_error(help_);
				break;

			case 'A':
				ary_number_ = Conv(optarg);
				break;

			case 'c':
				cube_number_ = Conv(optarg);
				break;

			case 'V':
				virtual_channel_number_ = Conv(optarg);
				break;

			case 'B':
				buffer_size_ = Conv(optarg);
				break;

			case 'F':
				flit_size_ = Conv(optarg);
				break;

			case 'L':
				link_length_ = Conv(optarg);
				break;

			case 'T':
				sim_length_ = Conv(optarg);
				break;

			case 'I':
				trace_fname_ = optarg;
				break;
		
			case 'O':
				outbuffer_size_ = Conv(optarg);
				break;

			case 'r':
				ran_seed_ = Conv(optarg);
				break;

			case 'R':
				routing_alg_ = Conv(optarg);
				break;

			//changed at 2022-4-4
			//http://c.biancheng.net/cpp/html/379.html
			case 'G':
				topo_file_path=optarg;
				break;

			case 'm':
				report_period=Conv(optarg);
				break;
			
			case 'C':
				reconfigurationFilePath=optarg;
				break;

			case 'l':
				packet_loss=true;
				break;

			case '?':
				throw init_error(help_);
			    break;

			default :
				throw init_error(usage);
				break;
		}
	}
	SRGen::wrg().set_seed(ran_seed_);
	vc_share_ = SHARE_;
}
string configuration:: help_ =
	string("h: help\n") +
	"?: help\n" +
	"A: array size\n" +
	"c: cube dimension\n" +
	"V: virtual channel number\n" +
    "B: buffer size\n" +
    "O: outbuffer size\n" +
	"F: flit size\n" +
	"L: link legnth\n" +
	"T: simulation length\n" +
	"r: random seed\n" +
	"I: trace file\n" +
	"R: routing algorithm: 0-dimension 1-opty\n";

ostream & operator<<(ostream & os, const configuration & cf) {
	os <<"****************configuration********************\n"<<
		"ary size:"<<cf.ary_number_<<"\n"<<
		"cube dimension:"<<cf.cube_number_<<"\n"<<
		"virtual channel number:"<<cf.virtual_channel_number_<<"\n"<<
		"buffer size:"<<cf.buffer_size_<<"\n"<<
		"outbuffer size:"<<cf.outbuffer_size_<<"\n"<<
		"flit size:"<<cf.flit_size_<<"\n"<<
		"link length:"<<cf.link_length_<<"\n"<<
		"simulation length:" <<cf.sim_length_<<"\n"<<
		"trace file:"<<cf.trace_fname_.c_str()<<"\n"<<
		"Routing algorithm:"<<cf.routing_alg_<<"\n"<<
		"****************configuration********************\n";
	return os;
}

const string&configuration::getTopoFilePath()const
{
	return topo_file_path;
}

time_type configuration::getReportPeriod()const
{
	return report_period;
}

const string&configuration::getReconfigurationFilePath()const
{
	return reconfigurationFilePath;
}

bool configuration::getPacketLoss()const
{
	return packet_loss;
}
