################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../gossiping.c \
../inotify.c \
../parser.c \
../shared.c 

OBJS += \
./gossiping.o \
./inotify.o \
./parser.o \
./shared.o 

C_DEPS += \
./gossiping.d \
./inotify.d \
./parser.d \
./shared.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


