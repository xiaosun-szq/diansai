/*
 * Sponsored License - for use in support of a program or activity
 * sponsored by MathWorks.  Not for government, commercial or other
 * non-sponsored organizational use.
 *
 * File: MIT_DBC.c
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

#include "MIT_DBC.h"
#include <string.h>

/* Block signals (default storage) */
B_MIT_DBC_T MIT_DBC_B;

/* Block states (default storage) */
DW_MIT_DBC_T MIT_DBC_DW;

/* External inputs (root inport signals with default storage) */
volatile ExtU_MIT_DBC_T MIT_DBC_U1;
volatile ExtU_MIT_DBC_T MIT_DBC_U2;
volatile ExtU_MIT_DBC_T MIT_DBC_U3;
/* External outputs (root outports fed by signals with default storage) */
volatile ExtY_MIT_DBC_T MIT_DBC_Y1;
volatile ExtY_MIT_DBC_T MIT_DBC_Y2;
volatile ExtY_MIT_DBC_T MIT_DBC_Y3;;

/* Real-time model */
static RT_MODEL_MIT_DBC_T MIT_DBC_M_;
RT_MODEL_MIT_DBC_T *const MIT_DBC_M = &MIT_DBC_M_;

/* Model step function */
void MIT_DBC_step(ExtU_MIT_DBC_T* MIT, ExtY_MIT_DBC_T *MIT_Y)
{
  /* S-Function (scanfdpack): '<Root>/CAN FD Pack1' incorporates:
   *  Inport: '<Root>/Kd'
   *  Inport: '<Root>/Kp'
   *  Inport: '<Root>/Position'
   *  Inport: '<Root>/Torque'
   *  Inport: '<Root>/Velocity'
   */
  /* S-Function (scanfdpack): '<Root>/CAN FD Pack1' */
  MIT_DBC_B.CANFDPack1.ID = 513;
  MIT_DBC_B.CANFDPack1.Length = 8U;
  MIT_DBC_B.CANFDPack1.Extended = 0U;
  MIT_DBC_B.CANFDPack1.Remote = 0;
  MIT_DBC_B.CANFDPack1.BRS = 1;
  MIT_DBC_B.CANFDPack1.ProtocolMode = 1;
  MIT_DBC_B.CANFDPack1.DLC = 8U;
  MIT_DBC_B.CANFDPack1.Data[0] = 0;
  MIT_DBC_B.CANFDPack1.Data[1] = 0;
  MIT_DBC_B.CANFDPack1.Data[2] = 0;
  MIT_DBC_B.CANFDPack1.Data[3] = 0;
  MIT_DBC_B.CANFDPack1.Data[4] = 0;
  MIT_DBC_B.CANFDPack1.Data[5] = 0;
  MIT_DBC_B.CANFDPack1.Data[6] = 0;
  MIT_DBC_B.CANFDPack1.Data[7] = 0;
  MIT_DBC_B.CANFDPack1.Data[8] = 0;
  MIT_DBC_B.CANFDPack1.Data[9] = 0;
  MIT_DBC_B.CANFDPack1.Data[10] = 0;
  MIT_DBC_B.CANFDPack1.Data[11] = 0;
  MIT_DBC_B.CANFDPack1.Data[12] = 0;
  MIT_DBC_B.CANFDPack1.Data[13] = 0;
  MIT_DBC_B.CANFDPack1.Data[14] = 0;
  MIT_DBC_B.CANFDPack1.Data[15] = 0;
  MIT_DBC_B.CANFDPack1.Data[16] = 0;
  MIT_DBC_B.CANFDPack1.Data[17] = 0;
  MIT_DBC_B.CANFDPack1.Data[18] = 0;
  MIT_DBC_B.CANFDPack1.Data[19] = 0;
  MIT_DBC_B.CANFDPack1.Data[20] = 0;
  MIT_DBC_B.CANFDPack1.Data[21] = 0;
  MIT_DBC_B.CANFDPack1.Data[22] = 0;
  MIT_DBC_B.CANFDPack1.Data[23] = 0;
  MIT_DBC_B.CANFDPack1.Data[24] = 0;
  MIT_DBC_B.CANFDPack1.Data[25] = 0;
  MIT_DBC_B.CANFDPack1.Data[26] = 0;
  MIT_DBC_B.CANFDPack1.Data[27] = 0;
  MIT_DBC_B.CANFDPack1.Data[28] = 0;
  MIT_DBC_B.CANFDPack1.Data[29] = 0;
  MIT_DBC_B.CANFDPack1.Data[30] = 0;
  MIT_DBC_B.CANFDPack1.Data[31] = 0;
  MIT_DBC_B.CANFDPack1.Data[32] = 0;
  MIT_DBC_B.CANFDPack1.Data[33] = 0;
  MIT_DBC_B.CANFDPack1.Data[34] = 0;
  MIT_DBC_B.CANFDPack1.Data[35] = 0;
  MIT_DBC_B.CANFDPack1.Data[36] = 0;
  MIT_DBC_B.CANFDPack1.Data[37] = 0;
  MIT_DBC_B.CANFDPack1.Data[38] = 0;
  MIT_DBC_B.CANFDPack1.Data[39] = 0;
  MIT_DBC_B.CANFDPack1.Data[40] = 0;
  MIT_DBC_B.CANFDPack1.Data[41] = 0;
  MIT_DBC_B.CANFDPack1.Data[42] = 0;
  MIT_DBC_B.CANFDPack1.Data[43] = 0;
  MIT_DBC_B.CANFDPack1.Data[44] = 0;
  MIT_DBC_B.CANFDPack1.Data[45] = 0;
  MIT_DBC_B.CANFDPack1.Data[46] = 0;
  MIT_DBC_B.CANFDPack1.Data[47] = 0;
  MIT_DBC_B.CANFDPack1.Data[48] = 0;
  MIT_DBC_B.CANFDPack1.Data[49] = 0;
  MIT_DBC_B.CANFDPack1.Data[50] = 0;
  MIT_DBC_B.CANFDPack1.Data[51] = 0;
  MIT_DBC_B.CANFDPack1.Data[52] = 0;
  MIT_DBC_B.CANFDPack1.Data[53] = 0;
  MIT_DBC_B.CANFDPack1.Data[54] = 0;
  MIT_DBC_B.CANFDPack1.Data[55] = 0;
  MIT_DBC_B.CANFDPack1.Data[56] = 0;
  MIT_DBC_B.CANFDPack1.Data[57] = 0;
  MIT_DBC_B.CANFDPack1.Data[58] = 0;
  MIT_DBC_B.CANFDPack1.Data[59] = 0;
  MIT_DBC_B.CANFDPack1.Data[60] = 0;
  MIT_DBC_B.CANFDPack1.Data[61] = 0;
  MIT_DBC_B.CANFDPack1.Data[62] = 0;
  MIT_DBC_B.CANFDPack1.Data[63] = 0;

  {
    /* --------------- START Packing signal 0 ------------------
     *  startBit                = 52
     *  length                  = 12
     *  desiredSignalByteLayout = BIGENDIAN
     *  dataType                = UNSIGNED
     *  factor                  = 0.01221
     *  offset                  = 0.0
     *  minimum                 = 0.0
     *  maximum                 = 0.0
     * -----------------------------------------------------------------------*/
    {
      real_T outValue = 0;

      {
        real_T result = MIT->Kd;

        /* no offset to apply */
        result = result * (1 / 0.01221);

        /* round to closest integer value for integer CAN signal */
        outValue = round(result);
      }

      {
        uint16_T packedValue;
        if (outValue > (real_T)(4095)) {
          packedValue = (uint16_T) 4095;
        } else if (outValue < (real_T)(0)) {
          packedValue = (uint16_T) 0;
        } else {
          packedValue = (uint16_T) (outValue);
        }

        {
          {
            MIT_DBC_B.CANFDPack1.Data[6] = MIT_DBC_B.CANFDPack1.Data[6] |
              (uint8_T)((uint16_T)((uint16_T)(packedValue & (uint16_T)0xFU) << 4));
            MIT_DBC_B.CANFDPack1.Data[5] = MIT_DBC_B.CANFDPack1.Data[5] |
              (uint8_T)((uint16_T)((uint16_T)(packedValue & (uint16_T)0xFF0U) >>
              4));
          }
        }
      }
    }

    /* --------------- START Packing signal 1 ------------------
     *  startBit                = 32
     *  length                  = 12
     *  desiredSignalByteLayout = BIGENDIAN
     *  dataType                = UNSIGNED
     *  factor                  = 0.1221
     *  offset                  = 0.0
     *  minimum                 = 0.0
     *  maximum                 = 0.0
     * -----------------------------------------------------------------------*/
    {
      real_T outValue = 0;

      {
        real_T result = MIT->Kp;

        /* no offset to apply */
        result = result * (1 / 0.1221);

        /* round to closest integer value for integer CAN signal */
        outValue = round(result);
      }

      {
        uint16_T packedValue;
        if (outValue > (real_T)(4095)) {
          packedValue = (uint16_T) 4095;
        } else if (outValue < (real_T)(0)) {
          packedValue = (uint16_T) 0;
        } else {
          packedValue = (uint16_T) (outValue);
        }

        {
          {
            MIT_DBC_B.CANFDPack1.Data[4] = MIT_DBC_B.CANFDPack1.Data[4] |
              (uint8_T)((uint16_T)(packedValue & (uint16_T)0xFFU));
            MIT_DBC_B.CANFDPack1.Data[3] = MIT_DBC_B.CANFDPack1.Data[3] |
              (uint8_T)((uint16_T)((uint16_T)(packedValue & (uint16_T)0xF00U) >>
              8));
          }
        }
      }
    }

    /* --------------- START Packing signal 2 ------------------
     *  startBit                = 8
     *  length                  = 16
     *  desiredSignalByteLayout = BIGENDIAN
     *  dataType                = UNSIGNED
     *  factor                  = 0.0003815
     *  offset                  = -12.5
     *  minimum                 = 0.0
     *  maximum                 = 0.0
     * -----------------------------------------------------------------------*/
    {
      real_T outValue = 0;

      {
        real_T result = MIT->Position;

        /* full scaling operation */
        result = (result - -12.5) * (1 / 0.0003815);

        /* round to closest integer value for integer CAN signal */
        outValue = round(result);
      }

      {
        uint16_T packedValue;
        if (outValue > (real_T)(65535)) {
          packedValue = (uint16_T) 65535;
        } else if (outValue < (real_T)(0)) {
          packedValue = (uint16_T) 0;
        } else {
          packedValue = (uint16_T) (outValue);
        }

        {
          {
            MIT_DBC_B.CANFDPack1.Data[1] = MIT_DBC_B.CANFDPack1.Data[1] |
              (uint8_T)((uint16_T)(packedValue & (uint16_T)0xFFU));
            MIT_DBC_B.CANFDPack1.Data[0] = MIT_DBC_B.CANFDPack1.Data[0] |
              (uint8_T)((uint16_T)((uint16_T)(packedValue & (uint16_T)0xFF00U) >>
              8));
          }
        }
      }
    }

    /* --------------- START Packing signal 3 ------------------
     *  startBit                = 56
     *  length                  = 12
     *  desiredSignalByteLayout = BIGENDIAN
     *  dataType                = UNSIGNED
     *  factor                  = 0.01466
     *  offset                  = -30.0
     *  minimum                 = 0.0
     *  maximum                 = 0.0
     * -----------------------------------------------------------------------*/
    {
      real_T outValue = 0;

      {
        real_T result = MIT->Torque;

        /* full scaling operation */
        result = (result - -30.0) * (1 / 0.01466);

        /* round to closest integer value for integer CAN signal */
        outValue = round(result);
      }

      {
        uint16_T packedValue;
        if (outValue > (real_T)(4095)) {
          packedValue = (uint16_T) 4095;
        } else if (outValue < (real_T)(0)) {
          packedValue = (uint16_T) 0;
        } else {
          packedValue = (uint16_T) (outValue);
        }

        {
          {
            MIT_DBC_B.CANFDPack1.Data[7] = MIT_DBC_B.CANFDPack1.Data[7] |
              (uint8_T)((uint16_T)(packedValue & (uint16_T)0xFFU));
            MIT_DBC_B.CANFDPack1.Data[6] = MIT_DBC_B.CANFDPack1.Data[6] |
              (uint8_T)((uint16_T)((uint16_T)(packedValue & (uint16_T)0xF00U) >>
              8));
          }
        }
      }
    }

    /* --------------- START Packing signal 4 ------------------
     *  startBit                = 28
     *  length                  = 12
     *  desiredSignalByteLayout = BIGENDIAN
     *  dataType                = UNSIGNED
     *  factor                  = 0.01466
     *  offset                  = -30.0
     *  minimum                 = 0.0
     *  maximum                 = 0.0
     * -----------------------------------------------------------------------*/
    {
      real_T outValue = 0;

      {
        real_T result = MIT->Velocity;

        /* full scaling operation */
        result = (result - -30.0) * (1 / 0.01466);

        /* round to closest integer value for integer CAN signal */
        outValue = round(result);
      }

      {
        uint16_T packedValue;
        if (outValue > (real_T)(4095)) {
          packedValue = (uint16_T) 4095;
        } else if (outValue < (real_T)(0)) {
          packedValue = (uint16_T) 0;
        } else {
          packedValue = (uint16_T) (outValue);
        }

        {
          {
            MIT_DBC_B.CANFDPack1.Data[3] = MIT_DBC_B.CANFDPack1.Data[3] |
              (uint8_T)((uint16_T)((uint16_T)(packedValue & (uint16_T)0xFU) << 4));
            MIT_DBC_B.CANFDPack1.Data[2] = MIT_DBC_B.CANFDPack1.Data[2] |
              (uint8_T)((uint16_T)((uint16_T)(packedValue & (uint16_T)0xFF0U) >>
              4));
          }
        }
      }
    }
  }

  /* Outport: '<Root>/Out1' */
  memcpy(&MIT_Y->Out1[0], &MIT_DBC_B.CANFDPack1.Data[0], sizeof(uint8_T) <<
         6U);
}

/* Model initialize function */
void MIT_DBC_initialize(void)
{
  MIT_DBC_U1.Position = 0;
  MIT_DBC_U1.Kp =20;
  MIT_DBC_U1.Kd =6;
  MIT_DBC_U1.Velocity = 0.1;
  MIT_DBC_U1.Torque = 4;

  MIT_DBC_U2.Position = 0;
  MIT_DBC_U2.Kp = 20;
  MIT_DBC_U2.Kd = 6;
  MIT_DBC_U2.Velocity = 0.1;
  MIT_DBC_U2.Torque = 4;

  MIT_DBC_U3.Position = 0;
  MIT_DBC_U3.Kp = 4;
  MIT_DBC_U3.Kd = 2;
  MIT_DBC_U3.Velocity = 0.1;
  MIT_DBC_U3.Torque = 1;
  /* (no initialization code required) */
}

/* Model terminate function */
void MIT_DBC_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
