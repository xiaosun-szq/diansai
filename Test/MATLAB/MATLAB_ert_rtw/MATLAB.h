/*
 * Sponsored License - for use in support of a program or activity
 * sponsored by MathWorks.  Not for government, commercial or other
 * non-sponsored organizational use.
 *
 * File: MATLAB.h
 *
 * Code generated for Simulink model 'MATLAB'.
 *
 * Model version                  : 1.6
 * Simulink Coder version         : 9.9 (R2023a) 19-Nov-2022
 * C/C++ source code generated on : Mon Nov  9 17:23:48 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_MATLAB_h_
#define RTW_HEADER_MATLAB_h_
#ifndef MATLAB_COMMON_INCLUDES_
#define MATLAB_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* MATLAB_COMMON_INCLUDES_ */

#include "MATLAB_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T Angle_out;                    /* '<Root>/Angle_out' */
} ExtY_MATLAB_T;

/* Real-time Model Data Structure */
struct tag_RTM_MATLAB_T {
  const char_T * volatile errorStatus;
};

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_MATLAB_T MATLAB_Y;

/* Model entry point functions */
extern void MATLAB_initialize(void);
extern void MATLAB_step(void);
extern void MATLAB_terminate(void);

/* Real-time Model object */
extern RT_MODEL_MATLAB_T *const MATLAB_M;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<Root>/Discrete Transfer Fcn' : Unused code path elimination
 * Block '<S1>/FixPt Data Type Duplicate' : Unused code path elimination
 * Block '<S2>/Data Type Duplicate' : Unused code path elimination
 * Block '<S2>/Data Type Propagation' : Unused code path elimination
 * Block '<Root>/Scope' : Unused code path elimination
 */

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
 * '<Root>' : 'MATLAB'
 * '<S1>'   : 'MATLAB/Rate Limiter Dynamic'
 * '<S2>'   : 'MATLAB/Rate Limiter Dynamic/Saturation Dynamic'
 */
#endif                                 /* RTW_HEADER_MATLAB_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
