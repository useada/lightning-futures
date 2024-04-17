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
#include <define.h>
#include <trader_api.h>
#include <params.hpp>
#include "contract_parser.h"


class trader_simulator : public dummy_trader
{

	enum order_state
	{
		OS_INVALID,
		OS_IN_MATCH,
		OS_CANELED,
	};

	struct order_match
	{
		uint32_t	queue_seat; //队列前面有多少个
		order_state		state;
		order_flag		flag;
		estid_t estid;

		order_match(estid_t& ord, order_flag flg)noexcept :estid(ord), queue_seat(0), state(OS_INVALID), flag(flg)
		{}
	};
	struct position_item
	{
		//仓位
		uint32_t	postion;
		//价格
		double_t	price;
		//冻结
		uint32_t	frozen;

		position_item()noexcept :
			postion(0),
			price(.0F),
			frozen(0)
		{}

		uint32_t usable()const noexcept
		{
			return postion - frozen;
		}

		bool empty()const noexcept
		{
			return postion == 0;
		}

		void clear()noexcept
		{
			postion = 0;
			price = .0F;
			frozen = 0;
		}
	};

	struct position_detail
	{
	
		//今仓
		position_item today_long;
		position_item today_short;

		//昨仓
		position_item yestoday_long;
		position_item yestoday_short;

		bool empty()const noexcept
		{
			return today_long.empty() && today_short.empty() && yestoday_long.empty() && yestoday_short.empty();
		}

		uint32_t get_total()const noexcept
		{
			return today_long.postion + today_short.postion + yestoday_long.postion + yestoday_short.postion;
		}

		int32_t get_real()const noexcept
		{
			return today_long.postion + yestoday_long.postion - (today_short.postion + yestoday_short.postion);
		}

		uint32_t get_long_position()const noexcept
		{
			return today_long.postion + yestoday_long.postion;
		}

		uint32_t get_short_position()const noexcept
		{
			return today_short.postion + yestoday_short.postion;
		}
		uint32_t get_long_frozen()const noexcept
		{
			return today_long.frozen + yestoday_long.frozen;
		}

		uint32_t get_short_frozen()const noexcept
		{
			return today_short.frozen + yestoday_short.frozen;
		}
		position_detail()
		{}
	};


private:
	

	uint32_t _trading_day;

	daytm_t _current_time;
	
	uint32_t _order_ref;

	//撮合时候用
	std::map<code_t,tick_info> _current_tick_info;

	//上一帧的成交量，用于计算上一帧到这一帧成交了多少
	std::map<code_t,uint64_t> _last_frame_volume ;

	account_info _account_info;
	
	uint32_t	_interval;			//间隔毫秒数
	
	contract_parser	_contract_parser;	//合约信息配置

	std::map<estid_t, order_info> _order_info;

	std::map<code_t, std::vector<order_match>> _order_match;

	std::map<code_t, position_detail> _position_info;

public:

	trader_simulator(const params& config)noexcept;
	
	virtual ~trader_simulator()noexcept;

	
public:
	
	virtual void push_tick(const tick_info& tick) noexcept override;
	
	virtual void crossday(uint32_t trading_day) noexcept override;

	virtual const account_info& get_account() noexcept override
	{
		return _account_info;
	}

	virtual void update()noexcept override ;

public:

	virtual uint32_t get_trading_day()const noexcept override;
	// td
	virtual bool is_usable()const noexcept override;

	virtual estid_t place_order(offset_type offset, direction_type direction, const code_t& code, uint32_t count, double_t price, order_flag flag) noexcept override;

	virtual bool cancel_order(estid_t estid) noexcept override;

	virtual std::shared_ptr<trader_data> get_trader_data() noexcept override;

private:
	
	estid_t make_estid()noexcept;

	uint32_t get_buy_front(const code_t& code, double_t price)noexcept;
	
	uint32_t get_sell_front(const code_t& code, double_t price)noexcept;

	void match_entrust(const tick_info* tick)noexcept;

	void handle_entrust(const tick_info* tick, order_match& match, order_info& order, uint32_t max_volume)noexcept;

	void handle_sell(const tick_info* tick, order_match& match, order_info& order, uint32_t deal_volume)noexcept;
	
	void handle_buy(const tick_info* tick, order_match& match, order_info& order, uint32_t deal_volume)noexcept;

	void order_deal(order_info& order, uint32_t deal_volume)noexcept;

	void order_error(error_type type, estid_t estid, error_code err)noexcept;

	void order_cancel(const order_info& order)noexcept;

	void remove_order(estid_t estid)noexcept;

	void visit_match_info(estid_t estid, std::function<void(order_match&)> cursor)noexcept;
	//冻结
	error_code frozen_deduction(estid_t estid, const code_t& code, offset_type offset, direction_type direction, uint32_t count, double_t price)noexcept;
	//解冻
	bool unfrozen_deduction(const code_t& code, offset_type offset, direction_type direction, uint32_t last_volume, double_t price)noexcept;

};
