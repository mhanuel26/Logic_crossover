################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_cache.c \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_flexio.c \
../drivers/fsl_flexio_logic.c \
../drivers/fsl_gpio.c \
../drivers/fsl_lpuart.c 

OBJS += \
./drivers/fsl_cache.o \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_flexio.o \
./drivers/fsl_flexio_logic.o \
./drivers/fsl_gpio.o \
./drivers/fsl_lpuart.o 

C_DEPS += \
./drivers/fsl_cache.d \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_flexio.d \
./drivers/fsl_flexio_logic.d \
./drivers/fsl_gpio.d \
./drivers/fsl_lpuart.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MIMXRT1011DAE5A -DCPU_MIMXRT1011DAE5A_cm7 -DDATA_SECTION_IS_CACHEABLE=1 -D_DEBUG=1 -DSDK_DEBUGCONSOLE=1 -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DUSB_STACK_BM -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\board" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\source" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\usb\device\source\ehci" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\usb\include" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\osa" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\usb\device\include" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\usb\device\source" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\usb\phy" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\drivers" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\device" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\CMSIS" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\component\lists" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\component\serial_manager" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\utilities" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\xip" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\component\uart" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\usb\device\class\cdc" -I"C:\Users\iglesim\Documents\MCUXpressoIDE_11.1.0_3209\workspace\evkmimxrt1010_dev_cdc_vcom_bm\usb\device\class" -O0 -fno-common -g3 -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


