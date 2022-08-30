/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Kalman_Filter.c
 *
 * Code generated for Simulink model 'Kalman_Filter'.
 *
 * Model version                  : 1.4
 * Simulink Coder version         : 9.7 (R2022a) 13-Nov-2021
 * C/C++ source code generated on : Sun Aug 28 17:09:25 2022
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "Kalman_Filter.h"
#include "rtwtypes.h"

/* Block signals and states (default storage) */
DW rtDW;

/* External inputs (root inport signals with default storage) */
ExtU rtU;

/* External outputs (root outports fed by signals with default storage) */
ExtY rtY;

/* Real-time model */
static RT_MODEL rtM_;
RT_MODEL *const rtM = &rtM_;

/* Model step function */
void Kalman_Filter_step(void)
{
  real_T rtb_K_k;
  real_T rtb_Xk1k1;

  /* Delay: '<Root>/Delay1' */
  rtb_Xk1k1 = rtDW.Delay1_DSTATE;

  /* MATLAB Function: '<Root>/MATLAB Function' incorporates:
   *  Constant: '<Root>/Constant'
   *  Constant: '<Root>/Constant1'
   *  Delay: '<Root>/Delay'
   *  Sum: '<Root>/Add2'
   */
  rtb_K_k = (rtDW.Delay_DSTATE + 0.01) / ((rtDW.Delay_DSTATE + 0.01) + 0.1);

  /* Sum: '<Root>/Add1' incorporates:
   *  Inport: '<Root>/Input'
   *  Product: '<Root>/MatrixMultiply'
   *  Sum: '<Root>/Add'
   */
  rtb_Xk1k1 += (rtU.Input - rtb_Xk1k1) * rtb_K_k;

  /* Outport: '<Root>/Output' */
  rtY.Output = rtb_Xk1k1;

  /* Update for Delay: '<Root>/Delay1' */
  rtDW.Delay1_DSTATE = rtb_Xk1k1;

  /* Update for Delay: '<Root>/Delay' incorporates:
   *  Constant: '<Root>/Constant1'
   *  Constant: '<Root>/Constant2'
   *  Product: '<Root>/Matrix Multiply'
   *  Sum: '<Root>/Add2'
   *  Sum: '<Root>/Add3'
   */
  rtDW.Delay_DSTATE = (1.0 - rtb_K_k) * (rtDW.Delay_DSTATE + 0.01);
}

/* Model initialize function */
void Kalman_Filter_initialize(void)
{
  /* (no initialization code required) */
}

/* Model Reset*/
void Kalman_Filter_reset(void)
{
    rtDW.Delay_DSTATE = 0;
    rtDW.Delay1_DSTATE = 0;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
