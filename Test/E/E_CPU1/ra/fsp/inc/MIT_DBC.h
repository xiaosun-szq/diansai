/*
 * Sponsored License - for use in support of a program or activity
 * sponsored by MathWorks.  Not for government, commercial or other
 * non-sponsored organizational use.
 *
 * File: MIT_DBC.h
 *
 * Code generated for Simulink model 'MIT_DBC'.
 *
 * Model version                  : 1.2
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Fri Jun 19 20:55:12 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef MIT_DBC_h_
#define MIT_DBC_h_
#ifndef MIT_DBC_COMMON_INCLUDES_
#define MIT_DBC_COMMON_INCLUDES_
#include <math.h>
#include "rtwtypes.h"
#include "math.h"
#include "can_fd_message.h"
#endif                                 /* MIT_DBC_COMMON_INCLUDES_ */

#include "MIT_DBC_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Block signals (default storage) */
typedef struct {
  CAN_FD_MESSAGE_BUS CANFDPack1;       /* '<Root>/CAN FD Pack1' */
} B_MIT_DBC_T;

/* Block states (default storage) for system '<Root>' */
typedef struct {
  int_T CANFDPack1_ModeSignalID;       /* '<Root>/CAN FD Pack1' */
} DW_MIT_DBC_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T Kd;                           /* '<Root>/Kd' */
  real_T Kp;                           /* '<Root>/Kp' */
  real_T Position;                     /* '<Root>/Position' */
  real_T Torque;                       /* '<Root>/Torque' */
  real_T Velocity;                     /* '<Root>/Velocity' */
} ExtU_MIT_DBC_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  uint8_T Out1[64];                    /* '<Root>/Out1' */
} ExtY_MIT_DBC_T;

/* Real-time Model Data Structure */
struct tag_RTM_MIT_DBC_T {
  const char_T * volatile errorStatus;
};

/* Block signals (default storage) */
extern B_MIT_DBC_T MIT_DBC_B;

/* Block states (default storage) */
extern DW_MIT_DBC_T MIT_DBC_DW;

/* External inputs (root inport signals with default storage) */
extern volatile ExtU_MIT_DBC_T MIT_DBC_U1;
extern volatile ExtU_MIT_DBC_T MIT_DBC_U2;
extern volatile ExtU_MIT_DBC_T MIT_DBC_U3;
/* External outputs (root outports fed by signals with default storage) */
extern volatile ExtY_MIT_DBC_T MIT_DBC_Y1;
extern volatile ExtY_MIT_DBC_T MIT_DBC_Y2;
extern volatile ExtY_MIT_DBC_T MIT_DBC_Y3;
/* Model entry point functions */
extern void MIT_DBC_initialize(void);
extern void MIT_DBC_step(ExtU_MIT_DBC_T* MIT, ExtY_MIT_DBC_T *MIT_Y);
extern void MIT_DBC_terminate(void);

/* Real-time Model object */
extern RT_MODEL_MIT_DBC_T *const MIT_DBC_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'MIT_DBC'
 */
#endif                                 /* MIT_DBC_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
