################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/comunicacion.c \
../src/fileSystem.c \
../src/leerConfigFS.c \
../src/loggers_config_fs.c \
../src/operacionesFs.c 

C_DEPS += \
./src/comunicacion.d \
./src/fileSystem.d \
./src/leerConfigFS.d \
./src/loggers_config_fs.d \
./src/operacionesFs.d 

OBJS += \
./src/comunicacion.o \
./src/fileSystem.o \
./src/leerConfigFS.o \
./src/loggers_config_fs.o \
./src/operacionesFs.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/comunicacion.d ./src/comunicacion.o ./src/fileSystem.d ./src/fileSystem.o ./src/leerConfigFS.d ./src/leerConfigFS.o ./src/loggers_config_fs.d ./src/loggers_config_fs.o ./src/operacionesFs.d ./src/operacionesFs.o

.PHONY: clean-src

