#include "evaluate.h"
#include "context.h"
#include <simulator.h>
#include <log_wapper.hpp>
#include <file_wapper.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <interface.h>

#pragma comment (lib,"simulator.lib")

evaluate::evaluate():_simulator(nullptr)
{
}
evaluate::~evaluate()
{
	if (_simulator)
	{
		destory_simulator(_simulator);
		_simulator = nullptr;
	}
}

bool evaluate::init_from_file(const std::string& config_path)
{
	boost::property_tree::ptree	simulator_config;
	boost::property_tree::ptree  recorder_config;

	if (!file_wapper::exists(config_path.c_str()))
	{
		LOG_ERROR("evaluate_driver init_from_file config_path not exit : %s", config_path.c_str());
		return false;
	}
	try
	{
		boost::property_tree::ptree config_root;
		boost::property_tree::ini_parser::read_ini(config_path, config_root);
		simulator_config = config_root.get_child("simulator");
		recorder_config = config_root.get_child("recorder");
	}
	catch (...)
	{
		LOG_ERROR("evaluate_driver init_from_file read_ini error : %s", config_path.c_str());
		return false;
	}
	//simulator

	_simulator = create_simulator(this,simulator_config);
	if (_simulator == nullptr)
	{
		LOG_ERROR("evaluate_driver init_from_file create_simulator error : %s", config_path.c_str());
		return false;
	}
	return this->init();
	
}



double evaluate::get_money()
{
	if (_simulator)
	{
		auto acc = _simulator->get_account();
		if(acc)
		{
			return acc->money ;
		}
	}
	return 0;
}

void evaluate::play(uint32_t tradeing_day)
{
	if(_simulator)
	{
		_simulator->set_trading_day(tradeing_day);
		_simulator->play();
	}
}

trader_api* evaluate::get_trader()
{
	return _simulator;
}

market_api* evaluate::get_market()
{
	return _simulator;
}