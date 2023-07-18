﻿// recorder.cpp: 目标的源文件。
//

#include "csv_recorder.h"
#include <filesystem>
#include <data_types.hpp>
#include <time_utils.hpp>

csv_recorder::csv_recorder(const char* basic_path) :_crossday_flow_csv(std::string(),rapidcsv::LabelParams(0,0))
{
	if (!std::filesystem::exists(basic_path))
	{
		std::filesystem::create_directories(basic_path);
	}
	time_t now_time = get_now();
	_basic_path = std::string(basic_path)+"/"+datetime_to_string(now_time,"%Y-%m-%d");
	if (!std::filesystem::exists(_basic_path.c_str()))
	{
		std::filesystem::create_directories(_basic_path.c_str());
	}

	_crossday_flow_csv.SetColumnName(0, "time");
	_crossday_flow_csv.SetColumnName(1, "trading_day");
	_crossday_flow_csv.SetColumnName(2, "place_order_amount");
	_crossday_flow_csv.SetColumnName(3, "entrust_amount");
	_crossday_flow_csv.SetColumnName(4, "trade_amount");
	_crossday_flow_csv.SetColumnName(5, "cancel_amount");
	_crossday_flow_csv.SetColumnName(6, "error_amount");
	_crossday_flow_csv.SetColumnName(7, "money");
	_crossday_flow_csv.SetColumnName(8, "frozen");
}

//结算表
void csv_recorder::record_crossday_flow(time_t time, uint32_t trading_day, const order_statistic& statistic, const account_info& account)
{
	try
	{
		size_t count = _crossday_flow_csv.GetRowCount();
		std::vector<std::string> row_data;
		row_data.emplace_back(datetime_to_string(time));
		row_data.emplace_back(std::to_string(trading_day));
		row_data.emplace_back(std::to_string(statistic.place_order_amount));
		row_data.emplace_back(std::to_string(statistic.entrust_amount));
		row_data.emplace_back(std::to_string(statistic.trade_amount));
		row_data.emplace_back(std::to_string(statistic.cancel_amount));
		row_data.emplace_back(std::to_string(statistic.error_amount));
		row_data.emplace_back(std::to_string(account.money));
		row_data.emplace_back(std::to_string(account.frozen_monery));
		_crossday_flow_csv.InsertRow<std::string>(count, row_data);
		_crossday_flow_csv.Save(_basic_path + "/crossday_flow.csv");
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("csv_recorder record_crossday_flow exception : %s", e.what());
	}
}