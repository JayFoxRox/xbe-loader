XBE_TITLE = xbe-loader
GEN_XISO = $(XBE_TITLE).iso
SRCS = $(CURDIR)/main.c $(CURDIR)/xiso_driver.c $(CURDIR)/network.c $(CURDIR)/http_client.c $(CURDIR)/write_log.c
NXDK_NET=y

include $(NXDK_DIR)/Makefile
