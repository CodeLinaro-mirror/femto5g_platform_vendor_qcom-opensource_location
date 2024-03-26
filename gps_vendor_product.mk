# vendor opensource packages
ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)

PRODUCT_PACKAGES += libloc_api_v02
PRODUCT_PACKAGES += libgnsspps
PRODUCT_PACKAGES += libsynergy_loc_api
PRODUCT_PACKAGES += izat_remote_api_headers
PRODUCT_PACKAGES += loc_sll_if_headers
PRODUCT_PACKAGES += libloc_socket

PRODUCT_PACKAGES += liblocClient_fidl_if_headers
PRODUCT_PACKAGES += liblocApiV02_fidl_headers
PRODUCT_PACKAGES += libfidl_loc_api
PRODUCT_PACKAGES += libloc_fidl_core
PRODUCT_PACKAGES += libloc_fidl_sim
PRODUCT_PACKAGES += libLocationFidlV02
PRODUCT_PACKAGES += locIdlClient_daemon

PRODUCT_PACKAGES += liblocation_api_msg
PRODUCT_PACKAGES += liblocation_integration_api
PRODUCT_PACKAGES += liblocation_client_api

PRODUCT_PACKAGES += commonapi4someip.ini
PRODUCT_PACKAGES += vsomeip-client.json
PRODUCT_PACKAGES += libLocIdlAPI-proxy
PRODUCT_PACKAGES += libLocIdlAPI-someip
PRODUCT_PACKAGES += LocIdlAPIClient
endif#BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
