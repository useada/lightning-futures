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
#include "define.h"
#include "define_types.hpp"


#define LIGHTNING_VERSION 0.2.0


enum context_type
{
	CT_INVALID,
	CT_RUNTIME,
	CT_EVALUATE
};

struct ltobj
{
	void* obj_ptr;

	context_type obj_type;

	//ltobj(context_type type):obj_type(type), obj_ptr(nullptr) {}
};

extern "C"
{
#define VOID_DEFAULT

#define LT_INTERFACE_DECLARE(return_type, func_name, args) EXPORT_FLAG return_type lt_##func_name args

#define LT_INTERFACE_CHECK(object_name, default_return) \
	if (lt.obj_type != CT_RUNTIME && lt.obj_type != CT_EVALUATE)\
	{\
		return default_return; \
	}\
	object_name* c = (object_name*)(lt.obj_ptr); \
	if (c == nullptr)\
	{\
		return default_return; \
	}

#define LT_INTERFACE_CALL(func_name, args) return c->func_name args;

#define LT_INTERFACE_IMPLEMENTATION(return_type ,default_return, object_name ,func_name, formal_args, real_args) return_type lt_##func_name formal_args\
{\
LT_INTERFACE_CHECK(object_name,default_return)\
LT_INTERFACE_CALL(func_name,real_args)\
}

	typedef void (PORTER_FLAG * tick_callback)(const tick_info&);

	typedef void (PORTER_FLAG * entrust_callback)(const order_info&);

	typedef void (PORTER_FLAG * deal_callback)(estid_t, uint32_t );

	typedef void (PORTER_FLAG * trade_callback)(estid_t, const code_t&, offset_type, direction_type, double_t, uint32_t);

	typedef void (PORTER_FLAG * cancel_callback)(estid_t, const code_t&, offset_type, direction_type, double_t, uint32_t, uint32_t);

	typedef void (PORTER_FLAG * error_callback)(error_type , estid_t, error_code);

	typedef void (PORTER_FLAG * cycle_callback)();

	
	struct order_event
	{
		entrust_callback on_entrust;

		deal_callback on_deal;

		trade_callback on_trade;

		cancel_callback on_cancel;

		error_callback on_error;

		order_event() = default;
		
		/*:
			on_entrust(nullptr),
			on_deal(nullptr),
			on_trade(nullptr),
			on_cancel(nullptr),
			on_error(nullptr)
			{}*/
	};

	EXPORT_FLAG ltobj lt_create_context(context_type ctx_type, const char* config_path);
	
	EXPORT_FLAG void lt_destory_context(ltobj& obj);
	
	/*登录*/
	LT_INTERFACE_DECLARE(bool, login_account, (const ltobj&));

	/*注销*/
	LT_INTERFACE_DECLARE(void, logout_account, (const ltobj&));

	/*启动*/
	LT_INTERFACE_DECLARE(bool, start_service, (const ltobj&));
	
	/*停止*/
	LT_INTERFACE_DECLARE(bool, stop_service, (const ltobj&));
	
	/*
	下单
	*/
	LT_INTERFACE_DECLARE(estid_t, place_order, (const ltobj&, offset_type, direction_type, const code_t&, uint32_t, double_t, order_flag));
	
	/*
	* 撤销订单
	*/
	LT_INTERFACE_DECLARE(bool, cancel_order, (const ltobj&, estid_t));

	/**
	* 获取仓位信息
	*/
	LT_INTERFACE_DECLARE(const position_info&, get_position, (const ltobj&, const code_t&));

	/**
	* 获取委托订单
	**/
	LT_INTERFACE_DECLARE(const order_info&, get_order, (const ltobj&, estid_t));
	
	/**
	* 订阅行情
	**/
	LT_INTERFACE_DECLARE(void, subscribe, (const ltobj&, const std::set<code_t>&, tick_callback));
	
	/**
	* 取消订阅行情
	**/
	LT_INTERFACE_DECLARE(void, unsubscribe, (const ltobj&, const std::set<code_t>&));

	/**
	* 获取时间
	*
	*/
	LT_INTERFACE_DECLARE(daytm_t, get_last_time, (const ltobj&));

	
	/*
	* 绑定实时回调 
	*/
	LT_INTERFACE_DECLARE(void, bind_realtime_event, (const ltobj&, const order_event&, cycle_callback, cycle_callback, cycle_callback));

	
	/**
	* 播放历史数据
	*
	*/
	LT_INTERFACE_DECLARE(void, playback_history, (const ltobj&));

	/**
	* 模拟跨天结算
	*
	*/
	LT_INTERFACE_DECLARE(void, simulate_crossday, (const ltobj&,uint32_t));
	/**
	* 获取最后一次下单时间
	*	跨交易日返回0
	*/
	LT_INTERFACE_DECLARE(daytm_t, last_order_time, (const ltobj&));
	
	/**
	* 获取当前交易日的订单统计
	*	跨交易日会被清空
	*/
	LT_INTERFACE_DECLARE(const order_statistic&, get_order_statistic, (const ltobj&,const code_t&));

	
	/**
	* 获取交易日
	*/
	LT_INTERFACE_DECLARE(uint32_t, get_trading_day, (const ltobj&));
	
	
	/**
	* 获取收盘时间
	*/
	LT_INTERFACE_DECLARE(daytm_t, get_close_time, (const ltobj&));

	/**
	* 返回include标签中的配置信息
	*/
	LT_INTERFACE_DECLARE(const char *, get_include_config, (const ltobj&, const char *));


	/**
	* 获取今日行情数据
	*/
	LT_INTERFACE_DECLARE(const today_market_info&, get_today_market_info, (const ltobj&,const code_t&));


	/**
	* 获取前一个tick
	*/
	LT_INTERFACE_DECLARE(const tick_info&, get_previous_tick, (const ltobj&, const code_t&));

}
