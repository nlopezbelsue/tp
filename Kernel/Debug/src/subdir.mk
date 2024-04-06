################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/comunicacion.c \
../src/consola.c \
../src/deadlock.c \
../src/ejecuciones.c \
../src/init.c \
../src/leerConfigKernel.c \
../src/liberarKernel.c \
../src/main.c \
../src/pcb.c \
../src/planificador.c 

C_DEPS += \
./src/comunicacion.d \
./src/consola.d \
./src/deadlock.d \
./src/ejecuciones.d \
./src/init.d \
./src/leerConfigKernel.d \
./src/liberarKernel.d \
./src/main.d \
./src/pcb.d \
./src/planificador.d 

OBJS += \
./src/comunicacion.o \
./src/consola.o \
./src/deadlock.o \
./src/ejecuciones.o \
./src/init.o \
./src/leerConfigKernel.o \
./src/liberarKernel.o \
./src/main.o \
./src/pcb.o \
./src/planificador.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2023-2c-Operaneta/Cliente-Servidor/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/comunicacion.d ./src/comunicacion.o ./src/consola.d ./src/consola.o ./src/deadlock.d ./src/deadlock.o ./src/ejecuciones.d ./src/ejecuciones.o ./src/init.d ./src/init.o ./src/leerConfigKernel.d ./src/leerConfigKernel.o ./src/liberarKernel.d ./src/liberarKernel.o ./src/main.d ./src/main.o ./src/pcb.d ./src/pcb.o ./src/planificador.d ./src/planificador.o

.PHONY: clean-src

