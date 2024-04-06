################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/comunicacion.c \
../src/consola.c \
../src/generarPCB.c \
../src/kernel.c \
../src/leerConfigKernel.c \
../src/pcb.c \
../src/planificador.c 

C_DEPS += \
./src/comunicacion.d \
./src/consola.d \
./src/generarPCB.d \
./src/kernel.d \
./src/leerConfigKernel.d \
./src/pcb.d \
./src/planificador.d 

OBJS += \
./src/comunicacion.o \
./src/consola.o \
./src/generarPCB.o \
./src/kernel.o \
./src/leerConfigKernel.o \
./src/pcb.o \
./src/planificador.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/comunicacion.d ./src/comunicacion.o ./src/consola.d ./src/consola.o ./src/generarPCB.d ./src/generarPCB.o ./src/kernel.d ./src/kernel.o ./src/leerConfigKernel.d ./src/leerConfigKernel.o ./src/pcb.d ./src/pcb.o ./src/planificador.d ./src/planificador.o

.PHONY: clean-src

