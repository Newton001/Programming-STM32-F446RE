################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SDIO_UART/Src/FileHandle.c \
../SDIO_UART/Src/UartBuffer.c 

OBJS += \
./SDIO_UART/Src/FileHandle.o \
./SDIO_UART/Src/UartBuffer.o 

C_DEPS += \
./SDIO_UART/Src/FileHandle.d \
./SDIO_UART/Src/UartBuffer.d 


# Each subdirectory must supply rules for building sources it contributes
SDIO_UART/Src/%.o SDIO_UART/Src/%.su SDIO_UART/Src/%.cyclo: ../SDIO_UART/Src/%.c SDIO_UART/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/newto/STM32CubeIDE/workspace_1.16.0/SD_CARD_READER/SDIO_UART/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SDIO_UART-2f-Src

clean-SDIO_UART-2f-Src:
	-$(RM) ./SDIO_UART/Src/FileHandle.cyclo ./SDIO_UART/Src/FileHandle.d ./SDIO_UART/Src/FileHandle.o ./SDIO_UART/Src/FileHandle.su ./SDIO_UART/Src/UartBuffer.cyclo ./SDIO_UART/Src/UartBuffer.d ./SDIO_UART/Src/UartBuffer.o ./SDIO_UART/Src/UartBuffer.su

.PHONY: clean-SDIO_UART-2f-Src

