################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Rfid/MF522.c 

OBJS += \
./Rfid/MF522.o 

C_DEPS += \
./Rfid/MF522.d 


# Each subdirectory must supply rules for building sources it contributes
Rfid/%.o: ../Rfid/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	/home/eric/x-tools/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-gcc -I/home/eric/x-tools/arm-unknown-linux-gnueabi/arm-unknown-linux-gnueabi/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


