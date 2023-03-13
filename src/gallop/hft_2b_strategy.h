#pragma once
#include "strategy.h"
#include <random>

class hft_2b_strategy : public strategy
{
public:

	hft_2b_strategy(const code_t& code, uint32_t open_once, double open_delta, uint32_t history, int32_t protection, int32_t yestoday_multiple, int32_t random_offset) :
		strategy(),
		_code(code),
		_sell_order(INVALID_ESTID),
		_buy_order(INVALID_ESTID),
		_yestoday_sell_order(INVALID_ESTID),
		_yestoday_buy_order(INVALID_ESTID),
		_open_once(open_once),
		_open_delta(open_delta),
		_yestoday_multiple(yestoday_multiple),
		_history_count(history),
		_history_ma(0),
		_coming_to_close(0),
		_random(0, random_offset),
		_protection(protection)
	{
	};


	~hft_2b_strategy() {};


public:


	/*
	 *	初始化事件
	 *	生命周期中只会回调一次
	 */
	virtual void on_init() override;

	/*
	*	交易日初始化完成
	*/
	virtual void on_ready() override;

	/*
	 *	tick推送
	 */
	virtual void on_tick(const tick_info& tick, const deal_info& deal)  override;


	/*
	 *	订单接收回报
	 *  @is_success	是否成功
	 *	@order	本地订单
	 */
	virtual void on_entrust(const order_info& order) override;

	/*
	 *	成交回报
	 *
	 *	@localid	本地订单id
	 */
	virtual void on_trade(estid_t localid, const code_t& code, offset_type offset, direction_type direction, double_t price, uint32_t volume)  override;


	/*
	 *	撤单
	 *	@localid	本地订单id
	 */
	virtual void on_cancel(estid_t localid, const code_t& code, offset_type offset, direction_type directionv, double_t price, uint32_t cancel_volume, uint32_t total_volume)  override;

	/*
	 *	错误
	 *	@localid	本地订单id
	 *	@error 错误代码
	 */
	virtual void on_error(error_type type, estid_t localid, const uint32_t error) override;


private:

	void add_to_history(double_t price);

private:

	code_t _code;

	int32_t _protection;

	double _open_delta;

	uint32_t _open_once;

	uint32_t _yestoday_multiple;

	estid_t _sell_order;

	estid_t _buy_order;

	estid_t _yestoday_sell_order;

	estid_t _yestoday_buy_order;

	tick_info _last_tick;

	size_t _history_count;

	time_t _coming_to_close;

	double_t _history_ma;

	std::list<double_t> _history_price;

	std::default_random_engine _random_engine;

	std::uniform_int_distribution<int> _random;
};

