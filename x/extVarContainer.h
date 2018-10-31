//存放了单片机上一些不能转移的外部变量 
//例如extern int IntsToSend[10];上位机发送数组
//例如extern float speed;访问虚拟的车速

//请在单片机和电脑上编写不同的"environment.h"和"extVarContainer.h"
//使得文件imageprocess中的代码，可以跨越单片机和电脑，复制粘贴后即可运行在单片机上

#define CAMERA_ROWS 60
#define CAMERA_COLS 80

#define CAMERA_W    CAMERA_COLS
#define CAMERA_H	CAMERA_ROWS

/////////////////以下很乱
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include <memory.h>
#define int32 int32_t
#define uint8 uint8_t
#define uint16 uint16_t
#define int16 int16_t
#define THRESHOLD 255
#define right_barrier   1
#define left_barrier   2



typedef struct
{
	short x;
	short y;
}PosType;
typedef struct PID { float P, pout, I, iout, D, dout, OUT; }PID;


typedef enum {
	Normal,
	Straight_Perfect,
	Straight_Defect,
	Straight_Line_Very_Long,
	Straight_Line_Very_Long_But_Not_Now,
	Ring_step_zero,   //还没进环，甚至没左转
	Ring_step_one,		//正在左转中
	Ring_step_two,		//正在右转中
	Ring_step_three,	//正在左转中
	Ring_just_out,
	Obstacle, //10
	Cross,//11
	Hill//12
}road_status;

typedef enum {
	ba_phase_one,//近行无障碍
	ba_phase_two,//近行有障碍
	ba_phase_three,//已出
	ba_phase_four//已出

}Barrier_status;






int wide[60] = {
13,13,14,15,16,
16,17,17,18,19,
20,21,21,22,23,

23,24,24,25,26,
28,28,29,30,29,
28,29,30,30,30,

31,31,32,33,33,
34,35,36,37,37,
39,39,40,41,42,

43,43,45,46,46,
48,48,49,51,51,
53,53,55,55,56 };    //环形用的补线



int left[60] = { 0 };
int right[60] = { 0 };
int middle[60] = { 0 };
float WidthRatio[60] = {
5.818182,5.454545,6.666667,5.454545,5.454545,
4.615385,4.615385,4.000000,4.000000,3.750000,
3.529412,3.333333,3.157895,3.000000,2.857143,
2.727273,2.608696,2.500000,2.500000,2.307692,
2.307692,2.142857,2.142857,2.000000,2.000000,
1.935484,1.875000,1.818182,1.764706,1.714286,
1.714286,1.621622,1.621622,1.538462,1.538462,
1.500000,1.463415,1.428571,1.428571,1.395349,
1.363636,1.333333,1.304348,1.304348,1.250000,
1.250000,1.200000,1.200000,1.200000,1.153846,
1.153846,1.111111,1.111111,1.071429,1.071429,
1.071429,1.034483,1.034483,1.016949,1.000000 };
uint8 road_len_min[60] = {
	9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
	9, 9, 9, 9, 9, 9, 9, 9, 10, 11, 12, 13,
	14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
	24, 25, 26, 27, 28, 28, 29, 29, 29, 29, 29,
	30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30
};
uint8 road_len_max[60] = {
	24, 24, 24, 24, 24, 24, 25, 26, 27, 28,
	29, 29, 31, 32, 33, 34, 34, 36, 36, 37,
	38, 39, 40, 41, 41, 43, 44, 44, 46, 46,
	47, 48, 49, 49, 51, 51, 52, 53, 53, 54,
	55, 56, 56, 57, 58, 59, 59, 61, 61, 62,
	63, 64, 64, 66, 66, 67, 68, 69, 69, 69
};
int Cross_flag = 0;
int Cross_flag2 = 0;
int Cross_FIND = 0;
int num_l = 0, num_r = 0;
int Middle_Err;
int MIDDLE = 39;
int MIDDLE_plus_1 = 0;
int MIDDLE_plus_2 = 0;
int MIDDLE_plus_3 = 0;
int MIDDLE_plus_4 = 0;
int Control_Line[4] = { 0 };
int Control_Line_WE[4] = { 1,1,1,1 };

PosType LSM_l[10], LSM_r[10], z_chao[10];
float k, d;
int STOP = 0, Stop_cnt = 0;//停车线

int RoadType = 0;//1     2     3      4     5     6     7        8
			   //环路 十字 直道 坡道 左弯  右弯 急弯  左障碍 右障碍
int Ring_FIND = 0;//发现标志量
int Ring_FIND_AGAIN = 0;
uint8 Ring_Exit_Deriction[30] = { 1 };//加入对环形转弯方向的人为设置变量 0为向右 1为向左
int Ring_Exit = 0;//出口标志量
int Ring_Exit_line = 0;
int Ring_cnt = 0;//对环形进行计数 从0开始 最多到7 一个八个
int Ring_ExitConfirm = 0;
int Ring_ExtranceConfirm = 0;
int Ring_Strong;
int Ring_Extrance_Strong = 0;
int Ring_right = 0;
int Ring_left = 0;
int RingNum = 1;
int Ring_RUN = 0;
int RingDelayCnt = 0;
int ObatacleDelayCnt = 0;

extern float Distance;
float Hill_Distance[4] = { 1000,1000,1000,1000 };

extern int GO;
extern float 
;
extern float RingTime;
extern PID PID_SPEED;
extern PID PID_ANGLE;
extern int page_BOMA3;
extern int BOMA;
extern float Turn_Speed;
extern int Control_Line[4];
int RingCurnorLine[2] = { 0 };
float TurnSpeedPLUS = 0;
PID TurnPID_AGNLE_Change;
PID TurnPID_TURN_Change;
PID TurnPID_SPEED_Change;
int Control_Line_PLUS[4] = { 0 };//环路时给控制行加一些补偿

int Obstacle_FIND = 0;
int Obstacle_LEFT = 0;
int Obstacle_RIGHT = 0;
int Obstacle_Nearest_Width = 0;//用于检测出障碍的 离车比较近的一行的宽度
int Obstacle_Cnt = 0;
int ObstacleNextDelayCnt = 200;
int Obstacle_Line_PLUS[4];
int WidthForObstacleControlLine[4];
float OVER_Gain = 0;

float BendFixRatio = 1000;

int Hill_FIND = 0;
float Hill_FIND_Distance = 0;
int Hill_Cnt = 0;
int HillDelayCnt = 200;
extern int Acc_Y_show;


float ObstacleDistance = 1;
float RingSlowRadio = 0.8;
float InRingDistance[2] = { 0,0 };
int middle_control_last[3] = { 39,39,39 };

float StraightSlowRadio;


#define record_num   50 

float barrier_start_distance = 0;

road_status history_status[record_num] = { Normal };
int Road_Error_Recording[10] = { 0 };
road_status current_status = Normal;
int current_ring_timing = 0, current_ring_going_out_timing = 0;
int A_special_restriction_for_ring_out = 0;

int barrier_start_cnt = 0;

int Barrier_existance_and_polarity = 0;
Barrier_status barrier_status;
uint8 hill_to_be_confirm = 0;
int debug1;
int debug2;
uint8 Is_right_line_straight_evo();
uint8 Is_left_line_straight_evo();
uint8 check_barrier_status_evo();
uint8 Super_Ring_Check(const float k_local_left, const float d_local_left, const float k_local_right, const float d_local_right);
uint8 Stepping_over_zebra();
void barrier_status_transform();
uint8 Additional_Check_Ring (int left_change_row, int right_change_row, int *p_Ring_Confirm_third);
uint8 Is_line_all_white_or_black(uint8 line);
void clean_shadow();
void smooth_middle();
uint8 addtional_ring_status = 0;
int barrier_exit = 4; //在flash中被更改
uint8 head_clear_flag = 0;
uint8 Stepping_over_zebra_flag = 0, Terminal_zebra_flag = 0;
float zebra_find_distance = 0;
void check_cross_result();
float lamada_outer_ring = 1, lamada_inner_ring = 1;
uint8 hill_override_barrier = 0;
int32 Hill_FIND_distance_memory = 0;
extern int IntsToSend;





////////////////////////////////////////////////////

////////////////////////////////////////////////////

////////////////////////////////////////////////////
uint8 check_long_straight_line();
uint8 decide_barrier_polarity();
uint8 barrier_detaction_left();
uint8 barrier_detaction_right();
uint8 check_barrier_status();
uint8 Is_right_line_straight();
uint8 Is_left_line_straight();


extern int MIDDLE;
//寻迹主函数
void Path(void);
void Edge_Detection(void);//边线检测
void wave_filter(void);//简单的滤波
void middle_processing(void);
void Ring_right_middle_processing(void);
void Ring_Exit_Detection(void);
void Ring_Exit_Confirm(void);
void Ring_Extrance_Confirm(void);
void Middle_Err_Calculate(void);
float  Middle_Err_Filter(float middle_err);
void Edge_Detection_Further(void);//最下面几行进一步检测
void Cross_processing(void);//十字检测
void map(void);//逆透视变换
void Stop_Detection(void);//停止线检测
void Ring_Detection(void);//环形路口检测
void Obstacle_Detection(void);//障碍检测
void Straight_Slope_Bend_Detection(void);
void left_shizi1(short white, short cross_leftdown);
void left_shizi2(short cross_leftdown);
void left_shizi3(short white);
void right_shizi1(short white, short cross_rightdown);
void right_shizi2(short cross_rightdown);
void right_shizi3(short white);

int Straight_Detection(void);


int Ring_Turn_Detection(void);

void draw(void);//路径标记
void LeastSquareMethod(PosType *data, uint8_t size);
void fangxiang(void);


void Good_Game(void);
int Single_Straight_LEFT(void);
int Single_Straight_RIGHT(void);
int Single_Straight_MIDDLE(void);
void ObstacleRIGHT_middleProcess(void);
void ObstacleLEFT_middleProcess(void);

void Ring_SingleLine_Left(void);
void Ring_SingleLine_Right(void);
void Ring_Process(void);
void Obstacle_Process(void);
void MiddleLine_GridOn(void);
uint8 Hill_Detection(void);
int Single_Straight_LEFT_For_Hill(void);
int Single_Straight_RIGHT_For_Hill(void);
uint8 Is_A_Hill();
uint8 check_hill_character();
int Single_Straight_LEFT_PBB(int BeginLine, int EndLine);
int Single_Straight_RIGHT_PBB(int BeginLine, int EndLine);
uint8 StraightRoad_Detection();
void cross_process(void);
uint8 Cross_Detection(void);
void MiddleLine_On(void);
int division45(float kfloat);
extern int Middle_Err;
uint8 check_long_straight_line();

extern uint8 barrier_action_delayer;


////////////////////////////////////////////
////////////////////////////////////////////
int
bt_open_SD = 0,
bt_data_only = 0,
bt_barier = 0,
shut_protection = 0,
bt_to_mid = 0,
empty5 = 0,
empty6 = 0,
empty7 = 0,
empty8 = 0,
empty9 = 0;



int Acc_Y, Acc_X,Acc_Z;

////////////////////////////////////////////
////////////////////////////////////////////


#define BELL_ON {}
#define BELL_OFF {}

#define LED_RED 0
#define LED_GREEN 1
#define LED_BLUE 2

#define gpio_set(a,b) {}
#include <math.h>

////////////////////////////////////////////
////////////////////////////////////////////



#define RowMax 58
#define RowMin 5

////////////////////////////////////////////
////////////////////////////////////////////



float
new_pid_p_speed = 0,
new_pid_i_speed = 0,
new_pid_d_speed = 0,
bt_speed_target = 0,
bt_speedmaxpush = 0,
speedmaxreduce = 0,
enter_ring = 0,
inside_ring = 0,
out_ring = 0,
chasubi = 0;
////////////////////////////////////////////
////////////////////////////////////////////
float Distance = 0;
float RunTime = 0;
float Turn_Speed = 0;
int Acc_Y_show = 0;
int IntsToSend = 0;