//
// Created by Qw011 on 2026/7/20.
//

#ifndef E_CPU1_BLUEBERRY_TRACKER_H
#define E_CPU1_BLUEBERRY_TRACKER_H

#endif //E_CPU1_BLUEBERRY_TRACKER_H
#ifndef BLUEBERRY_TRACKER_H
#define BLUEBERRY_TRACKER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "arm_kinematics.h" /* 依赖前文生成的电机解算库 */

    /* 跟踪器状态机枚举 */
    typedef enum {
        STATE_SEARCH = 0,   /* 状态：寻找新目标 */
        STATE_TRACKING,     /* 状态：锁定并动态追踪目标 */
        STATE_PICKING       /* 状态：对准完毕，执行采摘 */
    } TrackerState_t;

    /* 跟踪器对象句柄 (OOP) */
    typedef struct {
        TrackerState_t state;

        /* 当前锁定的唯一目标坐标 */
        int16_t locked_x;
        int16_t locked_y;
        int16_t locked_z;

        /* 防抖与采摘计时器 */
      volatile  uint16_t align_stable_cnt;
      volatile  uint16_t picking_timer;

        /* 绑定的运动控制器句柄指针 */
        ArmController_t *arm_ctrl;

    } BlueberryTracker_t;

    /* ================= 方法声明 ================= */

    void Tracker_Init(BlueberryTracker_t *self, ArmController_t *ctrl);

    MotorIncrement_t Tracker_Process(BlueberryTracker_t *self,
                                     target_3d_t *targets,
                                     uint8_t count,
                                     float a, float b, float c,
                                     float current_d);
    extern  BlueberryTracker_t tracker;

#ifdef __cplusplus
}
#endif

#endif /* BLUEBERRY_TRACKER_H */