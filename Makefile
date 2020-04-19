PULP_APP = convolution
PULP_APP_SRCS = conv_kernel.c convolution.c

PULP_CFLAGS += -O3

include $(PULP_SDK_HOME)/install/rules/pulp.mk
