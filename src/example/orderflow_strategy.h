#pragma once
#include "strategy.h"
#include "receiver.h"
#include "engine.h"



class orderflow_strategy : public lt::strategy,public lt::bar_receiver
{

	struct persist_data
	{
		uint32_t trading_day;
		estid_t sell_order;
		estid_t buy_order;
	};
public:

	orderflow_strategy(lt::straid_t id, lt::engine& engine, const code_t& code, uint32_t period, uint32_t open_once, double_t delta, uint32_t limit) :
		lt::strategy(id, engine, true, true),
		_code(code),
		_open_once(open_once),
		_period(period),
		_order_data(nullptr),
		_delta(delta),
		_limit(limit)
	{
	};

	~orderflow_strategy()
	{
		_order_data = nullptr;
	};


public:


	/*
	 *	��ʼ���¼�
	 *	����������ֻ��ص�һ��
	 */
	virtual void on_init(lt::subscriber& suber) override;

	/*
	*	�����ճ�ʼ�����
	*/
	virtual void on_ready() override;

	/*
	 *	bar����
	 */
	virtual void on_bar(uint32_t period, const bar_info& bar) override;


	/*
	 *	�������ջر�
	 *  @is_success	�Ƿ�ɹ�
	 *	@order	���ض���
	 */
	virtual void on_entrust(const order_info& order) override;

	/*
	 *	�ɽ��ر�
	 *
	 *	@localid	���ض���id
	 */
	virtual void on_trade(estid_t localid, const code_t& code, offset_type offset, direction_type direction, double_t price, uint32_t volume)  override;


	/*
	 *	����
	 *	@localid	���ض���id
	 */
	virtual void on_cancel(estid_t localid, const code_t& code, offset_type offset, direction_type directionv, double_t price, uint32_t cancel_volume, uint32_t total_volume)  override;

	/*
	 *	����
	 *	@localid	���ض���id
	 *	@error �������
	 */
	virtual void on_error(error_type type, estid_t localid, const uint32_t error) override;

	/*
	 *	����
	 */
	virtual void on_destroy(lt::unsubscriber& unsuber)override;

private:

	void try_buy();

	void try_sell();

	//��Լ����
	code_t _code;

	//һ�ο��ֶ�����
	uint32_t _open_once;

	//k�����ڣ����ӣ�
	uint32_t _period;

	//ʧ��delta
	double_t _delta;

	//ʧ�ⴥ���źŵ���ֵ
	uint32_t _limit ;

	persist_data* _order_data;

};
