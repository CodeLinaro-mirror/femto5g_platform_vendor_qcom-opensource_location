# vendor opensource packages
ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)

PRODUCT_PACKAGES += libloc_api_v02
PRODUCT_PACKAGES += libgnsspps
PRODUCT_PACKAGES += libsynergy_loc_api
PRODUCT_PACKAGES += libloc_socket

endif#BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
