KERNELDIR ?= /lib/modules/$(shell uname -r)/build  # Kernel directory
PWD := $(shell pwd)  # Current working directory
DEVICE_PATH := /dev/specific # device path

# Kernel module and task scheduler source files
obj-m := specific.o

# Default target: Build both kernel module and task scheduler
all: kernel thread install create

# Build kernel module
kernel:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

# Build threads 
thread:
	g++ threads.cpp -lpthread -o threads

# Install kernel module, create device file
install:
	# Install the kernel module
	sudo insmod specific.ko

# Create device file
create:
	# Create the device file using mknod if it doesn't exist
	@if [ ! -e "$(DEVICE_PATH)" ]; then \
		MAJOR=$(shell grep -i 'specific' /proc/devices | awk '{print $$1}'); \
		MINOR=0; \
		if [ -z "$$MAJOR" ]; then \
			echo "Error: Major number for device not found. Check device in /proc/devices"; \
			exit 1; \
		fi; \
		sudo mknod $(DEVICE_PATH) c $$MAJOR $$MINOR; \
		echo "Device created at $(DEVICE_PATH) with major $$MAJOR and minor $$MINOR"; \
	else \
		echo "Device file $(DEVICE_PATH) already exists."; \
	fi

# Remove kernel module and remove the device file
remove:
	# Remove the kernel module
	sudo rmmod specific
	# Remove the device file
	sudo rm -f $(DEVICE_PATH)

# Clean up build files and device files
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -f threads
	# Remove kernel module binary
	rm -f specific.ko

# Declare phony targets
.PHONY: all kernel thread install create remove clean