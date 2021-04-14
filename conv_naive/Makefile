APP = convolution
APP_SRCS += conv_kernel.c convolution.c
APP_CFLAGS += -I.
APP_CFLAGS += -O3 -g

# if uncommented, the next line disables HW loops
#APP_CFLAGS += -mnohwloop
# if uncommented, the next line disables post-increments
#APP_CFLAGS += -mnopostmod
# if uncommented, the next line disables the MAC instruction
# APP_CFLAGS += -mnomac


include $(RULES_DIR)/pmsis_rules.mk
