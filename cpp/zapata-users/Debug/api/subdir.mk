################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../api/users.cpp 

OBJS += \
./api/users.o 

CPP_DEPS += \
./api/users.d 


# Each subdirectory must supply rules for building sources it contributes
api/%.o: ../api/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++0x -I/home/pf/Develop/COOKING/zapata/cpp/zapata-core -I/home/pf/Develop/COOKING/zapata/cpp/zapata-http -I/home/pf/Develop/COOKING/zapata/cpp/zapata-net -I/home/pf/Develop/COOKING/zapata/cpp/zapata-rest -I/home/pf/Develop/COOKING/zapata/cpp/zapata-oauth -I/home/pf/Develop/COOKING/zapata/cpp/zapata-users/ -I/home/pf/Develop/COOKING/zapata/cpp/zapata-users -I/usr/include/zapata -I/home/pf/Develop/COOKING/zapata/cpp/zapata-mongodb -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


