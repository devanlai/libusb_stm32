/* This file is the part of the Lightweight USB device Stack for STM32 microcontrollers
 *
 * Copyright ©2016 Dmitry Filimonchuk <dmitrystu[at]gmail[dot]com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
.*/

#ifndef _USBD_CORE_H_
#define _USBD_CORE_H_
#if defined(__cplusplus)
    extern "C" {
#endif

/**\addtogroup USBD_HW_CORE USB Device HW driver and core API
 * @{ */

/**\addtogroup USBD_HW USB hardware driver
 * \brief Contains HW driver API 
 * @{ */
/**\anchor USB_EVENTS
 * \name USB device events
 * @{ */
#define usbd_evt_reset      0   /**<\brief Reset.*/
#define usbd_evt_sof        1   /**<\brief Start of frame.*/
#define usbd_evt_susp       2   /**<\brief Suspend.*/
#define usbd_evt_wkup       3   /**<\brief Wakeup.*/
#define usbd_evt_eptx       4   /**<\brief Data packet transmitted*/
#define usbd_evt_eprx       5   /**<\brief Data packet received.*/
#define usbd_evt_epsetup    6   /**<\brief Setup packet received.*/
#define usbd_evt_error      7   /**<\brief Data error.*/
#define usbd_evt_esof       8   /**<\brief Missed SOF.*/
#define usbd_evt_count      9
/** @} */

/**\anchor USB_LANES_STATUS
 * \name USB lanes connection states
 * @{ */
#define usbd_lane_unk       0   /**<\brief Unknown or proprietary charger.*/
#define usbd_lane_dsc       1   /**<\brief Lanes disconnected.*/
#define usbd_lane_sdp       2   /**<\brief Lanes connected to standard downstream port.*/
#define usbd_lane_cdp       3   /**<\brief Lanes connected to charging downstream port.*/
#define usbd_lane_dcp       4   /**<\brief Lanes connected to dedicated charging port.*/
/** @} */

/** \name USB HW capabilities
 * @{ */
#define USBD_HW_ADDRFST     (1 << 0)    /**<\brief Set address before STATUS_OUT.*/
#define USBD_HW_BC          (1 << 1)    /**<\brief Battery charging detection supported.*/
/** @} */
/** @} */

/**\addtogroup USBD_CORE USB device core
 * \brief Contains core API 
 * @{ */
#define USB_EPTYPE_DBLBUF   0x04    /**<\brief Doublebuffered endpoint (bulk endpoint only).*/

/**\name bmRequestType bitmapped field
 * @{ */
#define USB_REQ_DIRECTION   (1 << 7)    /**<\brief Request direction mask.*/
#define USB_REQ_HOSTTODEV   (0 << 7)    /**<\brief Request direction is HOST to DEVICE.*/
#define USB_REQ_DEVTOHOST   (1 << 7)    /**<\brief Request direction is DEVICE to HOST.*/
#define USB_REQ_TYPE        (3 << 5)    /**<\brief Request type mask.*/
#define USB_REQ_STANDARD    (0 << 5)    /**<\brief Standard request.*/
#define USB_REQ_CLASS       (1 << 5)    /**<\brief Class specified request.*/
#define USB_REQ_VENDOR      (2 << 5)    /**<\brief Vendor specified request.*/
#define USB_REQ_RECIPIENT   (3 << 0)    /**<\brief Request recipient mask.*/
#define USB_REQ_DEVICE      (0 << 0)    /**<\brief Request to device.*/
#define USB_REQ_INTERFACE   (1 << 0)    /**<\brief Request to interface.*/
#define USB_REQ_ENDPOINT    (2 << 0)    /**<\brief Request to endpoint.*/
#define USB_REQ_OTHER       (3 << 0)    /**<\brief Other request.*/
/** @} */


#if !defined(__ASSEMBLER__)
/**\brief USB device machine states.*/
enum usbd_machine_state {
    usbd_state_disabled,
    usbd_state_disconnected,
    usbd_state_default,         /**< Default.*/
    usbd_state_addressed,       /**< Addressed.*/
    usbd_state_configured,      /**< Configured.*/
};

/**\brief USB device control endpoint machine state.*/
enum usbd_ctl_state {
    usbd_ctl_idle,              /**<\brief Idle stage. Awaiting for SETUP packet.*/
    usbd_ctl_rxdata,            /**<\brief RX stage. Receiving DATA-OUT payload.*/
    usbd_ctl_txdata,            /**<\brief TX stage. Transmitting DATA-IN payload.*/
    usbd_ctl_ztxdata,           /**<\brief TX stage. Transmitting DATA-IN payload. Zero length
                                 * packet maybe required..*/
    usbd_ctl_lastdata,          /**<\brief TX stage. Last DATA-IN packed passed to buffer. Awaiting
                                 * for the TX completion.*/
    usbd_ctl_statusin,          /**<\brief STATUS-IN stage.*/
    usbd_ctl_statusout,         /**<\brief STATUS-OUT stage.*/
};

/**\brief Asynchronous device control commands.*/
enum usbd_commands {
    usbd_cmd_enable,            /**<\brief Enables device.*/
    usbd_cmd_disable,           /**<\brief Disables device.*/
    usbd_cmd_connect,           /**<\brief Connects device to host.*/
    usbd_cmd_disconnect,        /**<\brief Disconnects device from host.*/
    usbd_cmd_reset,             /**<\brief Resets device.*/
};

/**\brief Reporting status results.*/
typedef enum _usbd_respond {
    usbd_fail,                  /**<\brief Function has an error, STALLPID will be issued.*/
    usbd_ack,                   /**<\brief Function completes request accepted ZLP or data will be send.*/
    usbd_nak,                   /**<\brief Function is busy. NAK handshake.*/
} usbd_respond;

typedef struct _usbd_device usbd_device;

/**\brief Represents generic USB control request.*/
typedef struct {
    uint8_t     bmRequestType;  /**<\brief This bitmapped field identifies the characteristics of
                                 * the specific request.*/
    uint8_t     bRequest;       /**<\brief This field specifies the particular request.*/
    uint16_t    wValue;         /**<\brief It is used to pass a parameter to the device, specific to
                                 * the request.*/
    uint16_t    wIndex;         /**<\brief It is used to pass a parameter to the device, specific to
                                 * the request.*/
    uint16_t    wLength;        /**<\brief This field specifies the length of the data transferred
                                 * during the second phase of the control transfer.*/
    uint8_t     data[];         /**<\brief Data payload.*/
} usbd_ctlreq;

/** USB device status data.*/
typedef struct {
    void        *data_buf;      /**<\brief Pointer to data buffer used for control requests.*/
    void        *data_ptr;      /**<\brief Pointer to current data for control request.*/
    uint16_t    data_count;     /**<\brief Count remained data for control request.*/
    uint16_t    data_maxsize;   /**<\brief Size of the data buffer for control requests.*/
    uint8_t     ep0size;        /**<\brief Size of the control endpoint.*/
    uint8_t     device_cfg;     /**<\brief Current device configuration number.*/
    uint8_t     device_state;   /**<\brief Current \ref usbd_machine_state.*/
    uint8_t     control_state;  /**<\brief Current \ref usbd_ctl_state.*/
} usbd_status;

/**\brief Generic USB device event callback for events and endpoints processing
  * \param[in] dev pointer to USB device
  * \param event \ref USB_EVENTS "USB event"
  * \param ep active endpoint number
  * \note endpoints with same indexes i.e. 0x01 and 0x81 shares same callback.
  */
typedef void (*usbd_evt_callback)(usbd_device *dev, uint8_t event, uint8_t ep);

/**\brief USB control transfer completed callback function.
 * \param[in] dev pointer to USB device
 * \param[in] req pointer to usb request structure
 * \note usbd_device->complete_callback will be set to NULL after this callback completion.
 */
typedef void (*usbd_rqc_callback)(usbd_device *dev, usbd_ctlreq *req);

/**\brief USB control callback function.
 * \details Uses for the control request processing.
 *          Some requests will be handled by core if callback don't process it (returns FALSE).
 *            If request was not processed STALL PID will be issued.
 *          - GET_CONFIGURATION
 *          - SET_CONFIGURATION (passes to \ref usbd_cfg_callback)
 *          - GET_DESCRIPTOR (passes to \ref usbd_dsc_callback)
 *          - GET_STATUS
 *          - SET_FEATURE, CLEAR_FEATURE (endpoints only)
 *          - SET_ADDRESS
 * \param[in] dev points to USB device
 * \param[in] req points to usb control request
 * \param[out] *callback USB control transfer completion callback, default is NULL (no callback)
 * \return usbd_respond status.
 */
typedef usbd_respond (*usbd_ctl_callback)(usbd_device *dev, usbd_ctlreq *req, usbd_rqc_callback *callback);

/**\brief USB get descriptor callback function
 * \details Called when GET_DESCRIPTOR request issued
 * \param[in] req pointer to usb control request structure
 * \param[in,out] address pointer to the descriptor in memory. Points to req->data by default. You 
 * can use this buffer.
 * \param[in,out] dsize descriptor size. maximum buffer size by default.
 * \return usbd_ack if you passed the correct descriptor, usbd_fail otherwise.
 */
typedef usbd_respond (*usbd_dsc_callback)(usbd_ctlreq *req, void **address, uint16_t *dsize);

/**\brief USB set configuration callback function
 * \details called when SET_CONFIGURATION request issued
 * \param[in] dev pointer to USB device
 * \param[in] cfg configuration number.
 * \note if config is 0 device endpoints should be de-configured
 * \return TRUE if success
 */
typedef usbd_respond (*usbd_cfg_callback)(usbd_device *dev, uint8_t cfg);

/** @} */

/**\addtogroup USBD_HW
 * @{ */

/**\brief Enables or disables USB hardware
 * \param enable Enables USB when TRUE disables otherwise.
 */
typedef void (*usbd_hw_enable)(bool enable);

/**\brief Resets USB hardware.*/
typedef void (*usbd_hw_reset)(void);

/** Connects or disconnects USB hardware to/from usb host
 * \param connect Connects USB to host if TRUE, disconnects otherwise
 * \return lanes connection status.
 */
typedef uint8_t (*usbd_hw_connect)(bool connect);

/**\brief Sets USB hardware address
 * \param address USB address
 */
typedef void (*usbd_hw_setaddr)(uint8_t address);

/**\brief Configures endpoint
 * \param ep endpoint address. Use USB_EPDIR_ macros to set endpoint direction
 * \param eptype endpoint type. Use USB_EPTYPE_* macros.
 * \param epsize endpoint size in bytes
 * \return TRUE if success
 */
typedef bool (*usbd_hw_ep_config)(uint8_t ep, uint8_t eptype, uint16_t epsize);

/**\brief De-configures, cleans and disables endpoint
 * \param ep endpoint index
 * \note if you have two one-direction single-buffered endpoints with same index (i.e. 0x02 and 0x82)
 * both will be deconfigured.
 */
typedef void (*usbd_hw_ep_deconfig)(uint8_t ep);

/**\brief Reads data from OUT or control endpoint
 * \param ep endpoint index, should belong to OUT or CONTROL endpoint.
 * \param buf pointer to read buffer
 * \param blen size of the read buffer in bytes
 * \return size of the actually received data, -1 on error.
 */
typedef int32_t (*usbd_hw_ep_read)(uint8_t ep, void *buf, uint16_t blen);

/**\brief Writes data to IN or control endpoint
 * \param ep endpoint index, hould belong to IN or CONTROL endpoint
 * \param buf pointer to data buffer
 * \param blen size of data will be written
 * \return number of written bytes
 */
typedef int32_t (*usbd_hw_ep_write)(uint8_t ep, void *buf, uint16_t blen);

/** Stalls and unstalls endpoint
 * \param ep endpoint address
 * \param stall endpoint will be stalled if TRUE and unstalled otherwise.
 * \note Has no effect on inactive endpoints.
 */
typedef void (*usbd_hw_ep_setstall)(uint8_t ep, bool stall);

/**\brief Checks endpoint for stalled state
 * \param ep endpoint address
 * \return TRUE if endpoint is stalled
 */
typedef bool (*usbd_hw_ep_isstalled)(uint8_t ep);

/**\brief Polls USB hardware for the events
 * \param[in] dev pointer to usb device structure
 * \param callback callback to event processing subroutine
 */
typedef void (*usbd_hw_poll)(usbd_device *dev, usbd_evt_callback callback);

/**\brief Gets frame number from usb hardware.*/
typedef uint16_t (*usbd_hw_get_frameno)(void);


/**\brief Makes a string descriptor contains unique serial number from hardware ID's
 * \param[in] buffer pointer to buffer for the descriptor
 * \return of the descriptor in bytes
 */
typedef uint16_t (*usbd_hw_get_serialno)(void *buffer);

/**\brief Represents a hardware USB driver call table.*/
struct usbd_driver {
    uint32_t                caps;               /**<\brief HW capabilities */
    usbd_hw_enable          enable;             /**<\copybrief usbd_hw_enable */
    usbd_hw_reset           reset;              /**<\copybrief usbd_hw_reset */
    usbd_hw_connect         connect;            /**<\copybrief usbd_hw_connect */
    usbd_hw_setaddr         setaddr;            /**<\copybrief usbd_hw_setaddr */
    usbd_hw_ep_config       ep_config;          /**<\copybrief usbd_hw_ep_config */
    usbd_hw_ep_deconfig     ep_deconfig;        /**<\copybrief usbd_hw_ep_deconfig */
    usbd_hw_ep_read         ep_read;            /**<\copybrief usbd_hw_ep_read */
    usbd_hw_ep_write        ep_write;           /**<\copybrief usbd_hw_ep_write */
    usbd_hw_ep_setstall     ep_setstall;        /**<\copybrief usbd_hw_ep_setstall */
    usbd_hw_ep_isstalled    ep_isstalled;       /**<\copybrief usbd_hw_ep_isstalled */
    usbd_hw_poll            poll;               /**<\copybrief usbd_hw_poll */
    usbd_hw_get_frameno     frame_no;           /**<\copybrief usbd_hw_get_frameno */
    usbd_hw_get_serialno    get_serialno_desc;  /**<\copybrief usbd_hw_get_serialno */
};

/** @} */

/**\addtogroup USBD_CORE
 * @{ */

/**\brief Represents a USB device data.*/
struct _usbd_device {
    const struct usbd_driver    *driver;                /**<\copybrief usbd_driver */
    usbd_ctl_callback           control_callback;       /**<\copybrief usbd_ctl_callback */
    usbd_rqc_callback           complete_callback;      /**<\copybrief usbd_rqc_callback */
    usbd_cfg_callback           config_callback;        /**<\copybrief usbd_cfg_callback */
    usbd_dsc_callback           descriptor_callback;    /**<\copybrief usbd_dsc_callback */
    usbd_evt_callback           events[usbd_evt_count]; /**<\brief array of the event callbacks.*/
    usbd_evt_callback           endpoint[8];            /**<\brief array of the endpoint callbacks.*/
    usbd_status                 status;                 /**<\copybrief usbd_status */
};

/**\brief Initializes device structure
 * \param dev USB device that will be initialized
 * \param drv Pointer to hardware driver
 * \param ep0size Control endpoint 0 size
 * \param buffer Pointer to control request data buffer (32-bit aligned)
 * \param bsize Size of the data buffer
 */
inline static void usbd_init(usbd_device *dev, const struct usbd_driver *drv,
                             const uint8_t ep0size, uint32_t *buffer, const uint16_t bsize) {
    dev->driver = drv;
    dev->status.ep0size = ep0size;
    dev->status.data_ptr = buffer;
    dev->status.data_buf = buffer;
    dev->status.data_maxsize = bsize - __builtin_offsetof(usbd_ctlreq, data);
}

/**\brief Polls USB for events
 * \param dev Pointer to device structure
 * \note can be called as from main routine as from USB interrupt
 */
void usbd_poll(usbd_device *dev);

/**\brief Asynchronous device control
 * \param dev dev usb device \ref _usbd_device
 * \param cmd Asynchronous control command
 */
void usbd_control(usbd_device *dev, enum usbd_commands cmd) __attribute__((deprecated));

/**\brief Register callback for all control requests
 * \param dev usb device \ref _usbd_device
 * \param callback user control callback \ref usbd_ctl_callback
 */
inline static void usbd_reg_control(usbd_device *dev, usbd_ctl_callback callback) {
    dev->control_callback = callback;
}

/**\brief Register callback for SET_CONFIG control request
 * \param dev dev usb device \ref _usbd_device
 * \param callback pointer to user \ref usbd_cfg_callback
 */
inline static void usbd_reg_config(usbd_device *dev, usbd_cfg_callback callback) {
    dev->config_callback = callback;
}

/**\brief Register callback for GET_DESCRIPTOR control request
 * \param dev dev usb device \ref _usbd_device
 * \param callback pointer to user \ref usbd_ctl_callback
 */
inline static void usbd_reg_descr(usbd_device *dev, usbd_dsc_callback callback) {
    dev->descriptor_callback = callback;
}

/**\brief Configure endpoint
 * \param dev dev usb device \ref _usbd_device
 * \copydetails usbd_hw_ep_config
 */
inline static bool usbd_ep_config(usbd_device *dev, uint8_t ep, uint8_t eptype, uint16_t epsize) {
    return dev->driver->ep_config(ep, eptype, epsize);
}

/**\brief Deconfigure endpoint
 * \param dev dev usb device \ref _usbd_device
 * \copydetails usbd_hw_ep_deconfig
 */
inline static void usbd_ep_deconfig(usbd_device *dev, uint8_t ep) {
    dev->driver->ep_deconfig(ep);
}

/**\brief Register endpoint callback
 * \param dev dev usb device \ref _usbd_device
 * \param ep endpoint index
 * \param callback pointer to user \ref usbd_evt_callback callback for endpoint events
 */
inline static void usbd_reg_endpoint(usbd_device *dev, uint8_t ep, usbd_evt_callback callback) {
    dev->endpoint[ep & 0x07] = callback;
}

/**\brief Registers event callback
 * \param dev dev usb device \ref _usbd_device
 * \param evt device \ref USB_EVENTS "event" wants to be registered
 * \param callback pointer to user \ref usbd_evt_callback for this event
 */
inline static void usbd_reg_event(usbd_device *dev, uint8_t evt, usbd_evt_callback callback) {
    dev->events[evt] = callback;
}

/**\brief Write data to endpoint
 * \param dev dev usb device \ref _usbd_device
 * \copydetails usbd_hw_ep_write
 */
inline static int32_t usbd_ep_write(usbd_device *dev, uint8_t ep, void *buf, uint16_t blen) {
    return dev->driver->ep_write(ep, buf, blen);
}

/**\brief Read data from endpoint
 * \param dev dev usb device \ref _usbd_device
 * \copydetails usbd_hw_ep_read
 */
inline static int32_t usbd_ep_read(usbd_device *dev, uint8_t ep, void *buf, uint16_t blen) {
    return dev->driver->ep_read(ep, buf, blen);
}

/**\brief Stall endpoint
 * \param dev dev usb device \ref _usbd_device
 * \param ep endpoint address
 */
inline static void usbd_ep_stall(usbd_device *dev, uint8_t ep) {
    dev->driver->ep_setstall(ep, 1);
}

/**\brief Unstall endpoint
 * \param dev dev usb device \ref _usbd_device
 * \param ep endpoint address
 */
inline static void usbd_ep_unstall(usbd_device *dev, uint8_t ep) {
    dev->driver->ep_setstall(ep, 0);
}

/**\brief Enables or disables USB hardware
 * \param dev dev usb device \ref _usbd_device
 * \param enable Enables USB when TRUE disables otherwise
 */
inline static void usbd_enable(usbd_device *dev, bool enable) {
    dev->driver->enable(enable);
}

/**\brief Connects or disconnects USB hardware to/from usb host
 * \param dev dev usb device \ref _usbd_device
 * \param connect Connects USB to host if TRUE, disconnects otherwise
 * \return lanes connection status. \ref USB_LANES_STATUS
 */
inline static uint8_t usbd_connect(usbd_device *dev, bool connect) {
    return dev->driver->connect(connect);
}

#endif //(__ASSEMBLER__)
/** @} */
/** @} */

#if defined(__cplusplus)
    }
#endif
#endif //_USBD_STD_H_
