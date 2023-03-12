#pragma once
#include "define.h"

struct tick_info
{
	code_t id; //合约ID

	time_t time; //时间

	uint32_t tick; //毫秒数

	double_t price;  //pDepthMarketData->LastPrice

	double_t open;
	
	double_t close;

	double_t high;

	double_t low;

	double_t high_limit;

	double_t low_limit;

	double_t standard;

	uint32_t volume;

	uint32_t trading_day;

	std::array<std::pair<double_t, uint32_t>, 5> buy_order;
	std::array<std::pair<double_t, uint32_t>, 5> sell_order;

	double_t buy_price()const
	{
		if (buy_order.empty())
		{
			return 0;
		}
		return buy_order[0].first;
	}
	double_t sell_price()const
	{
		if (sell_order.empty())
		{
			return 0;
		}
		return sell_order[0].first;
	}
	tick_info()
		:time(0),
		tick(0),
		open(0),
		close(0),
		high(0),
		low(0),
		high_limit(0),
		low_limit(0),
		price(0),
		standard(0),
		volume(0),
		trading_day(0)
	{}


	int32_t total_buy_valume()const
	{
		int32_t reuslt = 0;
		for (auto& it : buy_order)
		{
			reuslt += it.second;
		}
		return reuslt;
	}

	int32_t total_sell_valume()const
	{
		int32_t reuslt = 0;
		for (auto& it : sell_order)
		{
			reuslt += it.second;
		}
		return reuslt;
	}
};

struct position_item
{
	//仓位
	uint32_t	postion;
	//价格
	double_t	price;
	//冻结
	uint32_t	frozen;

	position_item() :
		postion(0),
		price(.0F),
		frozen(0)
	{}

	uint32_t usable()const
	{
		return postion - frozen;
	}

	void clear()
	{
		postion = 0;
		price = .0F;
		frozen = 0 ;
	}
};

struct position_info
{
	code_t id; //合约ID

	//今仓
	position_item today_long;
	position_item today_short;


	//昨仓
	position_item yestoday_long;
	position_item yestoday_short;

	uint32_t get_total()const 
	{
		return today_long.postion + today_short.postion + yestoday_long.postion + yestoday_short.postion;
	}

	uint32_t get_long_position()const
	{
		return today_long.postion+ yestoday_long.postion;
	}

	uint32_t get_short_position()const
	{
		return today_short.postion + yestoday_short.postion;
	}
	uint32_t get_long_frozen()const
	{
		return today_long.frozen + yestoday_long.frozen;
	}

	uint32_t get_short_frozen()const
	{
		return today_short.frozen + yestoday_short.frozen;
	}
	position_info()
	{}
};
const position_info default_position;

struct account_info
{
	double money;

	double frozen_monery;

	account_info() :
		money(.0F),
		frozen_monery(.0F)
		
	{}
};
const account_info default_account;

typedef enum trading_optimal
{
	TO_INVALID = 0,
	//保证金最优（针对平今收取高手续费品种，平转开）
	TO_CLOSE_TO_OPEN = 1,
	//手续费最优（开转平，可以节约保证金做更多仓位）
	TO_OPEN_TO_CLOSE = 2
} trading_optimal;



/*
 *	订单标志
 */
typedef enum order_flag
{
	OF_NOR = '0',		//普通订单
	OF_FAK,			//全成全撤，不等待自动撤销
	OF_FOK,			//部成部撤，不等待自动撤销
} order_flag;

/*
 *	开平方向
 */
typedef enum offset_type
{
	OT_OPEN = '0',	//开仓
	OT_CLOSE		//平仓,上期为平昨
} offset_type;

/*
 *	多空方向
 */
typedef enum direction_type
{
	DT_LONG = '0',	//做多
	DT_SHORT		//做空
} direction_type;

struct order_info
{
	
	estid_t		est_id;

	code_t			code;

	std::string		unit_id ;

	uint32_t		total_volume;

	uint32_t		last_volume;

	time_t			create_time;

	offset_type		offset;

	direction_type	direction;

	double_t		price;

	order_info() :
		est_id(INVALID_ESTID),
		offset(OT_OPEN),
		direction(DT_LONG),
		total_volume(0),
		last_volume(0),
		create_time(0),
		price(.0f)
	{}

	bool is_valid()
	{
		return est_id != INVALID_ESTID;
	}

	
};
const order_info default_order ;


//订单统计数据
struct order_statistic
{
	//下单数量
	uint32_t place_order_amount;
	//委托数量
	uint32_t entrust_amount;
	//成交数量
	uint32_t trade_amount;
	//撤单数量
	uint32_t cancel_amount;
	//错误数量
	uint32_t error_amount;

	order_statistic():
		place_order_amount(0),
		entrust_amount(0),
		trade_amount(0),
		cancel_amount(0),
		error_amount(0)
	{}

};

const order_statistic default_statistic;

typedef enum error_type
{
	ET_PLACE_ORDER,
	ET_CANCEL_ORDER,
	ET_OTHER_ERROR
}error_type;