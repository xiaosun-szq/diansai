//
// Created by Qw011 on 2026/7/14.
//
//这是我的双电机FOC控制库
#include "foc_lib.h"
#include "hal_data.h"
#include "imu_i2c_driver.h"
#include <math.h>          // 用于 fabs()

// ==================== PID 控制器实现 ====================
//初始化函数，就是用来实例化PID参数的
void PID_Init(PIDController_t *pid, float Kp, float Ki, float Kd,
              float integral_limit, float output_limit)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral_limit = integral_limit;//限幅用的
    pid->output_limit = output_limit;    //他也是

    PID_Reset(pid);
}

void PID_Reset(PIDController_t *pid)
{
    pid->integral = 0;
    pid->error_prev = 0;
    pid->D_prev = 0;
    pid->timestamp_prev = 0;
}
//******************************************
//PID
float PID_Update(PIDController_t *pid, float error, unsigned long timestamp)
{
    // 1. 计算时间差（防止除零）
    float dt = (timestamp - pid->timestamp_prev) * 1e-6f;
    if (dt <= 0 || dt > 0.1f) dt = 1e-3f;

    // 2. 比例项
    float P = pid->Kp * error;

    // 3. 积分项（带限幅）
    pid->integral += pid->Ki * error * dt;
    pid->integral = _constrain(pid->integral, -pid->integral_limit, pid->integral_limit);

    // 4. 微分项计算（原始）
    float derivative = (error - pid->error_prev) / dt;

    // 5. 单独对微分项做低通滤波（alpha 固定为 0.2，可调）
    const float lpf_alpha = 0.2f;
    float D_filtered = lpf_alpha * (pid->Kd * derivative) + (1 - lpf_alpha) * pid->D_prev;

    // 6. 总输出
    float output = P + pid->integral + D_filtered;
    output = _constrain(output, -pid->output_limit, pid->output_limit);

    // 7. 更新状态
    pid->error_prev = error;
    pid->D_prev = D_filtered;
    pid->timestamp_prev = timestamp;

    return output;
}
//******************************************
//增量式 PID 这个可以先不用了解，我怕电机震动加他的，后来测试了下应该不用担心
float PID_Update_Incremental(PIDController_t *pid, float error, unsigned long timestamp)
{
    float dt = (timestamp - pid->timestamp_prev) * 1e-6f;
    if (dt <= 0 || dt > 0.1f) dt = 1e-3f;

    // 原始微分项（不带滤波）
    float derivative = (error - pid->error_prev) / dt;

    // 对微分项做低通滤波
    const float lpf_alpha = 0.2f;
    float D_filtered = lpf_alpha * (pid->Kd * derivative) + (1 - lpf_alpha) * pid->D_prev;

    // 增量式 PID 核心
    float delta_error = error - pid->error_prev;
    float delta_output = pid->Kp * delta_error +
                         pid->Ki * error * dt +
                         D_filtered;

    // 积分累计
    pid->integral += delta_output;
    pid->integral = _constrain(pid->integral, -pid->integral_limit, pid->integral_limit);

    // 更新状态
    pid->error_prev = error;
    pid->D_prev = D_filtered;
    pid->timestamp_prev = timestamp;

    return pid->integral;
}
//******************************************
float getSensorAngle(uint8_t a)
{
    uint8_t data1[2] = {0};
		uint8_t data2[2] = {0};

		uint16_t angle1;
    uint16_t angle2;

	switch(a){
//*************************************************************************
		case (1):{
    bool ret1, ret2;
    ret1 = i2c_read_bytes1(0x0E, &data1[0], 1);
    ret2 = i2c_read_bytes1(0x0F, &data1[1], 1);
    angle1 = ((data1[0] << 8) | data1[1]) & 0x0FFF;
		return (float)angle1;
	}
//*************************************************************************
    case (2):	{
    bool ret1, ret2;
    ret1 = i2c_read_bytes2(0x0E, &data2[0], 1);
    ret2 = i2c_read_bytes2(0x0F, &data2[1], 1);
    angle2 = ((data2[0] << 8) | data2[1]) & 0x0FFF;
		return (float)angle2;
	  }
		default:
    return 0;
	}
}
//*************************************************************************
//获取无圈数角度（±π范围）
float getAngle_Without_track(uint8_t a)
{
    float angle_rad = getSensorAngle(a) * 2.0f * PI / 4096.0f;
    if (angle_rad > PI) {
        angle_rad -= 2.0f * PI;
    }
    return -angle_rad;
}
//*************************************************************************
// 获取累计角度（含圈数）
static float p[4] = {0,0,0,0};

float getAngle(uint8_t a)
{
    float val = getAngle_Without_track(a);
    float d_angle = val - p[a-1];

    if (fabsf(d_angle) > (144.0f * PI / 180.0f)) {
        p[a+1] += (d_angle < 0) ? 1 : -1;
    }

    p[a-1] = val;
    return (float)p[a+1] * 2.0f * PI + p[a-1];
}
//*************************************************************************
// 计算角速度（微秒时间戳）
static float q[6] = {0,0,0,0,0,0};

float Sensor_AS5600_getVelocity(uint8_t a,unsigned long timestamp)
{
    float current_angle = getAngle(a);

    float Ts = (timestamp - q[a+3]) * 1e-6f;
    if (Ts <= 0) Ts = 1e-3f;

    float vel = ((float)(p[a+1] - q[a+1]) * 2.0f * PI +
                 (current_angle - q[a-1])) / Ts;

    q[a-1] = current_angle;
    q[a+1] = p[a+1];
    q[a+3] = timestamp;

		return vel;
}
//*************************************************************************
// ==================== 定时器函数 ====================底层配置函数
//(要知道怎么用它，micros();输出微秒计数，他在FOC控制中很重要，比如速度计算，低通滤波等。
//millis();输出毫秒计数，还记得在imu_i2c_driver中提到的g_system_ms吗，这里原理也是一样的，
//我把他放在了os.c的Task_1ms()自增计数，但在这里他比较冗余，没用到)
volatile uint32_t ms_counter = 0;
volatile uint32_t us_counter = 0;

void DWT_Init(void)
{

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;   // 使能 DWT
    DWT->CYCCNT = 0;                                  // 清零计数器
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;              // 使能 CYCCNT
}

// 微秒计数器
uint32_t micros(void)
{
	  us_counter = DWT->CYCCNT / 200;
    return us_counter;  // 200MHz → 200周期/微秒
}

uint32_t millis(void)
{
    return ms_counter;
}
// ==================== PWM输出函数 ====================
//这里调用的是瑞萨的PWM输出接口，换芯片直接换成对应PWM输出接口即可，其他地方再改改微秒时间戳函数即可，低耦合
void hal_pwm_init(void)
{
    R_GPT_Open(&g_timer7_ctrl, &g_timer7_cfg);
    R_GPT_Open(&g_timer8_ctrl, &g_timer8_cfg);
    R_GPT_Open(&g_timer9_ctrl, &g_timer9_cfg);
    R_GPT_Open(&g_timer11_ctrl, &g_timer11_cfg);
    R_GPT_Open(&g_timer13_ctrl, &g_timer13_cfg);

    R_GPT_Start(&g_timer7_ctrl);
    R_GPT_Start(&g_timer8_ctrl);
    R_GPT_Start(&g_timer9_ctrl);
    R_GPT_Start(&g_timer11_ctrl);
    R_GPT_Start(&g_timer13_ctrl);
}

void setPwm_motor1(float U1a, float U1b, float U1c)
{
    timer_info_t info;

    R_GPT_InfoGet(&g_timer7_ctrl, &info);
    R_GPT_DutyCycleSet(&g_timer7_ctrl, (uint32_t)(U1a * info.period_counts / VOLTAGE_POWER_SUPPLY), GPT_IO_PIN_GTIOCA);

    R_GPT_InfoGet(&g_timer8_ctrl, &info);
    R_GPT_DutyCycleSet(&g_timer8_ctrl, (uint32_t)(U1b * info.period_counts / VOLTAGE_POWER_SUPPLY), GPT_IO_PIN_GTIOCB);

    R_GPT_InfoGet(&g_timer9_ctrl, &info);
    R_GPT_DutyCycleSet(&g_timer9_ctrl, (uint32_t)(U1c * info.period_counts / VOLTAGE_POWER_SUPPLY), GPT_IO_PIN_GTIOCB);
}
void setPwm_motor2(float U2a, float U2b, float U2c)
{
    timer_info_t info;

    R_GPT_InfoGet(&g_timer11_ctrl, &info);
    R_GPT_DutyCycleSet(&g_timer11_ctrl, (uint32_t)(U2a * info.period_counts / VOLTAGE_POWER_SUPPLY), GPT_IO_PIN_GTIOCA);

    R_GPT_InfoGet(&g_timer11_ctrl, &info);
    R_GPT_DutyCycleSet(&g_timer1_ctrl, (uint32_t)(U2b * info.period_counts / VOLTAGE_POWER_SUPPLY), GPT_IO_PIN_GTIOCB);

    R_GPT_InfoGet(&g_timer13_ctrl, &info);
    R_GPT_DutyCycleSet(&g_timer2_ctrl, (uint32_t)(U2c * info.period_counts / VOLTAGE_POWER_SUPPLY), GPT_IO_PIN_GTIOCA);
}
//状态机设置，输出对应电机三相PWM
void setPwm(uint8_t motor_id, float Ua, float Ub, float Uc)
{
   switch(motor_id) {
        case 1: setPwm_motor1(Ua, Ub, Uc); break;
        case 2: setPwm_motor2(Ua, Ub, Uc); break;
        default: break;
    }
}

// ==================== FOC数学函数 ====================底层配置函数
//限位在0到2Π间
float _normalizeAngle(float angle)
{
    float a = fmodf(angle, 2.0f * PI);
    return a >= 0 ? a : (a + 2.0f * PI);
}
//电角度计算，MOTOR_POLE_PAIRS是极对数，在头文件可改，本次用的电机是7极对，MOTOR_POLE_PAIRS是7
float _electricalAngle(float mechanical_angle)
{
    return mechanical_angle * MOTOR_POLE_PAIRS;
}
//uint8_t a是选电机
void setPhaseVoltage(uint8_t a, float Uq, float Ud, float angle_el)
{
    float Ualpha, Ubeta;
    float voltage_mid = VOLTAGE_POWER_SUPPLY / 2.0f;
    float Ua, Ub, Uc;
//帕克逆变换
    Ualpha = Ud * cosf(angle_el+PY) - Uq * sinf(angle_el+PY);
    Ubeta  = Ud * sinf(angle_el+PY) + Uq * cosf(angle_el+PY);
//克拉克逆变换，这里加voltage_mid是为了偏置，防止出现负电压，因为电池提供不了负电压
    Ua = Ualpha + voltage_mid;
    Ub = (sqrtf(3.0f) * Ubeta - Ualpha) / 2.0f + voltage_mid;
    Uc = (-Ualpha - sqrtf(3.0f) * Ubeta) / 2.0f + voltage_mid;

	  setPwm(a, Ua, Ub, Uc);
}
//用于电流
float ThreePhase2Iq(float Ia, float Ib, float Ic, float theta_e)
{
    float I_alpha = (2.0f * Ia - Ib - Ic) / 3.0f;
    float I_beta  = (Ib - Ic) * sqrtf(3.0f) / 3.0f;

    float cos_theta = cosf(theta_e);
    float sin_theta = sinf(theta_e);

    return -I_alpha * sin_theta + I_beta * cos_theta;
}

// ==================== 滤波器函数 ====================
//低通
float LowPassFilter_operator(float x, unsigned long timestamp, float lpf_y_prev , unsigned long lpf_timestamp_prev)
{

    float dt = (timestamp - lpf_timestamp_prev) * 1e-6f;
    if (dt < 0) dt = 1e-3f;
    else if (dt > 0.3f) {
        lpf_y_prev = x;
        lpf_timestamp_prev = timestamp;
        return x;
    }
    float RC = 0.3f;
    float alpha = dt / (RC + dt);
    float y = alpha * lpf_y_prev + (1.0f - alpha) * x;
    lpf_y_prev = y;
    lpf_timestamp_prev = timestamp;
    return y;
}
// ==================== 开环速度控制 ====================(这是个测试函数，测式代码是否能运行，不用管)
static float shaft_angle = 0;
static unsigned long open_loop_timestamp = 0;

float velocityOpenloop(uint8_t a, float target_velocity)
{
    unsigned long now_us = micros();

    // 计算时间差（秒）
    float Ts = (now_us - open_loop_timestamp) * 1e-6f;
    if (Ts <= 0 || Ts > 0.5f) Ts = 1e-3f;

    // 开环角度累加：角度 += 速度 × 时间
    shaft_angle = _normalizeAngle(shaft_angle + target_velocity * Ts);

    // 输出固定电压（可调）
    float Uq = 24.0f;  // 可根据需要改成参数

    // 电角度 = 机械角 × 极对数（电机极对数在 foc_config.h 里的MOTOR_POLE_PAIRS）
    float angle_el = shaft_angle * MOTOR_POLE_PAIRS;

    // 输出电压矢量（Ud = 0）
    setPhaseVoltage(a, Uq, 0, angle_el);

    open_loop_timestamp = now_us;
    return Uq;
}

// ==================== 控制环函数 ====================(第一个参数uint8_t a，代表你要选的电机，1是电机1，2是电机2
//                                                      第二个参数float 什么什么什么，代表目标值
//                                                      第三个参数float 什么什么什么，代表当前值
//                                                      第四个参数unsigned long timestamp，要传入时间戳，也就是上面说的micros();
//                                                      第五个参数PIDController_t *pid，要传入实例化PID参数地址，看完motor_app_init，在去头文件看看就明白了)
//速度环
void velocityControlLoop(uint8_t a ,float target_velocity, float current_velocity,
	                       float angle_el, unsigned long timestamp,
	                      	PIDController_t *pid)
{
    float error = target_velocity - current_velocity;
    float Uq = PID_Update(pid, error, timestamp);
    Uq = _constrain(Uq, -VOLTAGE_LIMIT, VOLTAGE_LIMIT);

    setPhaseVoltage(a,-Uq, 0, angle_el);
}
//位置环
void positionControlLoop(uint8_t a, float target_position, float current_position,
                         float angle_el, unsigned long timestamp,
                         PIDController_t *pid)
{
    float error = target_position - current_position;

    float Uq = PID_Update(pid, error, timestamp);
    Uq = _constrain(Uq, -VOLTAGE_LIMIT, VOLTAGE_LIMIT);
    setPhaseVoltage(a, Uq, 0, -angle_el);
}
//电流环
void currentControlLoop(uint8_t a, float target_iq, float current_iq,
                        float angle_el, unsigned long timestamp,
                        PIDController_t *pid)
{
    float error = target_iq - current_iq;
    float Uq = PID_Update(pid, error, timestamp);
    Uq = _constrain(Uq, -VOLTAGE_LIMIT, VOLTAGE_LIMIT);

    setPhaseVoltage(a,target_iq-Uq, 0, angle_el);
}

// ==================== 应用层函数 ====================
 PIDController_t speed_pid[3];
 PIDController_t position_pid[3];
 PIDController_t current_pid[3];
//这是实例化双电机速度，位置，电流环PID参数，具体参数可以在头文件宏定义中改，也是传入PID_Update的第一个参数和
//positionControlLoop或velocityControlLoop或currentControlLoop的最后一个参数
void motor_app_init(void)
{
    hal_pwm_init();
    DWT_Init();
//看到PID_Init没，他就是我们讲的第一个函数，初始化函数，所以，你也可以吧motor_app_init函数看成把各种初始化函数打包，一起放在hal_entry.c里
//这样就不会显得特别杂乱，你也可以去hal_entry.c的hal_entry();里看看是不是有个motor_app_init();
    PID_Init(&speed_pid[1], SPEED1_PID_KP, SPEED1_PID_KI, SPEED1_PID_KD,
             SPEED1_INTEGRAL_LIMIT, SPEED1_OUTPUT_LIMIT);

    PID_Init(&position_pid[1], POSITION1_PID_KP, POSITION1_PID_KI, POSITION1_PID_KD,
             POSITION1_INTEGRAL_LIMIT, POSITION1_OUTPUT_LIMIT);

    PID_Init(&current_pid[1], CURRENT1_PID_KP, CURRENT1_PID_KI, CURRENT1_PID_KD,
             CURRENT1_INTEGRAL_LIMIT, CURRENT1_OUTPUT_LIMIT);

	  PID_Init(&speed_pid[2], SPEED2_PID_KP, SPEED2_PID_KI, SPEED2_PID_KD,
             SPEED2_INTEGRAL_LIMIT, SPEED2_OUTPUT_LIMIT);

    PID_Init(&position_pid[2], POSITION2_PID_KP, POSITION2_PID_KI, POSITION2_PID_KD,
             POSITION2_INTEGRAL_LIMIT, POSITION2_OUTPUT_LIMIT);

    PID_Init(&current_pid[2], CURRENT2_PID_KP, CURRENT2_PID_KI, CURRENT2_PID_KD,
             CURRENT2_INTEGRAL_LIMIT, CURRENT2_OUTPUT_LIMIT);

}
//*******************************************************
//这是运行函数，我们的重点就是改写他，通过在运行函数内调用不同的函数，进行不同的组合得到各种各样的电机控制效果，uint8_t a是要操作的电机，
//float target_position是目标值，比如下面我就是写了一个电机位置闭环，可以去os.c里面看看，是不是在Task_5ms();有个motor_app_loop(1, 0); ，
//他是不是就表示“每5ms运行一次motor_app_loop(1, 0)，把1电机角度向0角度靠拢”，我们这个项目原理就是通过imu(陀螺仪)计算出各种float target_posit
//ion目标值传入，经行位置闭环
void motor_app_loop(uint8_t a, float target_position)
{
    unsigned long now = micros();

    float angle_mech =  getAngle(a);
    float angle_el = _electricalAngle(angle_mech);
//	  float error = target_position - angle_mech;
//    float V = PID_Update(&position_pid[a], error, now);
//    V = _constrain(V, -VOLTAGE_LIMIT, VOLTAGE_LIMIT);
//
//    float Iq  = ThreePhase2Iq(I[0],I[1],I[2], angle_el);
//	  float V_error = V - Iq*12/8.0f;
//    float Uq = PID_Update(&current_pid[a], V_error, now);
//    Uq = _constrain(Iq, -VOLTAGE_LIMIT, VOLTAGE_LIMIT);

//    setPhaseVoltage(a,-V, 0, -angle_el);
//    setPhaseVoltage(a, 0,-target_position+angle_el, angle_el);

//	  currentControlLoop(a, V, ThreePhase2Iq(I[0],I[1],I[2], angle_el),angle_el, now, &current_pid[a]);
    positionControlLoop(a, target_position, angle_mech, angle_el, now, &position_pid[a]);
}