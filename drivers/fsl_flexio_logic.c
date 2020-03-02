/*
 * fsl_flexio_logic.c
 *
 *  Created on: 29 Feb 2020
 *      Author: iglesim
 */


/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexio_logic.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.flexio_logic"
#endif

/*<! @brief uart transfer state. */
enum _flexio_logic_transfer_states
{
    kFLEXIO_LOGIC_SampleIdle, /* Trigger armed. */
    kFLEXIO_LOGIC_SampleBusy  /* Sampling. */
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Codes
 ******************************************************************************/




static uint32_t FLEXIO_LOGIC_GetInstance(FLEXIO_LOGIC_Type *base)
{
    return FLEXIO_GetInstance(base->flexioBase);
}

/*!
 * brief Gets the default configuration to configure the FlexIO Camera. The configuration
 * can be used directly for calling the FLEXIO_CAMERA_Init().
 * Example:
   code
   flexio_camera_config_t config;
   FLEXIO_CAMERA_GetDefaultConfig(&userConfig);
   endcode
 * param config Pointer to the flexio_camera_config_t structure
*/
void FLEXIO_LOGIC_GetDefaultConfig(flexio_logic_config_t *config)
{
    assert(config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->samples = 4096;
    config->frequency = DEMO_FLEXIO_FREQUENCY;
}


void FLEXIO_LOGIC_Init(FLEXIO_LOGIC_Type *base, const flexio_logic_config_t *config)
{
	assert(base);
	assert(config);

	volatile uint32_t i          = 0;
	volatile uint32_t controlVal = 0;

	flexio_shifter_config_t shifterConfig;
	flexio_timer_config_t timerConfig;
	const uint32_t duty = 50;
	uint32_t freq_Hz = DEMO_FLEXIO_FREQUENCY;	//config->frequency;		// 10kHz

    assert((freq_Hz < FLEXIO_MAX_FREQUENCY) && (freq_Hz > FLEXIO_MIN_FREQUENCY));

    uint32_t lowerValue = 0; /* Number of clock cycles in high logic state in one period */
    uint32_t upperValue = 0; /* Number of clock cycles in low logic state in one period */
    uint32_t sum        = 0; /* Number of clock cycles in one period */

	/* Clear the shifterConfig & timerConfig struct. */
	memset(&shifterConfig, 0, sizeof(shifterConfig));
	memset(&timerConfig, 0, sizeof(timerConfig));


	// STEP 1
    /* FLEXIO_LOGIC shifter config */
    shifterConfig.timerSelect   = base->timerIdx[2];			// timer 2 generates the bit count for SHIFTBUF storage
    shifterConfig.timerPolarity = kFLEXIO_ShifterTimerPolarityOnPositive;
    shifterConfig.pinConfig     = kFLEXIO_PinConfigOutputDisabled;
    shifterConfig.pinSelect     = base->datPinStartIdx;
    shifterConfig.pinPolarity   = kFLEXIO_PinActiveHigh;
    shifterConfig.shifterMode   = kFLEXIO_ShifterModeReceive;
    shifterConfig.parallelWidth = FLEXIO_LOGIC_PARALLEL_DATA_WIDTH - 1U;
    shifterConfig.inputSource   = kFLEXIO_ShifterInputFromNextShifterOutput;
    shifterConfig.shifterStop   = kFLEXIO_ShifterStopBitDisable;
    shifterConfig.shifterStart  = kFLEXIO_ShifterStartBitDisabledLoadDataOnEnable;
    /* Configure the shifters as FIFO buffer. */
    for (i = base->shifterStartIdx; i < (base->shifterStartIdx + base->shifterCount - 1U); i++)
    {
        FLEXIO_SetShifterConfig(base->flexioBase, i, &shifterConfig);
    }
    shifterConfig.inputSource = kFLEXIO_ShifterInputFromPin;
    FLEXIO_SetShifterConfig(base->flexioBase, i, &shifterConfig);

    // STEP 2
    // Taken from the PWM example, we generate a square wave to be use as clock
    /* Configure the timer DEMO_FLEXIO_TIMER_CH for generating PWM */
    timerConfig.triggerSelect   = 0u;
    timerConfig.triggerSource   = kFLEXIO_TimerTriggerSourceInternal;
    timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveLow;
    timerConfig.pinConfig       = kFLEXIO_PinConfigOutput;
    timerConfig.pinPolarity     = kFLEXIO_PinActiveHigh;
    timerConfig.pinSelect       = base->sclkPinIdx;			 /* Set pwm output */
    timerConfig.timerMode       = kFLEXIO_TimerModeDual8BitPWM; 		// enable from the start
    timerConfig.timerOutput     = kFLEXIO_TimerOutputOneNotAffectedByReset;
    timerConfig.timerDecrement  = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput;
    timerConfig.timerDisable    = kFLEXIO_TimerDisableNever;
    timerConfig.timerEnable     = kFLEXIO_TimerEnabledAlways;
    timerConfig.timerReset      = kFLEXIO_TimerResetNever;
    timerConfig.timerStart      = kFLEXIO_TimerStartBitDisabled;
    timerConfig.timerStop       = kFLEXIO_TimerStopBitDisabled;


    /* Calculate timer lower and upper values of TIMCMP */
    /* Calculate the nearest integer value for sum, using formula round(x) = (2 * floor(x) + 1) / 2 */
    /* sum = DEMO_FLEXIO_CLOCK_FREQUENCY / freq_H */
    sum = (DEMO_FLEXIO_CLOCK_FREQUENCY * 2 / freq_Hz + 1) / 2;
    /* Calculate the nearest integer value for lowerValue, the high period of the pwm output */
    /* lowerValue = sum * duty / 100 */
    lowerValue = (sum * duty / 50 + 1) / 2;
    /* Calculate upper value, the low period of the pwm output */
    upperValue                   = sum - lowerValue;
    timerConfig.timerCompare = ((upperValue - 1) << 8U) | (lowerValue - 1);

    FLEXIO_SetTimerConfig(base->flexioBase, base->timerIdx[0], &timerConfig);

    /// STEP 3
    /* FLEXIO_LOGIC timer configuration to count samples
     * the clk source of this timer is the sampling clock which is generated using FlexIO in step 2
     */
    timerConfig.triggerSelect   = FLEXIO_TIMER_TRIGGER_SEL_PININPUT(base->trgPinIdx);
    timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveHigh;
    timerConfig.triggerSource   = kFLEXIO_TimerTriggerSourceInternal;
    timerConfig.pinConfig       = kFLEXIO_PinConfigOutputDisabled;
    timerConfig.pinSelect       = base->sclkPinIdx;
    timerConfig.pinPolarity     = kFLEXIO_PinActiveHigh;
    timerConfig.timerMode       = kFLEXIO_TimerModeSingle16Bit;
    timerConfig.timerOutput     = kFLEXIO_TimerOutputZeroNotAffectedByReset;
    timerConfig.timerDecrement  = kFLEXIO_TimerDecSrcOnPinInputShiftPinInput;
    timerConfig.timerReset      = kFLEXIO_TimerResetNever;
    timerConfig.timerDisable    = kFLEXIO_TimerDisableOnTimerCompare;			// disable on count compare
    timerConfig.timerEnable     = kFLEXIO_TimerEnableOnTriggerRisingEdge;
    timerConfig.timerStop       = kFLEXIO_TimerStopBitDisabled;
    timerConfig.timerStart      = kFLEXIO_TimerStartBitDisabled;
    timerConfig.timerCompare    = ((uint32_t)(config->samples * 2 - 1U));	// block size in bits or clock transitions

    FLEXIO_SetTimerConfig(base->flexioBase, base->timerIdx[1], &timerConfig);

    /* FLEXIO_LOGIC timer config to drive shift register
     * the clk source of timer to drive the shifter
     * is the CLK generated using FlexIO
     */
    timerConfig.triggerSelect   = FLEXIO_TIMER_TRIGGER_SEL_PININPUT(base->trgPinIdx);
    timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveHigh;
    timerConfig.triggerSource   = kFLEXIO_TimerTriggerSourceInternal;
    timerConfig.pinConfig       = kFLEXIO_PinConfigOutputDisabled;
    timerConfig.pinSelect       = base->sclkPinIdx;;
    timerConfig.pinPolarity     = kFLEXIO_PinActiveHigh;
    timerConfig.timerMode       = kFLEXIO_TimerModeSingle16Bit;
    timerConfig.timerOutput     = kFLEXIO_TimerOutputZeroNotAffectedByReset;
    timerConfig.timerDecrement  = kFLEXIO_TimerDecSrcOnPinInputShiftPinInput;
    timerConfig.timerReset      = kFLEXIO_TimerResetOnTimerTriggerRisingEdge;
    timerConfig.timerDisable    = kFLEXIO_TimerDisableOnPreTimerDisable;			// disable on Timer N-1 disable
    timerConfig.timerEnable     = kFLEXIO_TimerEnableOnTriggerRisingEdge;
    timerConfig.timerStop       = kFLEXIO_TimerStopBitDisabled;
    timerConfig.timerStart      = kFLEXIO_TimerStartBitDisabled;
    timerConfig.timerCompare    = 16U * base->shifterCount - 1U;

    FLEXIO_SetTimerConfig(base->flexioBase, base->timerIdx[2], &timerConfig);

    /* Clear flags. */
    FLEXIO_ClearShifterErrorFlags(base->flexioBase, ((1U << (base->shifterCount)) - 1U) << (base->shifterStartIdx));
    FLEXIO_ClearTimerStatusFlags(base->flexioBase, ((1U << (base->timerIdx[2])) - 1U) << (base->timerIdx[0]));

}


void FLEXIO_LOGIC_ReadBlocking(FLEXIO_LOGIC_Type *base, const flexio_logic_config_t *config)
{
    assert(config);

    uint32_t sampleSize = config->samples;		/// convert samples to actual bytes
    uint32_t sEvtNumber = ((base->shifterCount*4) *(8/config->pwidth)); // the actual samples in a Flexio Event
    uint8_t *sampleData = config->sDataPtr;
    while (sampleSize)
    {
        /* Wait until data transfer complete. */
        while (!(FLEXIO_LOGIC_GetStatusFlags(base) & kFLEXIO_LOGIC_RxDataRegFullFlag))
        {
        	__NOP();
        }

        for(int j = base->shifterStartIdx; j < base->shifterCount; j++){
        	*sampleData++ = base->flexioBase->SHIFTBUFBYS[j];
        }
        sampleSize = sampleSize - sEvtNumber;
    }

}

status_t FLEXIO_LOGIC_TransferCreateHandle(FLEXIO_LOGIC_Type *base,
                                          flexio_logic_handle_t *handle,
                                          flexio_logic_transfer_callback_t callback,
                                          void *userData)
{
    assert(handle);

    IRQn_Type flexio_irqs[] = FLEXIO_IRQS;

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    /* Set the Sampling state. */
    handle->sampleState = kFLEXIO_LOGIC_SampleIdle;

    /* Set the callback and user data. */
    handle->callback = callback;
    handle->userData = userData;

    /* Enable interrupt in NVIC. */
    EnableIRQ(flexio_irqs[FLEXIO_LOGIC_GetInstance(base)]);

    /* Save the context in global variables to support the double weak mechanism. */
    return FLEXIO_RegisterHandleIRQ(base, handle, FLEXIO_LOGIC_TransferHandleIRQ);
}


void FLEXIO_LOGIC_TransferHandleIRQ(void *logicType, void *logicHandle)
{


}



/*!
 * brief Resets the FLEXIO_CAMERA shifer and timer config.
 *
 * note After calling this API, call FLEXO_CAMERA_Init to use the FlexIO Camera module.
 *
 * param base Pointer to FLEXIO_CAMERA_Type structure
 */
void FLEXIO_LOGIC_Deinit(FLEXIO_LOGIC_Type *base)
{
    base->flexioBase->SHIFTCFG[base->shifterStartIdx] = 0;
    base->flexioBase->SHIFTCTL[base->shifterStartIdx] = 0;

    /* Clear the shifter flag. */
    base->flexioBase->SHIFTSTAT = (1U << base->shifterStartIdx);
    /* Clear the timer flag. */
    for(int i; i<3; i++){			// hardwire this at this point
		base->flexioBase->TIMCFG[base->timerIdx[i]]          = 0;
		base->flexioBase->TIMCMP[base->timerIdx[i]]          = 0;
		base->flexioBase->TIMCTL[base->timerIdx[i]]          = 0;
		base->flexioBase->TIMSTAT = (1U << base->timerIdx[i]);
    }
}

/*!
 * brief Gets the FlexIO Camera status flags.
 *
 * param base Pointer to FLEXIO_CAMERA_Type structure
 * return FlexIO shifter status flags
 *          arg FLEXIO_SHIFTSTAT_SSF_MASK
 *          arg 0
 */
uint32_t FLEXIO_LOGIC_GetStatusFlags(FLEXIO_LOGIC_Type *base)
{
    uint32_t status = 0;
    status          = ((FLEXIO_GetShifterStatusFlags(base->flexioBase) >> (base->shifterStartIdx)) &
              ((1U << (base->shifterCount)) - 1U));
    return status;
}

/*!
 * brief Clears the receive buffer full flag manually.
 *
 * param base Pointer to the device.
 * param mask status flag
 *      The parameter can be any combination of the following values:
 *          arg kFLEXIO_CAMERA_RxDataRegFullFlag
 *          arg kFLEXIO_CAMERA_RxErrorFlag
 */
void FLEXIO_LOGIC_ClearStatusFlags(FLEXIO_LOGIC_Type *base, uint32_t mask)
{
    if (mask & kFLEXIO_LOGIC_RxDataRegFullFlag)
    {
        FLEXIO_ClearShifterStatusFlags(base->flexioBase, ((1U << (base->shifterCount)) - 1U)
                                                             << (base->shifterStartIdx));
    }
    if (mask & kFLEXIO_LOGIC_RxErrorFlag)
    { /* Clear error flags if they are asserted to make sure the buffer would be available. */
        FLEXIO_ClearShifterErrorFlags(base->flexioBase, ((1U << (base->shifterCount)) - 1U) << (base->shifterStartIdx));
    }
}

/*!
 * brief Switches on the interrupt for receive buffer full event.
 *
 * param base Pointer to the device.
 */
void FLEXIO_LOGIC_EnableInterrupt(FLEXIO_LOGIC_Type *base)
{
    FLEXIO_EnableShifterStatusInterrupts(base->flexioBase, 1U << (base->shifterStartIdx));
}

/*!
 * brief Switches off the interrupt for receive buffer full event.
 *
 * param base Pointer to the device.
 *
 */
void FLEXIO_LOGIC_DisableInterrupt(FLEXIO_LOGIC_Type *base)
{
    FLEXIO_DisableShifterStatusInterrupts(base->flexioBase, 1U << (base->shifterStartIdx));
}
