/**
 * @file     fft_config.h
 * @details  配置算法参数，根据不同项目配置,郑州地铁10号线项目
 * @author
 * @date
 * @version
**/

#ifndef __CONFIG_H__
#define __CONFIG_H__

/* include */
#define MCU_ARM //若在STM32上跑，找不到此定义时须自行定义
#ifdef MCU_ARM
#include "arm_math.h" //根据代码路径配置
#else
#include "math.h"
#define PI					3.14159265358979f
#endif
/* macro */
#define DIAG_NUM_MAX      20
#define DIAG_SPEED_MIN    15
#define FILTER_LEN_NUM    9
#define FILTER_LEN_DEN    9
/************************* IIR PARAM ****************************/
#define POLYGON_FILTER_NUM_1    0.0202110431311719
#define POLYGON_FILTER_NUM_2    -0.0618784014310059
#define POLYGON_FILTER_NUM_3    0.126361691958874
#define POLYGON_FILTER_NUM_4    -0.168293917341436
#define POLYGON_FILTER_NUM_5    0.191049889368274
#define POLYGON_FILTER_NUM_6    -0.168293917341436
#define POLYGON_FILTER_NUM_7    0.126361691958874
#define POLYGON_FILTER_NUM_8    -0.0618784014310059
#define POLYGON_FILTER_NUM_9    0.0202110431311719

#define POLYGON_FILTER_DEN_1    1
#define POLYGON_FILTER_DEN_2    -5.37851988464675
#define POLYGON_FILTER_DEN_3    13.9721707234815
#define POLYGON_FILTER_DEN_4    -22.3042602479832
#define POLYGON_FILTER_DEN_5    23.7373979438715
#define POLYGON_FILTER_DEN_6    -17.1538692718108
#define POLYGON_FILTER_DEN_7    8.20869031281336
#define POLYGON_FILTER_DEN_8    -2.37814731181059
#define POLYGON_FILTER_DEN_9    0.320944012772122

#define POLYGON_wheel_diameter          0.805		// 单位m	轮径，默认是0.85
#define POLYGON_Sample_rate             4096			//采样频率
#define POLYGON_threshold_prognosis     17
#define POLYGON_threshold_prewarning    22
#define POLYGON_threshold_warning       27
#define POLYGON_time_length             10			//堆栈空间的长度

/*********************** FFT INIT PARAM ****************************/
#define DIAG_NUM_ZXZC_WHEEL     5     //轴箱轴承诊断与踏面诊断总数据长度
#define DIAG_NUM_DJZC           8     //电机诊断数据长度
#define DIAG_NUM_GEAR           1     //齿轮诊断数据长度
#define DIAG_NUM_CLXZC          8    //齿轮箱轴承诊断数据长度

/*阈值*/
#define threshold_NGB_prognosis    44
#define threshold_W_prognosis      49
#define threshold_Wheel_prognosis  54
#define threshold_gear_prognosis   54

#define threshold_NGB_prewarning   50
#define threshold_W_prewarning     55
#define threshold_Wheel_prewarning 60
#define threshold_gear_prewarning  60

#define threshold_NGB_warning      56
#define threshold_W_warning        61
#define threshold_Wheel_warning    65
#define threshold_gear_warning     65

/*轴箱轴承*/
#define ZXZC_low_limit_1           1.24   //内环故障频率下限,由最大轮径计算得到
#define ZXZC_low_limit_2           0.9704 //外环故障频率下限,由最大轮径计算得到
#define ZXZC_low_limit_3           0.83971//滚动体故障频率下限,由最大轮径计算得到
#define ZXZC_low_limit_4           0.046211//保持架故障频率下限,由最大轮径计算得到
#define ZXZC_low_limit_5           0.1053//踏面故障频率下限,由最大轮径计算得到
#define ZXZC_high_limit_1          1.3528
#define ZXZC_high_limit_2          1.0587
#define ZXZC_high_limit_3          0.91605
#define ZXZC_high_limit_4          0.050412
#define ZXZC_high_limit_5          0.1148
#define ZXZC_base_1           0.1
#define ZXZC_base_2           0.1
#define ZXZC_base_3           0.1
#define ZXZC_base_4           0.1
#define ZXZC_base_5           0.1
#define ZXZC_bia_1          (10)
#define ZXZC_bia_2          (10)
#define ZXZC_bia_3          (10)
#define ZXZC_bia_4          (10)
#define ZXZC_bia_5          (10)

/*电机轴承*/
#define DJ_low_limit_1           4.3444
#define DJ_low_limit_2           3.0796
#define DJ_low_limit_3           3.8467
#define DJ_low_limit_4           0.27997
#define DJ_low_limit_5           6.9474
#define DJ_low_limit_6           5.201
#define DJ_low_limit_7           4.5978
#define DJ_low_limit_8           0.28894
#define DJ_high_limit_1          4.7393
#define DJ_high_limit_2          3.3596
#define DJ_high_limit_3          4.1964
#define DJ_high_limit_4          0.30542
#define DJ_high_limit_5          7.579
#define DJ_high_limit_6          5.6738
#define DJ_high_limit_7          5.0158
#define DJ_high_limit_8          0.31521
#define DJ_base_1           0.1
#define DJ_base_2           0.1
#define DJ_base_3           0.1
#define DJ_base_4           0.1
#define DJ_base_5           0.1
#define DJ_base_6           0.1
#define DJ_base_7           0.1
#define DJ_base_8           0.1
#define DJ_bia_1          (10)
#define DJ_bia_2          (10)
#define DJ_bia_3          (10)
#define DJ_bia_4          (10)
#define DJ_bia_5          (10)
#define DJ_bia_6          (10)
#define DJ_bia_7          (10)
#define DJ_bia_8          (10)

/*齿轮*/
#define gear_low_limit   9.8946
#define gear_high_limit  10.7941

#define gear_base  0.1
#define gear_bia   (10)

/*齿轮箱轴承*/
#define CLX_low_limit_1           6.9474
#define CLX_low_limit_2           5.201
#define CLX_low_limit_3           4.5978
#define CLX_low_limit_4           0.28894
#define CLX_low_limit_5           6.1652
#define CLX_low_limit_6           4.6333
#define CLX_low_limit_7           3.8187
#define CLX_low_limit_8           0.28958
#define CLX_high_limit_1          7.579
#define CLX_high_limit_2          5.6738
#define CLX_high_limit_3          5.0158
#define CLX_high_limit_4          0.31521
#define CLX_high_limit_5          6.7257
#define CLX_high_limit_6          5.0545
#define CLX_high_limit_7          4.1658
#define CLX_high_limit_8          0.31591
#define CLX_base_1           0.1
#define CLX_base_2           0.1
#define CLX_base_3           0.1
#define CLX_base_4           0.1
#define CLX_base_5           0.1
#define CLX_base_6           0.1
#define CLX_base_7           0.1
#define CLX_base_8           0.1
#define CLX_bia_1          (10)
#define CLX_bia_2          (10)
#define CLX_bia_3          (10)
#define CLX_bia_4          (10)
#define CLX_bia_5          (10)
#define CLX_bia_6          (10)
#define CLX_bia_7          (10)
#define CLX_bia_8          (10)

// 预判门限值
#define ZC_PRO_INNER        44 //内环
#define ZC_PRO_OUTER        49 //外环
#define ZC_PRO_ROLLING      44 //滚动体
#define ZC_PRO_CAGE         44 //保持架
#define TM_PRO_CAGE         54 //踏面
// 预警
#define ZC_PRE_INNER        50 //内环
#define ZC_PRE_OUTER        55 //外环
#define ZC_PRE_ROLLING      50 //滚动体
#define ZC_PRE_CAGE         50 //保持架
#define TM_PRE_CAGE         60 //踏面
// 报警
#define ZC_WARN_INNER       56 //内环
#define ZC_WARN_OUTER       61 //外环
#define ZC_WARN_ROLLING     56 //滚动体
#define ZC_WARN_CAGE        56 //保持架
#define TM_WARN_CAGE        65 //踏面


/* type declaration */
/*算法参数配置信息 780字节*/
typedef struct
{
    char project[8];            //存放项目名称的字符串
    double dbx_filter_num_1;        //多边形过滤器参数
    double dbx_filter_num_2;
    double dbx_filter_num_3;
    double dbx_filter_num_4;
    double dbx_filter_num_5;
    double dbx_filter_num_6;
    double dbx_filter_num_7;
    double dbx_filter_num_8;
    double dbx_filter_num_9;
    double dbx_filter_den_1;
    double dbx_filter_den_2;
    double dbx_filter_den_3;
    double dbx_filter_den_4;
    double dbx_filter_den_5;
    double dbx_filter_den_6;
    double dbx_filter_den_7;
    double dbx_filter_den_8;
    double dbx_filter_den_9;
    float def_wheel_d;             //默认轮径
    int sample_freq;               //采样频率
    uint8_t diag_num_dbx;          //多边形诊断堆栈空间长度
    uint8_t diag_num_zxzc_tm;      //轴箱轴承及踏面诊断长度
    uint8_t diag_num_djzc;         //电机轴承诊断长度
    uint8_t diag_num_clxzc;        //齿轮箱轴承诊断长度
    uint8_t diag_num_cl;           //齿轮诊断长度
    //阈值
    uint8_t thre_dbx_warning;      //多边形阈值预警
    uint8_t thre_dbx_alarm_1;      //多边形阈值一级报警（较低级别）
    uint8_t thre_dbx_alarm_2;      //多边形阈值二级报警（较高级别）
    uint8_t thre_zc_ngb_warning;      //轴承内、滚、保阈值预警
    uint8_t thre_zc_ngb_alarm_1;      //轴承内、滚、保阈值一级报警（较低级别）
    uint8_t thre_zc_ngb_alarm_2;      //轴承内、滚、保阈值二级报警（较高级别）
    uint8_t thre_zc_wh_warning;      //轴承外环阈值预警
    uint8_t thre_zc_wh_alarm_1;      //轴承外环阈值一级报警（较低级别）
    uint8_t thre_zc_wh_alarm_2;      //轴承外环阈值二级报警（较高级别）
    uint8_t thre_tm_warning;      //踏面阈值预警
    uint8_t thre_tm_alarm_1;      //踏面阈值一级报警（较低级别）
    uint8_t thre_tm_alarm_2;      //踏面阈值二级报警（较高级别）
    uint8_t thre_cl_warning;      //齿轮阈值预警
    uint8_t thre_cl_alarm_1;      //齿轮阈值一级报警（较低级别）
    uint8_t thre_cl_alarm_2;      //齿轮阈值二级报警（较高级别）
    uint8_t thre_temp_warning;     //温度阈值预警
    uint8_t thre_temp_alarm;      //温度阈值报警
    uint8_t reserve[2];
    //轴箱轴承故障频率下限,由最大轮径计算得到，轴箱轴承故障与车速比例高低限: 数组顺序从0开始依次对应内外滚保，最后一个对应踏面
    float zxzc_low_limit[DIAG_NUM_ZXZC_WHEEL];       //内环、外环、滚动体、保持架、踏面
    //轴箱轴承故障频率上限,由最大轮径计算得到，
    float zxzc_high_limit[DIAG_NUM_ZXZC_WHEEL];      //内环、外环、滚动体、保持架、踏面
    //轴箱轴承故障 基准值
    float zxzc_base[DIAG_NUM_ZXZC_WHEEL];
    //轴箱轴承故障 偏置
    float zxzc_bias[DIAG_NUM_ZXZC_WHEEL];
    //电机轴承故障与车速比例高低限: 0、１、２、３、对应内外滚保,为一组，根据配置，可能有n组
    float djzc_low_limit[DIAG_NUM_MAX];       //内环、外环、滚动体、保持架、内环、外环、滚动体、保持架
    float djzc_high_limit[DIAG_NUM_MAX];      //内环、外环、滚动体、保持架、内环、外环、滚动体、保持架
    //基准
    float djzc_base[DIAG_NUM_MAX];
    //偏置
    float djzc_bias[DIAG_NUM_MAX];
    //齿轮故障与车速比例高低限
    float cl_low_limit;
    float cl_high_limit;
    float cl_base;
    float cl_bias;
    //齿轮箱轴承故障与车速比例高低限: 0、１、２、３、对应内外滚保,为一组，根据配置，可能有n组
    float clxzc_low_limit[DIAG_NUM_MAX];   //内环、外环、滚动体、保持架、内环、外环、滚动体、保持架....
    float clxzc_high_limit[DIAG_NUM_MAX];       //内环
    float clxzc_base[DIAG_NUM_MAX];//基准
    float clxzc_bias[DIAG_NUM_MAX];//偏置
}diag_config_t;
typedef struct
{
    int status_bearing_comprehensive;
    int status_Wheel;
}ZXZC_status_out_t;//输出状态结构
struct FFT_DIAG_SAVE
{
    int DB[DIAG_NUM_MAX];                 //诊断DB值
    float amp[DIAG_NUM_MAX];                //诊断幅值
    float fre[DIAG_NUM_MAX];                //频段最大值所在频率
};
struct FFT_DIAG_PARA
{
    float wheel_diameter;               //轮径，单位：
    float diag_speed;                   //车速
    int Sample_rate;                                //采样频率
    int diag_num;                                       //诊断项点数
    float low_limit[DIAG_NUM_MAX];  //下限
    float high_limit[DIAG_NUM_MAX]; //上限
    float base[DIAG_NUM_MAX];         //基准
    float bia[DIAG_NUM_MAX];            //偏置
    float threshold_prognosis[DIAG_NUM_MAX];
    float threshold_prewarning[DIAG_NUM_MAX];
    float threshold_warning[DIAG_NUM_MAX];
};
typedef struct
{
    int block_3s_N[3];
    int block_3s_W[3];
    int block_3s_G[3];
    int block_3s_B[3];
    int block_3s_Wheel[3];
}zxzc_stack_t;
typedef struct
{
    int block_3s_N[6];
    int block_3s_W[6];
    int block_3s_G[6];
    int block_3s_B[6];
}dj_clx_stack_t;
/**
 * 多边形算法需要的配置
 */

struct POLYGON_DIAG_PARA
{
    float wheel_diameter;       // m    轮径，默认是0.805
    float Sample_rate;          //采样频率
    float polygon_diag_speed;   // km/h 车辆运行的速度
    float threshold_prognosis;
    float threshold_prewarning;
    float threshold_warning;
    int time_length;            //堆栈空间的长度
};
/**
 * 多边形算法计算完之后的特征数据
 */
struct POLYGON_DIAG_SAVE
{
    int order_result;         //特征数据1  阶次
    float rms_result;         //特征数据2  加速度均方根
    float DB_result;          //特征数据3  DB值
    int indicator;            //特征数据4  评价指标，用于计算波磨指标
    float nameda_result;      //特征数据5  钢轨波磨波长，用于计算波磨波长
    int status_polygon;       //诊断状态
    float speed;
};
typedef struct
{
    int N_stack[POLYGON_time_length];
    float K_stack[POLYGON_time_length];
    float speed_stack[POLYGON_time_length];
}polygon_diag_stack_t;
struct CORRUGATION_DIAG_SAVE
{
    float indicator1357_result;              	 //1357位车轮对应钢轨的波磨指标
    float indicator2468_result;                //2468位车轮对应钢轨的波磨指标
    float nameda1357_result;              	 //1357位车轮对应钢轨的波长指标
    float nameda2468_result;              	 //1357位车轮对应钢轨的波长指标
};

extern diag_config_t diag_config_param;

/*  function  */
/*
Function:    轴箱轴承+踏面诊断
Description: 输入的数据计算后，返回轴箱轴承和踏面的诊断结果
             int *block_3s_N,int *block_3s_W，int *block_3s_G，int *block_3s_B，int *block_3s_Wheel
                          以上内外滚保4个buffer用于诊断中间值存储，不做输入输出
Input：            float *acc_buff                     采集存储了1s加速度buffer（轴箱冲击数据）
             int acc_buff_size                   1s内数据个数
             const struct FFT_DIAG_PARA fft_para 诊断所需参数配置，包括轮径，速度，采样率，频率范围等
Output:      FFT_DIAG_SAVE *fft_diag_save        中输出对应DB值，包括：DB,amp,fre
Return:      返回诊断状态结构体。状态值：0:预警， 1:1级报警， 2:2级报警
Others:
*/
ZXZC_status_out_t* ZXZC_Wheel_diag(float *acc_buff, const struct FFT_DIAG_PARA fft_para, struct FFT_DIAG_SAVE *fft_diag_save, zxzc_stack_t* stack);
/*
Function:    齿轮箱轴承诊断
Description: 输入的数据计算后，返回轴箱轴承和踏面的诊断结果
             int *block_3s_N,int *block_3s_W，int *block_3s_G，int *block_3s_B，int *block_3s_Wheel
                          以上内外滚保4个buffer用于诊断中间值存储，不做输入输出
Input：            float *acc_buff                     采集存储了1s加速度buffer（轴箱冲击数据）
             int acc_buff_size                   1s内数据个数
             const struct FFT_DIAG_PARA fft_para 诊断所需参数配置，包括轮径，速度，采样率，频率范围等
Output:      FFT_DIAG_SAVE *fft_diag_save        中输出对应DB值，包括：DB,amp,fre
Return:      返回诊断状态，状态值：0:预警， 1:1级报警， 2:2级报警
Others:
*/
int CLX_diag(float *acc_buff, const struct FFT_DIAG_PARA fft_para, struct FFT_DIAG_SAVE *fft_diag_save, dj_clx_stack_t* stack);
/*
Function:    电机轴承诊断
Description: 输入的数据计算后，返回轴箱轴承和踏面的诊断结果
             int *block_3s_N,int *block_3s_W，int *block_3s_G，int *block_3s_B，int *block_3s_Wheel
                                                   以上内外滚保4个buffer用于诊断中间值存储，不做输入输出
Input：            float *acc_buff                     采集存储了1s加速度buffer（轴箱冲击数据）
             int acc_buff_size                   1s内数据个数
             const struct FFT_DIAG_PARA fft_para 诊断所需参数配置，包括轮径，速度，采样率，频率范围等
Output:      FFT_DIAG_SAVE *fft_diag_save        中输出对应DB值，包括：DB,amp,fre
Return:      返回诊断状态，状态值：0:预警， 1:1级报警， 2:2级报警
Others:
*/
int DJ_diag(float *acc_buff, const struct FFT_DIAG_PARA fft_para, struct FFT_DIAG_SAVE *fft_diag_save, dj_clx_stack_t* stack);
/*
Function:    多边形诊断
Description: 输入一秒钟的加速度数据的buff，计算多边形之后输出结果
Input：            float *acc_buff                     采集存储了1s加速度buffer
             int acc_buff_size                   1s内数据个数
             struct POLYGON_DIAG_PARA polygon_para 诊断所需参数配置，输入参数结构体，包括轮径、采样频率、运行速度、预判、预警、报警阈值
             polygon_diag_stack_t* stack 用于分别存放8个通道最近10s指标值的堆栈空间，长度为:sizeof(polygon_diag_stack_t)*8
Output:      struct POLYGON_DIAG_SAVE *polygon_diag_save 诊断结果存储空间
Return:      无
Others:
*/
void polygon_diag(float *acc_buff, struct POLYGON_DIAG_PARA polygon_para, struct POLYGON_DIAG_SAVE *polygon_diag_save,polygon_diag_stack_t* stack);
/*
Function:    波磨诊断
Description: 将8个轴箱多边形计算出的结果进行波磨的计算，并输出两边钢轨的计算结果
Input：            float *indicator  存放8个轴箱多边形计算的评价指标的buffer(按照轴箱编号顺序存储)
             float *nameda     存放8个轴箱多边形计算后的钢轨波磨波长的buffer(按照轴箱编号顺序存储)
Output:      struct CORRUGATION_DIAG_SAVE *corrugation_diag_save 诊断结果存储buffer
Return:      无
Others:
*/
void corrugation_diag(float *indicator, float *nameda, struct CORRUGATION_DIAG_SAVE *corrugation_diag_save);
/*
Function:    齿轮诊断
Description: 输入的数据计算后，返回齿轮箱的齿轮诊断结果
Input：            float *acc_buff  采集存储了1s加速度buffer
             float *nameda     存放8个轴箱多边形计算后的钢轨波磨波长的buffer(按照轴箱编号顺序存储)
Output:      struct CORRUGATION_DIAG_SAVE *corrugation_diag_save 诊断结果存储buffer
Return:      无
Others:
*/
int gear_diag(float *acc_buff, const struct FFT_DIAG_PARA fft_para, struct FFT_DIAG_SAVE *fft_diag_save, int* block_3s_gear);

/*
Function:    为本次轴箱轴承的算法诊断配置参数，注意：速度、轮径、及采样率需要在调用该函数之后设置为当前值
Description: 配置算法配置文件中对应的相关参数
Output:      struct FFT_DIAG_PARA* fft_diag_paras 输出当前轴箱轴承计算所需参数，注意：速度、轮径、采样率需令外配置
Return:      无
Others:
*/
void fft_diag_ZXZC_Wheel_init(struct FFT_DIAG_PARA* fft_diag_paras);
/*
Function:    为本次电机轴承的算法诊断配置参数，注意：速度、轮径、及采样率需要在调用该函数之后设置为当前值
Description: 配置算法配置文件中对应的相关参数
Output:      struct FFT_DIAG_PARA* fft_diag_paras 输出当前轴箱轴承计算所需参数，注意：速度、轮径、采样率需令外配置
Return:      无
Others:
*/
void fft_diag_DJ_init(struct FFT_DIAG_PARA* fft_diag_paras);
/*
Function:    为本次齿轮箱轴承的算法诊断配置参数，注意：速度、轮径、及采样率需要在调用该函数之后设置为当前值
Description: 配置算法配置文件中对应的相关参数
Output:      struct FFT_DIAG_PARA* fft_diag_paras 输出当前轴箱轴承计算所需参数，注意：速度、轮径、采样率需令外配置
Return:      无
Others:
*/
void fft_diag_CLX_init(struct FFT_DIAG_PARA* fft_diag_paras);
/*
Function:    为本次齿轮诊断配置参数，注意：速度、轮径、及采样率需要在调用该函数之后设置为当前值
Description: 配置算法配置文件中对应的相关参数
Output:      struct FFT_DIAG_PARA* fft_diag_paras 输出当前轴箱轴承计算所需参数，注意：速度、轮径、采样率需令外配置
Return:      无
Others:
*/
void fft_diag_gear_init(struct FFT_DIAG_PARA* fft_diag_paras);

//void polygon_diag_init(struct FFT_DIAG_PARA* fft_diag_paras, float speed, float Wheel_diameter, int fs);

void diag_algorithm_init(void);
void diag_param_set(diag_config_t* init_diag_paras);
void fft_diag_init(void);
//520Hz低通滤波器
float IIR_polygon(float *x, float *y);

/*
Function:         单个采集加速度滤波处理（仅多边形适用）
Description:
Input：            float data 采集数据计算后的加速度
                   float* x 用于存放10s数据的当前通道的堆栈buffer
				   float* y 用于存放10s数据的当前通道的堆栈buffer
Output:           None
Return:           返回滤波后的结果。
Others:
*/
float IIR_filter_polygon(float data, float* x, float* y);
void new_fft(float*buff,int size);
#endif /*__CONFIG_H__*/

