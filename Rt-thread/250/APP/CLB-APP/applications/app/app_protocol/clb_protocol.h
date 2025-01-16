/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __CLB_PROTOCOL_H__
#define __CLB_PROTOCOL_H__

/* include */
#include "app_board.h"
#include "qzq_protocol.h"
#include "app_ad7606.h"

/* macro */

/* type declaration */

/*----------------报警值结构--------------*/
typedef enum
{
    QZQ_1,
    QZQ_2,
    QZQ_3,
    QZQ_4,

    QZQ_MAX,
}qzq_num_e;
typedef enum
{
    INDEX_1 = 0,   //1位轴端
    INDEX_2,
    INDEX_3,
    INDEX_4,
    INDEX_5,
    INDEX_6,
    INDEX_7,
    INDEX_8,
    INDEX_MAX,
}zc_num_e;
typedef union
{
    struct
    {
        uint8_t reserve :4;
        uint8_t qzq_4   :1;
        uint8_t qzq_3   :1;
        uint8_t qzq_2   :1;
        uint8_t qzq_1   :1; //故障标识 0：正常，1：故障
    }bits;
    uint8_t byte;
}qzq_state_t;
typedef union
{
    // 低到搞 num_1 bit7
    struct
    {
        uint8_t num_8   :1;
        uint8_t num_7   :1;
        uint8_t num_6   :1;
        uint8_t num_5   :1;
        uint8_t num_4   :1;
        uint8_t num_3   :1;
        uint8_t num_2   :1;
        uint8_t num_1   :1; //故障标识 0：正常，1：故障
    }bits;
    uint8_t byte;
}cgq_state_t;
typedef union
{
    struct
    {
        uint8_t cl_warning     :1; //预警
        uint8_t cl_1_alarm     :1; //一级报警
        uint8_t cl_2_alarm     :1; //二级报警
        uint8_t zc_warning     :1; //轴承预警
        uint8_t zc_1_alarm     :1; //轴承一级报警
        uint8_t zc_2_alarm     :1; //轴承二级报警
        uint8_t temp_warning   :1; //温度预警
        uint8_t temp_alarm     :1; //温度报警
    }bits;
    uint8_t byte;
}dj_clx_alarm_t;
typedef union
{
    struct
    {
        uint8_t zc_warning     :1; //预警
        uint8_t zc_1_alarm     :1; //一级报警
        uint8_t zc_2_alarm     :1; //二级报警
        uint8_t tm_warning     :1; //踏面预警
        uint8_t tm_1_alarm     :1; //踏面一级报警
        uint8_t tm_2_alarm     :1; //踏面二级报警
        uint8_t temp_warning   :1; //温度预警
        uint8_t temp_alarm     :1; //温度报警
    }bits;
    uint8_t byte;
}zxzc_alarm_t;
typedef struct
{
    uint8_t v1_voltage_fault  :1;
    uint8_t v2_voltage_fault  :1;
    uint8_t sd_save_fault     :1;
    uint8_t flash_save_fault  :1;
    uint8_t reserve           :4;
}sys_dev_state_t;
typedef struct
{
    uint8_t             train_num;
    uint8_t             version[2];
    uint8_t             diag_state;
    uint8_t             temp_state;
    sys_dev_state_t     sys_dev_state;             //Byte0 当前板卡设备状态
    qzq_state_t         qzq_fault;                 //Byte1 前置器故障
    cgq_state_t         zxzc_sensor;              //Byte2 轴箱轴承传感器故障
    zxzc_alarm_t        zxzc_alarm[8];             //Byte3-10 轴箱轴承预报警状态
    cgq_state_t         dj_sensor;                //Byte11 电机传感器故障
    dj_clx_alarm_t      dj_alarm[4];               //Byte12-15 电机轴承预报警状态
    cgq_state_t         clx_sensor;                //Byte16 齿轮箱传感器故障
    dj_clx_alarm_t      clx_alarm[4];              //Byte17-20 齿轮箱轴承预报警状态
    int8_t              zx_temp[8];                //Byte21-28 轴箱温度
    int8_t              dj_temp[4];                //Byte29-32 电机温度
    int8_t              clx_temp[4];               //Byte33-36 齿轮箱温度
    int8_t              env_temp[4];               //Byte37-40 环境温度
    uint8_t             train_id;                  //Byte41 当前车厢号
    uint8_t             reserve[9];                //Byte42-50 预留
}clb_port_msg_t;

/*-----------------------特征值结构--------------------- */
//温度存放结构体
typedef struct //16 bytes
{
    uint8_t zx_temp[8];           //轴箱轴承温度
    uint8_t dj_temp[4];             //电机温度
    uint8_t clx_temp[4];            //齿轮箱温度
}temp_tz_t;
//轴承故障特征数据
typedef struct //4 bytes
{
    uint8_t amplitude_inner_ring;           //内环故障特征数据
    uint8_t amplitude_outer_ring;           //外环故障特征数据
    uint8_t amplitude_rolling_body;         //滚动体故障特征数据
    uint8_t amplitude_holder;               //保持架故障特征数据
}bearing_tz_t;
//温度数据clx_temp
typedef struct //64 byte
{
    uint8_t train_id;//温度自检结果 1:err 0:ok ??如何算err
    temp_tz_t diag_res; //诊断结果（报警状态）RUNNING_OK=0x00,TEMP_WARN=0x01,TEMP_ALARM=0x02,ACC_ALARM_FIRST_GRADE=0x03,ACC_ALARM_SECOND_GRADE  0x04
    temp_tz_t temp;  //每秒的卡尔曼滤波之后的平均走行部温度
    temp_tz_t diff_temp;//环温与走行部温升的差值
    uint8_t env_temp[4];    //环境温度
    uint8_t reserve[11];
}save_tz_temp_t;

//轴箱轴承数据
typedef struct //8 byte
{
    uint8_t channel_id;      //通道编号1-8
    uint8_t tm_db;           //踏面DB值
    bearing_tz_t bearing_tz;  //type不同代表的位置不同 type_zx_zc（轴箱轴承）, type_dj_zc（电机驱动端存储数据），type_clx_zc（齿轮箱输入电机侧）
    uint8_t alarm_bearing;      //轴承故障      0：正常   1：预警   2：一级报警   3:二级报警
    uint8_t alarm_tm;       //轴承故障      0：正常   1：预警   2：一级报警   3:二级报警
}save_zxzc_t;
//齿轮箱或电机轴承数据
typedef struct //16 byte
{
    uint8_t channel_id;      //通道编号1-8
    uint8_t speed_valid_flag;
    bearing_tz_t bearing_tz1;  //type不同代表的位置不同 type_zx_zc（轴箱轴承）, type_dj_zc（电机驱动端存储数据），type_clx_zc（齿轮箱输入电机侧）
    bearing_tz_t bearing_tz2;  //type不同代表的位置不同 type_zx_zc（轴箱轴承）, type_dj_zc（电机驱动端存储数据），type_clx_zc（齿轮箱输入电机侧）
    uint8_t alarm_bearing;      //轴承故障      0：正常   1：预警   2：一级报警   3:二级报警
    uint8_t reserve[5];
}save_dj_clx_t;
//多边形数据
typedef struct //8 byte
{
    uint8_t channel_id;     //通道号1-8，对应轴箱轴承位编号
    uint8_t diag_res;      //诊断结果（报警状态）     RUNNING_OK=0x00,TEMP_WARN=0x01,TEMP_ALARM=0x02,ACC_ALARM_FIRST_GRADE=0x03,ACC_ALARM_SECOND_GRADE=0x04
    uint8_t TZ1_order_h;    //特征数据1  多边形阶次
    uint8_t TZ1_order_l;
    uint8_t TZ2_rms_h;      //1=0.01特征数据2  加速度均方根(不存)
    uint8_t TZ2_rms_l;
    uint8_t TZ3_DB_h;       //1=0.01特征数据3  DB值
    uint8_t TZ3_DB_l;
}polygon_tz_t;
//波磨数据
typedef struct //8 byte
{
    uint8_t indi1357_result_h;       //1357位车轮对应钢轨的波磨指标
    uint8_t indi1357_result_l;
    uint8_t indi2468_result_h;       //2468位车轮对应钢轨的波磨指标
    uint8_t indi2468_result_l;
    uint8_t nameda1357_result_h;     //1357位车轮对应钢轨的波长指标
    uint8_t nameda1357_result_l;
    uint8_t nameda2468_result_h;     //2468位车轮对应钢轨的波长指标
    uint8_t nameda2468_result_l;
}wave_tz_t;
//需要保存的多边形及波磨特征数据的结构类型
typedef struct//72 byte
{
    polygon_tz_t polygon_tz[8]; //对应8位轴箱的多边形特征值
    wave_tz_t wave_tz;          //8个轴箱算出的波磨特征值
}save_dbx_t;
//齿轮数据
typedef struct //8 byte 使用齿轮振动数据
{
    uint8_t channel_id;      //通道编号1-8
    uint8_t gear_DB_h;
    uint8_t gear_DB_l;          //1=0.01
    uint8_t gear_amp_h;         //1=0.01
    uint8_t gear_amp_l;
    uint8_t gear_fre_h;         //1=0.01
    uint8_t gear_fre_l;
    uint8_t alarm_status;       //齿轮故障特征数据
}save_gear_t;
//传感器数据
typedef struct //4 byte 使用齿轮振动数据
{
    qzq_state_t qzq_status; //前置器状态
    cgq_state_t zxzc_cgq_status; //轴箱轴承传感器状态
    cgq_state_t dj_cgq_status; //电机轴承传感器状态
    cgq_state_t clx_cgq_status; //齿轮箱轴承传感器状态
}save_cgq_t;

typedef struct
{
    uint8_t year;
    uint8_t mon;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t train_id;
    save_tz_temp_t temperature;
    save_zxzc_t zx_zc;
    save_dj_clx_t dj_clx_zc;
    save_dbx_t dbx;
    save_gear_t cl;
    save_cgq_t cgq;
    uint8_t reserve[20];
}feature_data_t;

struct DERAIL_CALC
{
    float *acc_buf;
//    uint16_t buf_len;
//    uint8_t peak[DERAIL_N][DERAIL_DATA_NUM];
//    uint8_t abs[DERAIL_N];
//    float uplift[SAMPLE_SIZE];
};

/*------------------发送报文结构--------------------*/
typedef struct
{
    uint8_t head[2];
    uint8_t size[2];   //除head、size、check外的字节数
    uint8_t check;
    uint8_t data[sizeof(clb_port_msg_t)];
}clb_port_info_t;
typedef enum
{
    AXLE_IMPACT  = 0x01,  //轴箱冲击，用于计算轴箱轴承与踏面
    AXLE_VIBRATE = 0x02,  //轴箱振动，用于计算多边形与波磨
    MOTOR_IMPACT = 0x03,  //电机冲击，用于计算电机轴承
    MOTOR_VIBRATE= 0x04,  //电机振动
    GEAR_IMPACT  = 0x05,  //齿轮冲击， 用于计算齿轮箱轴承
    GEAR_VIBRATE = 0x06,  //齿轮振动，用于计算齿轮箱齿轮
    QZQ_TEMP     = 0x07,  //前置器温度
    WAVE_GRIND   = 0x08,  //波磨
}clb_data_type_e;
typedef struct
{
    uint8_t head_h; //包头 H  byte0 66 (存储包头：66bb)
    uint8_t head_l; //包头 L  byte1 bb (存储包头：66bb)
    uint8_t data_len[2]; //数据包长度   byte2-3

    uint8_t type; //数据类型 byte4
    uint8_t year;  //年 byte5
    uint8_t month; //月 byte6
    uint8_t day;   //日 byte7
    uint8_t hour;  //时 byte8
    uint8_t min;   //分 byte9
    uint8_t sec;   //秒 byte10

    uint8_t pack_count_h; //数据包计数H byte11
    uint8_t pack_count_l; //数据包计数L byte12
    uint8_t current_id_h; //当前站ID H byte13
    uint8_t current_id_l; //当前站ID L byte14
    uint8_t next_id_h; //下一站ID H byte15

    uint8_t next_id_l; //下一站ID L byte16
    uint8_t speed_h; //速度 H byte17
    uint8_t speed_l; //速度 L byte18
    uint8_t wheel_diameter_h; //轮径 H byte19

    uint8_t wheel_diameter_l; //轮径 L byte20
    uint8_t channel; //adg1608片选模拟通道 byte21
    uint8_t host_slave_flag; //主从机标识 byte22 //适合主从机记录板自己记录的机型
    uint8_t train_id; //车厢号ID byte23 //适合主机记录板统一存所有车厢信息的机型
    uint8_t reserve[40];
}clb_save_head_t;
typedef struct
{
    uint8_t pt1_data[TEMP_OSR * 2];
    uint8_t pt2_data[TEMP_OSR * 2];
    uint8_t pt3_data[TEMP_OSR * 2];
    uint8_t pt4_data[TEMP_OSR * 2];
    uint8_t env_data[TEMP_OSR * 2];
}clb_save_temp_t;
typedef struct
{
    uint8_t head[2];
    uint8_t size[2];   //除head、size、check外的字节数
    uint8_t check;
    uint8_t data[sizeof(clb_save_head_t) + QZQ_MAX * sizeof(clb_save_temp_t)];
}clb_temp_save_info_t;
typedef struct
{
    uint8_t count[2];
    uint8_t channel;
    uint8_t pkg_cnts;
}clb_raw_save_pkg_t;
typedef struct
{
    uint8_t head[2];
    uint8_t size[2];   //除head、size、check外的字节数
    uint8_t check;
    uint8_t data[sizeof(clb_save_head_t) + sizeof(clb_raw_save_pkg_t) + 1024];
}clb_raw_save_info_t;
typedef struct
{
    uint8_t head[2];
    uint8_t size[2];   //除head、size、check外的字节数
    uint8_t check;
    uint8_t data[sizeof(feature_data_t)];
}clb_feature_save_info_t;


#endif /*__CLB_PROTOCOL_H__*/





