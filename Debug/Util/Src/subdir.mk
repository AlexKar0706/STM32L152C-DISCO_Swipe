################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Util/Src/LCD_user.c 

OBJS += \
./Util/Src/LCD_user.o 

C_DEPS += \
./Util/Src/LCD_user.d 


# Each subdirectory must supply rules for building sources it contributes
Util/Src/%.o: ../Util/Src/%.c Util/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I../TOUCHSENSING/App -I../Middlewares/ST/STM32_TouchSensing_Library/inc -I"C:/Users/Leha/STM32CubeIDE/workspace_1.8.0/Swipe_App/Swipe/Inc" -I"C:/Users/Leha/STM32CubeIDE/workspace_1.8.0/Swipe_App/Util/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Util-2f-Src

clean-Util-2f-Src:
	-$(RM) ./Util/Src/LCD_user.d ./Util/Src/LCD_user.o

.PHONY: clean-Util-2f-Src

