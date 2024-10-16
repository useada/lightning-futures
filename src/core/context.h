﻿/*
Distributed under the MIT License(MIT)

Copyright(c) 2023 Jihua Zou EMail: ghuazo@qq.com QQ:137336521

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files(the "Software"), to deal in the
Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and /or sell copies
of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once
#include <thread>
#include <log_wapper.hpp>
#include <define.h>
#include <lightning.h>
#include "event_center.hpp"
#include <market_api.h>
#include <trader_api.h>
#include <params.hpp>
#include "trading_section.h"

class context
{

public:

	context()noexcept;

	virtual ~context()noexcept;
	
private:

	context(const context&)noexcept = delete;

	context& operator=(const context&)noexcept = delete;

	context(context&&)noexcept = delete;

	context& operator=(context&&)noexcept = delete;

private:
	
	bool _is_runing ;
	// (实时)
	tick_callback _tick_callback;
	cycle_callback _init_callback;
	cycle_callback _update_callback;
	cycle_callback _destroy_callback;
	//实时事件，高频策略使用
	order_event realtime_event;
	
	//实时的线程
	std::thread * _realtime_thread;
	
	daytm_t _last_tick_time;

	uint32_t _max_position;

	int16_t _bind_cpu_core ;

	int16_t _thread_priority;

	uint32_t _loop_interval ;

	std::map<code_t,tick_info> _previous_tick;

	daytm_t _last_order_time;

	std::map<code_t, today_market_info> _today_market_info;

	std::map<code_t, order_statistic> _statistic_info;

	position_map			_position_info;

	entrust_map				_order_info;

	std::map<std::string,std::string>	_include_config;

	std::shared_ptr<trading_section> _section_config;

	
public:

	/*加载数据*/
	void load_trader_data()noexcept;

	/*启动*/
	bool start_service()noexcept;
	
	void update()noexcept;
	/*停止*/
	bool stop_service()noexcept;

	//绑定实时事件
	void bind_realtime_event(const order_event& event_cb, cycle_callback init_cb, cycle_callback update_cb,cycle_callback destroy_cb)noexcept
	{
		realtime_event = event_cb;
		this->_init_callback = init_cb;
		this->_update_callback = update_cb;
		this->_destroy_callback = destroy_cb;
	}

	estid_t place_order(offset_type offset, direction_type direction, const code_t& code, uint32_t count, double_t price, order_flag flag)noexcept;
	
	bool cancel_order(estid_t estid)noexcept;
	
	const position_info& get_position(const code_t& code)const noexcept;
	
	const order_info& get_order(estid_t estid)const noexcept;

	void find_orders(std::vector<order_info>& order_result, std::function<bool(const order_info&)> func) const noexcept;

	uint32_t get_total_position() const noexcept;

	void subscribe(const std::set<code_t>& tick_data,tick_callback tick_cb)noexcept;

	void unsubscribe(const std::set<code_t>& tick_data)noexcept;
	
	daytm_t get_last_time()noexcept;

	daytm_t last_order_time()noexcept;

	const order_statistic& get_order_statistic(const code_t& code)const noexcept;

	uint32_t get_trading_day()noexcept;

	daytm_t get_close_time()const noexcept;
	
	bool is_in_trading()const noexcept;

	inline uint32_t get_max_position()const noexcept
	{
		return _max_position;
	}

	//
	const today_market_info& get_today_market_info(const code_t& id)const noexcept;

	uint32_t get_total_pending()noexcept;

	const tick_info& get_previous_tick(const code_t& code)noexcept;

	const char* get_include_config(const char* key)noexcept;

protected :

	order_statistic get_all_statistic()const noexcept;

private:

	
	void check_crossday()noexcept;

	void handle_entrust(const std::vector<std::any>& param)noexcept;

	void handle_deal(const std::vector<std::any>& param)noexcept;

	void handle_trade(const std::vector<std::any>& param)noexcept;

	void handle_cancel(const std::vector<std::any>& param)noexcept;

	void handle_tick(const std::vector<std::any>& param)noexcept;

	void handle_error(const std::vector<std::any>& param)noexcept;

	void calculate_position(const code_t& code, direction_type dir_type, offset_type offset_type, uint32_t volume, double_t price)noexcept;
	
	void frozen_deduction(const code_t& code, direction_type dir_type, offset_type offset_type, uint32_t volume)noexcept;
	
	void unfreeze_deduction(const code_t& code, direction_type dir_type, offset_type offset_type, uint32_t volume)noexcept;

	void record_pending(const code_t& code, direction_type dir_type, offset_type offset_type, uint32_t volume)noexcept;

	void recover_pending(const code_t& code, direction_type dir_type, offset_type offset_type, uint32_t volume)noexcept;

	inline void print_position(const char* title)noexcept
	{
		if (!_position_info.empty())
		{
			LOG_INFO("print_position : ", title);
		}
		for (const auto& it : _position_info)
		{
			const auto& pos = it.second;
			LOG_INFO("position :", pos.id.get_id(), "today_long(", pos.today_long.postion, pos.today_long.frozen, ") today_short(", pos.today_short.postion, pos.today_short.frozen, ") yestoday_long(", pos.history_long.postion, pos.history_long.frozen, ") yestoday_short(", pos.history_short.postion, pos.history_short.frozen, ")");
			LOG_INFO("pending :", pos.id.get_id(), pos.long_pending, pos.short_pending);
		}
	}

protected:

	void init(const params& control_config, const params& include_config, bool reset_trading_day=false)noexcept;

public:

	virtual trader_api& get_trader()noexcept = 0;

	virtual market_api& get_market()noexcept = 0;

	virtual void on_update()noexcept = 0;

	virtual bool is_terminaled()noexcept = 0;


};

