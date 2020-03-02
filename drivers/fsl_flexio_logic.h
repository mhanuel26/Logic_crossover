#ifndef _FSL_FLEXIO_LOGIC_H_
#define _FSL_FLEXIO_LOGIC_H_

#include "fsl_common.h"
#include "fsl_flexio.h"
/*!
 * @addtogroup flexio_camera
 * @{
 */

#define DEMO_SAMPLES 	4096

typedef uint16_t FrameBuffer_t[DEMO_SAMPLES];
typedef uint16_t (*pFrameBuffer_t)[DEMO_SAMPLES];

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_TIME_DELAY_FOR_DUTY_CYCLE_UPDATE (2000000U)
#define DEMO_FLEXIO_BASEADDR FLEXIO1
#define DEMO_FLEXIO_OUTPUTPIN (26U) /* Select FLEXIO1_FLEXIO05 as PWM output */
#define DEMO_FLEXIO_TIMER_CH (0U)   /* Flexio timer0 used */

/* FlexIO Camera configurations */
#define BOARD_LOGIC_FLEXIO_INST                    FLEXIO1
// using two pins for now as there are almost no options for parallel interface on this board
// using GPIO8 and GPIO9
#define BOARD_LOGIC_FLEXIO_DATA_PIN_START_INDEX    0	//FLEXIO1_IO00 : FLEXIO1_IO(0 + FLEXIO_LOGIC_PARALLEL_DATA_WIDTH - 1)
// FLEXIO1_ IO03 uses GPIO_11 which can be connected safely to J54 header on EVK (Green D25 LED connected as well)
#define BOARD_LOGIC_FLEXIO_SCLK_PIN_INDEX		   	(3)  // FlEXIO1_IO03
// FLEXIO1_ IO26 uses GPIO_AD_14 which is ADC12_6 signal that is exposed on J26, an arduino connector pin A3
#define BOARD_LOGIC_FLEXIO_TRG_PIN_INDEX			(26U)


/* Select USB1 PLL (480 MHz) as flexio clock source */
#define FLEXIO_CLOCK_SELECT (7U)
/* Clock pre divider for flexio clock source */
#define FLEXIO_CLOCK_PRE_DIVIDER (7U)
/* Clock divider for flexio clock source */
#define FLEXIO_CLOCK_DIVIDER (15U)
#define DEMO_FLEXIO_CLOCK_FREQUENCY \
    (CLOCK_GetFreq(kCLOCK_Usb1PllClk) / (FLEXIO_CLOCK_PRE_DIVIDER + 1U) / (FLEXIO_CLOCK_DIVIDER + 1U))
/* FLEXIO output PWM frequency */
//#define DEMO_FLEXIO_FREQUENCY (48000U)
#define DEMO_FLEXIO_FREQUENCY (20000U)
#define FLEXIO_MAX_FREQUENCY (DEMO_FLEXIO_CLOCK_FREQUENCY / 2U)
#define FLEXIO_MIN_FREQUENCY (DEMO_FLEXIO_CLOCK_FREQUENCY / 256U)




/*! @name Driver version */
/*@{*/
/*! @brief FlexIO LOGIC driver version 0.0.1. */
#define FSL_FLEXIO_LOGIC_DRIVER_VERSION (MAKE_VERSION(0, 0, 1))
/*@}*/

/*! @brief Define the Logic CPI interface is now for EVK 2-bit width. */
// Still a 4-bit shift will take place, such as { 0, 0, FXIO_D[PINSEL+2:PINSEL]}
#define FLEXIO_LOGIC_PARALLEL_DATA_WIDTH (2U)

/*! @brief Error codes for the Camera driver. */
enum _flexio_camera_status
{
    kStatus_FLEXIO_LOGIC_RxBusy = MAKE_STATUS(kStatusGroup_FLEXIO_CAMERA, 0), /*!< Receiver is busy. */
    kStatus_FLEXIO_LOGIC_RxIdle = MAKE_STATUS(kStatusGroup_FLEXIO_CAMERA, 1), /*!< Camera receiver is idle. */
};

/*! @brief Define FlexIO Logic status mask. */
enum _flexio_logic_status_flags
{
    kFLEXIO_LOGIC_RxDataRegFullFlag = 0x1U, /*!< Receive buffer full flag. */
    kFLEXIO_LOGIC_RxErrorFlag       = 0x2U, /*!< Receive buffer error flag. */
};

/*!
 * @brief Define structure of configuring the FlexIO Camera device.
 */
typedef struct _flexio_logic_type
{
    FLEXIO_Type *flexioBase; /*!< FlexIO module base address. */
    uint32_t datPinStartIdx; /*!< First data pin (D0) index for flexio_camera.
                                  Then the successive following FLEXIO_CAMERA_DATA_WIDTH-1 pins
                                  are used as D1-D7.*/
    uint32_t sclkPinIdx;     /*!< Pixel clock pin (PCLK) index for flexio_camera. */
    uint32_t trgPinIdx;     /*!< Horizontal sync pin (HREF) index for flexio_camera. */

    uint32_t shifterStartIdx; /*!< First shifter index used for flexio_camera data FIFO. */
    uint32_t shifterCount;    /*!< The count of shifters that are used as flexio_camera data FIFO. */
    uint32_t timerIdx[3];        /*!< Timer index used for flexio_camera in FlexIO. */
} FLEXIO_LOGIC_Type;

/*! @brief Define FlexIO Logic user configuration structure. */
typedef struct _flexio_logic_config
{
    uint32_t samples;
    uint32_t frequency;
    uint8_t pwidth;
    uint8_t *sDataPtr;
} flexio_logic_config_t;

/*! @brief Define FlexIO Camera transfer structure. */
typedef struct _flexio_logic_transfer
{
    uint32_t dataAddress; /*!< Transfer buffer*/
    uint32_t dataNum;     /*!< Transfer num*/
} flexio_logic_transfer_t;

/* Forward declaration of the handle typedef. */
typedef struct _flexio_logic_handle flexio_logic_handle_t;

/*! @brief FlexIO LOGIC transfer callback function. */
typedef void (*flexio_logic_transfer_callback_t)(FLEXIO_LOGIC_Type *base,
                                                flexio_logic_handle_t *handle,
                                                status_t status,
                                                void *userData);

/*! @brief Define FLEXIO LOGIC handle structure*/
struct _flexio_logic_handle
{
    uint8_t *volatile txData;   /*!< Address of remaining data to send. */
    volatile size_t txDataSize; /*!< Size of the remaining data to send. */
    uint8_t *volatile rxData;   /*!< Address of remaining data to receive. */
    volatile size_t rxDataSize; /*!< Size of the remaining data to receive. */
    size_t txDataSizeAll;       /*!< Total bytes to be sent. */
    size_t rxDataSizeAll;       /*!< Total bytes to be received. */

    uint8_t *rxRingBuffer;              /*!< Start address of the receiver ring buffer. */
    size_t rxRingBufferSize;            /*!< Size of the ring buffer. */
    volatile uint16_t rxRingBufferHead; /*!< Index for the driver to store received data into ring buffer. */
    volatile uint16_t rxRingBufferTail; /*!< Index for the user to get data from the ring buffer. */

    flexio_logic_transfer_callback_t callback; /*!< Callback function. */
    void *userData;                           /*!< UART callback function parameter.*/

    volatile uint8_t sampleState; /*!< RX transfer state */
};


/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus*/

/*!
 * @name Initialization and configuration
 * @{
 */

/*!
 * @brief Ungates the FlexIO clock, resets the FlexIO module, and configures the FlexIO Camera.
 *
 * @param base Pointer to FLEXIO_CAMERA_Type structure
 * @param config Pointer to flexio_camera_config_t structure
 */
void FLEXIO_LOGIC_Init(FLEXIO_LOGIC_Type *base, const flexio_logic_config_t *config);

/*!
 * @brief Resets the FLEXIO_CAMERA shifer and timer config.
 *
 * @note After calling this API, call FLEXO_CAMERA_Init to use the FlexIO Camera module.
 *
 * @param base Pointer to FLEXIO_CAMERA_Type structure
 */
void FLEXIO_LOGIC_Deinit(FLEXIO_LOGIC_Type *base);

/*!
 * @brief Gets the default configuration to configure the FlexIO Camera. The configuration
 * can be used directly for calling the FLEXIO_CAMERA_Init().
 * Example:
   @code
   flexio_camera_config_t config;
   FLEXIO_CAMERA_GetDefaultConfig(&userConfig);
   @endcode
 * @param config Pointer to the flexio_camera_config_t structure
*/
void FLEXIO_LOGIC_GetDefaultConfig(flexio_logic_config_t *config);

/*!
 * @brief Enables/disables the FlexIO Camera module operation.
 *
 * @param base Pointer to the FLEXIO_CAMERA_Type
 * @param enable True to enable, false does not have any effect.
 */
static inline void FLEXIO_LOGIC_Enable(FLEXIO_LOGIC_Type *base, bool enable)
{
    if (enable)
    {
        base->flexioBase->CTRL |= FLEXIO_CTRL_FLEXEN_MASK;
    }else
    {
        base->flexioBase->CTRL &= ~FLEXIO_CTRL_FLEXEN_MASK;
    }
}

/*! @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets the FlexIO Camera status flags.
 *
 * @param base Pointer to FLEXIO_CAMERA_Type structure
 * @return FlexIO shifter status flags
 *          @arg FLEXIO_SHIFTSTAT_SSF_MASK
 *          @arg 0
 */
uint32_t FLEXIO_LOGIC_GetStatusFlags(FLEXIO_LOGIC_Type *base);

/*!
 * @brief Clears the receive buffer full flag manually.
 *
 * @param base Pointer to the device.
 * @param mask status flag
 *      The parameter can be any combination of the following values:
 *          @arg kFLEXIO_CAMERA_RxDataRegFullFlag
 *          @arg kFLEXIO_CAMERA_RxErrorFlag
 */
void FLEXIO_LOGIC_ClearStatusFlags(FLEXIO_LOGIC_Type *base, uint32_t mask);

/* @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Switches on the interrupt for receive buffer full event.
 *
 * @param base Pointer to the device.
 */
void FLEXIO_LOGIC_EnableInterrupt(FLEXIO_LOGIC_Type *base);

/*!
 * @brief Switches off the interrupt for receive buffer full event.
 *
 * @param base Pointer to the device.
 *
 */
void FLEXIO_LOGIC_DisableInterrupt(FLEXIO_LOGIC_Type *base);


void FLEXIO_LOGIC_ReadBlocking(FLEXIO_LOGIC_Type *base, const flexio_logic_config_t *config);


status_t FLEXIO_LOGIC_TransferCreateHandle(FLEXIO_LOGIC_Type *base,
                                          flexio_logic_handle_t *handle,
                                          flexio_logic_transfer_callback_t callback,
                                          void *userData);

void FLEXIO_LOGIC_TransferHandleIRQ(void *logicType, void *logicHandle);


/*! @} */

/*!
 * @name DMA support
 * @{
 */

/*!
 * @brief Enables/disables the FlexIO Camera receive DMA.
 *
 * @param base Pointer to FLEXIO_CAMERA_Type structure
 * @param enable True to enable, false to disable.
 *
 *    The FlexIO Camera mode can't work without the DMA or eDMA support,
 *    Usually, it needs at least two DMA or eDMA channels, one for transferring data from
 *    Camera, such as 0V7670 to FlexIO buffer, another is for transferring data from FlexIO
 *    buffer to LCD.
 *
 */
static inline void FLEXIO_LOGIC_EnableRxDMA(FLEXIO_LOGIC_Type *base, bool enable)
{
    FLEXIO_EnableShifterStatusDMA(base->flexioBase, 1 << base->shifterStartIdx, enable);
}

/*!
 * @brief Gets the data from the receive buffer.
 *
 * @param base Pointer to the device.
 * @return data Pointer to the buffer that keeps the data with count of base->shifterCount .
 */
static inline uint32_t FLEXIO_LOGIC_GetRxBufferAddress(FLEXIO_LOGIC_Type *base)
{
    return FLEXIO_GetShifterBufferAddress(base->flexioBase, kFLEXIO_ShifterBuffer, base->shifterStartIdx);
}

/*! @} */

#if defined(__cplusplus)
}
#endif /*_cplusplus*/

/*@}*/

#endif /*_FSL_FLEXIO_CAMERA_H_*/
