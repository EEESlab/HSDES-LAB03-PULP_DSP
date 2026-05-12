APP = test
APP_SRCS += test.c
APP_CFLAGS += -I.
APP_CFLAGS += -O3 -g

# APP_CFLAGS += -mnohwloop

ifdef VERBOSE_PERF
APP_CFLAGS += -DSTATS
endif


include $(RULES_DIR)/pmsis_rules.mk
