################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/comunicacion.c \
../src/fileSystem.c \
../src/init.c \
../src/leerConfigFS.c \
../src/limpiarFs.c \
../src/loggers_config_fs.c \
../src/main.c \
../src/operacionesFs.c 

C_DEPS += \
./src/comunicacion.d \
./src/fileSystem.d \
./src/init.d \
./src/leerConfigFS.d \
./src/limpiarFs.d \
./src/loggers_config_fs.d \
./src/main.d \
./src/operacionesFs.d 

OBJS += \
./src/comunicacion.o \
./src/fileSystem.o \
./src/init.o \
./src/leerConfigFS.o \
./src/limpiarFs.o \
./src/loggers_config_fs.o \
./src/main.o \
./src/operacionesFs.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2023-2c-Operaneta/Cliente-Servidor/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/comunicacion.d ./src/comunicacion.o ./src/fileSystem.d ./src/fileSystem.o ./src/init.d ./src/init.o ./src/leerConfigFS.d ./src/leerConfigFS.o ./src/limpiarFs.d ./src/limpiarFs.o ./src/loggers_config_fs.d ./src/loggers_config_fs.o ./src/main.d ./src/main.o ./src/operacionesFs.d ./src/operacionesFs.o

.PHONY: clean-src

