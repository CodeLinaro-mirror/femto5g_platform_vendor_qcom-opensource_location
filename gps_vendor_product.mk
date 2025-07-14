# vendor opensource packages
ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)

PRODUCT_PACKAGES += libloc_api_v02
PRODUCT_PACKAGES += libgnsspps
PRODUCT_PACKAGES += libsynergy_loc_api
PRODUCT_PACKAGES += libloc_socket
PRODUCT_PACKAGES += libloc_base_util

ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
PRODUCT_PACKAGES += liblocation_api_msg
PRODUCT_PACKAGES += liblocation_integration_api
PRODUCT_PACKAGES += liblocation_client_api
endif
endif#BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE

ifeq ($(GPS_LOWI_ONLY_BUILD),true)
PRODUCT_PACKAGES += libloc_base_util
endif
