################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Swipe/Src/swipe.c 

OBJS += \
./Swipe/Src/swipe.o 

C_DEPS += \
./Swipe/Src/swipe.d 


# Each subdirectory must supply rules for building sources it contributes
Swipe/Src/%.o: ../Swipe/Src/%.c Swipe/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../TOUCHSENSING/App -I../Middlewares/ST/STM32_TouchSensing_Library/inc -I"C:/Users/Leha/STM32CubeIDE/workspace_1.8.0/Swipe_App/Swipe/Inc" -I"C:/Users/Leha/STM32CubeIDE/workspace_1.8.0/Swipe_App/Util/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Swipe-2f-Src

clean-Swipe-2f-Src:
	-$(RM) ./Swipe/Src/swipe.d ./Swipe/Src/swipe.o

.PHONY: clean-Swipe-2f-Src

