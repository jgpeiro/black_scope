################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Lib/Tsc/tsc.c 

OBJS += \
./Lib/Tsc/tsc.o 

C_DEPS += \
./Lib/Tsc/tsc.d 


# Each subdirectory must supply rules for building sources it contributes
Lib/Tsc/%.o Lib/Tsc/%.su Lib/Tsc/%.cyclo: ../Lib/Tsc/%.c Lib/Tsc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNK_INCLUDE_STANDARD_VARARGS -DUSE_HAL_DRIVER -DSTM32G473xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Lcd" -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Scope" -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Tsc" -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Framebuf" -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Font" -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Nuklear" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Lib-2f-Tsc

clean-Lib-2f-Tsc:
	-$(RM) ./Lib/Tsc/tsc.cyclo ./Lib/Tsc/tsc.d ./Lib/Tsc/tsc.o ./Lib/Tsc/tsc.su

.PHONY: clean-Lib-2f-Tsc

