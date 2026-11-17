//
// Created by Qw011 on 2026/7/20.
//
//
// Created by Qw011 on 2026/7/20.
//
#include "MIT_DBC.h"
#include "CAN_MIT.h"
#include "blueberry_tracker.h"
#include "arm_kinematics.h"
#include <stdint.h>   // uint8_t int16_t
#include <stdbool.h>  // bool true false
#include "os.h"

BlueberryTracker_t tracker;
/* 内部辅助方法：计算两点间的平方距离 (纯C实现，避免使用 sqrt 引入数学库增加耦合) */
static int32_t calc_distance_sq(int16_t x1, int16_t y1, int16_t z1, int16_t x2, int16_t y2, int16_t z2) {
    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;
    int32_t dz = z2 - z1;
    return (dx * dx) + (dy * dy) + (dz * dz);
}

/* 初始化函数 */
void Tracker_Init(BlueberryTracker_t *self, ArmController_t *ctrl) {
    if (!self) return;
    self->state = STATE_SEARCH;
    self->locked_x = 0;
    self->locked_y = 0;
    self->locked_z = 0;
    self->align_stable_cnt = 0;
    self->picking_timer = 0;
    self->arm_ctrl = ctrl; /* 绑定底层 PID 结算器 */
}

/* 核心状态机更新函数 */
MotorIncrement_t Tracker_Process(BlueberryTracker_t *self,
                                 target_3d_t *targets,
                                 uint8_t count,
                                 float a, float b, float c,
                                 float current_d)

{
    abc = 0;
    /* 默认输出为 0，防止电机误动 */
    MotorIncrement_t zero_action = {0.0f, 0.0f, 0.0f, 0.0f};
    if (!self || !self->arm_ctrl) return zero_action;

    switch (self->state) {

        case STATE_SEARCH: {
            if (count > 0) {
                /* 策略：数组中如果有蓝莓，无脑锁定第 0 个作为当前抓取目标 */
                self->locked_x = targets[0].x_mm;
                self->locked_y = targets[0].y_mm;
                self->locked_z = targets[0].z_mm;

                self->state = STATE_TRACKING;
                self->align_stable_cnt = 0;
            }
            return zero_action; /* 刚锁定，本周期先不动 */
        }

        case STATE_TRACKING: {
            if (count == 0) {
                /* 画面无目标，回退搜索 */
                self->state = STATE_SEARCH;
                return zero_action;
            }

            /* 动态匹配：在最新数组中，找出跟上一帧锁定目标物理距离“最近”的那个点 */
            int32_t min_dist_sq = 2147483647;
            int8_t best_index = -1;

            for (uint8_t i = 0; i < count; i++) {
                int32_t dist = calc_distance_sq(self->locked_x, self->locked_y, self->locked_z,
                                                targets[i].x_mm, targets[i].y_mm, targets[i].z_mm);
                if (dist < min_dist_sq) {
                    min_dist_sq = dist;
                    best_index = i;
                }
            }

            /* 如果找出的点与上次锁定的点偏差在 100mm 以内(平方10000)，判定为是同一颗蓝莓，更新坐标 */
            if (best_index >= 0 && min_dist_sq < 10000) {
                self->locked_x = targets[best_index].x_mm;
                self->locked_y = targets[best_index].y_mm;
                self->locked_z = targets[best_index].z_mm;
            } else {
                /* 目标突然消失或偏移过大，放弃追踪，重新搜索 */
                self->state = STATE_SEARCH;
                return zero_action;
            }

            /* --- 核心联动：将选出的这 1 个稳定坐标，送入 PID 运动控制器结算 --- */
            MotorIncrement_t action = ArmController_Update(
                self->arm_ctrl,
                a, b, c,
                (float)self->locked_x, (float)self->locked_y, (float)self->locked_z,
                current_d
            );

            /* 判断结算输出是否全为 0 (意味着误差已经进入前文定义的 MAXERROR 死区容差) */
            if (action.m1_inc_rad == 0.0f && action.m2_inc_deg == 0.0f && action.m4_inc_rad == 0.0f) {
                self->align_stable_cnt++;
                /* 连续 10 帧(防抖)都在死区容差内，确认完全稳定对准，切入采摘状态 */
                if (self->align_stable_cnt > 10) {
                    self->state = STATE_PICKING;
                    self->picking_timer = 0;
                }
            } else {
                self->align_stable_cnt = 0; /* 若中途晃动出死区，重新防抖计数 */
            }

            return action; /* 返回结算好的动作增量给电机 */
        }

        case STATE_PICKING: {
            /* 在此状态下电机保持静止不动 (返回0增量)，防止执行夹取动作时机械臂发生位移 */
            self->picking_timer = 0;
            self->picking_timer++;

            abc = 90;
            if (self->picking_timer > 1000) {
                MIT_DBC_U3.Position = 0;
                MIT_DBC_U3.Kp = 2;
                MIT_DBC_U2.Kd = 0.3;
                MIT_DBC_U2.Velocity = 2;
                MIT_DBC_U2.Torque = 0.4;

                MIT_DBC_step(&MIT_DBC_U2, &MIT_DBC_Y3);
                canfd_send_test_data((can_ctrl_t *)&g_canfd1_ctrl, 0x204, MIT_DBC_Y3.Out1);
                if (self->picking_timer > 2000) {
                    self->state = STATE_SEARCH; /* 采摘完毕，重新回去寻找下一颗蓝莓 */
                }
            }
            return zero_action;
        }
    }

    return zero_action;
}