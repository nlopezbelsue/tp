################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/utilidades.c \
../src/utilsClient.c \
../src/utilsServer.c 

C_DEPS += \
./src/utilidades.d \
./src/utilsClient.d \
./src/utilsServer.d 

OBJS += \
./src/utilidades.o \
./src/utilsClient.o \
./src/utilsServer.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/utilidades.d ./src/utilidades.o ./src/utilsClient.d ./src/utilsClient.o ./src/utilsServer.d ./src/utilsServer.o

.PHONY: clean-src

