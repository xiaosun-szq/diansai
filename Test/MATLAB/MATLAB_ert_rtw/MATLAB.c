/*
 * Sponsored License - for use in support of a program or activity
 * sponsored by MathWorks.  Not for government, commercial or other
 * non-sponsored organizational use.
 *
 * File: MATLAB.c
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

#include "MATLAB.h"
#include "rtwtypes.h"

/* External outputs (root outports fed by signals with default storage) */
ExtY_MATLAB_T MATLAB_Y;

/* Real-time model */
static RT_MODEL_MATLAB_T MATLAB_M_;
RT_MODEL_MATLAB_T *const MATLAB_M = &MATLAB_M_;

/* Model step function */
void MATLAB_step(void)
{
  real_T rtb_Switch2;

  /* Product: '<S1>/delta rise limit' incorporates:
   *  SampleTimeMath: '<S1>/sample time'
   *
   * About '<S1>/sample time':
   *  y = K where K = ( w * Ts )
   */
  rtb_Switch2 = 0.001;

  /* Switch: '<S2>/Switch2' incorporates:
   *  Constant: '<Root>/Constant2'
   *  RelationalOperator: '<S2>/LowerRelop1'
   *  Sum: '<S1>/Difference Inputs1'
   *  UnitDelay: '<S1>/Delay Input2'
   *
   * Block description for '<S1>/Difference Inputs1':
   *
   *  Add in CPU
   *
   * Block description for '<S1>/Delay Input2':
   *
   *  Store in Global RAM
   */
  if (!(20.0 - MATLAB_Y.Angle_out > 0.001)) {
    /* Switch: '<S2>/Switch' incorporates:
     *  RelationalOperator: '<S2>/UpperRelop'
     */
    if (20.0 - MATLAB_Y.Angle_out < -0.001) {
      rtb_Switch2 = -0.001;
    } else {
      rtb_Switch2 = 20.0 - MATLAB_Y.Angle_out;
    }

    /* End of Switch: '<S2>/Switch' */
  }

  /* End of Switch: '<S2>/Switch2' */

  /* Sum: '<S1>/Difference Inputs2' incorporates:
   *  UnitDelay: '<S1>/Delay Input2'
   *
   * Block description for '<S1>/Difference Inputs2':
   *
   *  Add in CPU
   *
   * Block description for '<S1>/Delay Input2':
   *
   *  Store in Global RAM
   */
  MATLAB_Y.Angle_out += rtb_Switch2;
}

/* Model initialize function */
void MATLAB_initialize(void)
{
  /* (no initialization code required) */
}

/* Model terminate function */
void MATLAB_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
