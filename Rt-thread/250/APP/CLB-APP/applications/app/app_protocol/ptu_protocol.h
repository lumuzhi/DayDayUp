/**
 * @file
 * @details
 * @author
 * @date    Public information protocol
 * @version
**/

#ifndef __PTU_PROTOCOL_H__
#define __PTU_PROTOCOL_H__

/* include */
#include "app_board.h"

/* macro */
#define GROUP_NUM_4      4
#define GROUP_NUM_6      6


#define APP_PTU_HEAD     0xAA50

/* type declaration */
typedef enum
{
    TRAIN_1,
	TRAIN_2,
	TRAIN_3,
	TRAIN_4,
	TRAIN_5,
	TRAIN_6,

	TRAIN_NUM_MAX,
}train_num_e;
typedef struct
{
	uint8_t life[2];
	uint8_t times[6];
	uint8_t line_number[2]; //线路号
	uint8_t train_number[2];//列车号
	uint8_t ccu_speed[2];   //速度
	uint8_t curr_id[2];     //当前站ID
	uint8_t next_id[2];     //下一站ID
	uint8_t curr_train;     //当前车厢号
	uint8_t zxb_speed[2];   //速度
	uint8_t reserve[9];
}sys_public_msg_t, *psys_public_msg;

//系统设备状态
union record_dev_state
{
	struct
	{
		uint8_t v1_err   :1;
		uint8_t v2_err   :1;
		uint8_t save_err :1;
		uint8_t phy_err  :1;
		uint8_t nc3		  :4;
	}bit;
	uint8_t byte;
};

//通信故障
union commmun_state
{
	struct
	{
		uint8_t record_err :1;
		uint8_t txb_err :1;
		uint8_t clb_err :1;
		uint8_t tclb1_err :1;
		uint8_t tclb2_err :1;
		uint8_t zxb_err :1;
		uint8_t public_err:1;
		uint8_t derail_disable_flag :1;
	}bit;
	uint8_t byte;
};
typedef union
{
	struct //前置器故障
	{
		uint8_t front1_err  :1;
		uint8_t front2_err  :1;
		uint8_t front3_err  :1;
		uint8_t front4_err  :1;
		uint8_t reserve     :4;
	}bits;
	uint8_t byte;
}front_state_t, *pfront_state_t;
typedef union
{
	struct  //轴箱传感器故障
	{
		uint8_t sensor_8 :1;
		uint8_t sensor_7 :1;
		uint8_t sensor_6 :1;
		uint8_t sensor_5 :1;
		uint8_t sensor_4 :1;
		uint8_t sensor_3 :1;
		uint8_t sensor_2 :1;
		uint8_t sensor_1 :1;
	}bits;
	uint8_t byte;
}axle_sensor_t, *paxle_sensor_t;
typedef union
{
   struct
    {
    	uint8_t axle_bearing_warn_alarm : 1;//轴箱轴承预报警
    	uint8_t axle_bearing_1_alarm : 1;//轴箱轴承一级报警
    	uint8_t axle_bearing_2_alarm : 1;//轴箱轴承二级报警
    	uint8_t tread_warn_alarm : 1;//踏面预报警
    	uint8_t tread_1_alarm : 1;//踏面一级报警
    	uint8_t tread_2_alarm : 1;//踏面二级报警
    	uint8_t axle_temp_warn_alarm : 1;//轴箱温度预报警
    	uint8_t axle_temp_alarm : 1;//轴箱温度报警
    }bits;
    uint8_t  byte;
}axle_alarm_t, *paxle_alarm_t;
typedef union
{
	struct //脱轨报警
	{
		uint8_t derail_alarm_8 :1;
		uint8_t derail_alarm_7 :1;
		uint8_t derail_alarm_6 :1;
		uint8_t derail_alarm_5 :1;
		uint8_t derail_alarm_4 :1;
		uint8_t derail_alarm_3 :1;
		uint8_t derail_alarm_2 :1;
		uint8_t derail_alarm_1 :1;
	}bits;
	uint8_t byte;
}derail_alarm_t, *pderail_alarm_t;
typedef union
{
	struct //电机传感器故障
	{
		uint8_t rcc :4;
		uint8_t sensor_4 :1;
		uint8_t sensor_3 :1;
		uint8_t sensor_2 :1;
		uint8_t sensor_1 :1;
	}bits;
	uint8_t byte;
}motor_breaing_sensor_t, *pmotor_breaing_sensor_t;
typedef union
{
   struct//电机轴承故障
    {
    	uint8_t motor_gear_warn : 1;//电机齿轮预警
    	uint8_t motor_gear_1_alarm : 1;//端电机齿轮一级报警
    	uint8_t motor_gear_2_alarm  : 1;//端电机齿轮2级报警
    	uint8_t motor_breaing_warn : 1;//电机轴承预警
    	uint8_t motor_breaing_1_alarm  : 1;//电机轴承一级报警
    	uint8_t motor_breaing_2_alarm  : 1;//电机轴承2级报警
    	uint8_t motor_breaing_temp_alarm : 1;//电机轴承温报警
    	uint8_t motor_breaing_temp_warn : 1;//电机轴承温预警
    }bits;
    uint8_t  byte;
}motor_breaing_alarm_t, *pmotor_breaing_alarm;
typedef union
{
	struct  //齿轮轴承传感器故障
	{
		uint8_t rcc :4;
		uint8_t sensor_4 :1;
		uint8_t sensor_3 :1;
		uint8_t sensor_2 :1;
		uint8_t sensor_1 :1;
	}bits;
	uint8_t byte;
}gear_bearing_sensor_t, *pgear_bearing_sensor_t;
typedef union
{
   struct//齿轮轴承故障
    {
    	uint8_t gear_gear_warn: 1;//齿轮箱车轮侧齿轮预警
    	uint8_t gear_gear_1_alarm : 1;//齿轮箱车轮侧齿轮一级报警
    	uint8_t gear_gear_2_alarm : 1;//齿轮箱车轮侧齿轮2级报警
    	uint8_t gear_bearing_warn_alarm : 1;//齿轮箱车轮侧轴承预报警
    	uint8_t gear_bearing_1_alarm : 1;//齿轮箱车轮侧轴承一级报警
    	uint8_t gear_bearing_2_alarm : 1;//齿轮箱车轮侧轴承2级报警
    	uint8_t gear_bearing_warn : 1;//齿轮箱车轮侧轴承温度预警
    	uint8_t gear_bearing_temp_alarm : 1;//齿轮箱车轮侧轴承温度报警
    }bits;
    uint8_t  byte;
}gear_bearing_alarm_t, *pgear_bearing_alarm_t;
//sensor温度
typedef struct
{
	char axle_temp[8];
	char motor_temp[4];
	char gear_temp[4];
}sensor_temp_t, *psensor_temp_t;
//env温度
typedef struct
{
	char env_tem[4];
}env_temp_t, *penv_temp_t;
typedef union
{
	struct
	{
		uint8_t v1_err :1;
		uint8_t v2_err :1;
		uint8_t trdp_err :1;
		uint8_t mvb_err :1;
		uint8_t ncc :4;
	}bits;
	uint8_t byte;
}txb_dev_state_t, *ptxb_dev_state_t;
typedef union
{
	struct
	{
		uint8_t bear_serson4 :2;
		uint8_t bear_serson3 :2;
		uint8_t bear_serson2	:2;
		uint8_t bear_serson1	:2;

		uint8_t bear_serson8	:2;
		uint8_t bear_serson7 :2;
		uint8_t bear_serson6 :2;
		uint8_t bear_serson5 :2;

		uint8_t motor_serson4 :2;
		uint8_t motor_serson3 :2;
		uint8_t motor_serson2 :2;
		uint8_t motor_serson1 :2;

		uint8_t gear_serson4 :2;
		uint8_t gear_serson3 :2;
		uint8_t gear_serson2 :2;
		uint8_t gear_serson1 :2;

		uint8_t v1_err    :1;
		uint8_t v2_err    :1;
		uint8_t sd_err    :1;
		uint8_t flash_err :1;
		uint8_t ncc       :4;

	}bits;
	uint8_t byte[5];
}clb_dev_state_t, *pclb_dev_state_t;
typedef union
{
	struct
	{
		uint8_t v1_err      :1;
		uint8_t v2_err      :1;
		uint8_t ad_comm_err :1;
		uint8_t save_err    :1;
		uint8_t derail_disable_flag:1;
		uint8_t ncc1        :3;
	}bits;
	uint8_t byte;
}tclb_dev_state_t, *ptclb_dev_state_t;
typedef union
{
	struct
	{
		uint8_t v1_err      :1;
		uint8_t v2_err      :1;
		uint8_t save_err    :1;
		uint8_t speed_vilad :1;
		uint8_t ncc1        :4;
	}bits;
	uint8_t byte;
}zxb_dev_state_t, *pzxb_dev_state_t;
typedef struct
{
	sys_public_msg_t       sys_public_msg;          //Byte0-29
	union record_dev_state record_dev_state;        //Byte30
	union commmun_state    commmun_state;           //Byte31
	front_state_t          front_state;             //Byte32
	axle_sensor_t          axle_sensor;             //Byte33
	axle_alarm_t           axle_alarm[8];           //Byte34-41
	derail_alarm_t         derail_alarm;            //Byte42
	motor_breaing_sensor_t motor_breaing_sensor;    //Byte43
	motor_breaing_alarm_t  motor_breaing_alarm[4];  //Byte44-47
	gear_bearing_sensor_t  gear_bearing_sensor;     //Byte48
	gear_bearing_alarm_t   gear_bearing_alarm[4];   //Byte49-52
	sensor_temp_t          sensor_temp;             //Byte53-68
	env_temp_t             env_temp;                //Byte69-72
	txb_dev_state_t        txb_dev_state;           //Byte73
	clb_dev_state_t        clb_dev_state;           //Byte74-78
	tclb_dev_state_t       tclb_dev_state;          //Byte79
	zxb_dev_state_t        zxb_dev_state;           //Byte80
	uint8_t                reserve[19];             //Byte81-99
}monitor_common_data_t;
typedef struct
{
	uint8_t data_head[2];
	uint8_t data_len[2];
	uint8_t factory_code;
	uint8_t device_code;
	uint8_t life_signal[2];
	uint8_t target_addr[2];
	uint8_t resend_flag;
	uint8_t answer_flag;
	uint8_t udp_packet;
	uint8_t reserve[10];
	uint8_t train_num;
	uint8_t cmd;
	uint8_t data_info[sizeof(monitor_common_data_t) * TRAIN_NUM_MAX];
	uint8_t sum_crc[2];
}ptu_monitor_protocol_t;
typedef struct
{
	uint8_t target_addr;
	uint8_t cmd_date;
}cmd_date_t, *pcmd_date_t;
typedef struct
{
	uint8_t data_head[2];
	uint8_t data_len[2];
	uint8_t factory_code;
	uint8_t device_code;
	uint8_t life_signal[2];
	uint8_t target_addr[2];
	uint8_t resend_flag;
	uint8_t answer_flag;
	uint8_t udp_packet;
	uint8_t reserve[10];
	uint8_t train_num;
	uint8_t cmd;
	uint8_t data_inform[];
}ptu_recv_cmd_protocol_t, *pptu_recv_cmd_protocol_t;

#endif /*__PTU_PROTOCOL_H__*/





