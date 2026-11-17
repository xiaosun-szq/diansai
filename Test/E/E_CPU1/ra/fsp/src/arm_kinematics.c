#include "arm_kinematics.h"
#include <stdint.h>   // uint8_t int16_t
#include <stdbool.h>  // bool true false

ArmController_t arm_ctrl;

/* 内部辅助函数：求绝对值 (纯C实现，避免引入额外的 math.h 依赖) */
static float fast_abs(float val) {
    return (val < 0.0f) ? -val : val;
}

/* 构造函数实现 */
void ArmController_Init(ArmController_t *self, float kp, float ki, float kd, float k, float A) {
    if (!self) return;
    
    self->kp = kp;
    self->ki = ki;
    self->kd = kd;
    self->k_factor = k;
    self->target_A = A;
    
    self->m1_integral = 0.0f;
    self->m1_last_error = 0.0f;
}

/* 核心方法实现：输入坐标，输出所有电机的动作增量 */
MotorIncrement_t ArmController_Update(ArmController_t *self, 
                                      float a, float b, float c, 
                                      float x, float y, float z, 
                                      float d) 
{
    MotorIncrement_t out = {0.0f, 0.0f, 0.0f, 0.0f};
    
    if (!self) return out;
    
    /* 异常检测：如果返回的蓝莓坐标 (x,y,z) 全为 0，保持所有电机不变 */
    if (x == 0.0f && y == 0.0f && z == 0.0f) {
        return out; 
    }

    /* ---------------- M1 俯仰电机 (弧度制, PID 控制) ---------------- */
    float m1_error = y - b;
    
    if (fast_abs(m1_error) > M1MAXERROR) {
        self->m1_integral += m1_error;
        float p_term = self->kp * m1_error;
        float i_term = self->ki * self->m1_integral;
        float d_term = self->kd * (self->target_A - d) * (m1_error - self->m1_last_error);
        
        float m1_pid_out = p_term + i_term + d_term;
        out.m1_inc_rad = -m1_pid_out; /* 负误差导致电机角度增大 */
    } 
    self->m1_last_error = m1_error;
    
    /* ---------------- M2 水平电机 (角度制, 比例控制) ---------------- */
    float m2_error = x - a;
    
    if (fast_abs(m2_error) > M2MAXERROR) {
        out.m2_inc_deg = m2_error * 8.0f * self->k_factor;
    }
    
    /* ---------------- M3 升降电机 (角度制, 条件触发联动) ---------------- */
    float m3_error = y - b;
    
    if ((d > MAXD) || (fast_abs(m1_error) > MAXPY)) {
        if (fast_abs(m3_error) > M3MAXERROR) {
            out.m3_inc_deg = m3_error * 8.0f * self->k_factor;
        }
    }
    
    /* ---------------- M4 伸缩杆电机 (弧度制, 比例控制) ---------------- */
    float m4_error = z - c;
    
    if (fast_abs(m4_error) > M4MAXERROR) {
        out.m4_inc_rad = m4_error * 8.0f * self->k_factor; 
    }
    
    return out;
}

