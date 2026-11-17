#ifndef ARM_KINEMATICS_H
#define ARM_KINEMATICS_H

#ifdef __cplusplus
extern "C" {
#endif
#include <a.h>
    /* ================= 宏定义容差与死区参数 ================= */
#define M1MAXERROR  5.0f   /* M1 俯仰误差容差 (y-b) */
#define M2MAXERROR  5.0f   /* M2 水平误差容差 (x-a) */
#define M3MAXERROR  5.0f   /* M3 升降误差容差 (y-b) */
#define M4MAXERROR  5.0f   /* M4 伸缩误差容差 (z-c) */

#define MAXD        50.0f  /* 杆长触发阈值 (用于M3联动判断) */
#define MAXPY       15.0f  /* 俯仰误差绝对值触发阈值 (用于M3联动判断) */

    /* ================= 数据结构设计 (OOP 风格) ================= */

    /* 电机增量输出结构体 */
    typedef struct {
        float m1_inc_rad; /* M1 俯仰电机增量 (弧度) */
        float m2_inc_deg; /* M2 水平电机增量 (度) */
        float m3_inc_deg; /* M3 升降电机增量 (度) */
        float m4_inc_rad; /* M4 伸缩电机增量 (弧度) */
    } MotorIncrement_t;

    /* 控制器对象句柄 (类属性) */
    typedef struct {
        float kp;
        float ki;
        float kd;
        float k_factor;
        float target_A;
        float m1_integral;
        float m1_last_error;
    } ArmController_t;

    /* ================= 方法声明 ================= */

    void ArmController_Init(ArmController_t *self, float kp, float ki, float kd, float k, float A);

    MotorIncrement_t ArmController_Update(ArmController_t *self,
                                          float a, float b, float c,
                                          float x, float y, float z,
                                          float d);

extern ArmController_t arm_ctrl;

#ifdef __cplusplus
}
#endif

#endif /* ARM_KINEMATICS_H */