/*
 * Copyright (c) 2011-2021 Qualcomm Innovation Center, Inc.
 * All Rights Reserved
 * Confidential and Proprietary - Qualcomm Innovation Center, Inc.
 *
 * 2011-2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 *
 * Copyright (c) 2008 Atheros Communications Inc.
 * All Rights Reserved.
 *
 */

#ifndef _ATH_STA_IEEE80211_VAR_H
#define _ATH_STA_IEEE80211_VAR_H

/*
 * Emulation layer of net80211 module. In current NDIS driver implementation,
 * it acts as a shim layer between ATH layer and sample driver's STATION layer.
 */
#include <wlan_opts.h>
#include <ieee80211_options.h>
#include <_ieee80211.h>
#include <ieee80211.h>
#include <ieee80211_api.h>
#include <ieee80211_objmgr.h>
#include <wlan_cfg80211_api.h>
#include <ieee80211_crypto.h>
#include <ieee80211_crypto_wep_mbssid.h>
#include <ieee80211_node.h>
#include <ieee80211_proto.h>
#include <ieee80211_power.h>
#include <ieee80211_config.h>
#include <ieee80211_mlme.h>
#include <ieee80211_data.h>
#include <ieee80211_vap.h>
#include <ieee80211_resmgr.h>
#include <ieee80211_acs.h>
#include <ieee80211_extacs.h>
#include <ieee80211_cbs.h>
#include <ieee80211_acl.h>
#include <ieee80211_notify_tx_bcn.h>
#include <ieee80211_vap_ath_info.h>
#include <ieee80211_tsftimer.h>
#include <ieee80211_mlme_app_ie.h>
#include <ieee80211_bsscolor.h>
#include  <ext_ioctl_drv_if.h>

#include <if_athioctl.h>
#include <ieee80211_admctl.h>
#include <ieee80211_rrm_proto.h>
#include <ieee80211_wnm.h>
#include <ieee80211_quiet.h>
#include <asf_print.h>
#include <qdf_lock.h>
#include <qdf_defer.h>
#include <qdf_mem.h>
#include <qdf_hashtable.h>

#include "osif_wrap_private.h"

#include "ieee80211_ioctl.h"

#ifdef ATH_SUPPORT_TxBF
#include <ieee80211_txbf.h>
#endif

#include <qdf_atomic.h>

#if ATH_SW_WOW
#include <ieee80211_wow.h>
#endif
#include <ieee80211_mbo.h>

#include <ieee80211_qcn.h>

#include "wlan_crypto_global_def.h"
#include "wlan_crypto_global_api.h"
#include <wlan_spectral_public_structs.h>

#if ATH_PARAMETER_API
#include <ieee80211_papi.h>
#endif

#include <include/wlan_vdev_mlme.h>
#include <wlan_vdev_mgr_ucfg_api.h>
#include <wlan_cm_public_struct.h>

/*
 * NOTE: Although we keep the names of data structures (ieee80211com
 * and ieee80211vap), each OS will have different implementation of these
 * data structures. Linux/BSD driver will have the full implementation,
 * while in NDIS6.0 driver, this is just a glue between STATION layer
 * and ATH layer. ATH layer should use API's to access ic, iv, and ni,, * instead of referencing the pointer directly.
 */

#include <sys/queue.h>

#define QCA_INTF_IDXSIZE             3     /* Interface index value size */
#define ACL_HASHSIZE                 32    /* max ACL size */
#define IEEE80211_TXPOWER_MAX        100   /* .5 dbM (XXX units?) */
#define IEEE80211_TXPOWER_MIN        0     /* kill radio */

#define IEEE80211_DTIM_MAX           255   /* max DTIM period */
#define IEEE80211_DTIM_MIN           1     /* min DTIM period */
#define IEEE80211_DTIM_DEFAULT       1     /* default DTIM period */
#define IEEE80211_DTIM_DEFAULT_LP_IOT  41  /* 41x default DTIM period */

#define IEEE80211_LINTVAL_MAX        10   /* max listen interval */
#define IEEE80211_LINTVAL_MIN        1    /* min listen interval */
#define IEEE80211_LINTVAL_DEFAULT    1    /* default listen interval */

#define IEEE80211_BINTVAL_MAX        10000 /* max beacon interval (TU's) */
#define IEEE80211_BINTVAL_MIN        10    /* min beacon interval (TU's) */

#if ATH_SUPPORT_AP_WDS_COMBO
#define IEEE80211_BINTVAL_DEFAULT    400   /* default beacon interval (TU's) for 16 vap support */
#else
#define IEEE80211_BINTVAL_DEFAULT    100   /* default beacon interval (TU's) */
#endif

#define IEEE80211_BINTVAL_IWMAX       3500
#define IEEE80211_BINTVAL_IWMIN       40     /* min beacon interval */
#define IEEE80211_BINTVAL_LP_IOT_IWMIN 25    /* min beacon interval for LP IOT */

#define IEEE80211_BINTVAL_LP_IOT_DEFAULT  25
#define IEEE80211_OL_BINTVAL_MINVAL_RANGE1  40
#define IEEE80211_OL_MIN_VAP_MBSSID         1     /*minimum number of VAPs in MBSSID. To set bintval based on number of VAPS*/
#define IEEE80211_BINTVAL_MINVAL_RANGE2     100   /* used to compute the allowed bintval values based on number of vaps*/
#define IEEE80211_BINTVAL_MINVAL_RANGE3     200   /*if vap is <16 used this value as min allowed bintval value*/
#define IEEE80211_BINTVAL_VAPCOUNT1         2     /*if VAP is <=2 then min bintval is 40*/
#define IEEE80211_BINTVAL_VAPCOUNT2         8     /*if VAP is <=4 then min bintval is 100*/
#define IEEE80211_BINTVAL_VAPCOUNT3         16    /*if VAP is <=8 then min bintval is 200*/
#define MBSSID_BINTVAL_CHECK(_bintval, _numvaps, _bmode) \
        (((_numvaps <= IEEE80211_BINTVAL_VAPCOUNT1) && (_bintval >= IEEE80211_OL_BINTVAL_MINVAL_RANGE1)) ||\
        ((_numvaps <= IEEE80211_BINTVAL_VAPCOUNT2) && (_bintval >= IEEE80211_BINTVAL_MINVAL_RANGE2)) || \
        ((_numvaps <= IEEE80211_BINTVAL_VAPCOUNT3) && (_bmode == 0) && (_bintval >= IEEE80211_BINTVAL_MINVAL_RANGE3)) || \
        ((_numvaps <= IEEE80211_BINTVAL_VAPCOUNT3) && (_bmode == 1) && (_bintval >= IEEE80211_BINTVAL_MINVAL_RANGE2))? 1 : 0)
#define LIMIT_BEACON_PERIOD(_intval)                \
    do {                                            \
        if ((_intval) > IEEE80211_BINTVAL_MAX)      \
            (_intval) = IEEE80211_BINTVAL_MAX;      \
        else if ((_intval) < IEEE80211_BINTVAL_MIN) \
            (_intval) = IEEE80211_BINTVAL_MIN;      \
    } while (FALSE)

#define LIMIT_DTIM_PERIOD(_intval)                \
    do {                                            \
        if ((_intval) > IEEE80211_DTIM_MAX)      \
            (_intval) = IEEE80211_DTIM_MAX;      \
        else if ((_intval) < IEEE80211_DTIM_MIN) \
            (_intval) = IEEE80211_DTIM_MIN;      \
    } while (FALSE)

#define LIMIT_LISTEN_INTERVAL(_intval)                \
    do {                                            \
        if ((_intval) > IEEE80211_LINTVAL_MAX)      \
            (_intval) = IEEE80211_LINTVAL_MAX;      \
        else if ((_intval) < IEEE80211_LINTVAL_MIN) \
            (_intval) = IEEE80211_LINTVAL_MIN;      \
    } while (FALSE)

#define UP_CONVERT_TO_FACTOR_OF(_x,_y)             \
    do {                                           \
        if ((_x) < (_y))  break;                   \
        if ((_x)%(_y) == 0) {                      \
            break;                                 \
        } else {                                   \
            (_x) = (((_x/_y)+1) * (_y));           \
        }                                          \
    } while (FALSE)

#define DEFAULT_OBSS_SNR_THRESHOLD 0    /* default SNR threshold */
#define DEFAULT_OBSS_RX_SNR_THRESHOLD 0 /* default RX SNR threshold */
#define OBSS_SNR_MIN 0    /* min SNR */
#define OBSS_SNR_MAX 127 /* max SNR */

/* Definitions for valid VHT MCS map */
#define VHT_CAP_IE_NSS_MCS_0_7      0
#define VHT_CAP_IE_NSS_MCS_0_8      1
#define VHT_CAP_IE_NSS_MCS_0_9      2
#define VHT_CAP_IE_NSS_MCS_RES      3

#define VHT_CAP_IE_NSS_MCS_MASK     3
#define VHT_MAX_NSS 4
/* 8 chain TODO: Define VHT_MAX_NSS as 8 under #if QCA_SUPPORT_5SS_TO_8SS,
 * once VHT rate mask size is increased.
 */

#define VHT_MCSMAP_NSS1_MCS0_7  0xfffc  /* NSS=1 MCS 0-7 */
#define VHT_MCSMAP_NSS2_MCS0_7  0xfff0  /* NSS=2 MCS 0-7 */
#define VHT_MCSMAP_NSS3_MCS0_7  0xffc0  /* NSS=3 MCS 0-7 */
#define VHT_MCSMAP_NSS4_MCS0_7  0xff00  /* NSS=4 MCS 0-7 0xff.b0000,0000 */
#if QCA_SUPPORT_5SS_TO_8SS
#define VHT_MCSMAP_NSS5_MCS0_7  0xfc00  /* NSS=5 MCS 0-7 */
#define VHT_MCSMAP_NSS6_MCS0_7  0xf000  /* NSS=6 MCS 0-7 */
#define VHT_MCSMAP_NSS7_MCS0_7  0xc000  /* NSS=7 MCS 0-7 */
#define VHT_MCSMAP_NSS8_MCS0_7  0x0000  /* NSS=8 MCS 0-7 */
#endif /* QCA_SUPPORT_5SS_TO_8SS */

#define VHT_MCSMAP_NSS1_MCS0_8  0xfffd  /* NSS=1 MCS 0-8 */
#define VHT_MCSMAP_NSS2_MCS0_8  0xfff5  /* NSS=2 MCS 0-8 */
#define VHT_MCSMAP_NSS3_MCS0_8  0xffd5  /* NSS=3 MCS 0-8 */
#define VHT_MCSMAP_NSS4_MCS0_8  0xff55  /* NSS=3 MCS 0-8 0xff.b0101,0101*/
#if QCA_SUPPORT_5SS_TO_8SS
#define VHT_MCSMAP_NSS5_MCS0_8  0xfd55  /* NSS=5 MCS 0-8 */
#define VHT_MCSMAP_NSS6_MCS0_8  0xf555  /* NSS=6 MCS 0-8 */
#define VHT_MCSMAP_NSS7_MCS0_8  0xd555  /* NSS=7 MCS 0-8 */
#define VHT_MCSMAP_NSS8_MCS0_8  0x5555  /* NSS=8 MCS 0-8 */
#endif /* QCA_SUPPORT_5SS_TO_8SS */

#define VHT_MCSMAP_NSS1_MCS0_9  0xfffe  /* NSS=1 MCS 0-9 */
#define VHT_MCSMAP_NSS2_MCS0_9  0xfffa  /* NSS=2 MCS 0-9 */
#define VHT_MCSMAP_NSS3_MCS0_9  0xffea  /* NSS=3 MCS 0-9 */
#define VHT_MCSMAP_NSS4_MCS0_9  0xffaa  /* NSS=3 MCS 0-9 0xff.b1010,1010*/
#if QCA_SUPPORT_5SS_TO_8SS
#define VHT_MCSMAP_NSS5_MCS0_9  0xfeaa  /* NSS=5 MCS 0-9 */
#define VHT_MCSMAP_NSS6_MCS0_9  0xfaaa  /* NSS=6 MCS 0-9 */
#define VHT_MCSMAP_NSS7_MCS0_9  0xeaaa  /* NSS=7 MCS 0-9 */
#define VHT_MCSMAP_NSS8_MCS0_9  0xaaaa  /* NSS=8 MCS 0-9 */
#endif /* QCA_SUPPORT_5SS_TO_8SS */

/* Definitions for valid VHT MCS mask */
#define VHT_MCSMAP_NSS1_MASK   0xfffc   /* Single stream mask */
#define VHT_MCSMAP_NSS2_MASK   0xfff0   /* Dual stream mask */
#define VHT_MCSMAP_NSS3_MASK   0xffc0   /* Tri stream mask */
#define VHT_MCSMAP_NSS4_MASK   0xff00   /* four stream mask */
#if QCA_SUPPORT_5SS_TO_8SS
#define VHT_MCSMAP_NSS5_MASK   0xfc00   /* five stream mask */
#define VHT_MCSMAP_NSS6_MASK   0xf000   /* six stream mask */
#define VHT_MCSMAP_NSS7_MASK   0xc000   /* seven stream mask */
#define VHT_MCSMAP_NSS8_MASK   0x0000   /* eight stream mask */
#endif /* QCA_SUPPORT_5SS_TO_8SS */

/* Default VHT80 rate mask support all MCS rates except NSS=3 MCS 6 */
#define MAX_VHT80_RATE_MASK   0x3bffffff /* NSS=1 MCS 0-9, NSS=2 MCS 0-9, NSS=3 MCS 0-5 7-9 */

#define MAX_VHT_SGI_MASK   0x3ff /* MCS 0-9 */

/* Indicate support for VHT MCS10/11 */
#define ENABLE_VHT_MCS_10_11_SUPP   1

/* Indicate support for HE MCS12/13 */
#define ENABLE_HE_MCS_12_13_SUPP   1

#define DEFAULT_WLAN_VDEV_AP_KEEPALIVE_MAX_UNRESPONSIVE_TIME_SECS   \
                (IEEE80211_INACT_RUN * IEEE80211_INACT_WAIT)
#define DEFAULT_WLAN_VDEV_AP_KEEPALIVE_MAX_IDLE_TIME_SECS           \
                (DEFAULT_WLAN_VDEV_AP_KEEPALIVE_MAX_UNRESPONSIVE_TIME_SECS - 5)
#define DEFAULT_WLAN_VDEV_AP_KEEPALIVE_MIN_IDLE_TIME_SECS           \
                (DEFAULT_WLAN_VDEV_AP_KEEPALIVE_MAX_IDLE_TIME_SECS/2)

#define IEEE80211_BGSCAN_INTVAL_MIN            15  /* min bg scan intvl (secs) */
#define IEEE80211_BGSCAN_INTVAL_DEFAULT    (5*60)  /* default bg scan intvl */

#define IEEE80211_BGSCAN_IDLE_MIN             100  /* min idle time (ms) */
#define IEEE80211_BGSCAN_IDLE_DEFAULT         250  /* default idle time (ms) */

#define IEEE80211_CUSTOM_SCAN_ORDER_MAXSIZE   101  /* Max size of custom scan
                                                      order array */

#define IEEE80211_COVERAGE_CLASS_MAX           31  /* max coverage class */
#define IEEE80211_REGCLASSIDS_MAX              10  /* max regclass id list */

#define IEEE80211_PS_SLEEP                    0x1  /* STA is in power saving mode */
#define IEEE80211_PS_MAX_QUEUE                 50  /* maximum saved packets */

#define IEEE80211_MINIMUM_BMISS_TIME          500 /* minimum time without a beacon required for a bmiss */
#define IEEE80211_DEFAULT_BMISS_COUNT_MAX           3 /* maximum consecutive bmiss allowed */
#define IEEE80211_DEFAULT_BMISS_COUNT_RESET         2 /* number of  bmiss allowed before reset */

#define IEEE80211_BMISS_LIMIT                  15

#define IEEE80211_MAX_MCAST_LIST_SIZE          32 /* multicast list size */

#define IEEE80211_APPIE_MAX_FRAMES             10 /* max number frame types that can have app ie buffer setup */
#define IEEE80211_APPIE_MAX                  1024 /* max appie buffer size */
#define IEEE80211_OPTIE_MAX                   256 /* max optie buffer size */
#define IEEE80211_MAX_PRIVACY_FILTERS           4 /* max privacy filters */
#define IEEE80211_MAX_PMKID                     3 /* max number of PMKIDs */
#define IEEE80211_MAX_MISC_EVENT_HANDLERS       5
#define IEEE80211_MAX_RESMGR_EVENT_HANDLERS    32 /* max vdev resource mgr event handlers */
#define IEEE80211_MAX_DEVICE_EVENT_HANDLERS     6
#define IEEE80211_MAX_VAP_EVENT_HANDLERS        8
#define IEEE80211_MAX_VAP_MLME_EVENT_HANDLERS   4
#define IEEE80211_STA_NOASSOC_TIME              30000 /* 30 secs */


#define IEEE80211_INACT_WAIT                   15  /* inactivity interval (secs) */
#define IEEE80211_OBSS_NB_RU_TOLERANCE_TIME_MIN 300  /* Min Narrow Bandwidth RU tolerance time in seconds */
#define IEEE80211_OBSS_NB_RU_TOLERANCE_TIME_MAX 3600 /* Max Narrow Bandwidth RU tolerance time in seconds */
#define IEEE80211_OBSS_NB_RU_TOLERANCE_TIME_DEFVAL 1800 /* Default Narrow Bandwidth RU tolerance time in seconds */
#define IEEE80211_RUN_INACT_TIMEOUT_THRESHOLD       (u_int16_t)~0
#define IEEE80211_MLME_EXTERNAL_AUTH_TIMEOUT   (3000) /* 3000 msec*/
#define IEEE80211_MGMT_OFFCHAN_LIST_MAX        (20)
#define IEEE80211_SESSION_WAIT                  1  /* session timeout check interval (secs) */

#if ATH_PROXY_NOACK_WAR
#define TARGET_AST_RESERVE_TIMEOUT           1  /* ast reserve timeout  (secs) */
#endif

#define IEEE80211_MAGIC_ENGDBG           0x444247  /* Magic number for debugging purposes */
#define IEEE80211_DEFULT_KEEP_ALIVE_TIMEOUT    600  /* default keep alive timeout */
#define IEEE80211_FRAG_TIMEOUT                  1  /* fragment timeout in sec */

#define IEEE80211_CHAN_STATS_THRESOLD           40  /* percentage thresold */

#define IEEE80211_MS_TO_TU(x)                (((x) * 1000) / 1024)
#define IEEE80211_TU_TO_MS(x)                (((x) * 1024) / 1000)

#define IEEE80211_USEC_TO_TU(x)                ((x) >> 10)  /* (x)/1024 */
#define IEEE80211_TU_TO_USEC(x)                ((x) << 10)  /* (x)X1024 */

#define IEEE80211_DEFAULT_NOISE_FLOOR          161 /* -95dBm */

/*
 * Macros used for RSSI calculation.
 */
#define ATH_RSSI_EP_MULTIPLIER               (1<<7)  /* pow2 to optimize out * and / */

#define ATH_RSSI_LPF_LEN                     10
#define ATH_RSSI_DUMMY_MARKER                0x127
#define ATH_EP_MUL(x, mul)                   ((x) * (mul))
#define ATH_EP_RND(x, mul)                   ((((x)%(mul)) >= ((mul)/2)) ? ((x) + ((mul) - 1)) / (mul) : (x)/(mul))
#define ATH_RSSI_GET(x)                      ATH_EP_RND(x, ATH_RSSI_EP_MULTIPLIER)

#define RSSI_LPF_THRESHOLD                   -20

#define ATH_RSSI_OUT(x)                      (((x) != ATH_RSSI_DUMMY_MARKER) ? (ATH_EP_RND((x), ATH_RSSI_EP_MULTIPLIER)) : ATH_RSSI_DUMMY_MARKER)
#define ATH_RSSI_IN(x)                       (ATH_EP_MUL((x), ATH_RSSI_EP_MULTIPLIER))
#define ATH_LPF_RSSI(x, y, len) \
    ((x != ATH_RSSI_DUMMY_MARKER) ? ((((x) << 3) + (y) - (x)) >> 3) : (y))
#define ATH_RSSI_LPF(x, y) do {                     \
    if ((y) >= RSSI_LPF_THRESHOLD)                         \
        x = ATH_LPF_RSSI((x), ATH_RSSI_IN((y)), ATH_RSSI_LPF_LEN);  \
} while (0)
#define ATH_ABS_RSSI_LPF(x, y) do {                     \
    if ((y) >= (RSSI_LPF_THRESHOLD + ATH_DEFAULT_NOISE_FLOOR))      \
        x = ATH_LPF_RSSI((x), ATH_RSSI_IN((y)), ATH_RSSI_LPF_LEN);  \
} while (0)

#define IEEE80211_PWRCONSTRAINT_VAL(vap) \
    (((vap)->iv_bsschan->ic_maxregpower > (vap->iv_ic)->ic_curchanmaxpwr) ? \
        (vap)->iv_bsschan->ic_maxregpower - (vap->iv_ic)->ic_curchanmaxpwr : 3)

#define IEEE80211_RATE_MAX 256
#define IEEE80211_PCP_MAX 8

#define GLOBAL_SCN_SIZE 10
#define GLOBAL_SOC_SIZE GLOBAL_SCN_SIZE

#define MLME_MAX_AUTH_FAIL_CNT 4

#define WHC_DEFAULT_SFACTOR 70
#define WHC_DEFAULT_SKIPHYST 1
#define IS_NULL_ADDR(_a)     \
 ((_a)[0] == 0x00 &&         \
 (_a)[1] == 0x00 &&          \
 (_a)[2] == 0x00 &&          \
 (_a)[3] == 0x00 &&          \
 (_a)[4] == 0x00 &&          \
 (_a)[5] == 0x00)

#define MAX_BSS_SETS	1  /* Maximum MBSSID sets per radio */
#define mbss_alert(format, args...) \
		QDF_TRACE_FATAL(QDF_MODULE_ID_MBSSIE, format, ## args)

#define mbss_err(format, args...) \
		QDF_TRACE_ERROR(QDF_MODULE_ID_MBSSIE, format, ## args)

#define mbss_warn(format, args...) \
		QDF_TRACE_WARN(QDF_MODULE_ID_MBSSIE, format, ## args)

#define mbss_info(format, args...) \
		QDF_TRACE_INFO(QDF_MODULE_ID_MBSSIE, format, ## args)

#define mbss_debug(format, args...) \
		QDF_TRACE_DEBUG(QDF_MODULE_ID_MBSSIE, format, ## args)

static const u_int8_t ieee80211broadcastaddr[QDF_MAC_ADDR_SIZE] =
{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

/* maximum possible data size that can be passed from application*/
#define MAX_FIPS_BUFFER_SIZE (sizeof(struct ath_fips_cmd) + 1500)
#define IEEE80211_TPCREP_LEN 2
#define IEEE80211_TPC_LINKMARGIN_VAL 0
#define SAE_PWE_LOOP     0
#define SAE_PWE_H2E      1
#define SAE_PWE_LOOP_H2E 2
#define SAE_PK_ENABLE    1

struct ieee80211_chanhist{
    uint8_t    chanid;
    uint8_t    chanband;
    uint32_t   chanjiffies;
};

typedef struct ieee80211_country_entry{
    u_int16_t   countryCode;  /* HAL private */
    u_int16_t   regDmnEnum;   /* HAL private */
    u_int16_t   regDmn5G;
    u_int16_t   regDmn2G;
    u_int8_t    isMultidomain;
    u_int8_t    iso[3];       /* ISO CC + (I)ndoor/(O)utdoor or both ( ) */
} IEEE80211_COUNTRY_ENTRY;

/*
 * enum to specify the drain mode of frames from
 * backlog queue
 */
enum ieee80211_mgmt_drain_mode {
    IEEE80211_VDEV_MGMT_DRAIN,      /* For VDEV drain mode */
    IEEE80211_PEER_MGMT_DRAIN       /* For PEER drain mode */
};

/*
 * 802.11 control state is split into a common portion that maps
 * 1-1 to a physical device and one or more "Virtual AP's" (VAP)
 * that are bound to an ieee80211com instance and share a single
 * underlying device.  Each VAP has a corresponding OS device
 * entity through which traffic flows and that applications use
 * for issuing ioctls, etc.
 */

struct ieee80211vap;

//typedef  void *ieee80211_vap_state_priv_t;
typedef int ieee80211_vap_state_priv_t;

/**
 * struct wlan_cfg80211_band_chans - cfg per band channel definition
 * @channel_list_size: size of channel_list in units of channel pointers
 * @channel_list: array of pointers
 */
struct wlan_cfg80211_band_chans {
    uint32_t    channel_list_size;
    struct ieee80211_channel  *channel_list;
};

struct ath_fips_cmd {
    u_int32_t fips_cmd;/* 1 - Encrypt, 2 - Decrypt*/
    enum fips_mode mode; /* mode of AES - AES_CTR or AES_MIC */
    u_int32_t key_idx;
    u_int32_t key_cipher;
    u_int32_t key_len;
#define MAX_KEY_LEN_FIPS 32
    u_int8_t  key[MAX_KEY_LEN_FIPS];
#define MAX_IV_LEN_FIPS  16
    u_int8_t iv[MAX_IV_LEN_FIPS];
    u_int32_t pn_len;
#define MAC_PN_LENGTH 8
    u_int32_t pn[MAC_PN_LENGTH];
    u_int32_t header_len;
#define MAX_HDR_LEN 32
    u_int8_t header[MAX_HDR_LEN];
    u_int32_t data_len;
    u_int32_t data[1]; /* To be allocated dynamically*/
};

#if ATH_SUPPORT_WIRESHARK
/* avoid inclusion of ieee80211_radiotap.h everywhere */
struct ath_rx_radiotap_header;
#endif
struct ieee80211_ol_wifiposdesc;

typedef spinlock_t ieee80211_ic_state_lock_t;

typedef struct ieee80211_ven_ie {
    u_int8_t                      ven_ie[IEEE80211_MAX_IE_LEN];
    u_int8_t                      ven_ie_len;
    u_int8_t                      ven_oui[3];
    bool                          ven_oui_set;
} IEEE80211_VEN_IE;



typedef struct ieee80211_vht_mcs {
    u_int16_t     mcs_map;          /* Max MCS for each SS */
    u_int16_t     data_rate;        /* Max data rate */
    u_int8_t      higher_mcs_supp;  /* VHT MCS10/11 support for each SS */
} ieee80211_vht_mcs_t;

typedef struct ieee80211_vht_mcs_set {
    ieee80211_vht_mcs_t     rx_mcs_set; /* B0-B15 Max Rx MCS for each SS
                                            B16-B28 Max Rx data rate
                                            B29-B31 reserved */
    ieee80211_vht_mcs_t     tx_mcs_set; /* B32-B47 Max Tx MCS for each SS
                                            B48-B60 Max Tx data rate
                                            B61-B63 reserved */
}ieee80211_vht_mcs_set_t;


/*
 * Data common to one or more virtual AP's.  State shared by
 * the underlying device and the net80211 layer is exposed here;
 * e.g. device-specific callbacks.
 */

#ifdef ATH_HTC_MII_RXIN_TASKLET
typedef struct ieee80211_recv_mgt_args {
    struct ieee80211_node *ni;
    int32_t subtype;
    int32_t rssi;
    uint32_t rstamp;
}ieee80211_recv_mgt_args_t;

typedef struct _nawds_dentry{
    TAILQ_ENTRY(_nawds_dentry)  nawds_dlist;
    struct ieee80211vap              *vap ;
    u_int8_t                  mac[QDF_MAC_ADDR_SIZE];

}nawds_dentry_t;

void _ath_htc_netdeferfn_init(struct ieee80211com *ic);
void _ath_htc_netdeferfn_cleanup(struct ieee80211com *ic);


#define ATH_HTC_NETDEFERFN_INIT(_ic) _ath_htc_netdeferfn_init(_ic)
#define ATH_HTC_NETDEFERFN_CLEANUP(_ic) _ath_htc_netdeferfn_cleanup(_ic)


#else
#define ATH_HTC_NETDEFERFN_INIT(_ic)
#define ATH_HTC_NETDEFERFN_CLEANUP(_ic)
#endif

#define ACS_PEIODIC_SCAN_CHK            0x1
#define ACS_PEREODIC_OBSS_CHK           0x2
#define ACS_PEIODIC_BG_SCAN             0x4

#define  ATH_SCANENTRY_MAX 1024
#define  ATH_SCANENTRY_TIMEOUT 60

struct mlmeop {
    struct ieee80211req_mlme *mlme;
    wlan_if_t vap;
};

#define VHT160_CENTER_EDGE_DIFF 80     /* diff between center freq and channel edges */
#define VHT80_CENTER_EDGE_DIFF  40     /* diff between center freq and channel edges */
#define VHT40_PRIMARY_EDGE_DIFF 30     /* diff between center freq and channel edges */
#define PRIMARY_EDGE_DIFF       10     /* diff between center freq and channel edges */

/*
 * Data structure which contains restricted channels
 * parameters.
 */
struct restrict_channel_params {
    u_int8_t  restrict_channel_support;  /* flag for restricted channel support */
    u_int32_t low_5ghz_chan;             /* lower bound frequency */
    u_int32_t high_5ghz_chan;            /* higher bound frequency */
};

#define MAX_RADIO_CNT 3

#define VDEV_BITMAP_SLOTS 2

#define VDEV_BITMAP_SLOT0 0
#define VDEV_BITMAP_SLOT1 1
struct ieee80211_clone_params_list {
    LIST_ENTRY(ieee80211_clone_params_list)    link_entry;
    struct ieee80211_clone_params cp;
};

typedef struct _CLONE_PARAMS {
    struct ieee80211_clone_params_list *lh_first;
} CLONE_PARAMS;

#if UMAC_SUPPORT_CFG80211
struct radio_handler {
    int (*setcountry)(void *scn, char *cntry);
    int (*getcountry)(void *scn, char *str);
    int (*sethwaddr)(void *scn, char *addr);
    int (*gethwaddr)(struct net_device *dev, void *info, void *w, char *extra);
    int (*setparam)(void *scn, int param, int value);
    int (*getparam)(void *scn, int param, int *val);

#if UNIFIED_SMARTANTENNA
    int (*set_saparam)(void *scn, char *val);
    int (*get_saparam)(void *scn, char *val);
#endif

#if ATH_SUPPORT_DSCP_OVERRIDE
    void (*setdscp_tid_map)(void *scn, u_int8_t tos, u_int8_t tid);
    int (*getdscp_tid_map)(void *scn, u_int8_t tos);
#endif

#if ATH_SUPPORT_HYFI_ENHANCEMENTS
    int (*ald_getStatistics)(struct net_device *dev, void *info, void *w, char *extra);
#endif

#if PEER_FLOW_CONTROL
    void (*txrx_peer_stats)(void *scn, char *addr);
#endif
#if QCA_AIRTIME_FAIRNESS
    int (*set_atf_sched_dur)(void *scn, uint32_t ac, uint32_t duration);
#endif

    int (*get_aggr_burst)(struct net_device *dev,void *info,void *w,char *extra);
    int (*set_aggr_burst)(void *scn, uint32_t ac, uint32_t duration);
    int (*extended_commands)(struct net_device *dev,  void *command);
    int (*ucfg_phyerr)(void *scn, void *diag);
    int (*get_ath_stats) (void *scn, void *asc);
    void (*get_dp_fw_peer_stats)(void *scn, char *addr, uint8_t caps);
    void (*get_dp_htt_stats)(void *scn, void *data, uint32_t data_len);
    void (*get_cp_wmi_stats)(void *scn, void *buf_ptr, uint32_t buf_len);
    int (*get_target_pdev_id)(void *scn, uint32_t *val);
#if OBSS_PD
    int (*set_he_srg_bitmap)(void *scn, uint32_t *val, uint32_t param);
    int (*get_he_srg_bitmap)(void *scn, uint32_t *val, uint32_t param);
    int (*set_he_sr_config) (void *vscn, uint8_t param, uint8_t value, uint8_t data1, uint8_t data2);
    int (*get_he_sr_config) (void *vscn, uint8_t param, uint32_t *value);
    int (*set_sr_self_config) (void *vscn, uint32_t value, void *data, uint32_t data_len, uint32_t length);
    int (*get_sr_self_config) (void *vscn, uint8_t param, char value[], size_t length);
#endif /* OBSS PD */
    int (*set_muedca_mode)(void *scn, uint8_t mode);
    int (*get_muedca_mode)(void *scn, int* value);
    int (*set_non_ht_dup)(void *scn, uint8_t frame, bool enable);
    int (*get_non_ht_dup)(void *scn, uint8_t frame, uint8_t *value);
    int (*set_nav_override_config)(void *scn, uint8_t value, uint32_t threshold);
    int (*get_nav_override_config)(void *scn, int *value);
    int (*set_he_mesh_config)(void *scn, void *args);
    int (*get_he_mesh_config)(void *scn, int *value, uint8_t subcmd);
    void (*set_ba_timeout) (void *scn, uint8_t ac, uint32_t duration);
    void (*get_ba_timeout) (void *scn, uint8_t ac, uint32_t *duration);
    int (*set_pcp_tid_map) (void *scn, uint32_t pcp, uint32_t tid);
    int (*get_pcp_tid_map) (void *scn, uint32_t pcp, uint32_t *value);
    int (*set_tidmap_prty) (void *scn, uint32_t value);
    int (*get_tidmap_prty) (void *scn,  uint32_t *value);
    int (*set_col_6ghz_rnr) (void *scn, uint8_t value, uint8_t frm_val);
    int (*get_col_6ghz_rnr) (void *scn,  uint8_t *value);
#if DBG_LVL_MAC_FILTERING
    int (*set_dbglvlmac) (wlan_if_t vap, uint8_t *mac_addr, uint8_t mac_addr_len, uint8_t value);
    int (*get_dbglvlmac) (wlan_if_t vap,  uint8_t value);
#endif /* DBG_LVL_MAC_FILTERING */
#ifdef WLAN_SUPPORT_RX_PROTOCOL_TYPE_TAG
    int  (*ic_set_rx_pkt_protocol_tagging)(void *scn,
                    struct ieee80211_rx_pkt_protocol_tag *rx_pkt_protocol_tag_info);
#ifdef WLAN_SUPPORT_RX_TAG_STATISTICS
    int  (*ic_dump_rx_pkt_protocol_tag_stats)(void *scn, uint32_t protocol_type);
#endif /* WLAN_SUPPORT_RX_TAG_STATISTICS */
#endif /* WLAN_SUPPORT_RX_PROTOCOL_TYPE_TAG */
    int (*set_btcoex_duty_cycle)(void *scn, uint32_t period, uint32_t duration);
#ifdef WLAN_SUPPORT_RX_FLOW_TAG
    int (*ic_rx_flow_tag_op)(void *scn, struct ieee80211_rx_flow_tag *rx_flow_tag_info);
#endif /* WLAN_SUPPORT_RX_FLOW_TAG */
#if defined(WLAN_TX_PKT_CAPTURE_ENH) || defined(WLAN_RX_PKT_CAPTURE_ENH)
    int  (*ic_set_peer_pkt_capture_params)(void *scn,
                          struct ieee80211_pkt_capture_enh *peer_info);
#endif /* WLAN_TX_PKT_CAPTURE_ENH || WLAN_RX_PKT_CAPTURE_ENH */
};
#endif

typedef enum ieee80211_rep_move_state {
    REPEATER_MOVE_STOP = 0,
    REPEATER_MOVE_START = 1,
    REPEATER_MOVE_IN_PROGRESS = 2,
} ieee80211_rep_move_state_t;

typedef struct ieee80211_rep_move {
    ieee80211_rep_move_state_t state;
    ieee80211_ssid             ssid;
    u_int32_t                  chan;
    bool                       same_chan;
    u_int8_t                   bssid[QDF_MAC_ADDR_SIZE];
} ieee80211_rep_move_t;

#define NBR_SCAN_PERIOD            5      /* Neighbor scan period in sec */
#define RNR_MAX_ENTRIES            16     /* Reduced Neighbor Report entries */
#define RNR_MAX_SIZE               256    /* >= RNR_MAX_ENTRIES * (sizeof(rnr_ap_info_t) + sizeof(rnr_tbtt_info_t)) */
#define ACR_MAX_ENTRIES            16     /* AP Channel Report entries */
#define ACR_MAX_CHANNELS           16     /* Max number of channels for each op class */

typedef struct ap_chan_rpt {
    u_int8_t op_class;
    u_int8_t chan_cnt;
    u_int8_t chan[ACR_MAX_CHANNELS];
} ap_chan_rpt_t;

typedef struct chan_util_stats {
    u_int8_t free_medium;   /* Total Free medium available percentage*/
    u_int8_t ap_tx;    /* Total AP wlan Tx channel utilization percentage */
    u_int8_t ap_rx;    /* Total AP wlan Rx channel utilization percentage */
    u_int8_t obss_rx;     /* Other bss Rx channel utilization */

} chan_util_stats_t;

struct scan_chan_entry_stats {
    uint32_t freq;
    struct ieee80211_mib_cycle_cnts stats;
};

struct wlan_channel_stats {
    qdf_spinlock_t lock;
    struct channel_stats home_chan_stats;
    struct scan_chan_entry_stats entry_stats;
    struct channel_stats scan_chan_stats[NUM_CHANNELS];
};


struct last_scan_params {
   uint32_t scan_ev_started:1,
            scan_ev_completed:1;
   uint32_t scan_events;
   uint32_t scan_id;
   uint32_t scan_req_id;

   uint32_t min_rest_time;
   uint32_t max_rest_time;
   uint32_t idle_time;
   uint32_t max_scan_time;
   uint32_t dwell_time_active;
   uint32_t dwell_time_passive;

   uint32_t scan_f_passive:1,
            scan_f_bcast_probe:1,
            scan_f_continue_on_err:1,
            scan_f_forced:1,
            scan_f_2ghz:1,
            scan_f_5ghz:1,
            scan_f_wide_band:1;
   uint32_t scan_flags;
   struct chan_list chan_list;
};

/* MBSS IE support members */
struct mbssid_info {
#define IEEE80211_EMA_TEMP_MBSS_BUFFER_SIZE 700 /* 656 is the max size for
                                                   nonTx VAP in worst case
                                                   pp = 16
                                                   + 30 for IE header headroom,
                                                   rounding off to 700*/
    /* bssid-idx bitmap representing bssid-idx pool */
    unsigned long        bssid_index_bmap[MAX_BSS_SETS];

    /* pointer to the target Tx-VAP if operating in non-MBSSID mode */
    struct ieee80211vap  *target_transmit_vap;

    /* pointer to tx-vap */
    struct ieee80211vap  *transmit_vap;

    /* Total number of vaps in mbss_cache at nay point in time.
     * This is a read-on variable. User should acces it only
     * using API ieee80211_mbssid_get_num_vaps_in_mbss_cache */
    uint8_t              num_vaps;

    /* Max limit of non-tx vaps */
    uint8_t              max_non_transmit_vaps;

	/* max bssid indicator */
    uint8_t              max_bssid;

    /* mbss cache - a vap occupies a cache entry at vdev-create
     * and releases the slot only at vdev_delete. Each entry is
     * of type struct ieee80211_mbss_ie_cache_node */
    void                 *mbss_cache;

    /* Offset to the start of a beacon-position in the mbss_cache */
    uint16_t             *mbss_offset;

    /* Current profile-periodicity */
    uint8_t              current_pp;

    /* Is there pp change during the last vdev_up? */
    bool                 pp_change;

    /* mbssid attached or not */
    bool                 mbssid_init;

    /* mbss cache lock */
    qdf_spinlock_t       mbss_cache_lock;

    /* bssid map from known bssid element in probe request frame */
    uint32_t             known_bssid_map;

    /* If this flag is 1, non-tx VAPs will not inherit any IEs
     * from tx VAP. If it's 0, only RSN (WPA2/WPA3) is inherited.
     * WPA1 can't be inherited and in that case, non-inheritance IE
     * is added */
    bool                 non_inherit_enable;

    /* vap whose ssid matched in probe req frame */
    struct ieee80211vap *prb_req_ssid_match_vap;
    /* flag is used to indicate that probe response has been sent
     * when A3 in request frame is not FF */
    bool                 resp_sent;

    /* reference bssid */
    uint8_t              ref_bssid[QDF_MAC_ADDR_SIZE];

    /* rotation factor - it is the number of slots in the cache
     * by which if we move the current position of the tx-vap to
     * the right the tx-vap gets logically placed at the end slot
     * of the mbss cache. In other words, it is the distance bet-
     * ween the previous and the current tx-vap. Note that the
     * tx vap is always placed at the end slot logically. Initially,
     * the first vap created is placed physically at the last slot
     * and it is considered to be the logical tx vap till user
     * designates the tx vap. Rotation factor will be determined
     * wrt to the first vap created and the tx vap user chose when
     * user designates the tx vap for the first time. Also, note
     * that rotation factor is a cumulative rotation factor to retain
     * the reference of the positions of the previous tx vap. But
     * it can wrap around at the boundary of ema_max_ap (<= 2^n)
     * */
    uint8_t              rot_factor;

    /* Least significant byte of the previous bssid that was
     * assigned from user-space during the previous vap create.
     * Currently, only relevant during bssid-sanity check, done
     * as part of vap-create
     */
    uint8_t lsb_of_last_user_bssid;

    /* Derived by:  max_mgmt_frame_size    = IEEE80211_MAX_MGMT_SIZE_LIMIT
     *              rnr_size               = soc->ema_ap_rnr_field_size_limit
     *              mandatory_ie_size      = (IEEE80211_MAX_NON_TX_PROFILE_SIZE
     *                                        + IEEE80211_MAX_MBSS_NON_TX_PROFILE_SECURITY_LEN)
     *              max_common_beacon_size = soc->ema_ap_beacon_common_part_size
     *              mbssid_ie_headroom     = IEEE80211_MBSSID_MAX_HEADROOM_SIZE
     *              non_tx_profile_size    = (max_mgmt_frame_size - max_common_beacon_size
     *                                        - rnr_size - mbssid_ie_headroom
     *                                        - optional_ie_max_size - mandatory_ie_size)
     *
     * This variable indicates the maximum programmable space avaialble for
     * a non-tx profile
     */
    uint32_t non_tx_profile_size;

    /* Profile periodicity is calculated per SoC so the value
     * must be the max_pp across all pdevs on this SoC. Store
     * max_pp for this pdev here.
     */
    uint8_t max_pp;

    /* feature enable flag for optional IE support in non-tx vap
     */
    bool ema_ext_enabled;

    /* feature enable flag for split profile support
     */
    bool mbss_split_profile_enabled;

    /* total space available in the probe response frame for
     * new non-tx profiles to be added
     */
    int32_t ema_ap_available_prb_non_tx_space;

    /* Buffers used during beacon/probe response offset adjustments to hold the
     * MBSS IE until offsets are adjusted
     */
    uint8_t *bcn_bo_mbss_ie;
    uint8_t *prb_po_mbss_ie;

    /* var allows to skip device close during bringdown of tx vdev */
    bool skip_dev_close;
};

struct ieee80211_user_freq_list {
    uint16_t n_freq;
    uint16_t freq[ACS_MAX_CHANNEL_COUNT];
};

struct primary_allowed_chanlist {
    uint8_t  n_freq;
    uint16_t *freq;
};

struct om_notify_context {
    qdf_timer_t notify_timer;	/* timer to manage OM notification */
    uint8_t     omn_enable;     /* Global feature flag: 1- send OMN update, 0- don't send */
    uint8_t     omn_in_progress; /* Flag to indicate that OMN is in progress */
    uint32_t    omn_timeout;    /* Timeout in milliseconds for OMN update */
    struct ieee80211_ie_op_mode_ntfy  omn_ie;   /* OMN IE for Beacon, probe resp */
    struct ieee80211_ie_op_mode_ntfy  omn_assoc_ie; /* OMN IE for Assoc resp */

};

#define OMN_DEFAULT_UPDATE_TIME		(2000)	/* Default time in ms for broadcasting OMN IE */

/*
 * beacon offload control operations.
 * Used by func ic_beacon_offload_control.
 */
#define IEEE80211_BCN_OFFLD_TX_DISABLE   0
#define IEEE80211_BCN_OFFLD_TX_ENABLE    1
#define IEEE80211_BCN_OFFLD_SWBA_DISABLE 2
#define IEEE80211_BCN_OFFLD_SWBA_ENABLE  3

/*
 * Operating class table index defined as per operating classes
 * defined in Appendix E.
 */
#define OPCLS_TAB_IDX_NONE     0
#define OPCLS_TAB_IDX_US       1
#define OPCLS_TAB_IDX_EUROPE   2
#define OPCLS_TAB_IDX_JAPAN    3
#define OPCLS_TAB_IDX_GLOBAL   4
#define OPCLS_TAB_IDX_CHINA    5

/**
 * struct ieee80211_set_country_params - Country parameters like country code,
 * country ISO name are stored in this structure.
 * @cc - Country code.
 * @isoName - Country ISO name.
 * @no_chanchange_during_cc - True if current channel needs to validated in the
 * new country's channel channel list else false.
 * @cmd - indicates whether the country configuration is for a new country or
 * the same country but just a channel list update.(obsolete)
 */
struct ieee80211_set_country_params {
    uint16_t cc;
    char isoName[REG_ALPHA2_LEN+1];
    bool no_chanchange_during_cc;
    enum ieee80211_clist_cmd cmd;
};

/* struct user_rnr_data - Entry in list for user rnr buffers
 * @uid - unique ID
 * @is_copied - Mark as copied fully or partially copied or not copied
 * @uid_ap_remaining - APs not copied in RNR IE 1
 * @uid_hdr_ap_length - AP info length in user buffer
 * @uid_buf_length - Total buffer length for this UID
 * @uid_org_ap_cnt - Total APs in UID's user buffer
 * @user_buf - user buffer
 */
struct user_rnr_data {
    TAILQ_ENTRY(user_rnr_data) user_rnr_next_uid;
    uint8_t uid;
    bool is_copied;
    uint8_t uid_ap_remaining;
    uint8_t uid_ap_copied_cnt;
    uint8_t uid_hdr_ap_length;
    uint8_t uid_buf_length;
    uint8_t uid_org_ap_cnt;
    uint8_t *user_buf;
};

/* struct user_neighbor_ap - Store user rnr data
 * running_length - total length of user buffer
 * user_rnr_data - List of user rnr buffers
 */
struct ieee80211_user_nbr_ap_list {
    uint16_t running_length;
    TAILQ_HEAD(, user_rnr_data)    user_rnr_data_list;
    qdf_spinlock_t       user_rnr_lock;
};

/* struct special_vap_stats -  Stats related to special vdev mode
 * @cs_start_time - Time stamp of last channel switch start.
 * This time stamp is logged at the entry of the user space channel switch
 * command handlers.
 */
struct special_vap_stats {
    uint64_t cs_start_time;
};

typedef struct ieee80211com {
    osdev_t                       ic_osdev; /* OS opaque handle */
    qdf_device_t                  ic_qdf_dev; /* ADF opaque handle */
    qdf_netdev_t                  ic_netdev;  /* OS net device */
    bool                          ic_initialized;
    u_int16_t                     ic_scan_entry_max_count;     /* Global limit of number of  scan table entries */
    u_int16_t                     ic_scan_entry_timeout;       /* Timeout value for scan entries */
    spinlock_t                    ic_lock; /* lock to protect access to ic data */
    spinlock_t                    ic_state_check_lock; /* lock to protect access to ic data */
    unsigned long                 ic_lock_flags; /* flags for ic_lock */
    spinlock_t                    ic_main_sta_lock; /* lock to protect main sta */
    unsigned long                 ic_main_sta_lock_flags; /* flags for ic_main_sta_lock */
    spinlock_t                    ic_chan_change_lock;    /* lock to serialize more than one sequences
                                                            of chan-changes for VAPs */
    unsigned long                 ic_chan_change_lock_flags; /* flags for ic_chan_change_lock */

    spinlock_t                    ic_radar_found_lock; /* lock to serialize vap create and radar detection */

    spinlock_t                    ic_addba_lock; /* lock to protect  addba request and addba response*/
    u_int32_t                     ic_chanchange_serialization_flags; /* Channel change for all the VAPs together */
    ieee80211_ic_state_lock_t     ic_state_lock; /* lock to protect access to ic/sc state */
    spinlock_t                    ic_beacon_alloc_lock;
    unsigned long                 ic_beacon_alloc_lock_flags;

    TAILQ_HEAD(, ieee80211vap)    ic_vaps; /* list of vap instances */
    CLONE_PARAMS                  ic_cp;
    enum ieee80211_phytype        ic_phytype; /* XXX wrong for multi-mode */
    enum ieee80211_opmode         ic_opmode;  /* operation mode */
    uint32_t                      ic_supported_bands;
    bool                          ic_tgt_defined_nss_ratio_enabled; /* This flag is set if nss ratio is received from FW as part of service ready ext event. */
    uint32_t                      ic_tgt_defined_nss_ratio; /* Indicates the max nss value to be used for 160MHz with respect to the max nss of 80MHz. */

    u_int8_t                      ic_myaddr[QDF_MAC_ADDR_SIZE]; /* current mac address */
    u_int8_t                      ic_my_hwaddr[QDF_MAC_ADDR_SIZE]; /* mac address from EEPROM */

    u_int8_t                      ic_broadcast[QDF_MAC_ADDR_SIZE];

    u_int32_t                     ic_flags;       /* state flags */
    u_int32_t                     ic_flags_ext;   /* extension of state flags */
    u_int32_t                     ic_flags_ext2;  /* extension of state flags 2 */
    u_int32_t                     ic_chanswitch_flags;  /* Channel switch flags */
    u_int32_t                     ic_wep_tkip_htrate      :1, /* Enable htrate for wep and tkip */
                                  ic_non_ht_ap            :1, /* non HT AP found flag */
                                  ic_doth                 :1, /* 11.h flag */
                                  ic_ht20Adhoc            :1,
                                  ic_ht40Adhoc            :1,
                                  ic_htAdhocAggr          :1,
                                  ic_disallowAutoCCchange :1, /* disallow CC change when assoc completes */
                                  ic_p2pDevEnable         :1, /* Is P2P Enabled? */
                                  ic_ignoreDynamicHalt    :1, /* disallowed  */
                                  ic_override_proberesp_ie:1, /* overwrite probe response IE with beacon IE */
                                  ic_wnm                  :1, /* WNM support flag */
                                  ic_2g_csa               :1,
                                  ic_dropstaquery         :1,
                                  ic_blkreportflood       :1,
                                  ic_offchanscan          :1,  /* Offchan scan */
                                  ic_consider_obss_long_slot :1, /*Consider OBSS non-erp to change to long slot*/
                                  ic_sta_vap_amsdu_disable:1, /* Disable Tx AMSDU for station vap */
                                  ic_enh_ind_rpt          :1, /* enhanced independent repeater  */
                                  ic_strict_pscan_enable  :1, /* do not send probe request in passive channel */
                                  ic_min_snr_enable       :1, /* enable/disable min snr cli block */
                                  ic_no_vlan              :1,
                                  ic_atf_logging          :1,
                                  ic_papi_enable          :1,
                                  ic_ht40intol_scan_running :1,
                                  ic_user_coext_disable   :1,
                                  ic_49ghz_enabled      :1,
                                  ic_rpt_max_phy        :1,
                                  ic_rpt_ap_needs_dfs   :1,
                                  ic_mbss_automode      :1,
                                  ic_externalacs_enable :1; /* Whether external
                                                               auto channel
                                                               selection is
                                                               enabled */
#define CONSIDER_OBSS_LONG_SLOT_DEFAULT 0



    u_int32_t                     ic_caps;        /* capabilities */
    u_int32_t                     ic_caps_ext;    /* extension of capabilities */
    u_int16_t                     ic_cipher_caps; /* cipher capabilities */
    struct ieee80211vap           *ic_sta_vap;      /* the STA handle for vap up down */
    struct ieee80211vap           *ic_mon_vap;      /* the Monitor handle for vap up down */
    u_int8_t                      ic_ath_cap;     /* Atheros adv. capablities */
    u_int8_t                      ic_roaming;     /* Assoc state machine roaming mode */
#if ATH_SUPPORT_WRAP
#if WLAN_QWRAP_LEGACY
    u_int8_t                      ic_nwrapvaps;     /* Number of active WRAP APs */
    u_int8_t                      ic_npstavaps;
    struct ieee80211vap           *ic_mpsta_vap;    /* main proxy sta */
    struct wrap_com               *ic_wrap_com;     /* ic wrap dev specific */
    struct ieee80211vap           *ic_wrap_vap;     /* wrap vap  */
    u_int8_t                      ic_proxystarxwar;   /* Proxy Sta mode Rx WAR */
#if ATH_PROXY_NOACK_WAR
    struct {
    int                           blocking;
    qdf_semaphore_t                sem_ptr;
    }                             proxy_ast_reserve_wait;
    os_timer_t                    ic_ast_reserve_timer;
    qdf_atomic_t               ic_ast_reserve_event;
    int                           ic_ast_reserve_status;
#endif
#endif
    u_int8_t                      ic_nstavaps;
    u_int8_t                      ic_nscanpsta;
    bool                          wrap_disable_da_war;
    u_int8_t                      ic_wrap_vap_sgi_cap;   /* wrap vap configed by qcawifi with SGI to be leveraged for psta*/
#endif
    u_int32_t                     ic_ath_extcap;     /* Atheros extended capablities */
    struct wlan_supported_freq_bands ic_supported_freq_bands;/*bands supported on this radio */
    struct ieee80211_rateset      ic_sup_rates[IEEE80211_MODE_MAX];
    struct ieee80211_rateset      ic_sup_half_rates;
    struct ieee80211_rateset      ic_sup_quarter_rates;
    struct ieee80211_rateset      ic_sup_ht_rates[IEEE80211_MODE_MAX];
    u_int64_t                     ic_modecaps;    /* set of mode capabilities */
    bool                          ic_edge_ch_dep_applicable; /* edge channel deprioritize */
    u_int64_t                     ic_current_modecaps; /* Do not populate radar found channel mode */
    u_int16_t                     ic_curmode;     /* current mode */
    u_int16_t                     ic_intval;      /* default beacon interval for AP, ADHOC */
    u_int16_t                     ic_lintval;     /* listen interval for STATION */
    u_int16_t                     ic_holdover;    /* PM hold over duration */
    u_int16_t                     ic_bmisstimeout;/* beacon miss threshold (ms) */
    u_int16_t                     ic_txpowlimit;  /* global tx power limit */
    u_int16_t                     ic_uapsdmaxtriggers; /* max triggers that could arrive */

    /* 11n (HT) state/capabilities */
    u_int16_t                     ic_htflags;            /* HT state flags */
    u_int16_t                     ic_htcap;              /* HT capabilities */
    u_int16_t                     ic_htextcap;           /* HT extended capabilities */
    u_int16_t                     ic_ldpccap;            /* HT ldpc capabilities */
    u_int16_t                     ic_channelswitchingtimeusec; /* Channel Switching Time in usec */
    u_int8_t                      ic_maxampdu;           /* maximum rx A-MPDU factor */
    u_int8_t                      ic_mpdudensity;        /* MPDU density */
    u_int8_t                      ic_mpdudensityoverride;/* MPDU density override flag and value */
    u_int8_t                      ic_enable2GHzHt40Cap;  /* HT40 supported on 2GHz channels */
    u_int8_t                      ic_weptkipaggr_rxdelim; /* Atheros proprietary wep/tkip aggr mode - rx delim count */
    u_int8_t                      ic_no_weather_radar_chan;  /* skip weather radar channels*/
    u_int8_t                      ic_implicitbf;
    u_int8_t                      ic_nr_share_radio_flag; /* indicate for which radios the NR response is */
    u_int8_t                      ic_nr_share_enable;     /* share enable per radio */
#ifdef ATH_SUPPORT_TxBF
    union ieee80211_hc_txbf       ic_txbf;                /* Tx Beamforming capabilities */
    void    (*ic_v_cv_send)(struct ieee80211_node *ni, u_int8_t	*data_buf,
                u_int16_t buf_len);
    int     (*ic_txbf_alloc_key)(struct ieee80211com *ic, struct ieee80211_node *ni);
    void    (*ic_txbf_set_key)(struct ieee80211com *ic, struct ieee80211_node *ni);
    int     (*ic_txbf_del_key)(struct ieee80211com *ic, struct ieee80211_node *ni);
    void    (*ic_init_sw_cv_timeout)(struct ieee80211com *ic, struct ieee80211_node *ni);
	int     (*ic_set_txbf_caps)(struct ieee80211com *ic);
#ifdef TXBF_DEBUG
	void    (*ic_txbf_check_cvcache)(struct ieee80211com *ic, struct ieee80211_node *ni);
#endif
	void    (*ic_txbf_stats_rpt_inc)(struct ieee80211com *ic, struct ieee80211_node *ni);
	void    (*ic_txbf_set_rpt_received)(struct ieee80211com *ic, struct ieee80211_node *ni);
#endif
#if ATH_SUPPORT_WRAP
    int     (*ic_wrap_set_key)(struct ieee80211vap *vap);
    int     (*ic_wrap_del_key)(struct ieee80211vap *vap);
#endif
#if WLAN_OBJMGR_REF_ID_TRACE
    struct  ieee80211_node*  (*ic_ieee80211_find_node_debug)(struct ieee80211com *ic, const u_int8_t *macaddr, wlan_objmgr_ref_dbgid id, const char * func, int line, const char *file);
#else
    struct  ieee80211_node*  (*ic_ieee80211_find_node)(struct ieee80211com *ic, const u_int8_t *macaddr, wlan_objmgr_ref_dbgid id);
#endif //WLAN_OBJMGR_REF_ID_TRACE
    uint16_t (*ic_get_num_clients)(struct wlan_objmgr_pdev *pdev);
    void    (*ic_if_mgmt_drain) (struct ieee80211_node *ni, int force,
                                 enum ieee80211_mgmt_drain_mode drain_mode);
    void    (*ic_ieee80211_unref_node)(struct ieee80211_node *ni);
    bool    (*ic_is_macreq_enabled)(struct ieee80211com *ic);
    u_int32_t (*ic_get_mac_prealloc_idmask)(struct ieee80211com *ic);
    void    (*ic_set_mac_prealloc_id)(struct ieee80211com *ic, u_int8_t id, u_int8_t set);
    u_int8_t (*ic_get_qwrap_num_vdevs)(struct ieee80211com *ic);
    void    (*ic_update_target_caps)(struct ieee80211com *ic, enum ieee80211_phymode mode);
    uint8_t (*ic_get_bsta_fixed_idmask)(struct ieee80211com *ic);
    void    (*ic_config_update)(struct ieee80211com *ic);
#if OBSS_PD
    int	    (*ic_spatial_reuse)(struct ieee80211vap *vap);
    bool    (*ic_is_spatial_reuse_enabled)(struct ieee80211com *ic);
#endif
    /*
     * 11n A-MPDU and A-MSDU support
     */
    int                           ic_ampdu_limit;     /* A-MPDU length limit */
    int                           ic_ampdu_density;   /* A-MPDU density */
    int                           ic_ampdu_subframes; /* A-MPDU subframe limit */
    int                           ic_amsdu_limit;     /* A-MSDU length limit */
    u_int32_t                     ic_amsdu_max_size;  /* A-MSDU buffer max size */
    /* 6ghz rnr advertisement enabled for this radio.
     * Upper 4 bits represents en/disable/driver mode.
     * Lower 4 bits are each for beacon, probe response,
     * FILS/20TU prb rsp
     */
    uint8_t                       ic_6ghz_rnr_enable;
    /*
     * Global chain select mask for tx and rx. This mask is used only for 11n clients.
     * For legacy clients this mask is over written by a tx chain mask of 1. Rx chain
     * mask remains the global value for legacy clients.
     */
    u_int8_t                      ic_tx_chainmask;
    u_int8_t                      ic_rx_chainmask;

    /* actual number of rx/tx chains supported by HW */
    u_int8_t                      ic_num_rx_chain;
    u_int8_t                      ic_num_tx_chain;

#if ATH_SUPPORT_WAPI
    /* Max number of rx/tx chains supported by WAPI HW engine */
    u_int8_t                      ic_num_wapi_tx_maxchains;
    u_int8_t                      ic_num_wapi_rx_maxchains;
#endif
    /*
     * Number of spatial streams supported.
     */
    u_int8_t                      ic_spatialstreams;

    u_int8_t                      ic_multiDomainEnabled;
    IEEE80211_COUNTRY_ENTRY       ic_country;   /* Current country/regdomain */
    u_int8_t                      ic_isdfsregdomain; /* operating in DFS domain ? */
    qdf_atomic_t ic_fips_event; /* Set when wmi_fips_event occurs*/
    struct ath_fips_output *ic_output_fips;
    /*
     * Channel state:
     *
     * ic_channels is the set of available channels for the device;
     *    it is setup by the driver
     * ic_nchans is the number of valid entries in ic_channels
     * ic_chan_avail is a bit vector of these channels used to check
     *    whether a channel is available w/o searching the channel table.
     * ic_chan_active is a (potentially) constrained subset of
     *    ic_chan_avail that reflects any mode setting or user-specified
     *    limit on the set of channels to use/scan
     * ic_curchan is the current channel the device is set to; it may
     *    be different from ic_bsschan when we are off-channel scanning
     *    or otherwise doing background work
     * ic_bsschan is the channel selected for operation; it may
     *    be undefined (IEEE80211_CHAN_ANYC)
     * ic_prevchan is a cached ``previous channel'' used to optimize
     *    lookups when switching back+forth between two channels
     *    (e.g. for dynamic turbo)
     */
    int                           ic_nchans;  /* # entries in ic_channels */
    struct ieee80211_ath_channel      ic_channels[IEEE80211_CHAN_MAX+1];
    uint8_t                       ic_chan_avail_2g_5g[IEEE80211_CHAN_BYTES];
    uint8_t                       ic_chan_active_2g_5g[IEEE80211_CHAN_BYTES];
    uint8_t                       ic_chan_avail_6g[IEEE80211_CHAN_BYTES];
    uint8_t                       ic_chan_active_6g[IEEE80211_CHAN_BYTES];
    struct ieee80211_ath_channel      *ic_curchan;   /* current channel */
    struct ieee80211_ath_channel      *ic_prevchan;  /* previous channel */
    uint8_t                       ic_chanidx; /*active index for chanhist*/
    struct ieee80211_chanhist     ic_chanhist[IEEE80211_CHAN_MAXHIST];
                                              /*history of channel change*/

#if WLAN_SUPPORT_PRIMARY_ALLOWED_CHAN
     struct primary_allowed_chanlist *ic_primary_chanlist;
     bool                         ic_primary_allowed_enable;
#endif

    struct mi_node *ic_miroot; /* EXTAP MAC - IP table Root */

    /* regulatory class ids */
    u_int                         ic_nregclass;  /* # entries in ic_regclassids */
    u_int8_t                      ic_regclassids[IEEE80211_REGCLASSIDS_MAX];

    struct ieee80211_node_table   ic_sta; /* stations/neighbors */

#if IEEE80211_DEBUG_NODELEAK
    TAILQ_HEAD(, ieee80211_node)  ic_nodes;/* information of all nodes */
    ieee80211_node_lock_t         ic_nodelock;    /* on the list */
#endif
    os_timer_t                    ic_cw_timer;
    os_timer_t                    ic_inact_timer;
    os_timer_t                    ic_obss_nb_ru_tolerance_timer; /* Obss Narrow Bandwidth tolerance timer */
    uint32_t                      ic_obss_nb_ru_tolerance_time; /* Current Obss Narrow Bandwidth tolerance time */
#if UMAC_SUPPORT_WNM
    os_timer_t                    ic_bssload_timer;
    u_int32_t                     ic_wnm_bss_count;
    u_int32_t                     ic_wnm_bss_active;

#endif

    /* Default table for WME params */
    struct wme_phyParamType phyParamForAC[WME_NUM_AC][IEEE80211_MODE_MAX];
    struct wme_phyParamType bssPhyParamForAC[WME_NUM_AC][IEEE80211_MODE_MAX];
    /* XXX multi-bss: split out common/vap parts? */
    struct ieee80211_wme_state    ic_wme;  /* WME/WMM state */
    struct ieee80211_wme_tspec    ic_sigtspec; /* Signalling tspec */
    struct ieee80211_wme_tspec    ic_datatspec; /* Data tspec */

    /* Default MUEDCA parameters */
    struct muedcaParams           ic_muedca_defaultParams[MUEDCA_NUM_AC];
    uint8_t ic_muedca_mode_state;
    uint8_t ic_punctured_band;      /* Punctured Band setting */
    /* ic_he_mbssid_ctrl_frame_config:
     * B0: Basic Trigger setting
     * B1: BSR Trigger setting
     * B2: MU-RTS setting
     */
    uint8_t                       ic_he_mbssid_ctrl_frame_config;
    uint8_t                       ic_non_ht_dup; /* B0: Non-HT Dup beacon setting
                                                  * B1: Non-HT Dup Bcast Probe resp setting
                                                  * B2: Non-HT Dup FILS Discovery setting
                                                  */

    enum ieee80211_protmode       ic_protmode;    /* 802.11g protection mode */
    u_int16_t                     ic_sta_assoc;    /* stations associated */
    u_int16_t                     ic_nonerpsta;   /* # non-ERP stations */
    u_int16_t                     ic_longslotsta; /* # long slot time stations */
    u_int16_t                     ic_ht_sta_assoc;/* ht capable stations */
#if ATH_TxBF_DYNAMIC_LOF_ON_N_CHAIN_MASK
    u_int16_t                     ic_ht_txbf_sta_assoc;/* ht txbf capable stations */
#endif
    u_int16_t                     ic_ht_gf_sta_assoc;/* ht greenfield capable stations */
    u_int16_t                     ic_ht40_sta_assoc; /* ht40 capable stations */
    systime_t                     ic_last_non_ht_sta; /* the last time a non ht sta is seen on channel */
    systime_t                     ic_last_nonerp_present; /* the last time a non ERP beacon is seen on channel */
    int8_t                        ic_ht20_only;

    /*
     *  current channel max power, used to compute Power Constraint IE.
     *
     *  NB: local power constraint depends on the channel, but assuming it must
     *     be detected dynamically, we cannot maintain a table (i.e., will not
     *     know value until change to channel and detect).
     */
    u_int8_t                      ic_curchanmaxpwr;
    u_int8_t                      ic_chanchange_tbtt;
    u_int8_t                      ic_rcsa_count;
    uint16_t                      ic_chanchange_chan_freq;
    u_int8_t                      ic_chanchange_secoffset;
    u_int16_t                     ic_chanchange_chwidth;
    struct ieee80211_ath_channel      *ic_chanchange_channel;
    u_int64_t                     ic_chanchange_chanflag; //e.g. IEEE80211_CHAN_11AC_VHT40PLUS
    u_int32_t                     ic_mcst_of_rootap; /* Maximum channel switch time received from the root AP */
    bool                          ic_has_rootap_done_cac; /* Check if the RootAP has done CAC in the target channel */
    uint8_t                       ic_opclass_tbl_idx; /* Table index used per operating classes defined in Appendix E */
    /*
     * Maximum bw (corresponding operating triplets) to be enabled in the
     * Country IE for 6G.
     */
    uint8_t                       ic_enable_additional_triplets;
    struct ieee80211_ath_channel     *ic_tx_next_ch;
#if !ATH_SUPPORT_STATS_APONLY
    struct ieee80211_phy_stats    ic_phy_stats[IEEE80211_MODE_MAX]; /* PHY counters */
#endif

    u_int32_t                     ic_os_monrxfilter;
    ieee80211_resmgr_t            ic_resmgr; /* opage handle to resource manager */

    ieee80211_tsf_timer_mgr_t     ic_tsf_timer;     /* opaque handle to TSF Timer manager */

    ieee80211_notify_tx_bcn_mgr_t ic_notify_tx_bcn_mgr; /* opaque handle to "Notify Tx Beacon" manager */

    u_int8_t interface_id;       /* wifi interface number. eg. 1 for wifi1 */
    /* sta_not_connected_cfg: keeps track of channel configuration.
     * 0 - Proper channel configured,
     * 1 - channel zero configured to help scanning.
     */
    bool sta_not_connected_cfg;
    bool vap_down_in_progress;   /* 1 - vap down in progress, 0 - no pending vap down. */

    IEEE80211_REG_PARAMETERS      ic_reg_parm;
    wlan_dev_event_handler_table  *ic_evtable[IEEE80211_MAX_DEVICE_EVENT_HANDLERS];
    void                          *ic_event_arg[IEEE80211_MAX_DEVICE_EVENT_HANDLERS];

    struct asf_print_ctrl         ic_print;
    u_int8_t                      ic_num_ap_vaps;       /* Number of AP VAPs */
    u_int8_t                      ic_num_lp_iot_vaps;   /* Number of AP VAPs in lp_iot mode*/
    u_int8_t                      ic_need_vap_reinit;   /* Flag to re-intialize all VAPs */
    u_int8_t                      ic_tspec_active;      /* TSPEC is negotiated */
    u_int8_t                      cw_inter_found;   /* Flag to handle CW interference */
    u_int8_t                      ic_eacs_done;     /* Flag to indicate eacs status */
    ieee80211_acs_t               ic_acs;   /* opaque handle to acs */
    uint16_t                      ic_obss_done_freq; /* Obss done freq */
    struct external_acs_obj       ic_extacs_obj;    /* object for external channel selection */
    ieee80211_cbs_t               ic_cbs;   /* opaque handle to cbs */
    u_int32_t                     ic_acs_ctrlflags;   /* Flags to control ACS*/
    struct ieee80211_key          ic_bcast_proxykey;   /* default/broadcast for proxy STA */
    bool                          ic_softap_enable; /* For Lenovo SoftAP */

#ifdef ATH_BT_COEX
    enum ieee80211_opmode         ic_bt_coex_opmode; /* opmode for BT coex */
#endif

    u_int32_t                     ic_minframesize; /* Min framesize that can be recelived */

    /*
     * Table of registered cipher modules.
     */
    const struct ieee80211_cipher *ciphers[IEEE80211_CIPHER_MAX];
    int                           ic_ldpcsta_assoc;

    u_int32_t                     ic_chanbwflag;

#if ATH_SUPPORT_DSCP_OVERRIDE
    #define NUM_DSCP_MAP 16
    #define IP_DSCP_SHIFT 2
    #define IP_DSCP_MASK 0x3f
    #define IP_DSCP_MAP_LEN 64
    u_int32_t ic_dscp_tid_map[NUM_DSCP_MAP][IP_DSCP_MAP_LEN];
    u_int8_t ic_override_dscp;
    u_int8_t ic_override_igmp_dscp;
    u_int8_t ic_dscp_igmp_tid;
    u_int8_t ic_override_hmmc_dscp;
    u_int8_t ic_dscp_hmmc_tid;
#endif
#if UMAC_SUPPORT_ADMCTL
    void (*ic_node_update_dyn_uapsd)(struct ieee80211_node *ni, uint8_t ac, int8_t deli, int8_t trig);
    struct ieee80211_admctl_rt *ic_admctl_rt[IEEE80211_MODE_MAX];
    u_int16_t                     ic_mediumtime_reserved;
#endif
    u_int16_t                     ic_custom_scan_order[IEEE80211_CUSTOM_SCAN_ORDER_MAXSIZE];
    u_int32_t                     ic_custom_scan_order_size;
    u_int32_t                     ic_custom_chan_list_associated[IEEE80211_CUSTOM_SCAN_ORDER_MAXSIZE];
    u_int32_t                     ic_custom_chanlist_assoc_size;
    u_int32_t                     ic_custom_chan_list_nonassociated[IEEE80211_CUSTOM_SCAN_ORDER_MAXSIZE];
    u_int32_t                     ic_custom_chanlist_nonassoc_size;
    u_int32_t                     ic_use_custom_chan_list;

    /* virtual ap create/delete */
    struct ieee80211vap     *(*ic_vap_create_pre_init)(struct vdev_mlme_obj *, int flags);
    QDF_STATUS              (*ic_vap_create_post_init)(struct vdev_mlme_obj *, int flags);
    QDF_STATUS              (*ic_vap_create_init)(struct vdev_mlme_obj *);
    QDF_STATUS              (*ic_nss_vap_create)(struct vdev_mlme_obj *);
    QDF_STATUS              (*ic_nss_vap_destroy)(struct wlan_objmgr_vdev *);
    void                    (*ic_vap_delete)(struct wlan_objmgr_vdev *);
    void                    (*ic_vap_post_delete)(struct ieee80211vap *);
    void                    (*ic_vap_free)(struct ieee80211vap *);
    int                     (*ic_vap_alloc_macaddr) (struct ieee80211com *ic, u_int8_t *bssid);
    int                     (*ic_vap_free_macaddr) (struct ieee80211com *ic, u_int8_t *bssid);

    /* send management frame to driver, like hardstart */
    int                     (*ic_mgtstart)(struct ieee80211com *, wbuf_t);

    /* reset device state after 802.11 parameter/state change */
    int                     (*ic_init)(struct ieee80211com *);
    int                     (*ic_reset_start)(struct ieee80211com *, bool no_flush);
    int                     (*ic_reset)(struct ieee80211com *);
    int                     (*ic_reset_end)(struct ieee80211com *, bool no_flush);
    int                     (*ic_start)(struct ieee80211com *);
    int                     (*ic_stop)(struct ieee80211com *, bool flush);

    /* macaddr */
    int                     (*ic_set_macaddr)(struct ieee80211com *, u_int8_t *macaddr);

    /* regdomain */
    void                    (*ic_get_currentCountry)(struct ieee80211com *,
                                                     IEEE80211_COUNTRY_ENTRY *ctry);
    int                     (*ic_set_country)(struct ieee80211com *, char *isoName, u_int16_t cc, enum ieee80211_clist_cmd cmd);
    int                     (*ic_set_regdomain)(struct ieee80211com *, uint32_t regdomain, bool no_chanchange);
    bool                    (*ic_pdev_is_2ghz_supported)(struct ieee80211com *);
    int                     (*ic_get_regdomain)(struct ieee80211com *);
    int                     (*ic_set_quiet)(struct ieee80211_node *, u_int8_t *quiet_elm);
    void                    (*ic_set_bcn_offload_quiet_mode) (wlan_if_t vap, struct ieee80211com *ic);

    /* update device state for 802.11 slot time change */
    void                    (*ic_updateslot)(struct ieee80211com *);

    /* new station association callback/notification */
    void                    (*ic_newassoc)(struct ieee80211_node *, int);

    /* notify received beacon */
    void                    (*ic_beacon_update)(struct ieee80211_node *, int rssi);

    void                    (*ic_beacon_free)(struct ieee80211vap *vap);
    void                    (*ic_prb_rsp_free)(struct ieee80211vap *vap);
    /* node state management */
    struct ieee80211_node   *(*ic_node_alloc)(struct ieee80211vap *, const u_int8_t *macaddr, bool tmpnode, void *peer);
    void                    (*ic_node_free)(struct ieee80211_node *);
    void                    (*ic_preserve_node_for_fw_delete_resp)(struct ieee80211_node *);
    void                    (*ic_node_cleanup)(struct ieee80211_node *);
    u_int8_t                (*ic_node_getsnr)(const struct ieee80211_node *, int8_t ,u_int8_t);
#if QCA_SUPPORT_PEER_ISOLATION
    void                    (*ic_node_peer_isolation)(struct ieee80211_node *, bool);
#endif
    u_int32_t               (*ic_node_getrate)(const struct ieee80211_node *, u_int8_t type);
    void                    (*ic_node_authorize)(struct ieee80211_node *,u_int32_t authorize);
    void                    (*ic_get_min_and_max_power)(struct ieee80211com *ic,
                                                        int8_t *max_tx_power,
                                                        int8_t *min_tx_power);
    bool                    (*ic_is_regulatory_offloaded)(struct ieee80211com *ic);
    uint32_t                (*ic_get_modeSelect)(struct ieee80211com *ic);
    uint32_t                (*ic_get_chip_mode)(struct ieee80211com *ic);
    void                    (*ic_fill_hal_chans_from_reg_db)(struct ieee80211com *ic);
    u_int32_t                (*ic_node_get_last_txpower)(const struct ieee80211_node *);
    /* scanning support */
    void                    (*ic_led_scan_start)(struct ieee80211com *);
    void                    (*ic_led_scan_end)(struct ieee80211com *);
    int                     (*ic_set_channel)(struct ieee80211com *);
    void                    (*ic_enable_radar)(struct ieee80211com *, int no_cac);
    void                    (*ic_process_rcsa)(struct ieee80211com *);

#if ATH_SUPPORT_DFS && ATH_SUPPORT_STA_DFS
    void                    (*ic_enable_sta_radar)(struct ieee80211com *, int no_cac);
#endif
    /* change power state */
    void                    (*ic_pwrsave_set_state)(struct ieee80211com *, IEEE80211_PWRSAVE_STATE newstate);

    /* update protmode */
    void                    (*ic_update_protmode)(struct ieee80211com *);

    /* get hardware txq depth */
    u_int32_t               (*ic_txq_depth)(struct ieee80211com *);
    /* get hardware txq depth per ac*/
    u_int32_t               (*ic_txq_depth_ac)(struct ieee80211com *, int ac);

    /* channel width change */
    void                    (*ic_chwidth_change)(struct ieee80211_node *ni);

    /* Nss change */
    void                    (*ic_nss_change)(struct ieee80211_node *ni);

    void                    (*ic_ext_nss_change)(struct ieee80211_node *ni, uint8_t *peer_update_count);

    /* Frame injector config */
    int                     (*ic_frame_injector_config)(struct wlan_objmgr_vdev *vdev,
                                                        u_int32_t frametype,
                                                        u_int32_t enable,
                                                        u_int32_t inject_period,
                                                        u_int32_t duration,
                                                        u_int8_t *dstmac);

    /* beeliner Firmware Test command */
    void                    (*ic_ar900b_fw_test)(struct ieee80211com *ic, u_int32_t arg, u_int32_t value);
    /* Firmware Unit Test command Lithium onwards*/
    int32_t                 (*ic_fw_unit_test)(struct wlan_objmgr_vdev *, struct ieee80211_fw_unit_test_cmd *);

    /* coex configuration */
    int                     (*ic_coex_cfg)(struct wlan_objmgr_vdev *vdev, u_int32_t type, u_int32_t *arg);

#if UNIFIED_SMARTANTENNA
    /* Dynamic antenna switch */
    void                    (*ic_set_ant_switch)(struct ieee80211com *ic, u_int32_t ctrl_cmd_1, u_int32_t ctrl_cmd_2);
#endif
    /* Set user control table */
    void                    (*ic_set_ctrl_table)(struct ieee80211com *ic, u_int8_t *ctl_array, u_int16_t ctl_len);
    /* aggregation support */
    u_int8_t                ic_addba_mode; /* ADDBA mode auto or manual */
    void                    (*ic_set_ampduparams)(struct ieee80211_node *ni);
    void                    (*ic_set_weptkip_rxdelim)(struct ieee80211_node *ni, u_int8_t rxdelim);
    void                    (*ic_addba_requestsetup)(struct ieee80211_node *ni,
                                                     u_int8_t tidno,
                                                     struct ieee80211_ba_parameterset *baparamset,
                                                     u_int16_t *batimeout,
                                                     struct ieee80211_ba_seqctrl *basequencectrl,
                                                     u_int16_t buffersize
                                                     );
    int                     (*ic_addba_responsesetup)(struct ieee80211_node *ni,
                                                      u_int8_t tidno,
                                                      u_int8_t *dialogtoken, u_int16_t *statuscode,
                                                      struct ieee80211_ba_parameterset *baparamset,
                                                      u_int16_t *batimeout
                                                      );
    int                     (*ic_addba_resp_tx_completion)(struct ieee80211_node *ni,
                                                       u_int8_t tidno, int status
                                                       );
    int                     (*ic_addba_requestprocess)(struct ieee80211_node *ni,
                                                      u_int8_t dialogtoken,
                                                      struct ieee80211_ba_parameterset *baparamset,
                                                      u_int16_t batimeout,
                                                      struct ieee80211_ba_seqctrl basequencectrl);
    void                    (*ic_addba_responseprocess)(struct ieee80211_node *ni,
                                                        u_int16_t statuscode,
                                                        struct ieee80211_ba_parameterset *baparamset,
                                                        u_int16_t batimeout
                                                        );
    void                    (*ic_addba_clear)(struct ieee80211_node *ni);
    void                    (*ic_delba_process)(struct ieee80211_node *ni,
                                                struct ieee80211_delba_parameterset *delbaparamset,
                                                u_int16_t reasoncode
                                                );
    int                     (*ic_addba_send)(struct ieee80211_node *ni,
                                             u_int8_t tid,
                                             u_int16_t buffersize);
    void                    (*ic_delba_send)(struct ieee80211_node *ni,
                                             u_int8_t tid,
                                             u_int8_t initiator,
                                             u_int16_t reasoncode);
    void                    (*ic_addba_status)(struct ieee80211_node *ni,
                                               u_int8_t tid,
                                               u_int16_t *status);
    void                    (*ic_addba_setresponse)(struct ieee80211_node *ni,
                                                    u_int8_t tid,
                                                    u_int16_t statuscode);
    int                     (*ic_delba_tx_completion)(struct ieee80211_node *ni,
                                                u_int8_t tidno, int status);
    void                    (*ic_send_singleamsdu)(struct ieee80211_node *ni,
                                                    u_int8_t tid);
    void                    (*ic_addba_clearresponse)(struct ieee80211_node *ni);
    void                    (*ic_sm_pwrsave_update)(struct ieee80211_node *ni, int, int, int);
    /* update station power save state when operating in AP mode */
    void                    (*ic_node_psupdate)(struct ieee80211_node *, int, int);

    /* To get the number of frames queued up in lmac */
    int                     (*ic_node_queue_depth)(struct ieee80211_node *);

    int16_t                 (*ic_get_noisefloor)(struct ieee80211com *, struct ieee80211_ath_channel *, int);
    void                    (*ic_get_chainnoisefloor)(struct ieee80211com *, struct ieee80211_ath_channel *, int16_t *);
#if ATH_SUPPORT_VOW_DCS
    void                    (*ic_disable_dcsim)(struct ieee80211com *);
    void                    (*ic_enable_dcsim)(struct ieee80211com *);
#endif
    void                    (*ic_disable_dcscw)(struct ieee80211com *);
    void                    (*ic_dcs_restore)(struct ieee80211com *);
    void                    (*ic_set_config)(struct ieee80211vap *);
    bool                    (*ic_set_config_enable)(struct ieee80211vap *);

    void                    (*ic_set_safemode)(struct ieee80211vap *, int);
        /* rx monitor filter */
    int                     (*ic_set_rx_monitor_filter)(struct ieee80211com *, uint32_t);
	/* rx filter related */
    void                    (*ic_set_privacy_filters)(struct ieee80211vap *);

    int                     (*ic_rmgetcounters)(struct ieee80211com *ic, struct ieee80211_mib_cycle_cnts *pCnts);

    void                    (*ic_set_rx_sel_plcp_header)(struct ieee80211com *ic,
                                                         int8_t sel, int8_t query);

    int                     (*ic_subscribe_csa_interop_phy)(struct ieee80211com *ic,
                                                            bool subscribe);

    u_int8_t                (*ic_rcRateValueToPer)(struct ieee80211com *ic, struct ieee80211_node *ni, int txRateKbps);

    /* to get TSF values */
    u_int32_t               (*ic_get_TSF32)(struct ieee80211com *ic);
#ifdef ATH_USB
    /* to get generic timer expired tsf time, this is used to reduce the wmi command delay */
    u_int32_t               (*ic_get_target_TSF32)(struct ieee80211com *ic);
#endif
    u_int64_t               (*ic_get_TSF64)(struct ieee80211com *ic);

    /* To Set Transmit Power Limit */
    void                    (*ic_set_txPowerLimit)(struct wlan_objmgr_pdev *pdev,
                                                   uint32_t limit,
                                                   uint16_t tpcInDb,
                                                   uint32_t is2Ghz);

    void                    (*ic_set_txPowerAdjust)(struct ieee80211com *ic, int32_t adjust, u_int32_t is2Ghz);

    /* To get Transmit power in 11a common mode */
    u_int8_t                (*ic_get_common_power)(struct ieee80211com *ic, struct ieee80211_ath_channel *chan);

    /* To get Maximum Transmit Phy rate */
    u_int32_t               (*ic_get_maxphyrate)(struct ieee80211com *ic, struct ieee80211_node *ni);

    /* Set Rx Filter */
    void                    (*ic_set_rxfilter)(struct wlan_objmgr_pdev *pdev,
                                               uint32_t filter);

    /* Get Mfg Serial Num */
    int                     (*ic_get_mfgsernum)(struct ieee80211com *ic, u_int8_t *pSn, int limit);

    /* Get Current RSSI */
    u_int32_t               (*ic_get_curRSSI)(struct ieee80211com *ic);
    QDF_STATUS              (*ic_set_peer_latency_param_config)(struct ieee80211com *ic,
                                        uint8_t *peer_mac, uint32_t tid,
                                        uint8_t dl_ul_enable,
                                        uint32_t service_interval,
                                        uint32_t burst_size,
                                        uint32_t add_or_sub);

#ifdef ATH_SWRETRY
    /* Enable/Disable software retry */
    void                    (*ic_set_swretrystate)(struct ieee80211_node *ni, int flag);
    /* Schedule one single frame in LMAC upon PS-Poll, return 0 if scheduling a LMAC frame successfully */
    int                     (*ic_handle_pspoll)(struct ieee80211com *ic, struct ieee80211_node *ni);
    /* Check whether there is pending frame in LMAC tid Q's, return 0 if there exists */
    int                     (*ic_exist_pendingfrm_tidq)(struct ieee80211com *ic, struct ieee80211_node *ni);
    int                     (*ic_reset_pause_tid)(struct ieee80211com *ic, struct ieee80211_node *ni);
#endif
    u_int32_t               (*ic_get_wpsPushButton)(struct ieee80211com *ic);
    void            (*ic_green_ap_set_enable)( struct ieee80211com *ic, int val );
    int             (*ic_green_ap_get_enable)( struct ieee80211com *ic);
    void            (*ic_green_ap_set_print_level)(struct ieee80211com *ic, int val);
    int             (*ic_green_ap_get_print_level)(struct ieee80211com *ic);
    int16_t         (*ic_get_cur_chan_nf)(struct ieee80211com *ic);
    int16_t         (*ic_get_cur_hw_nf)(struct ieee80211com *ic);
    bool            (*ic_is_target_lithium)(struct wlan_objmgr_psoc *psoc);
    void            (*ic_get_cur_chan_stats)(struct ieee80211com *ic, struct ieee80211_chan_stats *chan_stats);
    int32_t         (*ic_ath_send_rssi)(struct ieee80211com *ic, u_int8_t *macaddr, struct ieee80211vap *vap);
    int32_t         (*ic_ath_request_stats)(struct ieee80211com *ic,  void *cmd);
    int32_t         (*ic_ath_enable_ap_stats)(struct ieee80211com *ic, u_int8_t val);
    int32_t         (*ic_ath_bss_chan_info_stats)(struct ieee80211com *ic, int param);
    /* update PHY counters */
    void                    (*ic_update_phystats)(struct ieee80211com *ic, enum ieee80211_phymode mode);
    void                    (*ic_clear_phystats)(struct ieee80211com *ic);
    void                    (*ic_log_text)(struct ieee80211com *ic,char *text);
    void                    (*ic_log_text_bh)(struct ieee80211com *ic,char *text);
    int                     (*ic_set_chain_mask)(struct ieee80211com *ic, ieee80211_device_param type,
                                                 u_int32_t mask);
    bool                    (*ic_need_beacon_sync)(struct ieee80211com *ic); /* check if ath is waiting for beacon sync */
#if IEEE80211_DEBUG_NODELEAK
    void                    (*ic_print_nodeq_info)(struct ieee80211_node *ni);
#endif
    void                    (*ic_bss_to40)(struct ieee80211com *);
    void                    (*ic_bss_to20)(struct ieee80211com *);

    enum ieee80211_cwm_extprotmode    (*ic_cwm_get_extprotmode)(struct ieee80211com *ic);
    enum ieee80211_cwm_extprotspacing (*ic_cwm_get_extprotspacing)(struct ieee80211com *ic);
    enum ieee80211_cwm_mode           (*ic_cwm_get_mode)(struct ieee80211com *ic);
    enum ieee80211_cwm_width          (*ic_cwm_get_width)(struct ieee80211com *ic);
    u_int32_t                         (*ic_cwm_get_enable)(struct ieee80211com *ic);
    u_int32_t                         (*ic_cwm_get_extbusythreshold)(struct ieee80211com *ic);
    int8_t                            (*ic_cwm_get_extoffset)(struct ieee80211com *ic);

    void                              (*ic_cwm_set_extprotmode)(struct ieee80211com *ic, enum ieee80211_cwm_extprotmode mode);
    void                              (*ic_cwm_set_extprotspacing)(struct ieee80211com *ic, enum ieee80211_cwm_extprotspacing sp);
    void                              (*ic_cwm_set_enable)(struct ieee80211com *ic, u_int32_t en);
    void                              (*ic_cwm_set_extoffset)(struct ieee80211com *ic, int8_t val);
    void                              (*ic_cwm_set_extbusythreshold)(struct ieee80211com *ic, u_int32_t threshold);
    void                              (*ic_cwm_set_mode)(struct ieee80211com *ic, enum ieee80211_cwm_mode mode);
    void                              (*ic_cwm_set_width)(struct ieee80211com *ic, enum ieee80211_cwm_width width);

    struct ieee80211_tsf_timer *
                            (*ic_tsf_timer_alloc)(struct ieee80211com *ic,
                                                    tsftimer_clk_id clk_id,
                                                    ieee80211_tsf_timer_function trigger_action,
                                                    ieee80211_tsf_timer_function overflow_action,
                                                    ieee80211_tsf_timer_function outofrange_action,
                                                    void *arg);

    void                    (*ic_tsf_timer_free)(struct ieee80211com *ic, struct ieee80211_tsf_timer *timer);

    void                    (*ic_tsf_timer_start)(struct ieee80211com *ic, struct ieee80211_tsf_timer *timer,
                                                    u_int32_t timer_next, u_int32_t period);

    void                    (*ic_tsf_timer_stop)(struct ieee80211com *ic, struct ieee80211_tsf_timer *timer);

#define IEEE80211_IS_TAPMON_ENABLED(__ic) 0
#if ATH_SUPPORT_WIRESHARK
    void            (*ic_fill_radiotap_hdr)(struct ieee80211com *ic, struct ath_rx_radiotap_header *rh, wbuf_t wbuf);
#endif /* ATH_SUPPORT_WIRESHARK */
#ifdef ATH_BT_COEX
    int                     (*ic_get_bt_coex_info)(struct ieee80211com *ic, u_int32_t infoType);
#endif
    /* Get MFP support */
    u_int32_t               (*ic_get_mfpsupport)(struct ieee80211com *ic);

    /* Set hw MFP Qos bits */
    void                    (*ic_set_hwmfpQos)(struct wlan_objmgr_pdev *pdev,
                                               uint32_t dot11w);
#ifdef ATH_SUPPORT_IQUE
    /* Set IQUE parameters: AC, Rate Control, and HBR parameters */
    void        (*ic_set_acparams)(struct ieee80211com *ic, u_int8_t ac, u_int8_t use_rts,
                                   u_int8_t aggrsize_scaling, u_int32_t min_kbps);
    void        (*ic_set_rtparams)(struct ieee80211com *ic, u_int8_t rt_index,
                                   u_int8_t perThresh, u_int8_t probeInterval);
    void        (*ic_get_iqueconfig)(struct ieee80211com *ic);
    void        (*ic_set_hbrparams)(struct ieee80211vap *, u_int8_t ac, u_int8_t enable, u_int8_t per);
#endif /*ATH_SUPPORT_IQUE*/

    u_int32_t   (*ic_get_goodput)(struct ieee80211_node *ni);

#if WLAN_SPECTRAL_ENABLE

    /* SPECTRAL Related data */
    int     (*ic_spectral_control)(struct ieee80211com *ic, u_int id,
                              void *indata, u_int32_t insize,
                              void *outdata, u_int32_t *outsize);

    /*  EACS with spectral analysis*/
    void                 (*ic_start_spectral_scan)(struct ieee80211com *ic, u_int8_t priority);
    void                 (*ic_stop_spectral_scan)(struct ieee80211com *ic);

#endif
#if ATH_SLOW_ANT_DIV
    void                 (*ic_antenna_diversity_suspend)(struct ieee80211com *ic);
    void                 (*ic_antenna_diversity_resume)(struct ieee80211com *ic);
#endif

    u_int8_t                (*ic_get_ctl_by_country)(struct ieee80211com *ic, u_int8_t *country, bool is2G);

    int                     (*ic_reg_notify_tx_bcn)(struct ieee80211com *ic,
                                                    ieee80211_tx_bcn_notify_func callback,
                                                    void *arg);
    int                     (*ic_dereg_notify_tx_bcn)(struct ieee80211com *ic);

    int                     (*ic_vap_pause_control)(struct ieee80211com *ic, struct ieee80211vap *vap, bool pause); /* if vap is null, pause all vaps */
    void                    (*ic_enable_rifs_ldpcwar) (struct ieee80211_node *ni, bool value);
    void                    (*ic_process_uapsd_trigger) (struct ieee80211com *ic, struct ieee80211_node *ni, bool enforce_sp_limit, bool *sent_eosp);
    int                     (*ic_is_hwbeaconproc_active) (struct ieee80211com *ic);
    void                    (*ic_hw_beacon_rssi_threshold_enable)(struct ieee80211com *ic,
                                                u_int32_t rssi_threshold);
    void                    (*ic_hw_beacon_rssi_threshold_disable)(struct ieee80211com *ic);

#if UMAC_SUPPORT_VI_DBG
    struct ieee80211_vi_dbg   *ic_vi_dbg;
    struct ieee80211_vi_dbg_params  *ic_vi_dbg_params;
    void   (*ic_set_vi_dbg_restart)(struct ieee80211com *ic);
    void   (*ic_set_vi_dbg_log) (struct ieee80211com *ic, bool enable);
#endif
    void   (*ic_set_noise_detection_param) (struct ieee80211com *ic, int cmd, int val);
    void   (*ic_get_noise_detection_param) (struct ieee80211com *ic, int cmd, int *val);
    struct ieee80211_quiet_param *ic_quiet;

    void        (*ic_rx_intr_mitigation)(struct ieee80211com *ic, u_int32_t enable);
    void        (*ic_set_beacon_interval)(struct ieee80211com *ic);
    u_int32_t   (*ic_get_txbuf_free) (struct ieee80211com *ic);
    void        (*ic_register_beacon_tx_status_event_handler)
                (struct ieee80211com *ic, bool unregister);
    void        (*ic_config_bsscolor_offload)
                (struct ieee80211vap *vap, bool disable);
    bool        (*ic_is_ifce_allowed_in_dynamic_mode) (void *scn);
    void        (*ic_assign_mbssid_ref_bssid) (void *scn, bool partially_random);

#ifdef ATH_HTC_MII_RXIN_TASKLET
    qdf_nbuf_queue_t      ic_mgmt_nbufqueue;
    os_mgmt_lock_t        ic_mgmt_lock;
    atomic_t              ic_mgmt_deferflags;

    TAILQ_HEAD(, ieee80211_node)  ic_nodeleave_queue;
    os_nodeleave_lock_t           ic_nodeleave_lock;
    atomic_t                      ic_nodeleave_deferflags;
    TAILQ_HEAD(, _nawds_dentry)       ic_nawdslearnlist;
    os_nawdsdefer_lock_t          ic_nawdsdefer_lock;
    atomic_t                      ic_nawds_deferflags;
#endif
#ifdef DBG
    u_int32_t               (*ic_hw_reg_read)(struct ieee80211com *ic, u_int32_t);
#endif
    qdf_work_t  assoc_sm_set_country_code;
    struct assoc_sm_set_country *set_country;
#ifdef ATH_SUPPORT_DFS
    qdf_work_t  dfs_cac_timer_start_work;
    u_int32_t   ic_dfs_waitfor_csa_sched:1; /* Waiting for CSA from the uplink */
    os_timer_t  ic_dfs_waitfor_csa_timer;   /* Wait for CSA from parent Repeater/Root timer */
    os_timer_t  ic_dfs_tx_rcsa_and_nol_ie_timer; /* dfs RCSA(reverse CSA) and NOL IE timer */
#if ATH_SUPPORT_DFS && ATH_SUPPORT_STA_DFS
    void          *ic_dfs_flags;
#endif

    /*
     * Update the channel list with the current set of DFS
     * NOL entries.
     *
     * + 'cmd' indicates what to do; for now it should just
     *   be DFS_NOL_CLIST_CMD_UPDATE which will update all
     *   channels, given the _entire_ NOL. (Rather than
     *   the earlier behaviour with clist_update, which
     *   was to either add or remove a set of channel
     *   entries.)
     */
    void        (*ic_dfs_start_tx_rcsa_and_waitfor_rx_csa)(struct ieee80211com *ic);
#endif
    void        (*ic_dprintf_ic)(struct ieee80211com *ic, unsigned verbose,
                                 unsigned category, const char *fmt, ...);

    void        (*ic_get_ext_chan_info)(struct ieee80211com *ic,
                                        struct ieee80211_ath_channel *chan,
                                        struct ieee80211_ath_channel_list *chan_info);
    struct ieee80211_ath_channel *(*ic_find_channel)(struct ieee80211com *ic,
                                                     int freq,
                                                     uint16_t des_cfreq2,
                                                     uint64_t flags);

    u_int       (*ic_chan2freq)(struct ieee80211com *ic,
                                const struct ieee80211_ath_channel *c);
    int         (*ic_find_any_valid_channel)(struct ieee80211com *ic,
                                             uint32_t chan_mode);

    u_int64_t                           (*ic_get_tx_hw_retries)(struct ieee80211com *ic);
    u_int64_t                           (*ic_get_tx_hw_success)(struct ieee80211com *ic);
    void                                (*ic_rate_node_update)(struct ieee80211_node *ni,
                                                               int isnew);

    /* isr dynamic aponly mode control */
    bool                                ic_aponly;

#if ATH_SUPPORT_FLOWMAC_MODULE
    int                                 (*ic_get_flowmac_enabled_State)(struct ieee80211com *ic);
#endif
    int                                 (*ic_get_rx_signal_dbm)(struct ieee80211com *ic, int8_t *signal_dbm);
    /* To store the id_mask of vaps which has downed by software,when actual physical radio interface is downed.*/
    u_int32_t                           id_mask_vap_downed[VDEV_BITMAP_SLOTS];
#if WLAN_SPECTRAL_ENABLE
    u_int32_t                           chan_clr_cnt;
    u_int32_t                           cycle_cnt;
    u_int32_t                           chan_freq;
#endif
    ieee80211_resmgr_t                  (*ic_resmgr_create)(struct ieee80211com *ic, ieee80211_resmgr_mode mode);
    void                                (*ic_vdev_beacon_template_update)(struct ieee80211vap *vap);
    int                                 (*ic_beacon_offload_control)(struct ieee80211vap *vap, u_int32_t bcn_ctrl_op);
    QDF_STATUS                          (*ic_prb_rsp_tmpl_send)(struct wlan_objmgr_vdev *);
    QDF_STATUS                          (*ic_vap_20tu_prb_rsp_init)(struct ieee80211vap *vap);
    void                                (*ic_prb_rsp_tmpl_alloc)(struct ieee80211vap *vap);
    void                                (*ic_vdev_prb_rsp_tmpl_update)(struct ieee80211vap *vap);
    QDF_STATUS                          (*ic_bcn_tmpl_send)(struct wlan_objmgr_vdev *);
    QDF_STATUS                          (*ic_fd_tmpl_send)(struct wlan_objmgr_vdev *);
    QDF_STATUS                          (*ic_fd_tmpl_update)(struct wlan_objmgr_vdev *);
    void                                (*ic_update_restart_param)(struct ieee80211vap *vap,
                                                                   bool reset,
                                                                   bool restart_success);
    int                                 (*ic_update_phy_mode)(struct mlme_channel_param *ch_param, struct ieee80211com *ic);
    void                                (*ic_incr_peer_count)(struct ieee80211com *ic, void *ni);
    int                                 (*ic_vap_set_param)(struct ieee80211vap *vap,
                                         ieee80211_param param, u_int32_t val);
    int                                 (*ic_vap_config_tid_latency_param)(struct ieee80211vap *vap,
                                         u_int32_t service_interval, u_int32_t burst_size,
										 u_int32_t latency_tid, u_int8_t dl_ul_enable);
    int                                 (*ic_vap_sifs_trigger)(struct ieee80211vap *vap,
                                         u_int32_t val);
    int                                 (*ic_vap_get_param)(struct ieee80211vap *vap,
                                         ieee80211_param param);
    int                                 (*ic_ol_net80211_set_mu_whtlist)(wlan_if_t vap,
                                         u_int8_t *macaddr, u_int16_t tidmask);
    int                                 (*ic_node_add_wds_entry)(void *vdev_handle,
                                         const u_int8_t *dest_mac, u_int8_t *peer_mac, u_int32_t flags);
    void                                (*ic_node_del_wds_entry)(void *vdev_handle,
                                         u_int8_t *dest_mac);
#ifdef AST_HKV1_WORKAROUND
    int                                 (*ic_node_lookup_wds_and_del)(void *vdev_handle,
                                         uint8_t *mac,
					 struct recv_auth_params_defer *auth_cookie);
#endif
    int                                 (*ic_vap_set_ratemask)(struct ieee80211vap *vap,
                                        u_int8_t preamble, u_int32_t mask_lower32,
                                        u_int32_t mask_higher32, u_int32_t mask_lower32_2);
    int16_t                             (*ic_vap_dyn_bw_rts)(struct ieee80211vap *vap, int param);
    int                                 (*ic_node_update_wds_entry)(void *vdev_handle,
                                         u_int8_t *wds_macaddr, u_int8_t *peer_macaddr, u_int32_t flags);
    void                                (*ic_node_set_bridge_mac_addr)(struct ieee80211com *ic,
                                         u_int8_t *bridge_mac);
    int                                 (*ic_node_use_4addr)(struct ieee80211_node *ni);
#ifdef QCA_SUPPORT_WDS_EXTENDED
    int                                 (*ic_node_wmi_send_wds_ext)(struct ieee80211_node *ni);
#endif
    int                                 (*ic_vap_set_qdepth_thresh)(struct ieee80211vap *vap,
                                         uint8_t *mac_addr, uint32_t tid,
                                         uint32_t update_mask, uint32_t thresh_val);
#if ATH_WOW
    /* Wake on Wireless used on clients to wake up the system with a magic packet */
    int                                 (*ic_wow_get_support)(struct ieee80211com *ic);
    int                                 (*ic_wow_enable)(struct ieee80211com *ic, int clearbssid);
    int                                 (*ic_wow_wakeup)(struct ieee80211com *ic);
    int                                 (*ic_wow_add_wakeup_event)(struct ieee80211com *ic, u_int32_t type);
    void                                (*ic_wow_set_events)(struct ieee80211com *ic, u_int32_t);
    int                                 (*ic_wow_get_events)(struct ieee80211com *ic);
    int                                 (*ic_wow_add_wakeup_pattern)
                                            (struct ieee80211com *ic, u_int8_t *, u_int8_t *, u_int32_t);
    int                                 (*ic_wow_remove_wakeup_pattern)(struct ieee80211com *ic, u_int8_t *, u_int8_t *);
    int                                 (*ic_wow_get_wakeup_pattern)
                                            (struct ieee80211com *ic, u_int8_t *,u_int32_t *, u_int32_t *);
    int                                 (*ic_wow_get_wakeup_reason)(struct ieee80211com *ic);
    int                                 (*ic_wow_matchpattern_exact)(struct ieee80211com *ic);
    void                                (*ic_wow_set_duration)(struct ieee80211com *ic, u_int16_t);
    void                                (*ic_wow_set_timeout)(struct ieee80211com *ic, u_int32_t);
	u_int32_t                           (*ic_wow_get_timeout)(struct ieee80211com *ic);
#endif /* ATH_WOW */
    void                                (*ic_tx_flush)(struct ieee80211com *ic);
    int                                 (*ic_set_ru26_tolerant)(struct ieee80211com *ic, bool value);

    u_int32_t                            ic_vhtcap;              /* VHT capabilities */
    ieee80211_vht_mcs_set_t              ic_vhtcap_max_mcs;      /* VHT Supported MCS set */
    u_int8_t                             ic_vht_ampdu;           /* VHT AMPDU value */
    u_int8_t                             ic_vht_amsdu;           /* VHT AMSDU value */
    u_int8_t                             ic_rx_amsdu;           /* RX AMSDU tidmask */
    u_int8_t                             ic_no_bfee_limit;       /* no MU BFEE limit */
    u_int16_t                            ic_vhtop_basic_mcs;     /* VHT Basic MCS set */
    void                                (*ic_power_attach)(struct ieee80211com *ic);
    void                                (*ic_power_detach)(struct ieee80211com *ic);
    void                                (*ic_power_vattach)(struct ieee80211vap *vap, int fullsleepEnable,
                                            u_int32_t sleepTimerPwrSaveMax, u_int32_t sleepTimerPwrSave, u_int32_t sleepTimePerf,
                                            u_int32_t inactTimerPwrsaveMax, u_int32_t inactTimerPwrsave, u_int32_t inactTimerPerf,
                                            u_int32_t smpsDynamic, u_int32_t pspollEnabled);
    void                                (*ic_power_vdetach)(struct ieee80211vap *vap);
    int                                 (*ic_power_sta_set_pspoll)(struct ieee80211vap *vap, u_int32_t pspoll);
    int                                 (*ic_power_sta_set_pspoll_moredata_handling)(struct ieee80211vap *vap, ieee80211_pspoll_moredata_handling mode);
    u_int32_t                           (*ic_power_sta_get_pspoll)(struct ieee80211vap *vap);
    ieee80211_pspoll_moredata_handling  (*ic_power_sta_get_pspoll_moredata_handling)(struct ieee80211vap *vap);
    int                                 (*ic_power_set_mode)(struct ieee80211vap* vap, ieee80211_pwrsave_mode mode);
    ieee80211_pwrsave_mode              (*ic_power_get_mode)(struct ieee80211vap* vap);
    u_int32_t                           (*ic_power_get_apps_state)(struct ieee80211vap* vap);
    int                                 (*ic_power_set_inactive_time)(struct ieee80211vap* vap, ieee80211_pwrsave_mode mode, u_int32_t inactive_time);
    u_int32_t                           (*ic_power_get_inactive_time)(struct ieee80211vap* vap, ieee80211_pwrsave_mode mode);
    int                                 (*ic_power_force_sleep)(struct ieee80211vap* vap, bool enable);
    int                                 (*ic_power_set_ips_pause_notif_timeout)(struct ieee80211vap* vap, u_int16_t pause_notif_timeout);
    u_int16_t                           (*ic_power_get_ips_pause_notif_timeout)(struct ieee80211vap* vap);
    int                                 (*ic_power_sta_send_keepalive)(struct ieee80211vap *vap);
    int                                 (*ic_power_sta_unpause)(struct ieee80211vap *vap);
    void                                (*ic_power_sta_event_dtim)(struct ieee80211vap *vap);
    void                                (*ic_power_sta_event_tim)(struct ieee80211vap *vap);
    int                                 (*ic_power_sta_unregister_event_handler)(struct ieee80211vap *vap, ieee80211_sta_power_event_handler evhandler, void *arg);
    int                                 (*ic_power_sta_register_event_handler)(struct ieee80211vap *vap, ieee80211_sta_power_event_handler evhandler, void *arg);
    void                                (*ic_power_sta_tx_start)(struct ieee80211vap *vap);
    void                                (*ic_power_sta_tx_end)(struct ieee80211vap *vap);
     /* ACS APIs for offload architecture */
    void                                (*ic_hal_get_chan_info)(struct ieee80211com *ic, u_int8_t flags);

    void                                (*ic_mcast_group_update)(struct ieee80211com *ic, int action, int wildcard, u_int8_t *mcast_ip_addr, int mcast_ip_addr_bytes, u_int8_t *ucast_mac_addr,  u_int8_t filter_mode,  u_int8_t nsrcs,  u_int8_t *srcs, u_int8_t *mask, u_int8_t vap_id);
    u_int32_t                           (*ic_num_mcast_tbl_elements)(struct ieee80211com *ic);
    u_int32_t                           (*ic_num_mcast_grps)(struct ieee80211com *ic);
#if ATH_SUPPORT_KEYPLUMB_WAR
		int 								(*ic_checkandplumb_key)(struct ieee80211vap *vap, struct ieee80211_node *ni);
#endif
#if ATH_SUPPORT_TIDSTUCK_WAR
		void					(*ic_clear_rxtid)(struct ieee80211com *ic, struct ieee80211_node *ni);
#endif
	void                                (*ic_hifitbl_update_target)(struct ieee80211vap *vap);
#if ATH_TxBF_DYNAMIC_LOF_ON_N_CHAIN_MASK
    void                                (*ic_txbf_loforceon_update)(struct ieee80211com *ic, bool loforcestate);
#endif
    void                                (*ic_set_ctl_table)(struct ieee80211com *ic, u_int8_t *ctl_array, u_int16_t ctl_len);
    void                                (*ic_set_mimogain_table)(struct ieee80211com *ic, u_int8_t *array_gain,
                                                                 u_int16_t tbl_len, bool multichain_gain_bypass);
    void                                (*ic_ratepwr_table_ops)(struct ieee80211com *ic, u_int8_t ops,
                                                               u_int8_t *ratepwr_tbl, u_int16_t ratepwr_len);
    void                                (*ic_set_ratepwr_chainmsk)(struct ieee80211com *ic, u_int32_t *ratepwr_chain_tbl,
                                                                u_int16_t num_rate, u_int8_t pream_type, u_int8_t ops);
    void                                (*ic_set_sta_fixed_rate)(struct ieee80211_node *ni);
    void                                (*ic_scan_enable_txq)(struct ieee80211com *ic);

    bool (*ic_support_phy_mode)(struct ieee80211com *ic, enum ieee80211_phymode);
    int  (*ic_get_bw_nss_mapping)(struct ieee80211vap *vap,
            struct ieee80211_bwnss_map *nssmap, u_int8_t chainmask);
    bool (*ic_rate_check)(struct ieee80211com *ic, int val);
#if QCA_PARTNER_PLATFORM
    struct partner_com_param            partner_com_params;
#endif
    int (*ic_fips_test)(struct ieee80211com *ic, wlan_if_t vap, struct ath_fips_cmd *fips_buf);
#ifdef WLAN_SUPPORT_TWT
    int (*ic_twt_req)(wlan_if_t vap, struct ieee80211req_athdbg *req);
#endif
    int (*ic_send_lcr_cmd)(struct wlan_objmgr_pdev *pdev, struct ieee80211_wlanconfig_lcr *lcr);
    int (*ic_send_lci_cmd)(struct wlan_objmgr_pdev *pdev, struct ieee80211_wlanconfig_lci *lci);

    QDF_STATUS (*ic_ema_config_init)(struct ieee80211com *ic);
    /* Set management frame retry limit */
    int                                 (*ic_set_mgmt_retry_limit)(struct wlan_objmgr_pdev *pdev,
                                                                   uint8_t);
    u_int32_t                           obss_snr_threshold; /* OBSS SNR threshold */
    u_int32_t                           obss_rx_snr_threshold; /* OBSS RX SNR threshold */
    void                                (*ic_node_pspoll)(struct ieee80211_node *ni, bool value);
    int                                 (*ic_tr69_request_process)(struct ieee80211vap *vap, int cmdid, void * arg1, void *arg2);
    bool                                (*ic_is_target_ar900b)(struct ieee80211com *ic);
    uint32_t                            (*ic_get_tgt_type)(struct ieee80211com *ic);
#if UMAC_SUPPORT_ACFG || UMAC_SUPPORT_ACFG_RECOVERY
    void                                *ic_acfg_handle;
#endif
    u_int8_t                            ic_tso_support;
    u_int8_t                            ic_lro_support;
    u_int8_t                            ic_sg_support;
    u_int8_t                            ic_gro_support;
    u_int8_t                            ic_offload_tx_csum_support;
    u_int8_t                            ic_offload_rx_csum_support;
    u_int8_t                            ic_rawmode_support;
    u_int8_t                            ic_dynamic_grouping_support;
    u_int8_t                            ic_dpd_support;
    u_int8_t                            ic_aggr_burst_support;
    u_int8_t                            ic_qboost_support;
    u_int8_t                            ic_sifs_frame_support;
    u_int8_t                            ic_block_interbss_support;
    u_int8_t                            ic_disable_reset_support;
    u_int8_t                            ic_msdu_ttl_support;
    u_int8_t                            ic_ppdu_duration_support;
    u_int8_t                            ic_promisc_support;
    u_int8_t                            ic_burst_mode_support;
    u_int8_t                            ic_peer_flow_control_support;
    u_int8_t                            ic_mesh_vap_support;
    u_int8_t                            ic_wds_support;

    /* Number of hops away from the root AP on this radio. This is used for
     * populating the WHC AP Info IE to help devices that could potentially
     * act as a range extender decide whether they are close enough to the
     * CAP to do so or not.
     */
    uint8_t                             dfs_spoof_test_regdmn;
    u_int8_t                            ic_disable_bcn_bwnss_map;       /* variable to store bandwidth-NSS mapping status*/
    u_int8_t                            ic_disable_bwnss_adv;           /* variable to enable/disable bwnss map advertising in management frames*/
    /* Monitor VAP filters */
    u_int8_t                            mon_filter_osif_mac:1,
                                        mon_filter_mcast_data:1,
                                        mon_filter_ucast_data:1,
                                        mon_filter_non_data:1;
    u_int8_t                            ic_cal_ver_check:1;
    struct global_ic_list               *ic_global_list;     /* back ptr to global_ic_list */

    /* Identifies the radio with max tx-chain capacity in
     * soc->hw_mode_ctx.target_band (initialized to 5GHz,
     * configurable in future). If all radios have same
     * tx-chain capacity in 'target_band' then the first
     * radio in that band is the marked as master-radio.
     */
    bool                                ic_master_radio;
    bool                                ic_nobackhaul_radio;
#if DBDC_REPEATER_SUPPORT
    bool                                ic_primary_radio;
    bool                                ic_preferredUplink;
    /* Store ic pointers from other radios in this list. Current ic is not
     * stored in this list.
     */
    struct ieee80211com                 *other_ic[MAX_RADIO_CNT-1];    /* this list has all other radio structure pointers*/
    bool                                fast_lane;
    struct ieee80211com                 *fast_lane_ic;    /* radio structure pointer for other fastlane radio*/
    u_int8_t                            ic_radio_priority;
    u_int8_t                            ic_extender_connection;  /* 0->init value
                                                                    1->connecting RE has no RootAP access
                                                                    2->connecting RE has RootAP access */
#endif
    struct ieee80211_nl_handle          *ic_nl_handle;
    spinlock_t                          ic_cip_lock; /* lock to protect crypto module access */
    struct restrict_channel_params      ic_rch_params; /*restrict channels parammeters */
    u_int32_t                           ic_num_clients;
    u_int32_t                           ic_def_num_clients; /* Setting default num clients based on target type*/
    u_int32_t                           ic_nac_client;
    u_int32_t                           ic_nac_bssid;
    u_int8_t                            ic_emiwar_80p80; /* EMI war to restrict cfreq1: 5775, cfreq2: 5210 combination in VHT80+80 */
#if ATH_GEN_RANDOMNESS
    u_int32_t                           random_gen_mode;
#endif
#if UMAC_SUPPORT_ACFG
    u_int8_t                            ic_diag_enable;
    spinlock_t                          ic_diag_lock;
    void                                *ic_diag_handle;
#endif
    /* Percentage obss channel utilization  thresold above
     * which chan stats events will be sent to user space.
     */
    u_int8_t                            ic_chan_stats_th;
    int                                 ic_min_snr;
#if MESH_MODE_SUPPORT
    u_int32_t                           meshpeer_timeout_cnt;
/* every IEEE80211_MESH_PEER_TIMEOUT_CNT*IEEE80211_SESSION_WAIT sec, call mesh peer timeout check func */
#define     IEEE80211_MESH_PEER_TIMEOUT_CNT  3
#endif
    u_int8_t                            tid_override_queue_mapping;
    u_int8_t                            bin_number; /* index to the bins */
    u_int8_t                            traf_bins;  /* number of bins w.r.t rate and interval */
    u_int8_t                            traf_stats_enable; /* The configuration parameter to enable/disable the statistics measurement */
    u_int32_t                           traf_rate;  /* The rate at which the received signal statistics are to be measured */
    u_int32_t                           traf_interval; /* The interval upto which the received signal statistics are to be measured */
    os_timer_t                          ic_noise_stats;  /* Timer to measure the noise statistics of each node */
    u_int8_t                            no_chans_available;
    u_int8_t                            ic_mon_decoder_type;    /* monitor header type, prism=1, radiotap=0 */
    u_int8_t                            ic_strict_doth;/* Strict doth enabled */
    u_int8_t                            ic_wb_subelem; /* Enable wide band channel switch subelement in CSA beacons */
    /* Number of non doth supporting stations associated
     *  on this radio interface.
     */
    u_int16_t                           ic_non_doth_sta_cnt;
    u_int8_t                            ic_chan_switch_cnt; /* Number of CSA/ECSA beacons to be transmitted before channel switch */
    u_int8_t                            ic_sec_offsetie; /* Enable Secondary offset IE in CSA beacons */
#define MON_DECODER_RADIOTAP  0
#define MON_DECODER_PRISM     1
    int (*ic_whal_ratecode_to_kbps)(uint8_t ratecode, uint8_t bandwidth, uint8_t gintval);
#if ALL_POSSIBLE_RATES_SUPPORTED
    int (*ic_get_supported_rates)(int htflag, int shortgi, int **rates);
    int (*ic_whal_kbps_to_mcs)(int kbps_rate, int shortgi, int htflag);
#else
    int (*ic_get_supported_rates)(int htflag, int shortgi, int nss, int ch_width, int **rates);
    int (*ic_whal_kbps_to_mcs)(int kbps_rate, int shortgi, int htflag, int nss, int ch_width);
#endif

    int (*ic_ucfg_testmode_cmd)(void *data, int cmd, char *userdata, unsigned int length); /* Handles UTF commands from FTM daemon */
#if UMAC_SUPPORT_CFG80211
    void (*ic_ucfg_testmode_resp)(struct wiphy *wiphy, void *buf, size_t buf_len); /* Handles sending responses of UTF commands to cfg80211 */
#endif

#ifdef QCA_NSS_WIFI_OFFLOAD_SUPPORT
    struct nss_vdev_ops *nss_vops; /* NSS vap function table */
    struct nss_wifi_internal_ops *nss_iops; /* NSS internal radio function rable */
    struct nss_wifi_radio_ops *nss_radio_ops; /* NSS generic radio function table */
#ifdef QCA_SUPPORT_WDS_EXTENDED
    struct nss_ext_vdev_ops *nss_ext_vops; /* NSS extended vap function table */
#endif	/* QCA_SUPPORT_WDS_EXTENDED */
#endif /* QCA_NSS_WIFI_OFFLOAD_SUPPORT */
    u_int32_t                           ic_auth_tx_xretry;
    struct wlan_objmgr_pdev *ic_pdev_obj;
    /* cfg80211 device attach for radio */
#if UMAC_SUPPORT_CFG80211
    u_int32_t                         ic_cfg80211_config; /* cfg80211 configuration */
    int (*ic_cfg80211_radio_attach) (struct device *dev, struct net_device *net_dev, struct ieee80211com *ic);
    /* cfg80211 device detach for radio */
    int (*ic_cfg80211_radio_detach) (struct ieee80211com *ic);
    int (*ic_cfg80211_update_channel_list) (struct ieee80211com *ic);

    struct wiphy                       *ic_wiphy; /* cfg80211 device */
    struct wireless_dev                ic_wdev;  /* wiress dev */
    struct wlan_cfg80211_band_chans    channel_list_g; /* Variable channel list */
    struct wlan_cfg80211_band_chans    channel_list_a; /* Variable channel list */
    struct radio_handler               ic_cfg80211_radio_handler;  /* cfg80211 radio handlers */
    struct spectral_cfg80211_vendor_cmd_handlers ic_cfg80211_spectral_handlers;  /* cfg80211 spectral handlers */
    struct ieee80211_supported_band wlan_band_2g; /* 2g band information that is required for wiphy */
    struct ieee80211_supported_band wlan_band_5g; /* 5g band information that is required for wiphy */
#if CFG80211_SUPPORT_11AX_HOSTAPD
    struct ieee80211_sband_iftype_data iftype_data_2g; /* HE capabilties for 2G */
    struct ieee80211_sband_iftype_data iftype_data_5g; /* HE capabilties for 5G */
    struct ieee80211_sband_iftype_data iftype_data_6g; /* HE capabilties for 6G */
    struct ieee80211_supported_band wlan_band_6g; /* 6g band information that is required for wiphy */
    struct wlan_cfg80211_band_chans    channel_list_6g; /* Variable channel list */
#endif /* CFG80211_SUPPORT_11AX_HOSTAPD */
#endif
#if QCN_IE
    u_int8_t                            ic_bpr_latency_comp; /* Stores time in ms to compensate for the
                                                               latency in sending a probe response */
    u_int8_t                            ic_bcn_latency_comp; /* Stores time in ms to compensate for the
                                                               latency in sending beacon */
    qdf_atomic_t                        ic_bpr_enable;       /* Store bpr enable state  */
#endif
#if QCA_SUPPORT_GPR
    struct acfg_offchan_tx_hdr          *acfg_frame;
    qdf_ktime_t                         ic_gpr_period;            /* Stores the gpr period in kernel time */
    qdf_hrtimer_data_t                  ic_gpr_timer;             /* hrtimer to send gpr  */
    u_int16_t                           ic_gpr_timer_start_count; /* Stores the number of times hrtimer restarted */
    u_int16_t                           ic_gpr_timer_resize_count; /* Stores the number of times hrtimer got resized */
    u_int16_t                           ic_gpr_send_count;        /* Stores the number of times gpr is sent */
    u_int8_t                            ic_gpr_enable_count;     /* number of vaps running gpr */
    u_int8_t                            ic_gpr_enable;           /* gpr status */
    u_int32_t                           ic_period;
#endif
    /* IC 11ax HE handle */
    struct ieee80211_he_handle          ic_he;
    /* HE target type */
    uint8_t                             ic_he_target;
    uint32_t                            ic_mgmt_ttl; /* MGMT_TTL in milliseconds */
    uint32_t                            ic_probe_resp_ttl; /* PROBE_RESP_TTL in milliseconds*/
    uint32_t                            ic_mu_ppdu_dur; /* MU PPDU duration in milliseconds*/
    uint32_t                            ic_tbtt_ctrl; /* TBTT_CTRL_CFG */
    /* SELF AP OBSS_PD_THRESHOLD value used to configure OBSS Packet Detect
     * threshold for Spatial Reuse feature.
     */
    uint32_t                            ic_ap_obss_pd_thresh;
    /* Self SRG OBSS Color Bitmap */
    uint32_t                            ic_srg_bss_color_bitmap[2];
    /* Self SRG BSSID Partial Bitmap */
    uint32_t                            ic_srg_partial_bssid_bitmap[2];
    uint8_t                             ic_hesiga_sr15_enable;
    /* Self SRG OBSS color enable bitmap */
    uint32_t                            ic_srg_obss_color_enable_bitmap[2];
    /* Self SRG OBSS BSSID enable bitmap */
    uint32_t                            ic_srg_obss_bssid_enable_bitmap[2];
    /* Self Non-SRG OBSS color enable bitmap */
    uint32_t                            ic_non_srg_obss_color_enable_bitmap[2];
    /* Self Non-SRG OBSS BSSID enable bitmap */
    uint32_t                            ic_non_srg_obss_bssid_enable_bitmap[2];
    /* Enable/disable PSR based Spatial Reuse */
    uint8_t                             ic_psr_tx_enable;
    uint8_t                             ic_safety_margin_psr;
    /* Default BA Buf Size for HE target */
    uint16_t                            ic_he_default_ba_buf_size;
    /* Flag to store target cap for Ul Muofdma */
    uint8_t                             ic_he_ul_muofdma;
    /* HE bsscolor handle representing all VAPs */
    struct ieee80211_bsscolor_handle    ic_bsscolor_hdl;
    /* User configured He Bss Color value */
    uint8_t                             ic_he_bsscolor;
    /* HE bsscolor change tbtt. */
    uint16_t                            ic_he_bsscolor_change_tbtt;
    /* NAV override config */
    uint32_t                            ic_nav_override_config;
    uint32_t                            ic_he_ul_trig_int:16, /* Trigger interval setting */
                                        ic_he_ul_ppdu_dur:13; /* UL PPDU Duration */
    uint8_t                             ic_he_bsscolor_override:1, /* Flag to enable/disable overriding bss color by user */
                                        ic_he_sr_enable:1, /* Spatial Reuse Setting */
                                        ic_he_ru_allocation:1, /* Flag to control RU allocation. */
                                        ic_is_heop_param_updated:1, /* HE Op Param update status */
                                        ic_mesh_mode:1; /* Mesh mode setting */
    uint8_t                             ic_tx_pkt_capture; /* Tx-Packet Capture */
    /* SR Ctrl PSR Disallowed config */
    uint8_t                             ic_he_srctrl_psr_disallowed:1,
    /* SR Ctrl Non-SRG OBSS PD SR Disallowed config */
                                        ic_he_srctrl_non_srg_obsspd_disallowed:1,
    /* SR Ctrl Non-SRG Offset Present */
                                        ic_he_srctrl_non_srg_offset_present:1,
    /* SR Ctrl SRG Information Present */
                                        ic_he_srctrl_srg_info_present:1,
    /* SR Ctrl HESIGA_Spatial_reuse_value15_allowed config */
                                        ic_he_srctrl_sr15_allowed:1,
    /* Saptial Reuse param updated */
                                        ic_is_spatial_reuse_updated:1;
    /* SRG OBSS Color Bitmap */
    uint32_t                            ic_he_srp_ie_srg_bss_color_bitmap[2];
    /* SRG BSSID Partial Bitmap */
    uint32_t                            ic_he_srp_ie_srg_partial_bssid_bitmap[2];
    uint8_t                             ic_rx_mon_lite; /* Rx-mon lite */
    /* SR Ctrl SRG OBSS PD Max/Min Offset */
    uint8_t                             ic_he_srctrl_srg_obsspd_max_offset;
    uint8_t                             ic_he_srctrl_srg_obsspd_min_offset;
    /* Non-SRG OBSS PD max offset */
    uint8_t                             ic_he_non_srg_obsspd_max_offset;
    /* SRG ACs enabled for SR Tx */
    uint32_t                            ic_he_sr_enable_per_ac;
    struct ieee80211_profile            *profile; /* Radio and VAP profile of the driver */
    struct ieee80211_ath_channel        *recovery_dfschan;  /* DFS channel during recovery  */
    qdf_event_t                         ic_wait_for_init_cc_response;
    bool                                ic_set_country_failed;
#ifdef CONFIG_DP_TRACE
    int (*ic_dptrace_set_param)(int cmd, int val1, int val2);
#endif
    struct proc_dir_entry                         *ic_proc;
    bool                                 ic_fw_ext_nss_capable; /* FW capability for EXT NSS Signaling */
    bool                                ic_ext_nss_capable; /* EXT NSS Capable */
    u_int8_t                            recovery_in_progress;
    u_int8_t                            mgmt_rx_snr;
    uint8_t                             ic_widebw_scan; /* wide band scan config */
    uint8_t                             ic_capture_latency; /* Store capture enable state  */
#define QOS_ACTION_FRAME_FRWD_TO_STACK 0x1
#define QOS_ACTION_FRAME_HNDL_UP_STACK 0x2
#define QOS_ACTION_FRAME_MASK (QOS_ACTION_FRAME_HNDL_UP_STACK | QOS_ACTION_FRAME_FRWD_TO_STACK)
    uint32_t                            ic_qos_acfrm_config; /* qos action frames config */
    void (*ic_print_peer_refs)(struct wlan_objmgr_vdev *vdev, bool assert);
    uint8_t                             ic_dfs_info_notify_channel_available; /* Enable DFS info notify to app */
    uint8_t                             ic_dfs_info_notify_radar_found;       /* Enable DFS info notify to app */
    uint8_t                                ic_debug_sniffer;
    uint8_t                             ic_num_csa_bcn_tmp_sent; /* Number of beacon template sent to the FW with CSA IE. */
    bool (*wide_band_scan_enabled)(struct ieee80211com *ic);
    void (*ic_process_nondata_tx_frames)(struct ieee80211com *ic, qdf_nbuf_t payload);
    uint32_t                            ic_csa_vdev_ids[WLAN_UMAC_PDEV_MAX_VDEVS]; /* Save vdev_id of the vap when it receives CSA event from FW. */
    uint32_t                            ic_csa_num_vdevs; /* Total number of vdevs which receives CSA event from FW. */
    int                                 (*ic_node_dump_wds_table) (struct ieee80211com *ic);
#if ATH_ACS_DEBUG_SUPPORT
    bool                                ic_acs_debug_support;  /* Manages the functioning of the ACS debug framework */
#endif
    void *dops;
    int (*ic_node_enable_sounding_int)(struct ieee80211_node *ni, uint32_t enable);
    int (*ic_node_set_su_sounding_int)(struct ieee80211_node *ni, uint32_t sounding_interval);
    int (*ic_node_set_mu_sounding_int)(struct ieee80211_node *ni, uint32_t sounding_interval);
    int (*ic_node_sched_mu_enable)(struct ieee80211_node *ni, uint32_t enable);
    int (*ic_node_sched_ofdma_enable)(struct ieee80211_node *ni, uint32_t enable);
    int (*ic_get_target_phymode)(struct ieee80211com *ic, uint32_t host_phymode,
                                 bool is_2gvht_en);
    ieee80211_rep_move_t                ic_repeater_move;
    /* Enable/disable CCK Tx, applies only to 2G */
    bool                                cck_tx_enable;
    bool                                schedule_bringup_vaps;
    struct last_scan_params scan_params;
    uint32_t                       multiq_support_enabled;
    u_int32_t                           device_id;   /* Device id from probe */
    u_int32_t                           vendor_id;   /* Vendor id from probe */
#if defined(WLAN_DFS_PARTIAL_OFFLOAD) && defined(HOST_DFS_SPOOF_TEST)
    uint8_t                             ic_rebuilt_chanlist:1,
                                        ic_tempchan:1;
    u_int32_t                           ic_curchan_flags;
    uint8_t                             ic_tmp_ch_width;
    uint16_t                            ic_tmp_center_freq_seg0;
    uint16_t                            ic_tmp_center_freq_seg1;
    uint8_t                             ic_tmp_sec_ch_offset;
    QDF_STATUS (*ic_is_host_dfs_check_enabled) (struct wlan_objmgr_pdev *pdev,
            bool *enabled);
#endif
    int                            ic_fw_rec_dbg[QDF_MODULE_ID_MAX];
    u_int8_t                       ic_tx_capture;  /* is tx capture enabled */
#if QCN_ESP_IE
    u_int8_t                            ic_esp_air_time_fraction;
    u_int8_t                            ic_esp_ppdu_duration;
    u_int8_t                            ic_esp_ba_window;
    u_int16_t                           ic_esp_periodicity;
    u_int32_t                           ic_fw_esp_air_time;
    bool                                ic_esp_flag;
#endif /* QCN_ESP_IE */
#if DBDC_REPEATER_SUPPORT
    uint8_t                  ic_preferred_bssid[QDF_MAC_ADDR_SIZE];
#endif

    /* MBSS IE */
    struct mbssid_info         ic_mbss;
    bool ru26_tolerant; /* RU26 tolerance status */
    u_int8_t                 ic_wifi_down_ind; /* wifi down indicator */
    u_int8_t                 ic_hw_nac_monitor_support;
    /* ppdu and burst minimum value requirement for Lithium based targets */
    u_int8_t                 ic_ppdu_min;
    u_int8_t                 ic_burst_min;
    int (*ic_node_chan_width_switch)    (void *data, struct ieee80211vap *vap);
#if ATH_SUPPORT_DFS && QCA_DFS_NOL_VAP_RESTART
    os_timer_t  ic_dfs_nochan_vap_restart_timer;   /* vap restart timer */
    bool ic_pause_stavap_scan;
#endif
#if defined(WLAN_DFS_FULL_OFFLOAD) && defined(QCA_DFS_NOL_OFFLOAD)
    bool ic_is_dfs_scan_violation; /* Status of dfs scan violation */
#endif
#ifdef OL_ATH_SMART_LOGGING
    bool                                smart_logging;
    void                                *smart_log_file;
    u_int32_t                           smart_log_skb_sz;
    bool                                smart_logging_p1pingfail_started;
    bool                                smart_logging_connection_fail_started;
#ifndef REMOVE_PKT_LOG
    qdf_mutex_t                         smart_log_fw_pktlog_mutex;
    bool                                smart_log_fw_pktlog_enabled;
    bool                                smart_log_fw_pktlog_blocked;
    bool                                smart_log_fw_pktlog_started;
#endif /* REMOVE_PKT_LOG */
#endif /* OL_ATH_SMART_LOGGING */
    struct wlan_channel_stats           ic_channel_stats;
    int                                 ic_uniform_rssi;
    bool                                ic_csa_interop_subscribed;         /* Subscribed for event */
    qdf_timer_t                         ic_csa_max_rx_wait_timer;
    uint32_t                            ic_pcp_tid_map[IEEE80211_PCP_MAX]; /* 8021p pcp to tid map */
    int                                 (*ic_set_pcp_tid_map)(struct wlan_objmgr_vdev *vdev,
                                                              uint32_t pcp, uint32_t tid);
    int                                 (*ic_set_tidmap_tbl_id)(struct wlan_objmgr_vdev *vdev,
                                                                uint32_t tblid);
    int                                 (*ic_set_tidmap_prty)(struct wlan_objmgr_vdev *vdev,
                                                              uint32_t val);
#if UMAC_SUPPORT_ACFG
    int                                 (*ic_acfg_cmd)(struct net_device *dev, void *data);
#endif
    struct ieee80211_ath_channel        *ic_radar_defer_chan;
    struct ieee80211_user_freq_list     ic_pri20_cfg_blockchanlist;
#ifdef WLAN_SUPPORT_RX_PROTOCOL_TYPE_TAG
    uint32_t                            rx_pkt_protocol_tag_mask;
#endif //WLAN_SUPPORT_RX_PKT_TAGGING
    u_int32_t                           ic_offchan_dwell_time;
    void                                (*ic_get_chan_grade_info)(struct ieee80211com *ic,
                                                                  uint32_t *hw_chan_grade_list);
#if ATH_ACS_DEBUG_SUPPORT
    int                                 (*ic_set_chan_grade_info)(struct ieee80211com *ic,
                                                                  void *channel_events,
                                                                  uint8_t nchan);
#endif
    bool                                ext_acs_request_in_progress; /* Whether external channel selection is already requested for this radio */
    bool                                ic_is_vdev_restart_sup; /* flag to indicate if vdev restart optimization is supported by FW */
    uint8_t                             ic_tidmap_prty; /* tidmap priority */
    u_int8_t                            ic_restart_reason;   /* bitmask to hold restart reason of ic */
    bool                                ic_is_restart_on_same_chan; /*flag to indicate vap restart on the same channel */
    struct om_notify_context            ic_omn_cxt; /*Operating mode notification context*/
    /* Lock between radar processing and mode switch. */
    spinlock_t                          ic_radar_mode_switch_lock;
    bool                                ic_re_ul_resp;
    int                                 (*ic_set_stats_update_period)(struct ieee80211vap *vap,
                                                                      uint32_t val);
    uint32_t                            (*ic_assemble_ratecode)(struct ieee80211vap *vap,
                                                                struct ieee80211_ath_channel *cur_chan,
                                                                uint32_t rate);
#if ATH_SUPPORT_DFS
    u_int8_t                            ic_scan_over_cac:1, /* Whether scan should be prioritized over cac for this radio */
                                        ic_is_cac_cancelled_by_scan:1; /* If CAC was cancelled due to scan issued */
#endif
    qdf_freq_t                          ic_radar_next_usr_freq; /* Radar_Next_user frequency to use after radar detection. It will be cleared after a radar detection.  */
#if OBSS_PD
    int (*ic_vap_set_self_sr_config)    (struct ieee80211vap *vap, uint32_t value, void *data, uint32_t data_len, uint32_t length);
    int (*ic_vap_get_self_sr_config)    (struct ieee80211vap *vap, uint8_t param, char value[], size_t length);
    int (*ic_vap_set_he_sr_config)      (struct ieee80211vap *vap, uint8_t param, uint8_t value, uint8_t data1, uint8_t data2);
    int (*ic_vap_get_he_sr_config)      (struct ieee80211vap *vap, uint8_t param, uint32_t *value);
    int (*ic_vap_set_he_srg_bitmap)     (struct ieee80211vap *vap, uint32_t *val, uint32_t param);
    int (*ic_vap_get_he_srg_bitmap)     (struct ieee80211vap *vap, uint32_t *val, uint32_t param);
    uint8_t                             self_srg_psr_support; /* Whether the target supports SRG and PSR based SR */
#endif
    struct ieee80211_set_country_params *ic_set_ctry_params; /* Store the country config params in ic before bringing the vaps down */
#if QCN_IE
    bool ic_rept_clients;
#endif
    uint8_t                             ic_wideband_capable:1,     /* Radio wideband (5-7GHz) capability */
                                        ic_wideband_csa_support:2; /* Controls support for wideband (5GHz-7GHz) CSA */
    bool                                ic_acs_precac_completed_chan_only; /* Controls support for ACS pre-CAC only channel selection */

    uint8_t                             ic_6ghz_rnr_unsolicited_prb_resp_active:1,  /* RNR unsolicited probe resp active */
                                        ic_6ghz_rnr_ess_24g_5g_co_located:1;  /* RNR member of ESS 2.4G/5G co-located */
    uint16_t                            ic_rtt_req_id;
    uint8_t                             ic_ftmrr_meas_token;
    uint8_t                             ic_ftmrr_dialogtoken;
    uint8_t                             ic_user_rnr_frm_ctrl; /* Frame level ctrl to allow user rnr */
    struct ieee80211_user_nbr_ap_list   ic_user_neighbor_ap;
    bool                                ic_colocated_rnr_filled;
    struct special_vap_stats ic_special_vap_stats;
} IEEE80211COM, *PIEEE80211COM;

struct assoc_sm_set_country{
    uint16_t cc;
    enum ieee80211_clist_cmd cmd;
    struct ieee80211_node *ni;
    struct ieee80211vap *vap;
};

typedef  int  (* ieee80211_vap_input_mgmt_filter ) (struct ieee80211_node *ni, wbuf_t wbuf,
                                                     int subtype, struct ieee80211_rx_status *rs);
typedef  int  (* ieee80211_vap_output_mgmt_filter ) (wbuf_t wbuf);

/* Structure in the node to store the received signal value,
 * minimum value, maximum value and median value for each
 * node at the end of every traffic rate mentioned
 */
struct noise_stats {
    u_int8_t    noise_value;
    u_int8_t    min_value;
    u_int8_t    max_value;
    u_int8_t    median_value;
};

#define ATH_MAX_SCAN_ENTRIES 256

#if ATH_WOW
struct ieee80211_wowpattern {
    u_int8_t wp_delayed:1,
             wp_set:1,
             wp_valid:1;
    u_int8_t wp_type;
#define IEEE80211_T_WOW_DEAUTH             1  /* Deauth Pattern */
#define IEEE80211_T_WOW_DISASSOC           2  /* Disassoc Pattern */
#define IEEE80211_T_WOW_MINPATTERN         IEEE80211_T_WOW_DEAUTH
#define IEEE80211_T_WOW_MAXPATTERN         8
};
#endif


typedef struct _APP_IE_LIST_HEAD {
    struct app_ie_entry     *stqh_first;    /* first element */
    struct app_ie_entry     **stqh_last;    /* addr of last next element */        \
    u_int16_t               total_ie_len;
    bool                    changed;      /* indicates that the IE contents have changed */
} APP_IE_LIST_HEAD;

#define NAWDS_LOCK_INIT(_p_lock)                   OS_RWLOCK_INIT(_p_lock)
#define NAWDS_WRITE_LOCK(_p_lock , _flags)         OS_RWLOCK_WRITE_LOCK(_p_lock , _flags)
#define NAWDS_WRITE_UNLOCK(_p_lock , _flags)       OS_RWLOCK_WRITE_UNLOCK(_p_lock , _flags)
#define nawds_rwlock_state_t(_lock_state)          rwlock_state_t (_lock_stat)
typedef rwlock_t nawdslock_t;



#if UMAC_SUPPORT_NAWDS

#ifndef UMAC_MAX_NAWDS_REPEATER
#error NAWDS feature is enabled but UMAC_MAX_NAWDS_REPEATER is not defined
#endif

struct ieee80211_nawds_repeater {
    /* Bits 0 - 7 NSS */
#define NAWDS_REPEATER_CAP_DS              0x01
#define NAWDS_REPEATER_CAP_TS              0x02
#define NAWDS_REPEATER_CAP_4S              0x04
#define NAWDS_REPEATER_CAP_5S              0x08
#define NAWDS_REPEATER_CAP_6S              0x10
#define NAWDS_REPEATER_CAP_7S              0x20
#define NAWDS_REPEATER_CAP_8S              0x40
#define NAWDS_REPEATER_CAP_NSS_RSVD        0x80

    /* Bits 8 - 15 CHWIDHT/HTMODE */
#define NAWDS_REPEATER_CAP_HT20            0x0100
#define NAWDS_REPEATER_CAP_HT2040          0x0200
    /* VHT Capability */
#define NAWDS_REPEATER_CAP_11ACVHT20       0x0400
#define NAWDS_REPEATER_CAP_11ACVHT40       0x0800
#define NAWDS_REPEATER_CAP_11ACVHT80       0x1000
#define NAWDS_REPEATER_CAP_11ACVHT80_80    0x2000
#define NAWDS_REPEATER_CAP_11ACVHT160      0x4000
#define NAWDS_REPEATER_CAP_CHWD_RSVD       0x8000

    /* Bits 16 - 23 TX BF */
#ifdef ATH_SUPPORT_TxBF
#define NAWDS_REPEATER_CAP_TXBF            0x010000
#define NAWDS_REPEATER_CAP_TXBF_RSVD       0xFE0000
#else
#define NAWDS_REPEATER_CAP_TXBF_RSVD       0xFF0000
#endif

    /* Bits 24 - 31 HE Capability */
#define NAWDS_REPEATER_CAP_11AXAHE20       0x01000000
#define NAWDS_REPEATER_CAP_11AXGHE20       0x02000000
#define NAWDS_REPEATER_CAP_11AXAHE40       0x04000000
#define NAWDS_REPEATER_CAP_11AXGHE40       0x08000000
#define NAWDS_REPEATER_CAP_11AXAHE80       0x10000000
#define NAWDS_REPEATER_CAP_11AXAHE160      0x20000000
#define NAWDS_REPEATER_CAP_11AXAHE80_80    0x40000000

#define NAWDS_REPEATER_CAP_RSVD            0x80000000

#define NAWDS_INVALID_CAP_MODE             (NAWDS_REPEATER_CAP_NSS_RSVD   | \
                                            NAWDS_REPEATER_CAP_CHWD_RSVD  | \
                                            NAWDS_REPEATER_CAP_TXBF_RSVD  | \
                                            NAWDS_REPEATER_CAP_RSVD)

#define NAWDS_MULTI_STREAMS                (NAWDS_REPEATER_CAP_DS | \
                                            NAWDS_REPEATER_CAP_TS | \
                                            NAWDS_REPEATER_CAP_4S | \
                                            NAWDS_REPEATER_CAP_5S | \
                                            NAWDS_REPEATER_CAP_6S | \
                                            NAWDS_REPEATER_CAP_7S | \
                                            NAWDS_REPEATER_CAP_8S)

#define NAWDS_5TO8_STREAMS                 (NAWDS_REPEATER_CAP_5S | \
                                            NAWDS_REPEATER_CAP_6S | \
                                            NAWDS_REPEATER_CAP_7S | \
                                            NAWDS_REPEATER_CAP_8S)

    u_int32_t caps;
    u_int8_t mac[QDF_MAC_ADDR_SIZE];
};

enum ieee80211_nawds_mode {
    IEEE80211_NAWDS_DISABLED = 0,
    IEEE80211_NAWDS_STATIC_REPEATER,
    IEEE80211_NAWDS_STATIC_BRIDGE,
    IEEE80211_NAWDS_LEARNING_REPEATER,
    IEEE80211_NAWDS_LEARNING_BRIDGE,
};

struct ieee80211_nawds {
    nawdslock_t lock;
    enum ieee80211_nawds_mode mode;
    u_int32_t defcaps;
    u_int8_t override;
    struct ieee80211_nawds_repeater repeater[UMAC_MAX_NAWDS_REPEATER];
    u_int8_t psk[32];
};
#endif

#if ATH_SUPPORT_NAC

struct ieee80211_nac_info {
    u_int8_t macaddr[QDF_MAC_ADDR_SIZE];
    u_int8_t avg_rssi;
    u_int8_t rssi;
    systime_t rssi_measured_time;
};

struct ieee80211_nac {
    struct ieee80211_nac_info bssid[NAC_MAX_BSSID];
    struct ieee80211_nac_info client[NAC_MAX_CLIENT];
};
#endif

#if ATH_SUPPORT_NAC_RSSI
struct ieee80211_nac_rssi {
  u_int8_t bssid_mac[6];
  u_int8_t client_mac[6];
  u_int8_t  chan_num;
  u_int8_t client_rssi_valid;
  u_int8_t client_rssi;
  u_int8_t vdev_id;
};
#endif

#if QCA_SUPPORT_PEER_ISOLATION
#define PEER_ISOLATION_HASH_BITS 4
#define PEER_ISOLATION_HASH_SIZE (1 << PEER_ISOLATION_HASH_BITS)

/* Isolated Peer details */
struct peer_isolation_entry {
    struct qdf_ht_entry node;
    struct rcu_head rcu;

    u_int8_t  mac[QDF_MAC_ADDR_SIZE];
};

/* Create a structure for the peer isolation list */
struct peer_isolation_list {
    u_int32_t num_peers;
    qdf_spinlock_t lock;
    qdf_ht_declare(peer_ht, PEER_ISOLATION_HASH_BITS);
};

static inline u_int32_t peer_isolation_hash_key(const u_int8_t *mac)
{
    u_int32_t i, key = 0;

    /* Use last MAC byte as Key */
    for (i = 0; i < QDF_MAC_ADDR_SIZE; i++)
        key += mac[i];

    return key;
}

static inline struct peer_isolation_entry*
peer_isolation_find(const u_int8_t *mac, struct peer_isolation_list *list)
{
    u_int32_t key;
    struct peer_isolation_entry *peer;

    if (list->num_peers == 0)
        return NULL;

    key = peer_isolation_hash_key(mac);

    qdf_ht_for_each_in_bucket(list->peer_ht, peer, node, key) {
        if (qdf_is_macaddr_equal((struct qdf_mac_addr *)mac,
                                 (struct qdf_mac_addr *)peer->mac)) {
            return peer;
        }
    }

    return NULL;
}
#endif

struct ieee80211_tim_set {
	int set;
	u_int16_t aid;
};

enum ieee80211_offchan_request_type {
    IEEE80211_OFFCHAN_TX = 0,
    IEEE80211_OFFCHAN_RX = 1,
};

struct ieee80211_offchan_req {
    u_int32_t freq;
    u_int32_t dwell_time;
    u_int64_t cookie;
    enum ieee80211_offchan_request_type request_type;
};

/* List to store the mgmt offchan request */
struct ieee80211_offchan_list {
    qdf_list_node_t  next_request;
    qdf_nbuf_t offchan_tx_frm;
    struct ieee80211_offchan_req req;
};

/* Used for MBSS IE */
struct ieee80211_mbss {
#define IEEE80211_EMA_MBSS_FLAGS_USER_CONFIGD_RSRC_PFL                0x00000001
#define IEEE80211_EMA_MBSS_FLAGS_USER_CONFIGD_RSRC_PFL_CLEAR          0x00000000

#define IEEE80211_EMA_MBSS_FLAGS_GET(_flags, _val)                    \
    ((_flags & _val))
#define IEEE80211_EMA_MBSS_FLAGS_SET(_flags, _val) do {               \
    _flags &= ~_val;                                                  \
    _flags |= _val;                                                   \
    } while (0)

    /* identifies the MBSS set this vap belongs to */
    uint8_t mbss_set_id;
    /* flag to indicate to mbssid ie update */
    bool mbssid_update_ie;
    /* flag to indicate probe response can be sent out */
    uint8_t mbssid_send_bcast_probe_resp;
    /* flag to indicate transmitting or non-transmitting VAP to FW */
    uint32_t mbssid_flags;
    /* boolean to identify a particular beacon update is specific
     * to a particular non-tx profile or not*/
    bool non_tx_profile_change;
    /* when template send is called with tx_profile identify the
     * beacon position for such an update */
    uint8_t current_bcn_pos;

#define MBSS_BCN_ENABLE 0
#define MBSS_BCN_DISABLE 1
    /* counter to keep track the number of times beaconing has been disabled or
     * enabled.It is incremented for every disable op and decremented for
     * each enable operation */
    qdf_atomic_t bcn_ctrl;

    /* Vendor IE space available per profile */
    int32_t available_vendor_ie_space;

    /* Optional IE space available per profile in beacon context*/
    int32_t available_bcn_optional_ie_space;
    /* Optional IE space available per profile in probe-rsp context*/
    int32_t available_prb_optional_ie_space;

    /* Total profile space allocated */
    uint32_t total_vendor_ie_size;
    uint32_t total_optional_ie_size;

    /* Buffer and lock for backup, local build, and non-ineritance IE */
    qdf_nbuf_t non_tx_pfl_ie_pool;
    qdf_spinlock_t non_tx_pfl_ie_pool_lock;
    uint16_t backup_length;

    /* Overflow flag to indicate total size overflow */
    bool ie_overflow;

    /* Counter to keep track of IE Overflows for Tx/NonTx VAPs Beacon */
    uint64_t ie_overflow_stats;

    /* Counter to keep track of profile size overflow for beacon */
    uint64_t ntx_pfl_rollback_stats;

    /* vap specific MBSSID related flags */
    uint32_t flags;

    /* Does vap have extended rate IE */
    bool is_xrates;
};

#define MON_MAC_HASHSIZE 32

#define MON_MAC_HASH(addr)  \
    (((const u_int8_t *)(addr))[QDF_MAC_ADDR_SIZE - 1] % MON_MAC_HASHSIZE)

struct ieee80211_mac_filter_list {
    LIST_ENTRY(ieee80211_mac_filter_list)   mac_entry;  /* link entry in mac_entry list*/
    u_int8_t mac_addr[QDF_MAC_ADDR_SIZE];
};

#ifdef MU_CAP_WAR_ENABLED
typedef enum {
    /*
     * Timer action to kick out the sole dedicated MU-MIMO 1X1 station
     * so that it can join back as SU-MIMO 2x2 client
     */
    MU_CAP_TIMER_CMD_KICKOUT_DEDICATED=0,


    /*
     * Timer action to kick out the 1 or multiple dedicated SU-MIMO 2x2 clients
     * when the AP can have multiple MU-MIMO clients instead.
     */
    MU_CAP_TIMER_CMD_KICKOUT_SU_CLIENTS=1
} MU_CAP_TIMER_CMD;

typedef enum {
    MU_CAP_CLIENT_NORMAL=0, /* Non-dedicated MU-MIMO capable client */
    MU_CAP_DEDICATED_SU_CLIENT=1, /* Dedicated SU-MIMO 2X2 client */
    MU_CAP_DEDICATED_MU_CLIENT=2, /* Dedicated MU=MIMO 1X1 client */
    MU_CAP_CLIENT_TYPE_MAX=3
} MU_CAP_CLIENT_TYPE;


typedef struct DEDICATED_CLIENT_MAC {
    u_int8_t macaddr[QDF_MAC_ADDR_SIZE];
    LIST_ENTRY(DEDICATED_CLIENT_MAC)list;
}DEDICATED_CLIENT_MAC;

typedef struct MU_CAP_WAR {
    /* List of all MU-Capable clients (including dedicated clients) */
    u_int16_t                  mu_cap_client_num[MU_CAP_CLIENT_TYPE_MAX];

#define MAX_PEER_NUM           IEEE80211_512_AID   /*max client numbers per ssid*/
    /*Mu-cap client address associated*/
    u_int8_t                   mu_cap_client_addr[MAX_PEER_NUM][QDF_MAC_ADDR_SIZE];

    /* Type of MU-capable client */
    MU_CAP_CLIENT_TYPE         mu_cap_client_flag[MAX_PEER_NUM];

    /* Lock for touching the MU-Cap array */
    qdf_spinlock_t             iv_mu_cap_lock;

    /* Timer for kicking out Dedicated clients */
    os_timer_t                 iv_mu_cap_timer;


#define MU_TIMER_PERIOD        15    /* 15*1000 seconds */
    u_int16_t                  mu_cap_timer_period;

    /* Command for the timer callback */
    MU_CAP_TIMER_CMD           mu_timer_cmd;

    /* List of clients to which we have sent MU-Disabled Probe response */
    ATH_LIST_HEAD(, DEDICATED_CLIENT_MAC) dedicated_client_list[IEEE80211_NODE_HASHSIZE];

    /*
     * Number of clients to which we have
     * sent MU-Disabled probe response.
     * This variable is only for display purpose,
     * i.e., displaying on mucapwar get command
     */
    u_int16_t                  dedicated_client_number;

#define MU_TIMER_STOP 0
#define MU_TIMER_PENDING 1
    u_int8_t                   iv_mu_timer_state:1,/* 1=pending, 0=stopped*/

                               /*
                                * MU-CAP WAR Enable/Disable
                                */
                               mu_cap_war:1,/*1: Enable, 0: Disable */

                               /*
                                * This overrides the
                                * MU-CAP WAR Probe response behaviour
                                * If this variable gets set,
                                * the AP WILL NEVER modify its VHTCAP
                                * in probe-response
                                */
                               mu_cap_war_override:1,

                               /*
                                * 1: Disable BFORMER in VHTCAP of
                                *    Probe-Response for dedicated client
                                *
                                * 0: Will not modify the existing
                                *    VHTCAP in probe-response
                                */
                               modify_probe_resp_for_dedicated:1;
} MU_CAP_WAR;
#endif

#if QCN_IE
#define DEFAULT_BCAST_PROBE_RESP_DELAY 8                 /* ms */
#define DEFAULT_BCAST_PROBE_RESP_LATENCY_COMPENSATION 5  /* ms */
#define DEFAULT_BEACON_LATENCY_COMPENSATION 5            /* ms */
#endif

#define MAX_NUM_TXPOW_MGT_ENTRY 16
#define MAX_NUM_TXPOW_FRAME_TYPE 3

#define MAX_VLAN 128


#define IEEE80211_VAP_IN_FIXED_RATE_MODE(vap) \
   ((vap->iv_fixed_rate.mode == IEEE80211_FIXED_RATE_LEGACY) || \
    (vap->iv_fixed_rate.mode == IEEE80211_FIXED_RATE_MCS) || \
    (vap->iv_fixed_rate.mode == IEEE80211_FIXED_RATE_VHT) || \
    (vap->iv_fixed_rate.mode == IEEE80211_FIXED_RATE_HE))

#if QCA_SUPPORT_RAWMODE_PKT_SIMULATION
#define FIXED_NUM_ENCAP_DUMP 0x1
#define FIXED_NUM_DECAP_DUMP 0x2
#endif

struct _wlan_assoc_sm;
typedef struct _wlan_assoc_sm *wlan_assoc_sm_t;

enum cm_resp_evt {
   CM_BSS_PEER_CREATE_SUCCESS_RESP,
   CM_BSS_PEER_CREATE_FAILURE_RESP,
   CM_BSS_PEER_DELETE_SUCCESS_RESP,
   CM_BSS_PEER_DELETE_FAILURE_RESP,
   CM_CONNECT_RESP,
   CM_DISCONNECT_RESP,
   CM_BSS_PEER_DELETE_IND,
   CM_BSS_PEER_SELECT_IND,
   CM_BSS_PEER_SELECT_IND_FAILURE,
};

#if SM_ENG_HIST_ENABLE
#define WLAN_CM_EVENT_HISTORY_SIZE 50
struct wlan_cm_action_entry {
    uint8_t event;
    QDF_STATUS status;
    uint64_t time;
};

struct wlan_cm_action_history {
    qdf_spinlock_t cm_history_lock;
    uint8_t index;
    struct wlan_cm_action_entry data[WLAN_CM_EVENT_HISTORY_SIZE];
};
#endif

struct cm_ext_obj {
    wlan_assoc_sm_t assoc_sm_handle;
    enum cm_resp_evt cm_evt_id;
    qdf_timer_t cm_defer_resp;
    struct wlan_cm_discon_rsp cm_discon_rsp;
    struct wlan_cm_connect_resp cm_conn_rsp;
#if SM_ENG_HIST_ENABLE
    struct wlan_cm_action_history cm_action_history;
#endif
};

#if DBG_LVL_MAC_FILTERING
#define DBGMAC_PEER_HASH_BITS 5
#define DBGMAC_PEER_HASH_SIZE (1 << DBGMAC_PEER_HASH_BITS)
struct dbgmac_peer_entry {
    struct qdf_ht_entry node;
    u_int8_t  mac[QDF_MAC_ADDR_SIZE];
};
struct dbglvl_mac_peer_list {
    u_int32_t num_peers;
    qdf_ht_declare(dbgmac_peer_ht, DBGMAC_PEER_HASH_BITS);
};

#define dbgmac_peer_get_hkey(_mac) WLAN_PEER_HASH(_mac)

/*
 * In peer list hashtable, find the peer with its MAC addr
 * @param mac:  MAC address of the peer
 * @param list: Peer list, it's a hashtable
 *
 * Return: struct dbgmac_peer_entry *
 */
static inline struct dbgmac_peer_entry *
dbgmac_peer_find(const u_int8_t *mac, struct dbglvl_mac_peer_list *list)
{
    u_int32_t key;
    struct dbgmac_peer_entry *peer;

    if (list->num_peers == 0)
        return NULL;

    key = dbgmac_peer_get_hkey(mac);
    qdf_ht_for_each_in_bucket(list->dbgmac_peer_ht, peer, node, key) {
        if (qdf_is_macaddr_equal((struct qdf_mac_addr *)mac,
                                 (struct qdf_mac_addr *)peer->mac)) {
            return peer;
        }
    }
    return NULL;
}

/*
 * Add a peer to peer list hashtable based on MAC address
 * @param mac:  MAC address of the peer
 * @param list: Peer list, it's a hashtable
 *
 * Return: 0-success, other-failure
 */
int dbgmac_peer_add(const u_int8_t *mac, struct dbglvl_mac_peer_list *list);

/*
 * Delete a peer from peer list hashtable based on MAC address
 * @param mac:  MAC address of the peer
 * @param list: Peer list, it's a hashtable
 *
 * Return: 0-success, other-failure
 */
int dbgmac_peer_del(u_int8_t *mac, struct dbglvl_mac_peer_list *list);

/*
 * Allocate peer list hashtable
 * Return: pointer of struct dbglvl_mac_peer_list
 */
static inline struct dbglvl_mac_peer_list *dbgmac_peer_list_alloc(void)
{
    struct dbglvl_mac_peer_list *l = NULL;
    l = qdf_mem_malloc(sizeof(struct dbglvl_mac_peer_list));
    return l;
}

/*
 * Free the whole peer list hashtable
 * @param list: Peer list, it's a hashtable
 */
static inline void dbgmac_peer_list_free(struct dbglvl_mac_peer_list *list)
{
    struct dbgmac_peer_entry *peer;
    struct qdf_ht_entry *tmp;
    u_int32_t bucket;

    if (list) {
        qdf_ht_for_each_safe(list->dbgmac_peer_ht, bucket, tmp, peer, node) {
            /* Remove from hash table */
            qdf_ht_remove(&peer->node);
            list->num_peers--;
            /* Free the buffer */
            qdf_mem_free(peer);
        }
        qdf_mem_free(list);
    }
}

/*
 * Dump the whole peer list hashtable
 * @param list: Peer list, it's a hashtable
 */
void dbgmac_peer_list_dump(struct dbglvl_mac_peer_list *list);
#endif  /* DBG_LVL_MAC_FILTERING */

#define DISABLE_RTT_RESPONDER_AND_INITIATOR_MODE 0
#define RTT_RESPONDER_MODE 0x1
#define RTT_INITIATOR_MODE 0x2
#define RTT_RESPONDER_AND_INITIATOR_MODE 0x3

typedef struct ieee80211vap {
    struct proc_dir_entry			  *iv_proc;
    TAILQ_ENTRY(ieee80211vap)         iv_next;     /* list of vap instances */
    struct ieee80211_wme_state        iv_wmestate; /* WME params corresponding to vap */
    struct ieee80211_muedca_state     iv_muedcastate; /* MUEDCA params
                                                       * corresponding to vap */
    u_int32_t                          iv_debug;   /* debug msg flags */
    enum ieee80211_opmode              iv_opmode;  /* operation mode */
#if WAR_DELETE_VAP
    void                              *iv_athvap; /* opaque ath vap pointer */
#endif

    struct ieee80211com               *iv_ic;     /* back ptr to common state */
//    struct net_device_stats           iv_devstats; /* interface statistics */

    void                              *iv_priv;   /* for extending vap functionality */
    os_if_t                           iv_ifp;     /* opaque handle to OS-specific interface */
    char                              *iv_netdev_name;     /* net dev name associated with the VAP */
    wlan_event_handler_table          *iv_evtable;/* vap event handlers */

    os_handle_t                       iv_mlme_arg[IEEE80211_MAX_VAP_MLME_EVENT_HANDLERS];  /* opaque handle used for mlme event handler */
    wlan_mlme_event_handler_table     *iv_mlme_evtable[IEEE80211_MAX_VAP_MLME_EVENT_HANDLERS];  /* mlme event handlers */

    os_handle_t                       iv_misc_arg[IEEE80211_MAX_MISC_EVENT_HANDLERS];
    wlan_misc_event_handler_table     *iv_misc_evtable[IEEE80211_MAX_MISC_EVENT_HANDLERS];

    void*                             iv_event_handler_arg[IEEE80211_MAX_VAP_EVENT_HANDLERS];
    ieee80211_vap_event_handler       iv_event_handler[IEEE80211_MAX_VAP_EVENT_HANDLERS];

    os_if_t                           iv_ccx_arg;  /* opaque handle used for ccx handler */
    wlan_ccx_handler_table            *iv_ccx_evtable;  /* ccx handlers */


    struct asf_print_ctrl             iv_print;

    u_int8_t                          iv_myaddr[QDF_MAC_ADDR_SIZE]; /* current mac address */
    u_int8_t                          iv_my_hwaddr[QDF_MAC_ADDR_SIZE]; /* mac address from EEPROM */

    u_int32_t                         iv_create_flags;   /* vap create flags */

    u_int32_t                         iv_flags;   /* state flags */
    u_int32_t                         iv_flags_ext;   /* extension of state flags */
    u_int32_t                         iv_flags_ext2;   /* extension 2 of state flags */
/* not multiple thread safe */
    u_int32_t                         iv_deleted:1,  /* if the vap deleted by user */
                                      iv_scanning:1, /* if the vap is scanning */
                                      iv_smps:1,     /* if the vap is in static mimo ps state */
                                      iv_standby:1,  /* if the vap is temporarily stopped */
                                      iv_cansleep:1, /* if the vap can sleep*/
                                      iv_sw_bmiss:1, /* use sw bmiss timer */
                                      iv_copy_beacon:1, /* enable beacon copy */
                                      iv_wapi:1,
                                      iv_sta_fwd:1, /* set to enable sta-fws fweature */
                                      iv_dynamic_mimo_ps, /* dynamic mimo ps enabled */
                                      iv_doth:1,     /* 802.11h enabled */
                                      iv_country_ie:1,  /* Country IE enabled */
                                      iv_wme:1, /* wme enabled */
                                      iv_dfswait:1,   /* if the vap is in dfswait state */
                                      iv_erpupdate:1, /* if the vap has erp update set */
                                      iv_needs_scheduler:1, /* this vap needs scheduler for off channel operation */

                                      iv_forced_sleep:1,        /*STA in forced sleep set PS bit for all outgoing frames */
                                      iv_qbssload:1, /* QBSS load IE enabled */
                                      iv_qbssload_update:1, /* update qbssload IE in beacon */
                                      iv_rrm:1, /* RRM Capabilities */
                                      iv_wnm:1, /* WNM Capabilities */
                                      iv_proxyarp:1, /* WNM Proxy ARP Capabilities */
                                      iv_dgaf_disable:1, /* Hotspot 2.0 DGAF Disable bit */
                                      iv_ap_reject_dfs_chan:1,  /* SoftAP to reject resuming in DFS channels */
                                      iv_smartnet_enable:1,  /* STA SmartNet enabled */
                                      iv_trigger_mlme_resp:1,  /* Trigger mlme response */
                                      iv_mfp_test:1;   /* test flag for MFP */
    u_int32_t                         iv_sgi:2,        /* Short Guard Interval 3600:3 1600:2 400:1 800:0 */
                                      iv_he_sgi:2,      /*Short Guard Interval 800:0 for he mode only. Kept as 2 bit for future provision of enhanced HW cap*/
                                      iv_data_sgi:2,   /* Short Guard Interval 3600:3 1600:2 400:1 800:0 for VHT & HE fixed rates */
                                      iv_he_data_sgi:2,   /* Short Guard Interval 3600:3 1600:2 800:0 for HE fixed rates*/
                                      iv_ratemask_default:1, /* flag to indicate using default ratemask */
                                      iv_key_flag:1,    /*For wakeup AP VAP when wds-sta connect to the AP only use when export (UMAC_REPEATER_DELAYED_BRINGUP || DBDC_REPEATER_SUPPORT)=1*/
                                      iv_list_scanning:1, /* if performe the iwlist scanning */
                                      iv_vap_is_down:1, /*Set when VAP down*/
#if ATH_SUPPORT_ZERO_CAC_DFS
                                      iv_pre_cac_timeout_channel_change:1,
#endif
                                      iv_needs_up_on_acs:1, /* if vap may require acs when another vap is brought down */
                                      iv_quick_reconnect:1, /* if vap (STA VAP) it attempting a quick reconnect to AP */
                                      /* Set when there is a change in MU-EDCA
                                       * params advertised by AP. It will be
                                       * reset once STA sends the updated
                                       * parameters to the target. */
                                      iv_update_muedca_params:1,
                                      iv_reset_ap_vaps:1, /* Used to check if reset is needed in wds_repeater mode */
                                      iv_stop_bss:1,    /* stop bss in progress */
                                      iv_peer_create_failed:1,    /* AP peer create failed */
                                      iv_ext_bssload:1,   /* Extended BSS Load IE in beacon */
                                      iv_ext_bssload_update:1,   /* update extended BSS Load IE in beacon */
                                      iv_he_ul_sgi:2, /* UL Short GI setting */
                                      iv_he_ul_ltf:2, /* UL LTF setting */
                                      iv_he_ul_ldpc:1, /* UL LDPC mode */
                                      iv_he_ul_stbc:1, /* UL STBC mode */
                                      iv_he_basic_mcs_for_bss:2,   /* basic mcs requirement for the bss */
                                      iv_wme_reset:1, /* flag to check and  reset wme parameters if phymode changes */
                                      iv_registered:1; /* Virtual interface registered with Kernel */
    u_int16_t                         iv_bcca_ie_status:2, /* BCCA IE update */
                                      iv_he_ul_nss:4, /* UL NSS setting */
                                      iv_he_ul_fixed_rate:4; /* UL Fixed rate MCS */
    u_int16_t                         iv_he_amsdu_in_ampdu_suprt:1, /* configur HE MAC Cap amsdu in ampdu */
                                      iv_he_subfee_sts_lteq80:3, /* configure HE PHY Cap subfee_sts_lteq80 */
                                      iv_he_subfee_sts_gt80:3,   /* configure HE PHY Cap subfee_sts_gt80 */
                                      iv_he_4xltf_800ns_gi:1,    /* configure HE PHY Cap 4xltf+800ns gi support */
                                      iv_he_1xltf_800ns_gi:1,    /* configure HE PHY Cap 1xltf+800ns gi support */
                                      iv_he_max_nc:3,            /* configure HE PHY Cap max_nc */
                                      iv_twt_rsp:1,              /* configure HE MAC Cap twt responder */
                                      iv_he_er_su_disable:1,     /* configure HE OP ER SU support */
                                      iv_he_1024qam_lt242ru_rx:1,/* configure HE PHY 1024-QAM < 242-tone RU Rx Support */
                                      iv_he_ulmu_data_disable_rx:1; /*configure HE MAC UL MU Data Disable RX Support */
    u_int16_t                         iv_he_full_bw_ulmumimo:1,  /* configure HE PHY Full Bandwidth UL MU-MIMO */
                                      iv_he_dcm_max_cons_rx:2,   /* configure HE PHY DCM Max Constellation Rx Support */
                                      iv_disable_inact_probe:1;  /* Disable FW Inactivity Probing behavior */

    u_int32_t                         iv_256qam:1,     /* 256 QAM support in 2.4GHz mode Enable:1 Disable:0 */
                                      iv_dec_bcn_loss:1,
                                      iv_11ng_vht_interop:1,     /* 2.4NG 256 QAM Interop mode Enable:1 Disable:0 */
                                      iv_mbo:1,    /* for mbo functionality */
                                      iv_oce:1,    /* for oce functionality */
#if DYNAMIC_BEACON_SUPPORT
                                      iv_dbeacon:1,         /* Flag for Dynamic Beacon */
                                      iv_dbeacon_runtime:1, /* Runtime flag to suspend/resume dbeacon for DFS or non DFS channel */
#endif

                                      iv_ext_acs_inprogress:1,     /* Whether external auto channel selection is
                                                                     progress */
                                      iv_send_additional_ies:1,    /* Enable sending of Extra IEs to host */
                                      vlan_set_flags:1,    /* Dev Flags to control vlan tagged packets
                                                              sent out by NW stack */
                                      iv_rescan:1,
#if QCA_SUPPORT_GPR
                                      iv_gpr_enable : 1,  /* If set, gratuitous probe response is enabled */
#endif
                                      iv_mcast_rate_config_defered:1, /* Non Zero - Multi cast rate confuguration is defered */
                                      iv_lastbcn_phymode_mismatch:1,        /* Phy mode mismatch between scan entry, BSS */
                                      iv_wep_mbssid:1,    /* force multicast wep keys in first 4 entries 0=yes, 1=no */
                                      iv_mlmeconnect:1,     /* Assoc state machine roaming mode */
                                      iv_wmm_power_save:1,
                                      iv_assoc_denial_notify:1, /* param to config assoc denial notification incase of ACL */
    /* channel_change_done is a flag value used to indicate that a channel
     * change happend for this VAP. This information (for now) is used to update
     * the beacon information and then reset. This is needed in case of DFS channel change
     */
                                      channel_change_done:1,
                                      appie_buf_updated:1,
                                      iv_doth_updated:1,
                                      mixed_encryption_mode:1,
                                      iv_ena_vendor_ie:1,
                                      iv_update_vendor_ie:1,
                                      iv_ccmpsw_seldec:1,  /* Enable/Disable encrypt/decrypt of frames selectively ( frames with KEYMISS) */
                                      iv_mgt_rate_config_defered:1, /* Non Zero - mgmt rate configuration is defered */
#if UMAC_SUPPORT_CHANUTIL_MEASUREMENT
                                      iv_chanutil_enab:1,
#endif /* UMAC_SUPPORT_CHANUTIL_MEASUREMENT */
#if ATH_SW_WOW
                                      iv_sw_wow:1,           /* Flags for sw wow */
#endif
                                      auto_assoc:1;    /* add flag to enable/disable auto-association */

    u_int8_t                          iv_unit;    /* virtual AP unit */
    u_int8_t                          iv_rawmode_pkt_sim;
    u_int8_t                          iv_he_basic_nss_for_bss;   /* basic nss requirement for the bss */
    u_int8_t                          iv_he_ul_ppdu_bw; /* UL channel width */
    u_int32_t                         iv_ema_ap_vendor_ie_size;

    /* multiple thread safe */
    u_int32_t                         iv_caps;    /* capabilities */
    u_int8_t                          iv_ath_cap;                       /* Atheros adv. capablities */
    u_int8_t                          iv_chanchange_count;              /* 11h counter for channel change */
    u_int16_t                         iv_he_bsscolor_change_count;      /* 11ax counter for bss color change */
    bool                              iv_he_bsscolor_change_ongoing;    /* is BCCA ongoing for this vap */
    bool                              iv_he_bsscolor_detcn_configd_vap; /* Have we configured collision
                                                                           detection in fw for this vap */
    bool                              iv_he_bsscolor_remove_ie;         /* if true trigger beacon_reinit
                                                                           to remove BCCA ie */
    u_int8_t                          iv_scan_priority_base;  /* Base value used to determine priority of scans requested by this VAP */
    int                               iv_mcast_rate; /* Multicast rate (Kbps) */
    int                               iv_mcast_fixedrate; /* fixed rate for multicast */
    atomic_t                          iv_rx_gate; /* pending rx threads */
    struct ieee80211_mac_stats        iv_unicast_stats;   /* mac statistics for unicast frames */
    struct ieee80211_mac_stats        iv_multicast_stats; /* mac statistics for multicast frames */
    struct tkip_countermeasure        iv_unicast_counterm;  /* unicast tkip countermeasure */
    struct tkip_countermeasure        iv_multicast_counterm;  /* unicast tkip countermeasure */

    ieee80211_tpe_config_user_params  iv_tpe_ie_config; /* User configured TPE IEs */
    spinlock_t                        iv_lock; /* lock to protect access to vap object data */
    unsigned long                     *iv_aid_bitmap; /* association id map */
    u_int8_t                          *iv_tim_bitmap; /* power-save stations w/ data*/
    unsigned long                     iv_lock_flags; /* flags for iv_lock */
    u_int16_t                         iv_max_aid;
    u_int16_t                         iv_mbss_max_aid; /* Per-VAP AID limit in MBSS group */
    u_int16_t                         iv_sta_assoc;   /* stations associated */
    u_int16_t                         iv_ax_sta_assoc; /* 11ax capable stations associated */
    u_int16_t                         iv_mu_bformee_sta_assoc;   /* mu mimo stations associated */
    u_int16_t                         iv_he_su_bformee_sta_assoc;
    u_int16_t                         iv_ps_sta;  /* stations in power save */
    u_int16_t                         iv_ps_pending;  /* ps sta's w/ pending frames */
    u_int8_t                          iv_dtim_count;  /* DTIM count from last bcn */
    u_int8_t                          iv_atim_window; /* ATIM window */
    u_int16_t                         iv_tim_len;     /* ic_tim_bitmap size (bytes) */
                                      /* set/unset aid pwrsav state */
    void                              (*iv_set_tim)(struct ieee80211_node *, int,bool isr_context);
    int                               (*iv_alloc_tim_bitmap)(struct ieee80211vap *vap);
    struct ieee80211_node             *iv_bss;    /* information for this node */
    struct ieee80211_rateset          iv_op_rates[IEEE80211_MODE_MAX]; /* operational rate set by os */
    u_int16_t                         iv_def_txkey;   /* default/group tx key index */
#if ATH_SUPPORT_AP_WDS_COMBO
    u_int8_t                          iv_no_beacon;   /* VAP does not transmit beacon/probe resp. */
#endif
    struct ieee80211_fixed_rate       iv_fixed_rate;  /* 802.11 rate or -1 */
    u_int32_t                         iv_fixed_rateset;  /* 802.11 rate set or -1(invalid) */
    u_int32_t                         iv_fixed_retryset; /* retries */
    u_int32_t                         iv_legacy_ratemasklower32;    /*lower 32 bit of ratemask for legacy mode*/
    u_int32_t                         iv_ht_ratemasklower32;    /*lower 32 bit of ratemask for HT mode*/
    /* The ratemask variables are used to configure the maximum NSS MCS support
     * on the AP. The mapping of the variables to the NSS MCS rate setting is
     * as follows:
     *
     * ratemasklower32      ==> b0 - b31
     * ratemaskhigher32     ==> b32 - b63
     * ratemasklower32_2    ==> b64 - b95
     *
     * Bitmap for ratemask(for HE targets):
     *
     *     NSS8/MCS0-11    NSS7/MCS0-11         NSS2/MCS0-11    NSS1/MCS0-11
     *  |               |               |     |               |              |
     *  |   b95 - b84   |   b83 - b72   |.....|   b23 - b12   |   b11 - b0   |
     *
     *
     * Bitmap for ratemask(for legacy targets):
     *
     *    NSS4/MCS0-9     NSS3/MCS0-9      NSS2/MCS0-9    NSS1/MCS0-9
     *  |               |               |               |              |
     *  |   b39 - b30   |   b29 - b20   |   b19 - b10   |    b9 - b0   |
     *
     *
     * Each bit in the map corresponds to an MCS value, and 12 bits will
     * correspond to one NSS setting for HE targets.
     * For example: b0-b11 will correspond to NSS1 and MCS0-11
     *
     * For legacy targets 10 bits will correspond to one NSS setting.
     * For example: b0-b9 will correspond to NSS1 and MCS0-9
     *
     *  */
    u_int32_t                         iv_vht_ratemasklower32;    /*lower 32 bit of ratemask for VHT mode*/
    u_int32_t                         iv_vht_ratemaskhigher32;  /*higher 32 bit of ratemask for VHT mode for beeliner*/
    u_int32_t                         iv_vht_ratemasklower32_2;
    u_int32_t                         iv_he_ratemasklower32;
    u_int32_t                         iv_he_ratemaskhigher32;
    u_int32_t                         iv_he_ratemasklower32_2;
    u_int8_t                          iv_wps_mode;    /* WPS mode */
    unsigned long                     iv_assoc_comeback_time;    /* assoc comeback information */
    int                               iv_skip_pmf_reassoc_to_hostap;
    int                               iv_rsn_override;

    ieee80211_privacy_exemption       iv_privacy_filters[IEEE80211_MAX_PRIVACY_FILTERS];    /* privacy filters */
    u_int32_t                         iv_num_privacy_filters;
    ieee80211_pmkid_entry             iv_pmkid_list[IEEE80211_MAX_PMKID];
    u_int8_t                          iv_rateCtrlEnable;  /* enable rate control */
    u_int8_t                          iv_rc_txrate_fast_drop_en;    /* enable tx rate fast drop*/
    u_int16_t                         iv_pmkid_count;

    u_int16_t                         iv_des_modecaps;   /* set of desired phy modes for this VAP */
    enum ieee80211_phymode            iv_des_mode; /* desired wireless mode for this interface. */
    enum ieee80211_phymode            iv_des_hw_mode; /* desired hardware mode for this interface. */
    enum ieee80211_phymode            iv_cur_mode; /* current wireless mode for this interface. */
    struct ieee80211_ath_channel      *iv_des_chan[IEEE80211_MODE_MAX]; /* desired channel for each PHY */
    struct ieee80211_ath_channel      *iv_bsschan;   /* bss channel */
    uint16_t                          iv_des_ibss_chan_freq;   /* desired ad-hoc channel */
    uint16_t                          iv_des_cfreq2;     /* Desired cfreq2 for VHT80_80 mode */
    int                               iv_des_nssid;       /* # desired ssids */
    ieee80211_ssid                    iv_des_ssid[IEEE80211_SCAN_MAX_SSID];/* desired ssid list */
    /* configure bssid of hidden ssid AP using iwpriv cmd */
    u_int8_t                          iv_conf_des_bssid[QDF_MAC_ADDR_SIZE];

    u_int8_t                          iv_smps_snrthresh;
    u_int8_t                          iv_smps_datathresh;
    int8_t                            iv_assoc_min_rssi_thrshld; /* minimum rssi threshold allowed for
                                                                   peer sta below which the assoc is
                                                                   denied */
    int                               iv_bmiss_count;
    int                               iv_bmiss_count_for_reset;
    int                               iv_bmiss_count_max;
    systime_t                         iv_last_beacon_time;         /* absolute system time, not TSF */
    systime_t                         iv_last_directed_frame;      /* absolute system time; not TSF */
    systime_t                         iv_last_ap_frame;            /* absolute system time; not TSF */
    systime_t                         iv_last_traffic_indication;  /* absolute system time; not TSF */
    systime_t                         iv_lastdata;                 /* absolute system time; not TSF */
    u_int64_t                         iv_txrxbytes;                /* No. of tx/rx bytes  */
    ieee80211_power_t                 iv_power;                    /* handle private to power module */
    ieee80211_sta_power_t             iv_pwrsave_sta;
    ieee80211_pwrsave_smps_t          iv_pwrsave_smps;
    u_int16_t                         iv_keep_alive_timeout;       /* keep alive time out */
    u_int16_t                         iv_inact_count;               /* inactivity count */



    /* APP IE implementation */
    APP_IE_LIST_HEAD                  iv_app_ie_list[IEEE80211_APPIE_MAX_FRAMES];

    IEEE80211_VEN_IE                 *iv_venie;

    struct wlan_mlme_app_ie          *vie_handle;       /*  vendor application IE handle */

    /* opt ie buffer - currently used for probereq and assocreq */
    struct ieee80211_app_ie_t         iv_opt_ie;
    u_int16_t                         iv_opt_ie_maxlen;

    /* U-APSD Settings */
    u_int8_t                          iv_uapsd;

    /* Copy of the beacon frame */
    u_int8_t                          *iv_beacon_copy_buf;
    int                               iv_beacon_copy_len;

    /* country ie data */
    u_int64_t                         iv_country_ie_chanflags;
    struct ieee80211_ie_country       iv_country_ie_data; /* country info element */

    bool                              iv_rtt_update;
    u_int8_t                          iv_ht40_intolerant;
    u_int8_t                          iv_chwidth;
    u_int8_t                          iv_sta_negotiated_ch_width; /*Channel width negitiated by station during association*/
    u_int8_t                          iv_chextoffset;
    u_int8_t                          iv_disable_HTProtection;
    u_int8_t                          dyn_bw_rts;              /* dynamic bandwidth RTS */
    u_int32_t                         iv_chscaninit;
    /* RATE DROPDOWN CONTROL */
    u_int32_t                         iv_ratedrop;


    ieee80211_mlme_priv_t             iv_mlme_priv;    /* opaque handle to mlme private information */
    ieee80211_aplist_config_t         iv_aplist_config;

    ieee80211_resmgr_vap_priv_t       iv_resmgr_priv;         /* opaque handle with resource manager private info */

    ieee80211_acl_t                   iv_acl;   /* opaque handle to acl */
    ieee80211_vap_ath_info_t          iv_vap_ath_info_handle; /* opaque handle for VAP_ATH_INFO */
    enum ieee80211_protmode           iv_protmode;            /* per vap 802.11g protection mode */

#if ATH_SUPPORT_WRAP
#if WLAN_QWRAP_LEGACY
    u_int8_t                          iv_psta:1,        /* VAP type is PSTA */
                                      iv_mpsta:1,       /* VAP type is MPSTA */
                                      iv_wrap:1,        /* VAP type is WRAP */
                                      iv_mat:1,         /* VAP type is MAT */
                                      iv_wired_pvap:1;  /* proxy vap for wired sta */
    u_int8_t                          iv_mat_addr[QDF_MAC_ADDR_SIZE];      /* mat address for PSTA */
#endif
#ifdef QCA_NSS_WIFI_OFFLOAD_SUPPORT
    u_int8_t                          iv_nss_qwrap_en;
#endif
#endif

    struct ieee80211_beacon_info      iv_beacon_info[100]; /*BSSID and RSSI info*/
    u_int8_t                          iv_essid[IEEE80211_NWID_LEN+1];
    u_int8_t                          iv_esslen;
    u_int8_t                          iv_ibss_peer_count;
    u_int8_t                          iv_beacon_info_count;
    struct ieee80211_ibss_peer_list   iv_ibss_peer[8]; /*BSSID and RSSI info*/

    bool                             beacon_reinit_done;
#if WDS_VENDOR_EXTENSION
    u_int8_t                          iv_wds_rx_policy;
#define WDS_POLICY_RX_UCAST_4ADDR       0x01
#define WDS_POLICY_RX_MCAST_4ADDR       0x02
#define WDS_POLICY_RX_DEFAULT           0x03
#define WDS_POLICY_RX_MASK              0x03
#endif
#ifdef ATH_SUPPORT_QUICK_KICKOUT
    u_int8_t                          iv_sko_th;        /* station kick out threshold */
#endif /*ATH_SUPPORT_QUICK_KICKOUT*/
    ieee80211_vap_input_mgmt_filter   iv_input_mgmt_filter;   /* filter  input mgmt frames */
    ieee80211_vap_output_mgmt_filter  iv_output_mgmt_filter;  /* filter outpur mgmt frames */
    struct ieee80211_roam_t           iv_roam; /* roam context */
#if WLAN_OBJMGR_REF_ID_TRACE
    u_int8_t                          iv_ref_leak_test_flag;
#endif
    QDF_STATUS                        (*iv_hostap_up_pre_init)(struct wlan_objmgr_vdev *, bool);
    QDF_STATUS                        (*iv_up_complete)(struct wlan_objmgr_vdev *);
    int                               (*iv_join)(struct ieee80211vap *);
    QDF_STATUS                        (*iv_down)(struct wlan_objmgr_vdev *);
    QDF_STATUS                        (*iv_stop_pre_init)(struct wlan_objmgr_vdev *);
    QDF_STATUS                        (*iv_get_restart_target_status)(struct wlan_objmgr_vdev *vdev, int);
    QDF_STATUS                        (*iv_get_phymode)(struct wlan_objmgr_vdev *vdev,
                                                           struct ieee80211_ath_channel *chan);
#if OBSS_PD
    QDF_STATUS                        (*iv_send_obss_spatial_reuse_param)(struct wlan_objmgr_vdev *vdev);
#endif
    QDF_STATUS                        (*iv_vap_start_rsp_handler)(struct vdev_start_response *, struct vdev_mlme_obj *);
    int                               (*iv_dfs_cac)(struct ieee80211vap *);
    void                              (*iv_update_ps_mode)(struct ieee80211vap *);
#if WLAN_SUPPORT_FILS
    void                              (*iv_cleanup)(struct ieee80211vap *);
#endif
    void                              (*iv_config_bss_color_offload)(struct ieee80211vap *, bool);
    int                               (*iv_listen)(struct ieee80211vap *);

    int                               (*iv_root_authorize)(struct ieee80211vap *, u_int32_t);
    void                              (*iv_update_node_txpow)(struct ieee80211vap *, u_int16_t , u_int8_t *);

    int                               (*iv_reg_vap_ath_info_notify)(struct ieee80211vap *,
                                                                    ath_vap_infotype,
                                                                    ieee80211_vap_ath_info_notify_func,
                                                                    void *);
    int                               (*iv_vap_ath_info_update_notify)(struct ieee80211vap *,
                                                                       ath_vap_infotype);
    int                               (*iv_dereg_vap_ath_info_notify)(struct ieee80211vap *);
    int                               (*iv_vap_ath_info_get)(struct ieee80211vap *,
                                                             ath_vap_infotype,
                                                             u_int32_t *, u_int32_t *);
    int                               (*iv_enable_radar_table)( struct ieee80211com *,
                                                         struct ieee80211vap *, u_int8_t, u_int8_t);
    int                               (*iv_vap_send)(struct ieee80211vap *, wbuf_t);
    int                               (*iv_peer_rel_ref)(struct ieee80211vap *,
                                                         struct ieee80211_node *, uint8_t *peer_mac);
#if ATH_WOW
#define IEEE80211_F_WOW_DEAUTH             1               /* Deauth Pattern */
#define IEEE80211_F_WOW_DISASSOC           2               /* Disassoc Pattern */
#endif
    bool                              (*iv_modify_bcn_rate)(struct ieee80211vap *);
    u_int16_t                         iv_mgt_rate;       /* rate to be used for management rates */
    u_int16_t                         iv_prb_rate;       /* rate to be used for probe-response */
    u_int16_t                         iv_prb_retry;      /* retry limit to be used for probe-response */
#if UMAC_SUPPORT_NAWDS
    struct ieee80211_nawds            iv_nawds;
#endif

    ieee80211_vap_tsf_offset          iv_tsf_offset;    /* TSF-related data utilized for concurrent multi-channel operations */
    struct ieee80211_chanutil_info     chanutil_info; /* Channel Utilization information */

                              /* in activity timeouts */
    u_int8_t    iv_inact_init;
    u_int8_t    iv_inact_auth;
    u_int8_t    iv_inact_probe;
    u_int16_t   cont_auth_fail;
    u_int16_t   max_cont_auth_fail;
    u_int32_t   iv_session;      /* STA session time */
#ifdef ATH_SUPPORT_TxBF
    u_int8_t    iv_txbfmode;
    u_int8_t    iv_autocvupdate;
    u_int8_t    iv_cvupdateper;
#endif
    struct ieee80211_node             *iv_ni;
    struct ieee80211_ath_channel          *iv_cswitch_chan;
    os_timer_t                        iv_cswitch_timer;
    os_timer_t                        iv_disconnect_sta_timer;
#if ATH_SUPPORT_WAPI
    u32    iv_wapi_urekey_pkts;/*wapi unicast rekey packets, 0 for disable*/
    u32    iv_wapi_mrekey_pkts;/*wapi muiticast rekey packets, 0 for disable*/
#endif

    struct ieee80211_quiet_param *iv_quiet;
#if UMAC_SUPPORT_RRM
    ieee80211_rrm_t             rrm; /* handle for rrm  */
#endif
#if UMAC_SUPPORT_WNM
    ieee80211_wnm_t             wnm; /* handle for wnm  */
#endif
#define IEEE80211_SCAN_BAND_ALL            (0)
#define IEEE80211_SCAN_BAND_2G_ONLY        (1)
#define IEEE80211_SCAN_BAND_5G_ONLY        (2)
#define IEEE80211_SCAN_BAND_CHAN_ONLY      (3)
    u_int8_t                    iv_scan_band;       /* only scan channels of requested band */
#if ATH_SUPPORT_HS20
    u_int8_t                    iv_access_network_type;
    u_int8_t                    iv_osen;
    u_int8_t                    iv_hessid[QDF_MAC_ADDR_SIZE];
    struct ieee80211_qos_map    iv_qos_map;
    u_int32_t                   iv_hotspot_xcaps;
    u_int32_t                   iv_hotspot_xcaps2;
    u_int32_t                   iv_hc_bssload;
#endif
#if ATH_SUPPORT_WPA_SUPPLICANT_CHECK_TIME
    int                 iv_rejoint_attemp_time;
#endif
    struct ieee80211_tim_set iv_tim_infor;
#if UMAC_SUPPORT_WNM
    u_int16_t           iv_wnmsleep_intval;
    u_int8_t            iv_wnmsleep_force;
#endif
#if ATH_SUPPORT_WRAP
    u_int8_t            iv_no_event_handler;
#if WLAN_QWRAP_LEGACY
    int (*iv_wrap_mat_tx)(struct ieee80211vap *out, wbuf_t);
    int (*iv_wrap_mat_rx)(struct ieee80211vap *in, wbuf_t);
#endif
#endif
    bool iv_is_started;
    bool iv_is_up;
#if ATH_SUPPORT_HYFI_ENHANCEMENTS
    u_int8_t            iv_nopbn;  /* no push button notification */
#endif
#if UMAC_PER_PACKET_DEBUG
#define PROC_FNAME_SIZE 20
    int16_t iv_userrate;
    int8_t iv_userretries;
    int8_t iv_usertxpower;
    struct proc_dir_entry   *iv_proc_entry;
    struct proc_dir_entry   *iv_proc_root;
    u_int8_t                 iv_proc_fname[PROC_FNAME_SIZE];
    int8_t iv_usertxchainmask;
#endif
    /* vap tx dynamic aponly mode control */
    bool                    iv_aponly;
    /* force to tx with one chain for legacy client */
    bool                    iv_force_onetxchain;
    bool                       iv_set_vht_mcsmap;

    u_int8_t                   iv_vht_fixed_mcs;        /* VHT Fixed MCS Index */
    u_int8_t                   iv_he_fixed_mcs;         /* HE  Fixed MCS Index */
    u_int8_t                   iv_opmode_notify;        /* Op Mode notification On:1 Off:0 */
    u_int8_t                   iv_rtscts_enabled;       /* RTS-CTS 1: enabled, 0: disabled */
    u_int8_t                   iv_rc_num_retries;       /* Number of HW retries across rate-series */
    int                        iv_bcast_fixedrate;      /* Bcast data rate */
    u_int16_t                  iv_vht_tx_mcsmap;        /* VHT TX MCS MAP */
    u_int16_t                  iv_vht_rx_mcsmap;        /* VHT RX MCS MAP */
    u_int16_t                  iv_disabled_legacy_rate_set; /* disable the legacy rates set for nodes connected to this vap. Each bit represents a legacy rate*/
    u_int16_t                  iv_configured_vht_mcsmap;  /* All only those VHT mcs rates for nodes connected to this vap on this field */
    u_int8_t                   iv_disabled_ht_mcsset[16]; /* disable these HT MCS rates set for nodes connected to this vap. Each bit represents a MCS rate*/
    ieee80211_vht_mcs_set_t    iv_vhtcap_max_mcs;      /* VHT Supported MCS set */
    spinlock_t                 init_lock;
    u_int16_t                  min_dwell_time_passive;  /* Min dwell time for scan */
    u_int16_t                  max_dwell_time_passive;	/* Max dwell time for scan */
    rwlock_t            iv_tim_update_lock;
#if QCA_LTEU_SUPPORT
    u_int32_t                  scan_rest_time;          /* overriding san param */
    u_int32_t                  scan_idle_time;          /* overriding san param */
    u_int16_t                  mu_start_delay;          /* Delay between setting channel and starting MU */
    u_int16_t                  wifi_tx_power;           /* WiFi STA's Tx power, assumed for MU computation */
#endif

    char                       iv_oper_rates[IEEE80211_RATE_MAX]; /*operational rates set by the user*/
    char                       iv_basic_rates[IEEE80211_RATE_MAX]; /*basic rates set by the user*/
    u_int32_t                  iv_vht80_ratemask;       /* VHT80 Auto Rate MASK */
    u_int16_t                  iv_vht_sgimask;          /* VHT SGI MASK */
/*
 To support per vap dscp to tid mapping.
 If vap specific dscp to tid mapping is not enabled,
 radio specific dscp to tid mapping will be effective by default.
*/
#if ATH_SUPPORT_DSCP_OVERRIDE
    u_int8_t iv_dscp_map_id;
    u_int8_t                   iv_vap_dscp_priority;        /* VAP Based DSCP - per vap priority */
#endif
 u_int8_t                   iv_pause_scan;
#if ATH_PERF_PWR_OFFLOAD
    u_int8_t                   iv_tx_encap_type;        /* per VAP tx encap type - should be used
                                                           when setting up HTT desc for packets for this VAP */
    u_int8_t                   iv_rx_decap_type;        /* per VAP Rx decap type */
#if QCA_SUPPORT_RAWMODE_PKT_SIMULATION
    u_int8_t                   iv_rawmodesim_txaggr:4,    /* per VAP Enable/disable raw mode simulation Tx A-MSDU aggregation */
                               iv_rawmodesim_debug_level:2,     /* per VAP raw mode simulation debug level. 0:disable
                                                                   1:enable & print header 2: enable & print entire packet*/
                               iv_fixed_frm_cnt_flag:2;   /* 1:fixed encap frame set 2:fixed decap frame mode set */
    int                        iv_num_encap_frames;       /* number of frames to dump before and after encap */
    int                        iv_num_decap_frames;       /* number of frames to dump before and after decap */
#endif /* QCA_SUPPORT_RAWMODE_PKT_SIMULATION */
#endif /* ATH_PERF_PWR_OFFLOAD */
    u_int8_t                   mhdr_len;
#if MESH_MODE_SUPPORT
    u_int8_t                   iv_mesh_vap_mode;        /* mesh vap flag */
#endif
    u_int8_t                   mac_entries;
    u_int32_t                  iv_sifs_trigger_rate;    /* per VAP sifs trigger rate */
    u_int32_t                  iv_sifs_trigger_time;    /* per VAP sifs trigger interval */
    u_int8_t                   mcast_encrypt_addr[QDF_MAC_ADDR_SIZE];      /* address for Encrypt_mcast frame */
    ATH_LIST_HEAD(,ieee80211_mac_filter_list) mac_filter_hash[32];
    u_int32_t                  iv_smart_mesh_cfg; /* smart MESH configuration */
#if MESH_MODE_SUPPORT
    u_int32_t                  mhdr; /* for mesh testing */
    u_int32_t                  mdbg; /* for mesh testing */
    u_int8_t bssid_mesh[QDF_MAC_ADDR_SIZE];
    u_int32_t                  iv_mesh_cap; /*for mesh vap capabilities */
#endif
#if ATH_SUPPORT_NAC
    struct ieee80211_nac       iv_nac;

    int                       (*iv_neighbour_rx)(struct ieee80211vap *, u_int32_t bssid_idx,
                                          enum ieee80211_nac_param action, enum ieee80211_nac_mactype type,
                                          u_int8_t macaddr[QDF_MAC_ADDR_SIZE]);
    int				(*iv_neighbour_get_max_addrlimit)(
		    			struct ieee80211vap *,
					enum ieee80211_nac_mactype type);
#endif
    u_int32_t                  iv_mesh_mgmt_txsend_config:1, /* flag to enable/disable probe and disassoc tx in mesh mode*/
                               iv_novap_reset:1,    /* per VAP Enable/disable reset in NSS setting */
                               lci_enable:1,
                               lcr_enable:1,
                               iv_disable_htmcs:1,
                               iv_bcast_rate_config_defered:1, /* Non Zero - Broadcast cast rate confuguration is defered */
                               iv_send_deauth:1, /* for sending deauth instead of disassoc while doing apdown */
                               iv_wep_keycache:1, /* static wep keys are allocated in first four slots in keycahe */
                               iv_cswitch_rxd:1,
                               iv_tx_stbc:1,              /* TX STBC Enable:1 Disable:0 */
                               iv_rx_stbc:3,              /* RX STBC Enable:(1,2,3) Disable:0 */
                               iv_special_vap_mode:1,     /* special vap flag */
                               iv_special_vap_is_monitor:1, /* special vap itself is monitor vap */
                               iv_smart_monitor_vap:1,     /* smart monitar vap */
                               iv_rev_sig_160w:1,      /* Use revised signalling
                                                          for 160/80+80 MHz */
                               channel_switch_state:1, /* flag to decide if vap undergoes channel switch*/
                               iv_disable_ht_tx_amsdu:1, /* 0-Enable/1-Disable 11n Tx AMSDU */
                               iv_cts2self_prot_dtim_bcn:1, /* 0-Disable/1-Enable CTS2SELF protection for DTIM beacons */
                               iv_enable_vsp:1, /* 1-Enable/0-Disable VSP */
                               iv_cfg_raw_dwep_ind:1,  /* flag to indicate if dummy/null key plumbing is reqd for dynamic WEP */
                               iv_no_cac:1,            /* is cac needed */
                               watermark_threshold_flag:1,    /* flag to indicate the threshold reached status */
                               iv_csmode:2,        /* Channel switch mode to be announced */
                               iv_beacon_prot:1,        /* Beacon Protection announcement*/
#if ATH_DATA_RX_INFO_EN
                               rxinfo_perpkt:1, /* flag to decide whether to update rxinfo per packet*/
#endif
                               iv_enable_ecsaie:1; /* enabel/disable ECSA IE */
    uint8_t                    rtt_enable; /* Enable/disable RTT initiator and responder modes */
    struct ieee80211_clone_params cp;
    ieee80211_mbo_t            mbo; /* pointer to mbo functionality */
    ieee80211_oce_t            oce; /* pointer to oce functionality */
    u_int32_t                  nbr_scan_period;      /* Neighbor AP scan period in sec */
    u_int32_t                  nbr_scan_ts;          /* Neighbor AP scan timestamp */
    bool                       rnr_enable;           /* enable inclusion of RNR IE in Beacon and Prb-Rsp */
    bool                       rnr_enable_fd;        /* enable inclusion of RNR IE in FILS Discovery */
    bool                       rnr_ap_info_complete;
    bool                       ap_chan_rpt_enable;   /* enable inclusion of AP Channel Report IE in Beacon and Prb-Rsp */
    bool                       ap_chan_rpt_ssid_filter;   /* Filter AP Channel Report IE in Beacon and Prb-Rsp based on VAP SSID */
    bool                       ap_chan_rpt_opclass_filter;   /* Filter AP Channel Report IE in Beacon and Prb-Rsp based on VAP opclass*/
    u_int8_t                   rnr_ap_info[RNR_MAX_SIZE];
    u_int32_t                  rnr_ap_info_len;
    ap_chan_rpt_t              ap_chan_rpt[ACR_MAX_ENTRIES];
    u_int32_t                  ap_chan_rpt_cnt;
    u_int64_t                  iv_estimate_tbtt;
#if QCA_LTEU_SUPPORT
    u_int32_t                  scan_probe_spacing_interval; /* Num of intervals in dwell time, at end of interval a prb req is sent */
#endif
    u_int32_t                  watermark_threshold;         /* config parameter to store the threshold value */
    u_int32_t                  watermark_threshold_reached; /* number of times the threshold has been breached */
    u_int32_t                  assoc_high_watermark;        /* Max Number of clients has been associated post crossing the threshold */
    u_int32_t                  assoc_high_watermark_time;   /* Time stamp when the max number of clients has been associated */
#ifdef MU_CAP_WAR_ENABLED
    MU_CAP_WAR                 iv_mu_cap_war;
#endif
    u_int32_t                  iv_cfg_nstscap_war;
   /* Configure association WAR for 160 MHz width (i.e. 160/80+80 MHz
    * modes). Some STAs may have an issue associating with us if we advertise
    * 160/80+80 MHz related capabilities in probe response/association response.
    * Hence this WAR suppresses 160/80+80 MHz related information in probe
    * responses and association responses for such STAs.
    * Starting from LSB
    * First bit set        = Default WAR behavior (VHT_OP modified)
    * First+second bit set = (VHT_OP+ VHT_CAP modified)
    * No bit set (default) = WAR disabled
    */
#define IEEE80211_VAP_CFG_ASSOC_WAR_160_MASK 0x0003
    u_int8_t                   iv_cfg_assoc_war_160w;
    u_int8_t                   iv_ecsa_opclass;  /* opClass to advertise in ECSA IE */
    u_int8_t                   iv_enable_sae_pwid;  /* support sae pwid IE*/
    u_int8_t                   iv_sae_pk_en;        /* support sae pk IE */
    u_int8_t                   iv_sae_pwe;          /* support sae password IE */
    u_int8_t                   iv_tx_power_enable;  /* support tx power as OCE attribute in Beacon and Probe response */
    int8_t                     iv_tx_power_value;   /* OCE Userdefine TX power value */
    u_int64_t                  iv_ip_subnet_id;     /* support ip subnet identifier as OCE attribute in Beacon and Probe response */
    u_int8_t                   iv_planned_ess;
    u_int8_t                   iv_edge_ess;
    u_int8_t                   iv_bss_rssi_threshold;
    u_int8_t                   iv_rsnx_override;    /* rsnx override value */
#if UMAC_SUPPORT_ACFG
    u_int32_t                  iv_diag_warn_threshold;
    u_int32_t                  iv_diag_err_threshold;
#endif
#if DYNAMIC_BEACON_SUPPORT
#define DEFAULT_DBEACON_SNR_THRESHOLD  60            /* dBm,  default snr threshold */
#define DEFAULT_DBEACON_TIMEOUT 30                    /* in secs, timeout of timer*/
    int8_t                     iv_dbeacon_snr_thr;   /* STA probe req SNR threshold for Dynamic beacon */
    u_int16_t                  iv_dbeacon_timeout;    /* Duration the vap beacon after STA connected */
    os_timer_t                 iv_dbeacon_suspend_beacon;
    qdf_spinlock_t             iv_dbeacon_lock;
#endif
    u_int8_t                   iv_txpow_mgt_frm[MAX_NUM_TXPOW_MGT_ENTRY];
    u_int8_t                   iv_txpow_frm[MAX_NUM_TXPOW_FRAME_TYPE][MAX_NUM_TXPOW_MGT_ENTRY];
    void                       (*iv_txpow_mgmt)(struct ieee80211vap *,int frame_subtype,u_int8_t transmit_power);
    void                       (*iv_txpow)(struct ieee80211vap *,int frame_type,int frame_subtype,u_int8_t transmit_power);
    qdf_atomic_t               iv_is_start_sent; /* Flag used to serialize vdev_start and vdev_stop commands */
    qdf_atomic_t               iv_restart_pending; /* Flag used to serialize vdev_start and vdev_restart commands */
    qdf_atomic_t               iv_is_start_resp_received; /* Flag used to serialize vdev_start and vdev_restart commands */
    struct wlan_objmgr_vdev    *vdev_obj; /* UMAC vdev object pointer */
    struct vdev_mlme_obj       *vdev_mlme; /* MLME vdev object pointer */
#if UMAC_SUPPORT_CFG80211
    u_int8_t                    iv_cfg80211_create;
    struct cfg80211_chan_def    chandef_notify; /* channel notify data structure */
    qdf_work_t                  cfg80211_channel_notify_workq; /* WORKQ for cfg80211 channel notify event */
#endif
    u_int32_t                  iv_he_ctrl:1,              /* HE Signals control support Enable:1 Disable:0 */
                               iv_he_twtreq:1,            /* HE TWT Request Enable:1 Disable:0 */
                               iv_he_twtres:1,            /* HE TWT Responder Enable:1 Disable:0 */
                               iv_he_frag:2,              /* HE Frag Size - 0,1,2,3 */
                               iv_he_max_frag_msdu:3,     /* HE Max Frag MSDU : 2^n */
                               iv_he_min_frag_size:2,     /* HE Min Frag BA size : 0-128,1-256, 2-512, 3-no restriction */
                               iv_he_multi_tid_aggr:3,    /* HE  Multi TID : Signals multi TID not more than this value +1 */
                               iv_he_link_adapt:2,        /* HE Signals Link Adaptation Enable:1 Disable:0 */
                               iv_he_all_ack:1,           /* HE ALL Ack - Single Bit Signalling  Enable:1 Disable:0 */
                               iv_he_ul_mu_sched:1,       /* HE Signals RU allocation Enable:1 Disable:0 */
                               iv_he_actrl_bsr:1,         /* HE ACtrl - Buffer status report enable:1 Disable:0 */
                               iv_he_32bit_ba:1,          /* HE 32bit BA support Enable:1 Disable:0 */
                               iv_he_mu_cascade:1,        /* HE Signals MU Cascade Enable:1 Disable:0 */
                               iv_he_omi:1,               /* HE Signals OMI  Enable:1 Disable:0 */
                               iv_he_ofdma_ra:1,          /* HE Signals OFDMA Random Access  Enable:1 Disable:0 */
                               iv_he_max_ampdu_len_exp:2, /* HE Signals value of Maximum AMPDU Exponent Extension*/
                               iv_he_amsdu_frag:1,        /* HE Signals AMSDU Frag support Enable:1 Disable:0 */
                               iv_he_multi_tid_aggr_tx:3, /* HE  Multi TID Tx: Signals multi TID not more than this value +1 */
                               iv_he_flex_twt:1,          /* HE Signals Flex TWT Enable:1 Disable:0 */
                               iv_he_bsr_supp:1;          /* HE BSR Support Enable:1 Disable:0 */


    u_int32_t                  iv_he_relaxed_edca:1,      /* HE Signalls EDCA support Enable:1 Disable:0 */
                               iv_he_spatial_reuse:1,     /* HE Signals Spatial Reuse Enable:1 Disable:0 */
                               iv_he_multi_bss:1,         /* HE Multi Bss Enable:1 Disable:0 */
                               iv_he_su_ppdu_1x_ltf_800ns_gi:1, /* HE Signals 800ns GI for 1x ltf */
                               iv_he_su_mu_ppdu_4x_ltf_800ns_gi:1, /* HE Signals 800ns GI for 4x ltf */
                               iv_he_ndp_4x_ltf_3200ns_gi:1, /* HE Signals 3200ns GI for NDP with 4x HE-LTF */
                               iv_he_er_su_ppdu_1x_ltf_800ns_gi:1, /* HE Signals 800ns GI for ER SU with 4x HE-LTF */
                               iv_he_er_su_ppdu_4x_ltf_800ns_gi:1, /* HE Signals 800ns GI for ER SU with 1x HE-LTF */
                               iv_he_su_bfee:1,           /* HE SU BFEE */
                               iv_he_su_bfer:1,           /* HE SU BFER */
                               iv_he_mu_bfee:1,           /* HE MU BFEE */
                               iv_he_mu_bfer:1,           /* HE MU BFER */
                               iv_he_ul_muofdma:1,        /* HE UL MU OFDMA */
                               iv_he_dl_muofdma:1,        /* HE DL MU OFDMA */
                               iv_he_dl_muofdma_bfer:1,   /* HE DL MU OFDMA BFER*/
                               iv_he_ul_mumimo:1,         /* HE DL MU MIMO */
                               iv_he_sounding_mode:4,     /* HE/VHT, SU/MU and Trig/Non-Trig sounding */
                               iv_he_muedca:1;            /* HE MU EDCA */

    u_int32_t                  iv_he_tx_mcsnssmap;        /* HE TX MCS NSS MAP */
    u_int32_t                  iv_he_rx_mcsnssmap;        /* HE RX MCS NSS MAP */

    u_int32_t                  iv_he_default_pe_durn:3,   /* HE Default Pe Durn */
                               iv_he_twt_required:1,      /* HE TWT required  */
                               iv_he_rts_threshold:10,    /* HE RTS threshold duration */
                               iv_he_part_bss_color:1,    /* HE partial BSS color */
                               iv_he_vht_op_present:1,    /* HE VHT OP Info present */
                               iv_he_multiple_bssid_ap:1, /* HE Multiple BSSID AP*/
                               iv_he_tx_bssid_ind:1,      /* HE Transmit BSSID indicator */
                               iv_he_6g_bcast_prob_rsp:1; /* HE 6GHz Bcast Probe Resp */
    u_int8_t                   iv_he_6g_bcast_prob_rsp_intval; /* HE 6GHz Bcast Probe Resp intval */

    u_int16_t                  iv_he_extended_range:1,    /* HE Extended Range*/
                               iv_he_dcm:1,               /* HE DCM Coding*/
                               iv_he_ltf:3,               /* HE LTF */
                               iv_he_ar_ldpc:2;           /* HE Autorate LDPC */
    u_int16_t                  iv_he_ar_gi_ltf;           /* HE Autorate GI LTF */

    uint32_t                    iv_bcn_offload_enable:1,
                               iv_ext_nss_support:1,        /* Flag to enable/disable extended NSS support. Referred on TX.
                                                             Support can be enabled only if capability is enabled */
                               iv_lite_monitor:1,         /* To disable promiscous mode in monitor vap */
                               iv_vht_mcs10_11_supp:1,      /* Enables/Disables overall support for VHT MCS10/11 */
                               iv_he_mcs12_13_supp:1,      /* Enables/Disables overall support for HE MCS12/13 */
                               iv_vht_mcs10_11_nq2q_peer_supp:1, /* Enables/Disables support of VHT MCS10/11
                                                           * for non Q2Q peer */
                               iv_refuse_all_addbas:1,      /* User configurability to refuse all incoming add block acks */
                               iv_ba_buffer_size:1,       /* Block Ack Buffer Size */
                               iv_mscs:1,                 /* MSCS procedures support */
#ifdef QCA_OL_DMS_WAR
                               dms_amsdu_war:1,              /* Indicate if DMS A-MSDU WAR is required for Multicast packets */
                               iv_me_amsdu:1,                /* Enable ME with AMSDU encapsulation */
#endif
                               iv_read_rxprehdr:1,         /* Rx preheader read enable for packets received on vap dev */
    /* Enable/disable the advertisement of STA-mode's maximum capabilities instead of it's operating mode in the capability related IEs.
       This flag precedence will be superceded by the ic_enable2GHzHt40Cap in 2.4GHz STA modes */
                               iv_sta_max_ch_cap:1,
                               iv_rawmode_open_war:1, /* rawmode open war configuration */
                               restart_txn:1,
                               iv_sr_ie_reset:1,
                               iv_dpp_vap_mode:1,     /* dpp vap flag */
                               iv_csa_interop_phy:1,
                               iv_csa_interop_bss:1,
                               iv_csa_interop_auth:1,
                               force_cleanup_peers:1,
                               enable_multi_group_key:1,
                               iv_cfg80211_acs_pending:1,
                               iv_enable_mcast_rc:1,
                               iv_enable_max_ch_sw_time_ie:1, /* enable/disable Maximum Channel Switch IE */
                               iv_wmm_enable:1,
                               rnr_enable_tbtt:1;      /* enable calculation of TBTT in RNR IE */

    qdf_atomic_t               vap_bcn_event;             /* Set when wmi_bcn_event occurs*/
    systick_t                  vap_bcn_stats_time;        /* last beacon stats received time */
    qdf_atomic_t               vap_prb_fils_event;        /* Set when probe fils update event occurs*/
    qdf_atomic_t               vap_tsf_event;             /* Set when tsf event occurs */
    uint64_t                   iv_tsf;
    void (*get_vdev_bcn_stats)(struct ieee80211vap *vap);
    void (*reset_bcn_stats)(struct ieee80211vap *vap);
    void (*get_vdev_prb_fils_stats)(struct ieee80211vap *vap);
    u_int8_t                   iv_csl_support;            /* Config for CSL*/
    bool                       iv_remove_csa_ie; /* Need to remove CSA IE or not*/
    bool                       iv_no_restart; /* No need to restart the vaps */
    u_int8_t                   iv_he_max_mcs_supp;        /* Maximum MCS support */
#if QCN_IE
    u_int8_t                   iv_bpr_enable : 1;     /* If set, broadcast probe response is enabled */
    u_int8_t                   iv_bpr_delay;          /* Holds broadcast probe response time in ms*/
    qdf_ktime_t                iv_next_beacon_tstamp; /* Stores the timestamp of the next beacon to be scheduled */
    qdf_hrtimer_data_t         bpr_timer;             /* Holds a hrtimer to send broadcast probe response from a vap */
    u_int16_t                  iv_bpr_timer_start_count; /* Stores the number of times hrtimer restarted */
    u_int16_t                  iv_bpr_timer_resize_count; /* Stores the number of times hrtimer got resized */
    u_int16_t                  iv_bpr_callback_count; /* Stores the number of times broadcast probe response is sent */
    u_int16_t                  iv_bpr_timer_cancel_count; /* Stores the number of times the beacon is sent instead of
                                                             broadcast probe response */
    u_int16_t                  iv_bpr_unicast_resp_count; /* Stores the number of times unicast probe response is sent */
#endif
    u_int16_t                   iv_htflags;            /* HT state flags */
#if ATH_ACL_SOFTBLOCKING
#define SOFTBLOCK_WAIT_TIME_DEFAULT 10000
#define SOFTBLOCK_WAIT_TIME_MIN 0
#define SOFTBLOCK_WAIT_TIME_MAX 100000
#define SOFTBLOCK_ALLOW_TIME_DEFAULT 15000
#define SOFTBLOCK_ALLOW_TIME_MIN 0
#define SOFTBLOCK_ALLOW_TIME_MAX 100000
    u_int32_t                  iv_softblock_wait_time;  /* wait time in sofblokcing functionality */
    u_int32_t                  iv_softblock_allow_time; /* allow time in softblocking functionality */
#endif
    uint64_t                wmi_tx_mgmt; /* mgmt frames sent over this vap */
    uint64_t                wmi_tx_mgmt_completions; /* mgmt completions received over this vap */
#if ATH_SUPPORT_NAC_RSSI
    struct ieee80211_nac_rssi       iv_nac_rssi;
    int         (*iv_scan_nac_rssi)(struct ieee80211vap *vap, enum ieee80211_nac_rssi_param cmd,
                                   u_int8_t *, u_int8_t *, u_int8_t );
#endif

#if QCA_SUPPORT_PEER_ISOLATION
    struct peer_isolation_list peer_isolation_list;    /* Peer isolation list */
#endif
    uint64_t                wmi_tx_mgmt_sta; /* mgmt frames sent over this vap using sta peers */
    uint64_t                wmi_tx_mgmt_completions_sta; /* mgmt completions received over this vap using sta peers*/
    uint8_t*                 iv_sta_assoc_resp_ie;
    qdf_semaphore_t          iv_sem_lock; /* Lock to serialize mlme_stop_bss and mlme_start_bss */
    uint16_t                 iv_sta_assoc_resp_len;
#if UMAC_SUPPORT_WPA3_STA
    bool                     iv_sta_external_auth_enabled; /* external auth is enabled by supplicant*/
    qdf_timer_t              iv_sta_external_auth_timer;   /* Start timer to track external auth by supplicant */
    u_int8_t                 iv_sae_max_auth_attempts;     /* indicate maximum no.of auth attempts that STA can */
#endif

   /* MBSS IE feature */
   struct ieee80211_mbss    iv_mbss;
   int32_t                  iv_available_bcn_cmn_space; /* available space in common part of the beacon */
   int32_t                  iv_available_prb_cmn_space; /* available space in common part of the probe response */
   bool                     iv_vap_up_in_progress; /* indicate whether beacon_init comes from beacon alloc or update */
   qdf_timer_t              peer_cleanup_timer;
   qdf_timer_t              peer_del_wait_timer;
   qdf_timer_t              peer_del_bss_tid_flush;

   qdf_list_t            iv_mgmt_offchan_req_list;
   qdf_spinlock_t        iv_mgmt_offchan_tx_lock;
   qdf_atomic_t          iv_mgmt_offchan_cmpl_pending;
   struct ieee80211_offchan_req iv_mgmt_offchan_current_req;
   /* to indicate vap start request failed */
   bool                     vap_start_failure;
   /* to indicate whether action was taken for vap start failure*/
   bool                     vap_start_failure_action_taken;

   u_int8_t                 iv_csa_interop_bss_active;
   uint8_t                  iv_csa_deauth_mode; /* During CSA, Send deauth either by 1. unicast, 2. bcast */
    uint32_t                iv_pcp_tid_map[IEEE80211_PCP_MAX]; /* pcp-tid map */
    uint8_t                 iv_tidmap_tbl_id; /* tidmap tbl_id in use */
    uint8_t                 iv_tidmap_prty; /* priority order for tid-mapping */
   uint16_t                iv_vlan_map[2 * MAX_VLAN];
   uint32_t                 iv_mcast_rc_stale_period;
   uint32_t                 max_group_keys;
   uint16_t                 iv_max_mtu_size;
    bool                   iv_bcn_csa_tmp_sent; /* Set this flag when host
                                                 * sends CSA IE to FW and clear
                                                 * it in CSA event handler. */
    /* To indicate STA is performing roam operation */
    bool                     iv_roam_inprogress;
#if ATH_SUPPORT_UORA
    u_int8_t                          iv_ocw_range; /* OFDMA Contention Window */
#endif
   uint64_t                 iv_vap_tbtt_qtime; /* Tbtt of the vap */
   uint64_t                 iv_tbtt_multisoc_delta;
    /* Flag to add RRM capabilities IE in assoc request for STA mode */
    bool                    iv_rrm_cap_ie;
    /* Set iv_user_disabled_vap_doth flag, if the user configures vap_doth as 0*/
    bool                    iv_user_disabled_vap_doth;
    uint8_t                 iv_oob_update;
    uint32_t                iv_obss_pd_thresh;
    uint32_t                iv_self_sr_enable_per_ac;
    uint8_t                 iv_psr_tx_enable; /* PSR Tx enable */
    /* SRP IE parameters */
    /* SR Ctrl PSR Disallowed config */
    uint8_t                             iv_he_srctrl_psr_disallowed:1,
    /* SR Ctrl Non-SRG OBSS PD SR Disallowed config */
                                        iv_he_srctrl_non_srg_obsspd_disallowed:1,
    /* SR Ctrl Non-SRG Offset Present */
                                        iv_he_srctrl_non_srg_offset_present:1,
    /* SR Ctrl SRG Information Present */
                                        iv_he_srctrl_srg_info_present:1,
    /* SR Ctrl HESIGA_Spatial_reuse_value15_allowed config */
                                        iv_he_srctrl_sr15_allowed:1,
    /* Saptial Reuse param updated */
                                        iv_is_spatial_reuse_updated:1;
    /* SRG OBSS Color Bitmap */
    uint32_t                            iv_he_srp_ie_srg_bss_color_bitmap[2];
    /* SRG BSSID Partial Bitmap */
    uint32_t                            iv_he_srp_ie_srg_partial_bssid_bitmap[2];
    /* SR Ctrl SRG OBSS PD Max/Min Offset */
    uint8_t                             iv_he_srp_ie_srg_obsspd_max_offset;
    uint8_t                             iv_he_srp_ie_srg_obsspd_min_offset;
    /* Non-SRG OBSS PD max offset */
    uint8_t                             iv_he_srp_ie_non_srg_obsspd_max_offset;
#if QCN_IE
    uint8_t                             iv_bsscolor_rptr_info;
#endif
    uint8_t                             iv_fwd_action_frames_to_app;
    /* 6G operation min rate in Mbps for sta to use for their Tx PPDU's in BSS */
    uint8_t                             iv_6g_he_op_min_rate;
    bool                                iv_6g_comp;
    uint32_t                            iv_keymgmt_6g_mask;
    int                                 iv_is_6g_wps;
    bool                                iv_bss_rsp_evt_status; /* bss peer response event status */
    uint32_t                            iv_bss_rsp_status; /* response status received from fw */
#if DBG_LVL_MAC_FILTERING
    struct dbglvl_mac_peer_list         *dbgmac_peer_list;    /* Peer list for MAC addr based filter */
#endif
    u_int8_t                            iv_sm_gap_ps:1,
                                        iv_static_mimo_ps:1;
} IEEE80211VAP, *PIEEE80211VAP;

#define PEER_CREATED                 0

static inline bool
wlan_vap_get_bss_rsp_evt_status(struct ieee80211vap *vap)
{
   return vap->iv_bss_rsp_evt_status;
}

static inline bool
wlan_vap_is_bss_created(struct ieee80211vap *vap)
{
   return (vap->iv_bss_rsp_status == 0) ? true : false;
}

static inline void
wlan_vap_set_bss_status(struct ieee80211vap *vap, uint32_t status)
{
   vap->iv_bss_rsp_evt_status = true;
   vap->iv_bss_rsp_status = status;
}

#if QCN_IE
#define EFF_CHAN_TIME(_chantime, _buffer)  (((_chantime) && (_chantime > _buffer)) ? ((_chantime) - (_buffer)) : (0))
#endif

#define NSTSWAR_IS_VHTCAP_CHANGE(x)         ((x))
/* Bitmap positions which define 160 MHz association WAR actions */
#define ASSOCWAR160_CONFIG_VHT_OP_CHANGE     0x00000001
#define ASSOCWAR160_CONFIG_VHT_OP_CHANGE_S   0
#define ASSOCWAR160_CONFIG_VHT_CAP_CHANGE    0x00000002
#define ASSOCWAR160_CONFIG_VHT_CAP_CHANGE_S  1

/* XXX When more bit definitions are added above, please add to the below master list as well */
#define ASSOCWAR160_CONFIG_VHT_ALL_CHANGES   (ASSOCWAR160_CONFIG_VHT_OP_CHANGE | ASSOCWAR160_CONFIG_VHT_CAP_CHANGE)

#define ASSOCWAR160_IS_VHT_OP_CHANGE(x)     ((x) & (1 << ASSOCWAR160_CONFIG_VHT_OP_CHANGE_S))
#define ASSOCWAR160_IS_DEFAULT_CONFIG(x)    ASSOCWAR160_IS_VHT_OP_CHANGE((x))
#define ASSOCWAR160_IS_VHT_CAP_CHANGE(x)    (ASSOCWAR160_IS_DEFAULT_CONFIG((x)) && \
                                                ((x) & (1 << ASSOCWAR160_CONFIG_VHT_CAP_CHANGE_S)))
#define ASSOCWAR160_IS_VALID_CHANGE(x)      (ASSOCWAR160_IS_DEFAULT_CONFIG((x)) &&  \
                                                (((x) | ASSOCWAR160_CONFIG_VHT_ALL_CHANGES) == \
                                                 ASSOCWAR160_CONFIG_VHT_ALL_CHANGES))

/* Default status of revised signalling for 160/80+80 MHz*/
#define DEFAULT_REV_SIG_160_STATUS          1

/* Smart MESH configuration */
#define SMART_MESH_DWDS 0x01                /* Dynamic WDS */
#define SMART_MESH_ACL_ENHANCEMENT 0x02      /* ACL enhancements */
#define SMART_MESH_80211_EVENTS   0x04       /* 802.11 Event enhancments */
#define SMART_MESH_80211_STATS     0x08      /* 802.11 Station statistics */
#define SMART_MESH_NONCONNETED_STATS 0x10    /* 802.11 frames of non connected clients */

#if MESH_MODE_SUPPORT
/* Mesh VAP capabiltiies */
#define MESH_CAP_BEACON_ENABLED  0x01    /* Enable/Disable Beacon on Mesh VAP */
#define MESH_DEBUG_ENABLED 0x01          /* Enable mesh debug */
#define MESH_DEBUG_DUMP_STATS 0x02       /* Dump rx stats for mesh mode */
#endif

struct global_ic_list {
    spinlock_t    radio_list_lock; /* lock to protect access to radio list */
    struct ieee80211com *global_ic[MAX_RADIO_CNT];
    u_int8_t      num_global_ic;
#if DBDC_REPEATER_SUPPORT
    u_int8_t      num_stavaps_up;
    bool          dbdc_process_enable;
    bool          is_dbdc_rootAP;
    bool          force_client_mcast_traffic; /* Set this flag when DBDC Repeater wants to send
                                               * MCAST traffic of connected client to RootAP on corresponding STA VAP
                                               */
    u_int8_t       delay_stavap_connection;
    u_int16_t      disconnect_timeout; /* Interface manager app uses this timeout to bring down AP VAPs after STAVAP disconnection*/
    u_int16_t      reconfiguration_timeout;  /* Interface manager app uses this timeout to bring down AP VAPs after STAVAP disconnection*/
    bool          iface_mgr_up;
    bool          always_primary;
    bool          drop_secondary_mcast;
    bool          same_ssid_support;
    struct ieee80211vap     *max_priority_stavap_up;    /* sta vap pointer of connected radio having higher priority*/
    u_int8_t      num_l2uf_retries;
    u_int8_t      num_fast_lane_ic;
    u_int8_t      num_rptr_clients;
    u_int8_t      extender_info;  /* 0x00->no stavap connection and RootAP access on Repeater AP
                                     0x0F->invalid state
                                     0xF0->stavap connection is there, but no rootap access on Repeater AP
                                     0xFF->stavap connection and rootap access is there on Repeater AP
                                   */
    u_int8_t      preferred_list_stavap[MAX_RADIO_CNT][QDF_MAC_ADDR_SIZE];
                                  /*Mac list of RE to which this AP has to connect*/
    u_int8_t      denied_list_apvap[MAX_RADIO_CNT][QDF_MAC_ADDR_SIZE];
                                  /*Mac list of RE STAvaps to which AP should deauth*/
    u_int8_t      ap_preferrence;
                                 /* 0 - No preferrence
                                    1 - connect to RootAP
                                    2 - connect to RE, whose bssid matches with preferred_list_stavap
                                  */
    bool          same_ssid_disable;      /* global disable same ssid feature */
    systime_t     rootap_access_downtime;
#endif
};

#ifndef __ubicom32__
#define IEEE80211_ADDR_EQ(a1,a2)        (OS_MEMCMP(a1, a2, IEEE80211_ADDR_LEN) == 0)
#define IEEE80211_ADDR_COPY(dst,src)    OS_MEMCPY(dst, src, IEEE80211_ADDR_LEN)
#else
#define IEEE80211_ADDR_EQ(a1,a2)        (OS_MACCMP(a1, a2) == 0)
#define IEEE80211_ADDR_COPY(dst,src)    OS_MACCPY(dst, src)
#endif
#define IEEE80211_ADDR_IS_VALID(a)      (a[0]!=0 || a[1]!=0 ||a[2]!= 0 || a[3]!=0 || a[4]!=0 || a[5]!=0)
#define IEEE80211_SSID_IE_EQ(a1,a2)     (((((char*) a1)[1]) == (((char*) a2)[1])) && (OS_MEMCMP(a1, a2, 2+(((char*) a1)[1])) == 0))

/* ic_flags */
#define IEEE80211_F_FF                   0x00000001  /* CONF: ATH FF enabled */
#define IEEE80211_F_TURBOP               0x00000002  /* CONF: ATH Turbo enabled*/
#define IEEE80211_F_PROMISC              0x00000004  /* STATUS: promiscuous mode */
#define IEEE80211_F_ALLMULTI             0x00000008  /* STATUS: all multicast mode */
/* NB: this is intentionally setup to be IEEE80211_CAPINFO_PRIVACY */
#define IEEE80211_F_PRIVACY              0x00000010  /* CONF: privacy enabled */
#define IEEE80211_F_PUREG                0x00000020  /* CONF: 11g w/o 11b sta's */
#define IEEE80211_F_SCAN                 0x00000080  /* STATUS: scanning */
#define IEEE80211_F_SIBSS                0x00000200  /* STATUS: start IBSS */
/* NB: this is intentionally setup to be IEEE80211_CAPINFO_SHORT_SLOTTIME */
#define IEEE80211_F_SHSLOT               0x00000400  /* STATUS: use short slot time*/
#define IEEE80211_F_PMGTON               0x00000800  /* CONF: Power mgmt enable */
#define IEEE80211_F_DESBSSID             0x00001000  /* CONF: des_bssid is set */
#define IEEE80211_F_DFS_CHANSWITCH_PENDING 0x00002000  /* STATUS: channel switch event pending after DFS RADAR */
#define IEEE80211_F_BGSCAN               0x00004000  /* CONF: bg scan enabled */
#define IEEE80211_F_SWRETRY              0x00008000  /* CONF: sw tx retry enabled */
#define IEEE80211_F_TXPOW_FIXED          0x00010000  /* TX Power: fixed rate */
#define IEEE80211_F_IBSSON               0x00020000  /* CONF: IBSS creation enable */
#define IEEE80211_F_SHPREAMBLE           0x00040000  /* STATUS: use short preamble */
#define IEEE80211_F_DATAPAD              0x00080000  /* CONF: do alignment pad */
#define IEEE80211_F_USEPROT              0x00100000  /* STATUS: protection enabled */
#define IEEE80211_F_USEBARKER            0x00200000  /* STATUS: use barker preamble*/
#define IEEE80211_F_TIMUPDATE            0x00400000  /* STATUS: update beacon tim */
#define IEEE80211_F_WPA1                 0x00800000  /* CONF: WPA enabled */
#define IEEE80211_F_WPA2                 0x01000000  /* CONF: WPA2 enabled */
#define IEEE80211_F_WPA                  0x01800000  /* CONF: WPA/WPA2 enabled */
#define IEEE80211_F_DROPUNENC            0x02000000  /* CONF: drop unencrypted */
#define IEEE80211_F_COUNTERM             0x04000000  /* CONF: TKIP countermeasures */
#define IEEE80211_F_HIDESSID             0x08000000  /* CONF: hide SSID in beacon */
#define IEEE80211_F_NOBRIDGE             0x10000000  /* CONF: disable internal bridge */
#define IEEE80211_F_WMEUPDATE            0x20000000  /* STATUS: update beacon wme */
#define IEEE80211_F_COEXT_DISABLE        0x40000000  /* CONF: DISABLE 2040 coexistance */
#define IEEE80211_F_CHANSWITCH           0x80000000  /* force chanswitch */

/* ic_flags_ext and/or iv_flags_ext */
#define IEEE80211_FEXT_WDS                 0x00000001  /* CONF: 4 addr allowed */
#define IEEE80211_FEXT_COUNTRYIE           0x00000002  /* CONF: enable country IE */
#define IEEE80211_FEXT_SCAN_PENDING        0x00000004  /* STATE: scan pending */
#define IEEE80211_FEXT_BGSCAN              0x00000008  /* STATE: enable full bgscan completion */
#define IEEE80211_FEXT_UAPSD               0x00000010  /* CONF: enable U-APSD */
#define IEEE80211_FEXT_SLEEP               0x00000020  /* STATUS: sleeping */
#define IEEE80211_FEXT_EOSPDROP            0x00000040  /* drop uapsd EOSP frames for test */
#define IEEE80211_FEXT_MARKDFS             0x00000080  /* Enable marking of dfs interfernce */
#define IEEE80211_FEXT_REGCLASS            0x00000100  /* CONF: send regclassids in country ie */
#define IEEE80211_FEXT_CCMPSW_ENCDEC       0x00000400 /* enable or disable s/w ccmp encrypt decrypt support */
#define IEEE80211_FEXT_HIBERNATION         0x00000800  /* STATE: hibernating */
#define IEEE80211_FEXT_SAFEMODE            0x00001000  /* CONF: MSFT safe mode         */
#define IEEE80211_FEXT_DESCOUNTRY          0x00002000  /* CONF: desired country has been set */
#define IEEE80211_FEXT_PWRCNSTRIE          0x00004000  /* CONF: enable power capability or contraint IE */
#define IEEE80211_FEXT_DOT11D              0x00008000  /* STATUS: 11D in used */
#define IEEE80211_FEXT_RADAR               0x00010000  /* STATUS: 11D channel-switch detected */
#define IEEE80211_FEXT_AMPDU               0x00020000  /* CONF: A-MPDU supported */
#define IEEE80211_FEXT_AMSDU               0x00040000  /* CONF: A-MSDU supported */
#define IEEE80211_FEXT_HTPROT              0x00080000  /* CONF: HT traffic protected */
#define IEEE80211_FEXT_RESET               0x00100000  /* CONF: Reset once */
#define IEEE80211_FEXT_APPIE_UPDATE        0x00200000  /* STATE: beacon APP IE updated */
#define IEEE80211_FEXT_IGNORE_11D_BEACON   0x00400000  /* CONF: ignore 11d beacon */
#define IEEE80211_FEXT_WDS_AUTODETECT      0x00800000  /* CONF: WDS auto Detect/DELBA */
#define IEEE80211_FEXT_PUREB               0x01000000  /* 11b only without 11g stations */
#define IEEE80211_FEXT_HTRATES             0x02000000  /* disable HT rates */
#define IEEE80211_FEXT_HTVIE               0x04000000  /* HT CAP IE present */
//#define IEEE80211_FEXT_DUPIE             0x08000000  /* dupie (ANA,pre ANA ) */
#define IEEE80211_FEXT_AP                  0x08000000  /* Extender AP */
#define IEEE80211_FEXT_DELIVER_80211       0x10000000  /* CONF: deliver rx frames with 802.11 header */
#define IEEE80211_FEXT_SEND_80211          0x20000000  /* CONF: os sends down tx frames with 802.11 header */
#define IEEE80211_FEXT_WDS_STATIC          0x40000000  /* CONF: statically configured WDS */
#define IEEE80211_FEXT_PURE11N             0x80000000  /* CONF: pure 11n mode */

/* ic_flags_ext2 */
#define IEEE80211_FEXT2_CSA_WAIT 0x00000001 /* radio in middle of CSA */
#define IEEE80211_FEXT2_RESET_PRECACLIST   0x00000002  /* reset precac list */
#define IEEE80211_FEXT2_RNR_SELECTIVE_ADD  0x00000004  /* RNR selective add */

/* iv_flags_ext2 */
#define IEEE80211_FEXT2_PURE11AC           0x00000001  /* CONF: pure 11ac mode */
#define IEEE80211_FEXT2_BR_UPDATE           0x00000002  /* Basic Rates Update */
#define IEEE80211_FEXT2_STRICT_BW           0x00000004  /* CONF: restrict bw ont top of per 11ac/n */
#if ATH_NON_BEACON_AP
#define IEEE80211_FEXT2_NON_BEACON          0x00000008  /*non-beaconing AP VAP*/
#endif
#define IEEE80211_FEXT2_SON                0x00000010  /* Wi-Fi SON mode (with APS) */
#define IEEE80211_FEXT2_MBO                0x00000020  /* MBO */
#define IEEE80211_FEXT2_AP_INFO_UPDATE     0x00000040  /* Wi-Fi WHC AP info update */
#define IEEE80211_FEXT2_BACKHAUL           0x00000080
#define IEEE80211_FEXT2_SON_SPL_RPT         0x00000100 /* SON SPCL Repeate mode */
#define IEEE80211_FEXT2_FILS               0x00000200
#define IEEE80211_FEXT2_NOCABQ             0x00000200  /* Don't buffer multicast when STA in ps mode */
#define IEEE80211_FEXT2_MBSS_VAP           0x00000400  /* MBSS VAP */
#define IEEE80211_FEXT2_NON_TRANSMIT_VAP   0x00000800  /* MBSS Non-transmitting VAP */
#define IEEE80211_FEXT2_MBSS_MASK          0x00000C00

/* ic_caps */
#define IEEE80211_C_WEP                  0x00000001  /* CAPABILITY: WEP available */
#define IEEE80211_C_TKIP                 0x00000002  /* CAPABILITY: TKIP available */
#define IEEE80211_C_AES                  0x00000004  /* CAPABILITY: AES OCB avail */
#define IEEE80211_C_AES_CCM              0x00000008  /* CAPABILITY: AES CCM avail */
#define IEEE80211_C_HT                   0x00000010  /* CAPABILITY: 11n HT available */
#define IEEE80211_C_CKIP                 0x00000020  /* CAPABILITY: CKIP available */
#define IEEE80211_C_FF                   0x00000040  /* CAPABILITY: ATH FF avail */
#define IEEE80211_C_TURBOP               0x00000080  /* CAPABILITY: ATH Turbo avail*/
#define IEEE80211_C_IBSS                 0x00000100  /* CAPABILITY: IBSS available */
#define IEEE80211_C_PMGT                 0x00000200  /* CAPABILITY: Power mgmt */
#define IEEE80211_C_HOSTAP               0x00000400  /* CAPABILITY: HOSTAP avail */
#define IEEE80211_C_AHDEMO               0x00000800  /* CAPABILITY: Old Adhoc Demo */
#define IEEE80211_C_SWRETRY              0x00001000  /* CAPABILITY: sw tx retry */
#define IEEE80211_C_TXPMGT               0x00002000  /* CAPABILITY: tx power mgmt */
#define IEEE80211_C_SHSLOT               0x00004000  /* CAPABILITY: short slottime */
#define IEEE80211_C_SHPREAMBLE           0x00008000  /* CAPABILITY: short preamble */
#define IEEE80211_C_MONITOR              0x00010000  /* CAPABILITY: monitor mode */
#define IEEE80211_C_TKIPMIC              0x00020000  /* CAPABILITY: TKIP MIC avail */
#define IEEE80211_C_WAPI                 0x00040000  /* CAPABILITY: ATH WAPI avail */
#define IEEE80211_C_WDS_AUTODETECT       0x00080000  /* CONF: WDS auto Detect/DELBA */
#define IEEE80211_C_WPA1                 0x00800000  /* CAPABILITY: WPA1 avail */
#define IEEE80211_C_WPA2                 0x01000000  /* CAPABILITY: WPA2 avail */
#define IEEE80211_C_WPA                  0x01800000  /* CAPABILITY: WPA1+WPA2 avail*/
#define IEEE80211_C_BURST                0x02000000  /* CAPABILITY: frame bursting */
#define IEEE80211_C_WME                  0x04000000  /* CAPABILITY: WME avail */
#define IEEE80211_C_WDS                  0x08000000  /* CAPABILITY: 4-addr support */
#define IEEE80211_C_WME_TKIPMIC          0x10000000  /* CAPABILITY: TKIP MIC for QoS frame */
#define IEEE80211_C_BGSCAN               0x20000000  /* CAPABILITY: bg scanning */
#define IEEE80211_C_UAPSD                0x40000000  /* CAPABILITY: UAPSD */
#define IEEE80211_C_DOTH                 0x80000000  /* CAPABILITY: enabled 11.h */


/* XXX protection/barker? */

#define IEEE80211_C_CRYPTO         0x0000002f  /* CAPABILITY: crypto alg's */

/* ic_caps_ext */
#define IEEE80211_CEXT_FASTCC      0x00000001  /* CAPABILITY: fast channel change */
#define IEEE80211_CEXT_P2P              0x00000002  /* CAPABILITY: P2P */
#define IEEE80211_CEXT_MULTICHAN        0x00000004  /* CAPABILITY: Multi-Channel Operations */
#define IEEE80211_CEXT_PERF_PWR_OFLD    0x00000008  /* CAPABILITY: the device supports perf and power offload */
#define IEEE80211_CEXT_11AC             0x00000010  /* CAPABILITY: the device supports 11ac */
#define IEEE80211_ACS_CHANNEL_HOPPING   0x00000020  /* CAPABILITY: the device support acs channel hopping */

#if ATH_SUPPORT_DFS && ATH_SUPPORT_STA_DFS
#define IEEE80211_CEXT_STADFS           0x00000040  /* CAPABILITY: the device support STA DFS */
#endif
#define IEEE80211_CEXT_11AX             0x00000080  /* CAPABILITY: the device supports 11ax */

/* ATF Schedular Flags */
#define IEEE80211_ATF_SCHED_STRICT          0x00000001      /* 1 - Strict, 0 - Fair */
#define IEEE80211_ATF_SCHED_OBSS            0x00000002
#define IEEE80211_ATF_GROUP_SCHED_POLICY    0x00000004

/* REPEATER channel_switch(CSH) options flags */

/* random channel selection
 * 1 - only from nonDFS channel list
 * 0 - all(dfs and nonDFS) channel list
 */
#define IEEE80211_CSH_OPT_NONDFS_RANDOM    0x00000001

/* If the CSA channel is DFS
 * 1 - ignore the CSA channel and choose a Random  channel locally
 * 0 - use the CSA channel
 */
#define IEEE80211_CSH_OPT_IGNORE_CSA_DFS   0x00000002

/* When STA brings up the AP VAP
 * 1 - CAC is done
 * 0 - CAC is not done
 */
#define IEEE80211_CSH_OPT_CAC_APUP_BYSTA   0x00000004

/* When STA brings up the AP VAP
 * 1 - CSA is sent
 * 0 - CSA is not sent
 */
#define IEEE80211_CSH_OPT_CSA_APUP_BYSTA   0x00000008

/* When Radar gets detected by REPEATER
 *  RCSA = Reverse CSA: A frame or a set of frames
 *  sent by REPEATER to its parent REPEATER when
 *  either 1) REPEATER detects RADAR
 *  or     2) REPEATER receives a RCSA from its child REPEATER
 *
 * 1 - RCSA is sent
 * 0 - RCSA is not sent
 */
#define IEEE80211_CSH_OPT_RCSA_TO_UPLINK   0x00000010

/* When RCSA is receives by Root/REPEATER
 * 1 - RCSA is processed
 * 0 - RCSA is not processed
 */
#define IEEE80211_CSH_OPT_PROCESS_RCSA     0x00000020

#define IEEE80211_CSH_OPT_APRIORI_NEXT_CHANNEL 0x00000040

/* When bringing up the Repeater AP vaps, skip the CAC, if the RootAP
 * HT20 DFS sub-channels are subset of the RepeaterAP HT20 sub-channels.
 */
#define IEEE80211_CSH_OPT_AVOID_DUAL_CAC    0x00000080

/* Channel change happens through user space (ucfg_set_freq/doth_chanswitch)
 */
#define IEEE80211_CHANCHANGE_STARTED        0x00000001

/* Channel change happens through beacon_update().
 */
#define IEEE80211_CHANCHANGE_BY_BEACONUPDATE     0x00000002

/* Channel change by dfs_task()
 */
#define IEEE80211_CHANCHANGE_MARKRADAR       0x00000004


#define IEEE80211_IS_CSH_NONDFS_RANDOM_ENABLED(_ic)   ((_ic)->ic_chanswitch_flags &   IEEE80211_CSH_OPT_NONDFS_RANDOM)
#define IEEE80211_CSH_NONDFS_RANDOM_ENABLE(_ic)       ((_ic)->ic_chanswitch_flags |=  IEEE80211_CSH_OPT_NONDFS_RANDOM)
#define IEEE80211_CSH_NONDFS_RANDOM_DISABLE(_ic)      ((_ic)->ic_chanswitch_flags &= ~IEEE80211_CSH_OPT_NONDFS_RANDOM)

#define IEEE80211_IS_CSH_IGNORE_CSA_DFS_ENABLED(_ic)  ((_ic)->ic_chanswitch_flags &   IEEE80211_CSH_OPT_IGNORE_CSA_DFS)
#define IEEE80211_CSH_IGNORE_CSA_DFS_ENABLE(_ic)      ((_ic)->ic_chanswitch_flags |=  IEEE80211_CSH_OPT_IGNORE_CSA_DFS)
#define IEEE80211_CSH_IGNORE_CSA_DFS_DISABLE(_ic)     ((_ic)->ic_chanswitch_flags &= ~IEEE80211_CSH_OPT_IGNORE_CSA_DFS)

#define IEEE80211_IS_CSH_CAC_APUP_BYSTA_ENABLED(_ic)  ((_ic)->ic_chanswitch_flags &   IEEE80211_CSH_OPT_CAC_APUP_BYSTA)
#define IEEE80211_CSH_CAC_APUP_BYSTA_ENABLE(_ic)      ((_ic)->ic_chanswitch_flags |=  IEEE80211_CSH_OPT_CAC_APUP_BYSTA)
#define IEEE80211_CSH_CAC_APUP_BYSTA_DISABLE(_ic)     ((_ic)->ic_chanswitch_flags &= ~IEEE80211_CSH_OPT_CAC_APUP_BYSTA)

#define IEEE80211_IS_CSH_CSA_APUP_BYSTA_ENABLED(_ic)  ((_ic)->ic_chanswitch_flags &   IEEE80211_CSH_OPT_CSA_APUP_BYSTA)
#define IEEE80211_CSH_CSA_APUP_BYSTA_ENABLE(_ic)      ((_ic)->ic_chanswitch_flags |=  IEEE80211_CSH_OPT_CSA_APUP_BYSTA)
#define IEEE80211_CSH_CSA_APUP_BYSTA_DISABLE(_ic)     ((_ic)->ic_chanswitch_flags &= ~IEEE80211_CSH_OPT_CSA_APUP_BYSTA)


#define IEEE80211_IS_CSH_RCSA_TO_UPLINK_ENABLED(_ic)  ((_ic)->ic_chanswitch_flags &   IEEE80211_CSH_OPT_RCSA_TO_UPLINK)
#define IEEE80211_CSH_RCSA_TO_UPLINK_ENABLE(_ic)      ((_ic)->ic_chanswitch_flags |=  IEEE80211_CSH_OPT_RCSA_TO_UPLINK)
#define IEEE80211_CSH_RCSA_TO_UPLINK_DISABLE(_ic)     ((_ic)->ic_chanswitch_flags &= ~IEEE80211_CSH_OPT_RCSA_TO_UPLINK)

#define IEEE80211_IS_CSH_PROCESS_RCSA_ENABLED(_ic)    ((_ic)->ic_chanswitch_flags &   IEEE80211_CSH_OPT_PROCESS_RCSA)
#define IEEE80211_CSH_PROCESS_RCSA_ENABLE(_ic)        ((_ic)->ic_chanswitch_flags |=  IEEE80211_CSH_OPT_PROCESS_RCSA)
#define IEEE80211_CSH_PROCESS_RCSA_DISABLE(_ic)       ((_ic)->ic_chanswitch_flags &= ~IEEE80211_CSH_OPT_PROCESS_RCSA)

#define IEEE80211_IS_CSH_OPT_APRIORI_NEXT_CHANNEL_ENABLED(_ic)  ((_ic)->ic_chanswitch_flags &   IEEE80211_CSH_OPT_APRIORI_NEXT_CHANNEL)
#define IEEE80211_CSH_OPT_APRIORI_NEXT_CHANNEL_ENABLE(_ic)      ((_ic)->ic_chanswitch_flags |=  IEEE80211_CSH_OPT_APRIORI_NEXT_CHANNEL)
#define IEEE80211_CSH_OPT_APRIORI_NEXT_CHANNEL_DISABLE(_ic)     ((_ic)->ic_chanswitch_flags &= ~IEEE80211_CSH_OPT_APRIORI_NEXT_CHANNEL)

#define IEEE80211_IS_CSH_OPT_AVOID_DUAL_CAC_ENABLED(_ic)        ((_ic)->ic_chanswitch_flags &   IEEE80211_CSH_OPT_AVOID_DUAL_CAC)
#define IEEE80211_CSH_OPT_AVOID_DUAL_CAC_ENABLE(_ic)            ((_ic)->ic_chanswitch_flags |=  IEEE80211_CSH_OPT_AVOID_DUAL_CAC)
#define IEEE80211_CSH_OPT_AVOID_DUAL_CAC_DISABLE(_ic)           ((_ic)->ic_chanswitch_flags &= ~IEEE80211_CSH_OPT_AVOID_DUAL_CAC)

#define IEEE80211_CHANCHANGE_STARTED_IS_SET(_ic)                ((_ic)->ic_chanchange_serialization_flags &   IEEE80211_CHANCHANGE_STARTED)
#define IEEE80211_CHANCHANGE_STARTED_SET(_ic)                   ((_ic)->ic_chanchange_serialization_flags |=  IEEE80211_CHANCHANGE_STARTED)
#define IEEE80211_CHANCHANGE_STARTED_CLEAR(_ic)                 ((_ic)->ic_chanchange_serialization_flags &= ~IEEE80211_CHANCHANGE_STARTED)

#define IEEE80211_CHANCHANGE_BY_BEACONUPDATE_IS_SET(_ic)        ((_ic)->ic_chanchange_serialization_flags &   IEEE80211_CHANCHANGE_BY_BEACONUPDATE)
#define IEEE80211_CHANCHANGE_BY_BEACONUPDATE_SET(_ic)           ((_ic)->ic_chanchange_serialization_flags |=  IEEE80211_CHANCHANGE_BY_BEACONUPDATE)
#define IEEE80211_CHANCHANGE_BY_BEACONUPDATE_CLEAR(_ic)         ((_ic)->ic_chanchange_serialization_flags &= ~IEEE80211_CHANCHANGE_BY_BEACONUPDATE)

#define IEEE80211_CHANCHANGE_MARKRADAR_IS_SET(_ic)              ((_ic)->ic_chanchange_serialization_flags &   IEEE80211_CHANCHANGE_MARKRADAR)
#define IEEE80211_CHANCHANGE_MARKRADAR_SET(_ic)                 ((_ic)->ic_chanchange_serialization_flags |=  IEEE80211_CHANCHANGE_MARKRADAR)
#define IEEE80211_CHANCHANGE_MARKRADAR_CLEAR(_ic)               ((_ic)->ic_chanchange_serialization_flags &= ~IEEE80211_CHANCHANGE_MARKRADAR)

/* Accessor APIs */
#define IEEE80211_FEXT_MARKDFS_ENABLE(_ic)          ((_ic)->ic_flags_ext |= IEEE80211_FEXT_MARKDFS)

#define IEEE80211_IS_UAPSD_ENABLED(_ic)             ((_ic)->ic_flags_ext & IEEE80211_FEXT_UAPSD)
#define IEEE80211_UAPSD_ENABLE(_ic)                 (ieee80211com_set_flags_ext((_ic), IEEE80211_FEXT_UAPSD))
#define IEEE80211_UAPSD_DISABLE(_ic)                (ieee80211com_clear_flags_ext((_ic), IEEE80211_FEXT_UAPSD))

#define IEEE80211_IS_SLEEPING(_ic)                  ((_ic)->ic_flags_ext & IEEE80211_FEXT_SLEEP)
#define IEEE80211_GOTOSLEEP(_ic)                    (ieee80211com_set_flags_ext((_ic), IEEE80211_FEXT_SLEEP))
#define IEEE80211_WAKEUP(_ic)                       (ieee80211com_clear_flags_ext((_ic), IEEE80211_FEXT_SLEEP))

#define IEEE80211_IS_HIBERNATING(_ic)               ((_ic)->ic_flags_ext & IEEE80211_FEXT_HIBERNATION)
#define IEEE80211_GOTOHIBERNATION(_ic)              (ieee80211com_set_flags_ext((_ic), IEEE80211_FEXT_HIBERNATION))
#define IEEE80211_WAKEUPFROMHIBERNATION(_ic)        (ieee80211com_clear_flags_ext((_ic), IEEE80211_FEXT_HIBERNATION))

#define IEEE80211_IS_PROTECTION_ENABLED(_ic)        ((_ic)->ic_flags & IEEE80211_F_USEPROT)
#define IEEE80211_ENABLE_PROTECTION(_ic)            (ieee80211com_set_flags((_ic), IEEE80211_F_USEPROT))
#define IEEE80211_DISABLE_PROTECTION(_ic)           (ieee80211com_clear_flags((_ic), IEEE80211_F_USEPROT))

#define IEEE80211_IS_SHPREAMBLE_ENABLED(_ic)        ((_ic)->ic_flags & IEEE80211_F_SHPREAMBLE)
#define IEEE80211_ENABLE_SHPREAMBLE(_ic)            (ieee80211com_set_flags((_ic), IEEE80211_F_SHPREAMBLE))
#define IEEE80211_DISABLE_SHPREAMBLE(_ic)           (ieee80211com_clear_flags((_ic), IEEE80211_F_SHPREAMBLE))

#define IEEE80211_IS_CAP_SHPREAMBLE_ENABLED(_ic)    ((_ic)->ic_caps & IEEE80211_C_SHPREAMBLE)
#define IEEE80211_ENABLE_CAP_SHPREAMBLE(_ic)        (ieee80211com_set_cap((_ic), IEEE80211_C_SHPREAMBLE))
#define IEEE80211_DISABLE_CAP_SHPREAMBLE(_ic)       (ieee80211com_clear_cap((_ic), IEEE80211_C_SHPREAMBLE))


#define IEEE80211_IS_BARKER_ENABLED(_ic)            ((_ic)->ic_flags & IEEE80211_F_USEBARKER)
#define IEEE80211_ENABLE_BARKER(_ic)                (ieee80211com_set_flags((_ic), IEEE80211_F_USEBARKER))
#define IEEE80211_DISABLE_BARKER(_ic)               (ieee80211com_clear_flags((_ic), IEEE80211_F_USEBARKER))

#define IEEE80211_IS_SHSLOT_ENABLED(_ic)            ((_ic)->ic_flags & IEEE80211_F_SHSLOT)
#define IEEE80211_ENABLE_SHSLOT(_ic)                (ieee80211com_set_flags((_ic), IEEE80211_F_SHSLOT))
#define IEEE80211_DISABLE_SHSLOT(_ic)               (ieee80211com_clear_flags((_ic), IEEE80211_F_SHSLOT))

#define IEEE80211_IS_DATAPAD_ENABLED(_ic)           ((_ic)->ic_flags & IEEE80211_F_DATAPAD)
#define IEEE80211_ENABLE_DATAPAD(_ic)               (ieee80211com_set_flags((_ic), IEEE80211_F_DATAPAD))
#define IEEE80211_DISABLE_DATAPAD(_ic)              (ieee80211com_clear_flags((_ic), IEEE80211_F_DATAPAD))

#define IEEE80211_COEXT_DISABLE(_ic)                (ieee80211com_set_flags((_ic), IEEE80211_F_COEXT_DISABLE))

#define IEEE80211_IS_COUNTRYIE_ENABLED(_ic)         ((_ic)->ic_flags_ext & IEEE80211_FEXT_COUNTRYIE)
#define IEEE80211_ENABLE_COUNTRYIE(_ic)             (ieee80211com_set_flags_ext((_ic), IEEE80211_FEXT_COUNTRYIE))
#define IEEE80211_DISABLE_COUNTRYIE(_ic)            (ieee80211com_clear_flags_ext((_ic), IEEE80211_FEXT_COUNTRYIE))

#define IEEE80211_IS_11D_ENABLED(_ic)               ((_ic)->ic_flags_ext & IEEE80211_FEXT_DOT11D)
#define IEEE80211_ENABLE_11D(_ic)                   (ieee80211com_set_flags_ext((_ic), IEEE80211_FEXT_DOT11D))
#define IEEE80211_DISABLE_11D(_ic)                  (ieee80211com_clear_flags_ext((_ic), IEEE80211_FEXT_DOT11D))

#define IEEE80211_HAS_DESIRED_COUNTRY(_ic)          ((_ic)->ic_flags_ext & IEEE80211_FEXT_DESCOUNTRY)
#define IEEE80211_SET_DESIRED_COUNTRY(_ic)          (ieee80211com_set_flags_ext((_ic), IEEE80211_FEXT_DESCOUNTRY))
#define IEEE80211_CLEAR_DESIRED_COUNTRY(_ic)        (ieee80211com_clear_flags_ext((_ic), IEEE80211_FEXT_DESCOUNTRY))

#define IEEE80211_IS_11D_BEACON_IGNORED(_ic)        ((_ic)->ic_flags_ext & IEEE80211_FEXT_IGNORE_11D_BEACON)
#define IEEE80211_ENABLE_IGNORE_11D_BEACON(_ic)     (ieee80211com_set_flags_ext((_ic), IEEE80211_FEXT_IGNORE_11D_BEACON))
#define IEEE80211_DISABLE_IGNORE_11D_BEACON(_ic)    (ieee80211com_clear_flags_ext((_ic), IEEE80211_FEXT_IGNORE_11D_BEACON))

#define IEEE80211_IS_RADAR_ENABLED(_ic)             ((_ic)->ic_flags_ext & IEEE80211_FEXT_RADAR)
#define IEEE80211_ENABLE_RADAR(_ic)                 (ieee80211com_set_flags_ext((_ic), IEEE80211_FEXT_RADAR))
#define IEEE80211_DISABLE_RADAR(_ic)                (ieee80211com_clear_flags_ext((_ic), IEEE80211_FEXT_RADAR))

#define IEEE80211_IS_CHAN_SWITCH_STARTED(_ic)       IEEE80211_IS_RADAR_ENABLED(_ic)
#define IEEE80211_CHAN_SWITCH_START(_ic)            IEEE80211_ENABLE_RADAR(_ic)
#define IEEE80211_CHAN_SWITCH_END(_ic)              IEEE80211_DISABLE_RADAR(_ic)

#define IEEE80211_IS_HTVIE_ENABLED(_ic)             ((_ic)->ic_flags_ext & IEEE80211_FEXT_HTVIE)
#define IEEE80211_ENABLE_HTVIE(_ic)                 (ieee80211com_set_flags_ext((_ic), IEEE80211_FEXT_HTVIE))
#define IEEE80211_DISABLE_HTVIE(_ic)                (ieee80211com_clear_flags_ext((_ic), IEEE80211_FEXT_HTVIE))

#define IEEE80211_IS_AMPDU_ENABLED(_ic)             ((_ic)->ic_flags_ext & IEEE80211_FEXT_AMPDU)
#define IEEE80211_ENABLE_AMPDU(_ic)                 (ieee80211com_set_flags_ext((_ic), IEEE80211_FEXT_AMPDU))
#define IEEE80211_DISABLE_AMPDU(_ic)                (ieee80211com_clear_flags_ext((_ic), IEEE80211_FEXT_AMPDU))

#define IEEE80211_IS_HTPROT_ENABLED(_ic)             ((_ic)->ic_flags_ext & IEEE80211_FEXT_HTPROT)
#define IEEE80211_ENABLE_HTPROT(_ic)                 (ieee80211com_set_flags_ext((_ic), IEEE80211_FEXT_HTPROT))
#define IEEE80211_DISABLE_HTPROT(_ic)                (ieee80211com_clear_flags_ext((_ic), IEEE80211_FEXT_HTPROT))

#define IEEE80211_GET_BCAST_ADDR(_c)                ((_c)->ic_broadcast)

#define IEEE80211_STATE_LOCK_INIT(_ic)             spin_lock_init(&(_ic)->ic_state_lock)
#define IEEE80211_STATE_LOCK_DESTROY(_ic)
#define IEEE80211_STATE_LOCK(_ic)                  spin_lock_dpc(&(_ic)->ic_state_lock)
#define IEEE80211_STATE_UNLOCK(_ic)                spin_unlock_dpc(&(_ic)->ic_state_lock)


#define IEEE80211_COMM_LOCK(_ic)                   spin_lock(&(_ic)->ic_lock)
#define IEEE80211_COMM_UNLOCK(_ic)                 spin_unlock(&(_ic)->ic_lock)
#define IEEE80211_COMM_LOCK_BH(_ic)                spin_lock_bh(&(_ic)->ic_lock)
#define IEEE80211_COMM_UNLOCK_BH(_ic)              spin_unlock_bh(&(_ic)->ic_lock)
#define IEEE80211_COMM_LOCK_IRQ(_ic)               spin_lock_irqsave(&(_ic)->ic_lock,(_ic)->ic_lock_flags)
#define IEEE80211_COMM_UNLOCK_IRQ(_ic)             spin_unlock_irqrestore(&(_ic)->ic_lock,(_ic)->ic_lock_flags)

#define IEEE80211_STATE_CHECK_LOCK(_ic)            spin_lock(&(_ic)->ic_state_check_lock)
#define IEEE80211_STATE_CHECK_UNLOCK(_ic)          spin_unlock(&(_ic)->ic_state_check_lock)

#if ATH_BEACON_DEFERRED_PROC
#define STA_VAP_DOWNUP_LOCK(_ic)                   spin_lock_bh(&(_ic)->ic_main_sta_lock)
#define STA_VAP_DOWNUP_UNLOCK(_ic)                 spin_unlock_bh(&(_ic)->ic_main_sta_lock)
#else
#define STA_VAP_DOWNUP_LOCK(_ic)                   spin_lock_irqsave(&(_ic)->ic_main_sta_lock,(_ic)->ic_main_sta_lock_flags)
#define STA_VAP_DOWNUP_UNLOCK(_ic)                 spin_unlock_irqrestore(&(_ic)->ic_main_sta_lock,(_ic)->ic_main_sta_lock_flags)
#endif


#if ATH_BEACON_DEFERRED_PROC
#define IEEE80211_CHAN_CHANGE_LOCK(_ic)             spin_lock_bh(&(_ic)->ic_chan_change_lock)
#define IEEE80211_CHAN_CHANGE_UNLOCK(_ic)           spin_unlock_bh(&(_ic)->ic_chan_change_lock)
#else
#define IEEE80211_CHAN_CHANGE_LOCK(_ic)             spin_lock_irqsave(&(_ic)->ic_chan_change_lock,(_ic)->ic_chan_change_lock_flags)
#define IEEE80211_CHAN_CHANGE_UNLOCK(_ic)           spin_unlock_irqrestore(&(_ic)->ic_chan_change_lock,(_ic)->ic_chan_change_lock_flags)
#endif

#define IEEE80211_RADAR_FOUND_LOCK(_ic)             spin_lock_dpc(&(_ic)->ic_radar_found_lock)
#define IEEE80211_RADAR_FOUND_UNLOCK(_ic)           spin_unlock_dpc(&(_ic)->ic_radar_found_lock)

#define IEEE80211_RADAR_MODE_SWITCH_LOCK(_ic)       spin_lock_dpc(&(_ic)->ic_radar_mode_switch_lock)
#define IEEE80211_RADAR_MODE_SWITCH_UNLOCK(_ic)     spin_unlock_dpc(&(_ic)->ic_radar_mode_switch_lock)

#if ATH_BEACON_DEFERRED_PROC
#define ATH_BEACON_ALLOC_LOCK(_ic)                 spin_lock_bh(&(_ic)->ic_beacon_alloc_lock)
#define ATH_BEACON_ALLOC_UNLOCK(_ic)               spin_unlock_bh(&(_ic)->ic_beacon_alloc_lock)
#else
#define ATH_BEACON_ALLOC_LOCK(_ic)                 spin_lock_irqsave(&(_ic)->ic_beacon_alloc_lock,(_ic)->ic_beacon_alloc_lock_flags)
#define ATH_BEACON_ALLOC_UNLOCK(_ic)               spin_unlock_irqrestore(&(_ic)->ic_beacon_alloc_lock,(_ic)->ic_beacon_alloc_lock_flags)
#endif

#if DBDC_REPEATER_SUPPORT
#define GLOBAL_IC_LOCK_BH(_rl)                        spin_lock_bh(&(_rl)->radio_list_lock)
#define GLOBAL_IC_UNLOCK_BH(_rl)                      spin_unlock_bh(&(_rl)->radio_list_lock)
#else
#define GLOBAL_IC_LOCK_BH(_rl)
#define GLOBAL_IC_UNLOCK_BH(_rl)
#endif

#define	IEEE80211_STAT_LOCK(_vaplock)              spin_lock((_vaplock))
#define	IEEE80211_STAT_UNLOCK(_vaplock)            spin_unlock((_vaplock))

typedef spinlock_t ieee80211_p2p_gosche_lock_t;
#define IEEE80211_P2P_GOSCHE_LOCK_INIT(_gos)       spin_lock_init(&((_gos)->lock));
#define IEEE80211_P2P_GOSCHE_LOCK_DESTROY(_gos)    spin_lock_destroy(&((_gos)->lock))
#define IEEE80211_P2P_GOSCHE_LOCK(_gos)            spin_lock(&((_gos)->lock))
#define IEEE80211_P2P_GOSCHE_UNLOCK(_gos)          spin_unlock(&((_gos)->lock))

typedef spinlock_t ieee80211_tsf_timer_lock_t;
#define IEEE80211_TSF_TIMER_LOCK_INIT(_tsf)        spin_lock_init(&((_tsf)->lock));
#define IEEE80211_TSF_TIMER_LOCK_DESTROY(_tsf)     spin_lock_destroy(&((_tsf)->lock))
#define IEEE80211_TSF_TIMER_LOCK(_tsf)             spin_lock(&((_tsf)->lock))
#define IEEE80211_TSF_TIMER_UNLOCK(_tsf)           spin_unlock(&((_tsf)->lock))

typedef spinlock_t ieee80211_resmgr_oc_sched_lock_t;
#define IEEE80211_RESMGR_OCSCHE_LOCK_INIT(_ocslock)     spin_lock_init(&((_ocslock)->scheduler_lock));
#define IEEE80211_RESMGR_OCSCHE_LOCK_DESTROY(_ocslock)  spin_lock_destroy(&((_ocslock)->scheduler_lock))
#define IEEE80211_RESMGR_OCSCHE_LOCK(_ocslock)          spin_lock(&((_ocslock)->scheduler_lock))
#define IEEE80211_RESMGR_OCSCHE_UNLOCK(_ocslock)        spin_unlock(&((_ocslock)->scheduler_lock))

#define IEEE80211_VAP_LOCK(_vap)                   spin_lock_irqsave(&_vap->iv_lock, _vap->iv_lock_flags)
#define IEEE80211_VAP_UNLOCK(_vap)                 spin_unlock_irqrestore(&_vap->iv_lock , _vap->iv_lock_flags)

#ifndef  ATH_BEACON_DEFERRED_PROC
#define OS_BEACON_DECLARE_AND_RESET_VAR(_flags)                unsigned long _flags = 0
#define OS_BEACON_READ_LOCK(_p_lock , _p_state, _flags)        OS_RWLOCK_READ_LOCK_IRQSAVE(_p_lock , _p_state, _flags)
#define OS_BEACON_READ_UNLOCK(_p_lock , _p_state, _flags)      OS_RWLOCK_READ_UNLOCK_IRQRESTORE(_p_lock , _p_state, _flags)
#define OS_BEACON_WRITE_LOCK(_p_lock , _p_state, _flags)       OS_RWLOCK_WRITE_LOCK_IRQSAVE(_p_lock , _p_state, _flags)
#define OS_BEACON_WRITE_UNLOCK(_p_lock , _p_state, _flags)     OS_RWLOCK_WRITE_UNLOCK_IRQRESTORE(_p_lock , _p_state, _flags)
#else
#define OS_BEACON_DECLARE_AND_RESET_VAR(_flags)                /* Do nothing */
#define OS_BEACON_READ_LOCK(_p_lock , _p_state, _flags)        OS_RWLOCK_READ_LOCK(_p_lock , _p_state)
#define OS_BEACON_READ_UNLOCK(_p_lock , _p_state, _flags)      OS_RWLOCK_READ_UNLOCK(_p_lock , _p_state)
#define OS_BEACON_WRITE_LOCK(_p_lock , _p_state, _flags)       OS_RWLOCK_WRITE_LOCK(_p_lock , _p_state)
#define OS_BEACON_WRITE_UNLOCK(_p_lock , _p_state, _flags)     OS_RWLOCK_WRITE_UNLOCK(_p_lock , _p_state)
#endif

#define IEEE80211_NTX_PFL_IE_POOL_LOCK(_vap) qdf_spin_lock_bh(&((_vap)->iv_mbss.non_tx_pfl_ie_pool_lock))
#define IEEE80211_NTX_PFL_IE_POOL_UNLOCK(_vap) qdf_spin_unlock_bh(&((_vap)->iv_mbss.non_tx_pfl_ie_pool_lock))

#define    KEYIX_INVALID    ((u_int8_t) -1)

/*
 * Some times hardware passes the frames without decryption. S/W can
 * choose to decrypt them or to drop them. When enabled, all the frames
 * with KEYMISS set, would be decrypted in s/w and if not they will be
 * ignored
 */
#define IEEE80211_VAP_CCMPSW_ENCDEC_ENABLE(_v) \
            ((_v)->iv_ccmpsw_seldec = 1)

#define IEEE80211_VAP_CCMPSW_ENCDEC_DISABLE(_v)\
            ((_v)->iv_ccmpsw_seldec = 0)


#define IC_FLAG_FUNCS(xyz) \
     static INLINE int ieee80211_ic_##xyz##_is_set (struct ieee80211com *_ic) { \
        return (_ic->ic_##xyz == 1); \
     } \
     static INLINE int ieee80211_ic_##xyz##_is_clear (struct ieee80211com *_ic) { \
        return (_ic->ic_##xyz == 0); \
     } \
     static INLINE void ieee80211_ic_##xyz##_set (struct ieee80211com *_ic) { \
        _ic->ic_##xyz = 1; \
     } \
     static INLINE void  ieee80211_ic_##xyz##_clear (struct ieee80211com *_ic) { \
        _ic->ic_##xyz = 0; \
     }

IC_FLAG_FUNCS(wep_tkip_htrate)
IC_FLAG_FUNCS(non_ht_ap)
IC_FLAG_FUNCS(doth)
IC_FLAG_FUNCS(ht20Adhoc)
IC_FLAG_FUNCS(ht40Adhoc)
IC_FLAG_FUNCS(htAdhocAggr)
IC_FLAG_FUNCS(disallowAutoCCchange)
IC_FLAG_FUNCS(ignoreDynamicHalt)
IC_FLAG_FUNCS(p2pDevEnable)
IC_FLAG_FUNCS(override_proberesp_ie)
IC_FLAG_FUNCS(2g_csa)
IC_FLAG_FUNCS(wnm)
IC_FLAG_FUNCS(offchanscan)
IC_FLAG_FUNCS(enh_ind_rpt)
IC_FLAG_FUNCS(rpt_max_phy)
IC_FLAG_FUNCS(mbss_automode)
IC_FLAG_FUNCS(externalacs_enable)

#define IEEE80211_VAP_IS_DROP_UNENC(_v)             ((_v)->iv_flags & IEEE80211_F_DROPUNENC)
#define IEEE80211_VAP_DROP_UNENC_ENABLE(_v)         (ieee80211vap_set_flag((_v), IEEE80211_F_DROPUNENC))
#define IEEE80211_VAP_DROP_UNENC_DISABLE(_v)        (ieee80211vap_clear_flag((_v), IEEE80211_F_DROPUNENC))

#define IEEE80211_VAP_COUNTERM_ENABLE(_v)           (ieee80211vap_set_flag((_v), IEEE80211_F_COUNTERM))
#define IEEE80211_VAP_COUNTERM_DISABLE(_v)          (ieee80211vap_clear_flag((_v), IEEE80211_F_COUNTERM))
#define IEEE80211_VAP_IS_COUNTERM_ENABLED(_v)       ((_v)->iv_flags & IEEE80211_F_COUNTERM)

#define IEEE80211_VAP_WPA_ENABLE(_v)                (ieee80211vap_set_flag((_v), IEEE80211_F_WPA))
#define IEEE80211_VAP_WPA_DISABLE(_v)               (ieee80211vap_clear_flag((_v), IEEE80211_F_WPA))
#define IEEE80211_VAP_IS_WPA_ENABLED(_v)            ((_v)->iv_flags & IEEE80211_F_WPA)

#define IEEE80211_VAP_PUREG_ENABLE(_v)              (ieee80211vap_set_flag((_v), IEEE80211_F_PUREG))
#define IEEE80211_VAP_PUREG_DISABLE(_v)             (ieee80211vap_clear_flag((_v), IEEE80211_F_PUREG))
#define IEEE80211_VAP_IS_PUREG_ENABLED(_v)          ((_v)->iv_flags & IEEE80211_F_PUREG)

#define IEEE80211_VAP_PRIVACY_ENABLE(_v)            (ieee80211vap_set_flag((_v), IEEE80211_F_PRIVACY))
#define IEEE80211_VAP_PRIVACY_DISABLE(_v)           (ieee80211vap_clear_flag((_v), IEEE80211_F_PRIVACY))
#define IEEE80211_VAP_IS_PRIVACY_ENABLED(_v)        ((_v)->iv_flags & IEEE80211_F_PRIVACY)

#define IEEE80211_VAP_HIDESSID_ENABLE(_v)           (ieee80211vap_set_flag((_v), IEEE80211_F_HIDESSID))
#define IEEE80211_VAP_HIDESSID_DISABLE(_v)          (ieee80211vap_clear_flag((_v), IEEE80211_F_HIDESSID))
#define IEEE80211_VAP_IS_HIDESSID_ENABLED(_v)       ((_v)->iv_flags & IEEE80211_F_HIDESSID)

#define IEEE80211_VAP_NOBRIDGE_ENABLE(_v)           (ieee80211vap_set_flag((_v), IEEE80211_F_NOBRIDGE))
#define IEEE80211_VAP_NOBRIDGE_DISABLE(_v)          (ieee80211vap_clear_flag((_v), IEEE80211_F_NOBRIDGE))
#define IEEE80211_VAP_IS_NOBRIDGE_ENABLED(_v)       ((_v)->iv_flags & IEEE80211_F_NOBRIDGE)

#define IEEE80211_VAP_IS_TIMUPDATE_ENABLED(_v)      ((_v)->iv_flags_ext & IEEE80211_F_TIMUPDATE)
#define IEEE80211_VAP_TIMUPDATE_ENABLE(_v)          (ieee80211vap_set_flag_ext((_v), IEEE80211_F_TIMUPDATE))
#define IEEE80211_VAP_TIMUPDATE_DISABLE(_v)         (ieee80211vap_clear_flag_ext((_v), IEEE80211_F_TIMUPDATE))

#define IEEE80211_VAP_IS_UAPSD_ENABLED(_v)          ((_v)->iv_flags_ext & IEEE80211_FEXT_UAPSD)
#define IEEE80211_VAP_UAPSD_ENABLE(_v)              (ieee80211vap_set_flag_ext((_v), IEEE80211_FEXT_UAPSD))
#define IEEE80211_VAP_UAPSD_DISABLE(_v)             (ieee80211vap_clear_flag_ext((_v), IEEE80211_FEXT_UAPSD))

#define IEEE80211_VAP_IS_SLEEPING(_v)               ((_v)->iv_flags_ext & IEEE80211_FEXT_SLEEP)
#define IEEE80211_VAP_GOTOSLEEP(_v)                 (ieee80211vap_set_flag_ext((_v), IEEE80211_FEXT_SLEEP))
#define IEEE80211_VAP_WAKEUP(_v)                    (ieee80211vap_clear_flag_ext((_v), IEEE80211_FEXT_SLEEP))

#define IEEE80211_VAP_IS_EOSPDROP_ENABLED(_v)       ((_v)->iv_flags_ext & IEEE80211_FEXT_EOSPDROP)
#define IEEE80211_VAP_EOSPDROP_ENABLE(_v)           (ieee80211vap_set_flag_ext((_v), IEEE80211_FEXT_EOSPDROP))
#define IEEE80211_VAP_EOSPDROP_DISABLE(_v)          (ieee80211vap_clear_flag_ext((_v), IEEE80211_FEXT_EOSPDROP))

#define IEEE80211_VAP_IS_HTRATES_ENABLED(_v)        ((_v)->iv_flags_ext & IEEE80211_FEXT_HTRATES)
#define IEEE80211_VAP_HTRATES_ENABLE(_v)            (ieee80211vap_set_flag_ext((_v), IEEE80211_FEXT_HTRATES))
#define IEEE80211_VAP_HTRATES_DISABLE(_v)           (ieee80211vap_clear_flag_ext((_v), IEEE80211_FEXT_HTRATES))

#define IEEE80211_VAP_SAFEMODE_ENABLE(_v)           (ieee80211vap_set_flag_ext222((_v), IEEE80211_FEXT_SAFEMODE))
#define IEEE80211_VAP_SAFEMODE_DISABLE(_v)          (ieee80211vap_clear_flag_ext222((_v), IEEE80211_FEXT_SAFEMODE))
#define IEEE80211_VAP_IS_SAFEMODE_ENABLED(_v)       ((_v)->iv_flags_ext & IEEE80211_FEXT_SAFEMODE)

#define IEEE80211_VAP_DELIVER_80211_ENABLE(_v)      (ieee80211vap_set_flag_ext((_v), IEEE80211_FEXT_DELIVER_80211))
#define IEEE80211_VAP_DELIVER_80211_DISABLE(_v)     (ieee80211vap_clear_flag_ext((_v), IEEE80211_FEXT_DELIVER_80211))
#define IEEE80211_VAP_IS_DELIVER_80211_ENABLED(_v)  ((_v)->iv_flags_ext & IEEE80211_FEXT_DELIVER_80211)

#define IEEE80211_VAP_SEND_80211_ENABLE(_v)         (ieee80211vap_set_flag_ext((_v), IEEE80211_FEXT_SEND_80211))
#define IEEE80211_VAP_SEND_80211_DISABLE(_v)        (ieee80211vap_clear_flag_ext((_v), IEEE80211_FEXT_SEND_80211))
#define IEEE80211_VAP_IS_SEND_80211_ENABLED(_v)     ((_v)->iv_flags_ext & IEEE80211_FEXT_SEND_80211)

#define IEEE80211_VAP_WDS_ENABLE(_v)                (ieee80211vap_set_flag_ext((_v), IEEE80211_FEXT_WDS))
#define IEEE80211_VAP_WDS_DISABLE(_v)               (ieee80211vap_clear_flag_ext((_v), IEEE80211_FEXT_WDS))
#define IEEE80211_VAP_IS_WDS_ENABLED(_v)            ((_v)->iv_flags_ext & IEEE80211_FEXT_WDS)

#define IEEE80211_VAP_STATIC_WDS_ENABLE(_v)         (ieee80211vap_set_flag_ext((_v), IEEE80211_FEXT_WDS_STATIC))
#define IEEE80211_VAP_STATIC_WDS_DISABLE(_v)        (ieee80211vap_clear_flag_ext((_v), IEEE80211_FEXT_WDS_STATIC))
#define IEEE80211_VAP_IS_STATIC_WDS_ENABLED(_v)     ((_v)->iv_flags_ext & IEEE80211_FEXT_WDS_STATIC)

#define IEEE80211_VAP_WDS_AUTODETECT_ENABLE(_v)     (ieee80211vap_set_flag_ext((_v), IEEE80211_FEXT_WDS_AUTODETECT))
#define IEEE80211_VAP_WDS_AUTODETECT_DISABLE(_v)    (ieee80211vap_clear_flag_ext((_v), IEEE80211_FEXT_WDS_AUTODETECT))
#define IEEE80211_VAP_IS_WDS_AUTODETECT_ENABLED(_v) ((_v)->iv_flags_ext & IEEE80211_FEXT_WDS_AUTODETECT)

#define IEEE80211_VAP_PURE11N_ENABLE(_v)            (ieee80211vap_set_flag_ext((_v), IEEE80211_FEXT_PURE11N))
#define IEEE80211_VAP_PURE11N_DISABLE(_v)           (ieee80211vap_clear_flag_ext((_v), IEEE80211_FEXT_PURE11N))
#define IEEE80211_VAP_IS_PURE11N_ENABLED(_v)        ((_v)->iv_flags_ext & IEEE80211_FEXT_PURE11N)

#define IEEE80211_VAP_PURE11AC_ENABLE(_v)           (ieee80211vap_set_flag_ext2((_v), IEEE80211_FEXT2_PURE11AC))
#define IEEE80211_VAP_PURE11AC_DISABLE(_v)          (ieee80211vap_clear_flag_ext2((_v), IEEE80211_FEXT2_PURE11AC))
#define IEEE80211_VAP_IS_PURE11AC_ENABLED(_v)       ((_v)->iv_flags_ext2 & IEEE80211_FEXT2_PURE11AC)

#define IEEE80211_VAP_STRICT_BW_ENABLE(_v)           (ieee80211vap_set_flag_ext2((_v), IEEE80211_FEXT2_STRICT_BW))
#define IEEE80211_VAP_STRICT_BW_DISABLE(_v)          (ieee80211vap_clear_flag_ext2((_v), IEEE80211_FEXT2_STRICT_BW))
#define IEEE80211_VAP_IS_STRICT_BW_ENABLED(_v)       ((_v)->iv_flags_ext2 & IEEE80211_FEXT2_STRICT_BW)

#if ATH_NON_BEACON_AP
#define IEEE80211_VAP_NON_BEACON_ENABLE(_v)           (ieee80211vap_set_flag_ext22((_v), IEEE80211_FEXT2_NON_BEACON))
#define IEEE80211_VAP_NON_BEACON_DISABLE(_v)          (ieee80211vap_clear_flag_ext22((_v), IEEE80211_FEXT2_NON_BEACON))
#define IEEE80211_VAP_IS_NON_BEACON_ENABLED(_v)       ((_v)->iv_flags_ext2 & IEEE80211_FEXT2_NON_BEACON)
#endif
#define IEEE80211_VAP_BACKHAUL_ENABLE(_v)           (ieee80211vap_set_flag_ext2((_v), IEEE80211_FEXT2_BACKHAUL))
#define IEEE80211_VAP_BACKHAUL_DISABLE(_v)          (ieee80211vap_clear_flag_ext2((_v), IEEE80211_FEXT2_BACKHAUL))
#define IEEE80211_VAP_IS_BACKHAUL_ENABLED(_v)       ((_v)->iv_flags_ext2 & IEEE80211_FEXT2_BACKHAUL)

#define IEEE80211_VAP_NOCABQ_ENABLE(_v)           (ieee80211vap_set_flag_ext2((_v), IEEE80211_FEXT2_NOCABQ))
#define IEEE80211_VAP_NOCABQ_DISABLE(_v)          (ieee80211vap_clear_flag_ext2((_v), IEEE80211_FEXT2_NOCABQ))
#define IEEE80211_VAP_IS_NOCABQ_ENABLED(_v)       ((_v)->iv_flags_ext2 & IEEE80211_FEXT2_NOCABQ)

#define IEEE80211_VAP_IS_MBSS_ENABLED(_v) \
    ((_v)->iv_flags_ext2 & IEEE80211_FEXT2_MBSS_VAP)

#define IEEE80211_VAP_MBSS_NON_TRANS_ENABLE(_v) do { \
    ieee80211vap_set_flag_ext2((_v), \
            (IEEE80211_FEXT2_NON_TRANSMIT_VAP | IEEE80211_FEXT2_MBSS_VAP)); \
  } while(0)

#define IEEE80211_VAP_MBSS_NON_TRANS_DISABLE(_v) do { \
    ieee80211vap_clear_flag_ext2((_v), IEEE80211_FEXT2_NON_TRANSMIT_VAP); \
    ieee80211vap_clear_flag_ext2((_v), IEEE80211_FEXT2_MBSS_VAP); \
 } while(0)

#define IEEE80211_VAP_IS_MBSS_NON_TRANSMIT_ENABLED(_v)  \
  (((_v)->iv_flags_ext2 & IEEE80211_FEXT2_MBSS_MASK) ==		\
    (IEEE80211_FEXT2_NON_TRANSMIT_VAP | IEEE80211_FEXT2_MBSS_VAP))

INLINE static int
isorbi_ie(wlan_if_t vap, u_int8_t *frm)
{
    /*add code to identify IE*/
    if (IEEE80211_VAP_IS_BACKHAUL_ENABLED(vap))
        return 1;
    else
        return 0;
}

#define IEEE80211_VAP_PUREB_ENABLE(_v)              (ieee80211vap_set_flag_ext((_v), IEEE80211_FEXT_PUREB))
#define IEEE80211_VAP_PUREB_DISABLE(_v)             (ieee80211vap_clear_flag_ext((_v), IEEE80211_FEXT_PUREB))
#define IEEE80211_VAP_IS_PUREB_ENABLED(_v)          ((_v)->iv_flags_ext & IEEE80211_FEXT_PUREB)

#define IEEE80211_VAP_APPIE_UPDATE_ENABLE(_v)       (ieee80211vap_set_flag_ext((_v), IEEE80211_FEXT_APPIE_UPDATE))
#define IEEE80211_VAP_APPIE_UPDATE_DISABLE(_v)      (ieee80211vap_clear_flag_ext((_v), IEEE80211_FEXT_APPIE_UPDATE))
#define IEEE80211_VAP_IS_APPIE_UPDATE_ENABLED(_v)   ((_v)->iv_flags_ext & IEEE80211_FEXT_APPIE_UPDATE)

#define IEEE80211_VAP_AUTOASSOC_ENABLE(_v)       ((_v)->auto_assoc = 1)
#define IEEE80211_VAP_AUTOASSOC_DISABLE(_v)      ((_v)->auto_assoc = 0)
#define IEEE80211_VAP_IS_AUTOASSOC_ENABLED(_v)   ((_v)->auto_assoc == 1) ? 1 : 0

#define VAP_FLAG_FUNCS(xyz) \
     static INLINE int ieee80211_vap_##xyz##_is_set (struct ieee80211vap *_vap) { \
        return (_vap->iv_##xyz == 1); \
     } \
     static INLINE int ieee80211_vap_##xyz##_is_clear (struct ieee80211vap *_vap) { \
        return (_vap->iv_##xyz == 0); \
     } \
     static INLINE void ieee80211_vap_##xyz##_set (struct ieee80211vap *_vap) { \
        _vap->iv_##xyz =1; \
     } \
     static INLINE void  ieee80211_vap_##xyz##_clear (struct ieee80211vap *_vap) { \
        _vap->iv_##xyz = 0; \
     }

VAP_FLAG_FUNCS(deleted)
VAP_FLAG_FUNCS(smps)
VAP_FLAG_FUNCS(sw_bmiss)
VAP_FLAG_FUNCS(copy_beacon)
VAP_FLAG_FUNCS(wapi)
VAP_FLAG_FUNCS(cansleep)
VAP_FLAG_FUNCS(sta_fwd)
VAP_FLAG_FUNCS(scanning)
VAP_FLAG_FUNCS(standby)
VAP_FLAG_FUNCS(dynamic_mimo_ps)
VAP_FLAG_FUNCS(wme)
VAP_FLAG_FUNCS(doth)
VAP_FLAG_FUNCS(country_ie)
VAP_FLAG_FUNCS(dfswait)
VAP_FLAG_FUNCS(erpupdate)
VAP_FLAG_FUNCS(needs_scheduler)
VAP_FLAG_FUNCS(forced_sleep)
VAP_FLAG_FUNCS(qbssload)
VAP_FLAG_FUNCS(qbssload_update)
VAP_FLAG_FUNCS(rrm)
VAP_FLAG_FUNCS(wnm)
VAP_FLAG_FUNCS(ap_reject_dfs_chan)
VAP_FLAG_FUNCS(smartnet_enable)
VAP_FLAG_FUNCS(trigger_mlme_resp)
VAP_FLAG_FUNCS(mfp_test)
VAP_FLAG_FUNCS(proxyarp)
VAP_FLAG_FUNCS(dgaf_disable)
#if ATH_SUPPORT_HYFI_ENHANCEMENTS
VAP_FLAG_FUNCS(nopbn)
#endif
VAP_FLAG_FUNCS(ext_acs_inprogress)
VAP_FLAG_FUNCS(send_additional_ies)

VAP_FLAG_FUNCS(256qam)
#if ATH_SUPPORT_WRAP
#if WLAN_QWRAP_LEGACY
VAP_FLAG_FUNCS(psta)
VAP_FLAG_FUNCS(wrap)
VAP_FLAG_FUNCS(mpsta)
#endif
#endif
VAP_FLAG_FUNCS(11ng_vht_interop)
VAP_FLAG_FUNCS(mbo)
VAP_FLAG_FUNCS(oce)
VAP_FLAG_FUNCS(reset_ap_vaps)
VAP_FLAG_FUNCS(stop_bss)
VAP_FLAG_FUNCS(peer_create_failed)
VAP_FLAG_FUNCS(ext_bssload)
VAP_FLAG_FUNCS(ext_bssload_update)
VAP_FLAG_FUNCS(registered)
VAP_FLAG_FUNCS(sm_gap_ps)
VAP_FLAG_FUNCS(static_mimo_ps)

/* There should be only one ic_evtable */
#define IEEE80211COM_DELIVER_VAP_EVENT(_ic,_osif,_evt)  do {             \
        int i;                                                                                 \
        IEEE80211_COMM_LOCK(ic);                                                               \
        for(i=0;i<IEEE80211_MAX_DEVICE_EVENT_HANDLERS; ++i) {                                  \
            if ( _ic->ic_evtable[i]  && _ic->ic_evtable[i]->wlan_dev_vap_event) {              \
                (* _ic->ic_evtable[i]->wlan_dev_vap_event)(_ic->ic_event_arg[i],_ic,_osif,_evt);\
            }                                                                                  \
        }                                                                                      \
        IEEE80211_COMM_UNLOCK(ic);                                                             \
    } while(0)

#define OSIF_RADIO_DELIVER_EVENT_RADAR_DETECTED(_ic)  do {             \
        int _i;                                                                                 \
        IEEE80211_COMM_LOCK(_ic);                                                               \
        for(_i=0;_i<IEEE80211_MAX_DEVICE_EVENT_HANDLERS; ++_i) {                                  \
            if ( (_ic)->ic_evtable[_i]  && (_ic)->ic_evtable[_i]->wlan_radar_event) {              \
                (* (_ic)->ic_evtable[_i]->wlan_radar_event)((_ic)->ic_event_arg[_i], (_ic));\
            }                                                                                  \
        }                                                                                      \
        IEEE80211_COMM_UNLOCK(_ic);                                                             \
    } while(0)

#define OSIF_RADIO_DELIVER_EVENT_WATCHDOG(_ic,_reason)  do {             \
        int _i;                                                                                 \
        IEEE80211_COMM_LOCK(_ic);                                                               \
        for(_i=0;_i<IEEE80211_MAX_DEVICE_EVENT_HANDLERS; ++_i) {                                  \
            if ( (_ic)->ic_evtable[_i]  && (_ic)->ic_evtable[_i]->wlan_wdt_event) {              \
                (* (_ic)->ic_evtable[_i]->wlan_wdt_event)((_ic)->ic_event_arg[_i],(_ic),_reason);\
            }                                                                                  \
        }                                                                                      \
        IEEE80211_COMM_UNLOCK(_ic);                                                             \
    } while(0)

#define OSIF_RADIO_DELIVER_EVENT_CAC_START(_ic)  do {     \
    int _i;                                                 \
    IEEE80211_COMM_LOCK(_ic);                               \
    for(_i=0;_i<IEEE80211_MAX_DEVICE_EVENT_HANDLERS; ++_i) {\
      if ( (_ic)->ic_evtable[_i]  && (_ic)->ic_evtable[_i]->wlan_cac_start_event) { \
        (*(_ic)->ic_evtable[_i]->wlan_cac_start_event)((_ic)->ic_event_arg[_i], (_ic));\
      }                                                     \
    }                                                       \
    IEEE80211_COMM_UNLOCK(_ic);                             \
  } while(0)

#define OSIF_RADIO_DELIVER_EVENT_UP_AFTER_CAC(_ic)  do {     \
    int _i;                                                 \
    IEEE80211_COMM_LOCK(_ic);                               \
    for(_i=0;_i<IEEE80211_MAX_DEVICE_EVENT_HANDLERS; ++_i) {\
      if ( (_ic)->ic_evtable[_i]  && (_ic)->ic_evtable[_i]->wlan_up_after_cac_event) { \
        (*(_ic)->ic_evtable[_i]->wlan_up_after_cac_event)((_ic)->ic_event_arg[_i], (_ic));\
      }                                                     \
    }                                                       \
    IEEE80211_COMM_UNLOCK(_ic);                             \
  } while(0)

#define OSIF_RADIO_DELIVER_EVENT_CHAN_UTIL(_ic, _self_util, _obss_util)  do {     \
    int _i;                                                 \
    IEEE80211_COMM_LOCK(_ic);                               \
    for(_i=0;_i<IEEE80211_MAX_DEVICE_EVENT_HANDLERS; ++_i) {\
      if ( (_ic)->ic_evtable[_i]  && (_ic)->ic_evtable[_i]->wlan_chan_util_event) { \
        (*(_ic)->ic_evtable[_i]->wlan_chan_util_event)((_ic), _self_util, _obss_util);\
      }                                                     \
    }                                                       \
    IEEE80211_COMM_UNLOCK(_ic);                             \
  } while(0)

/*
 * Used by proto SM to check if calls to LMAC are syncronous or not.
 * The check assumes offload cmds to target firmware are not-syncronous.
 */
#define IEEE80211COM_IS_SYNCRONOUS(_ic) 0

/* Atheros ABOLT definitions */
#define IEEE80211_ABOLT_TURBO_G        0x01    /* Legacy Turbo G */
#define IEEE80211_ABOLT_TURBO_PRIME    0x02    /* Turbo Prime */
#define IEEE80211_ABOLT_COMPRESSION    0x04    /* Compression */
#define IEEE80211_ABOLT_FAST_FRAME     0x08    /* Fast Frames */
#define IEEE80211_ABOLT_BURST          0x10    /* Bursting */
#define IEEE80211_ABOLT_WME_ELE        0x20    /* WME based cwmin/max/burst tuning */
#define IEEE80211_ABOLT_XR             0x40    /* XR */
#define IEEE80211_ABOLT_AR             0x80    /* AR switches out based on adjacent non-turbo traffic */

/* Atheros Advanced Capabilities ABOLT definition */
#define IEEE80211_ABOLT_ADVCAP                  \
    (IEEE80211_ABOLT_TURBO_PRIME |              \
    IEEE80211_ABOLT_COMPRESSION |               \
    IEEE80211_ABOLT_FAST_FRAME |                \
    IEEE80211_ABOLT_XR |                        \
    IEEE80211_ABOLT_AR |                        \
    IEEE80211_ABOLT_BURST |                     \
    IEEE80211_ABOLT_WME_ELE)

/*
 * flags to be passed to ieee80211_vap_create function .
 */
#define IEEE80211_CLONE_BSSID   0x0001      /* allocate unique mac/bssid */
#define IEEE80211_NO_STABEACONS 0x0002      /* Do not setup the station beacon timers */
#define IEEE80211_CLONE_WDS             0x0004  /* enable WDS processing */
#define IEEE80211_CLONE_WDSLEGACY       0x0008  /* legacy WDS operation */
#define IEEE80211_PRIMARY_VAP           0x0010  /* primary vap */
#define IEEE80211_P2PDEV_VAP            0x0020  /* p2pdev vap */
#define IEEE80211_P2PGO_VAP             0x0040  /* p2p-go vap */
#define IEEE80211_P2PCLI_VAP            0x0080  /* p2p-client vap */
#define IEEE80211_P2P_DEVICE            (IEEE80211_P2PDEV_VAP | IEEE80211_P2PGO_VAP | IEEE80211_P2PCLI_VAP)

#define IEEE80211_FD_INTERVAL_MIN       20 /* Minimum FD interval */

#define NET80211_MEMORY_TAG     '11tN'

/* ic_htflags iv_htflags */
#define IEEE80211_HTF_SHORTGI40     0x0001
#define IEEE80211_HTF_SHORTGI20     0x0002

/* MFP support values */
typedef enum _ieee80211_mfp_type{
    IEEE80211_MFP_QOSDATA,
    IEEE80211_MFP_PASSTHRU,
    IEEE80211_MFP_HW_CRYPTO
} ieee80211_mfp_type;

#if ATH_SUPPORT_UORA
/* UORA Params */
#define IEEE_EOCW_MIN_DEFAULT            3 /* UORA EOCW_MIN Default Value */
#define IEEE_EOCW_MAX_DEFAULT            5 /* UORA EOCW_MIN Default Value */
#endif

/**
 * ieee80211vap_htallowed() - Check VAP if HT is allowed.
 * @vap: Pointer to struct ieee80211vap.
 *
 * Return: 1 is success, else 0.
 */
int ieee80211vap_htallowed(struct ieee80211vap *vap);
/**
 * ieee80211vap_vhtallowed() - Check VAP if VHT is allowed.
 * @vap: Pointer to struct ieee80211vap.
 *
 * Return: 1 is success, else 0.
 */
int ieee80211vap_vhtallowed(struct ieee80211vap *vap);
/**
 * ieee80211vap_6g_heallowed() - Check VAP if HE is allowed for only 6g radio.
 * @vap: Pointer to struct ieee80211vap.
 *
 * Return: 1 is success, else 0.
 */
int ieee80211vap_6g_heallowed(struct ieee80211vap *vap);
/**
 * ieee80211vap_heallowed() - Check VAP if HE is allowed.
 * @vap: Pointer to struct ieee80211vap.
 *
 * Return: 1 is success, else 0.
 */
int ieee80211vap_heallowed(struct ieee80211vap *vap);

void ieee80211_start_running(struct ieee80211com *ic);
void ieee80211_stop_running(struct ieee80211com *ic);
int ieee80211com_register_event_handlers(struct ieee80211com *ic,
                                     void *event_arg,
                                     wlan_dev_event_handler_table *evtable);
int ieee80211com_unregister_event_handlers(struct ieee80211com *ic,
                                     void *event_arg,
                                     wlan_dev_event_handler_table *evtable);

u_int16_t ieee80211_vaps_active(struct ieee80211com *ic);
u_int16_t ieee80211_vaps_ready(struct ieee80211com *ic, enum ieee80211_opmode opmode);
u_int8_t ieee80211_get_num_vaps_up(struct ieee80211com *ic);
u_int8_t ieee80211_get_num_ap_vaps_up(struct ieee80211com *ic);
u_int8_t ieee80211_get_num_beaconing_ap_vaps_up(struct ieee80211com *ic);
u_int8_t ieee80211_get_num_sta_vaps_up(struct ieee80211com *ic);

int ieee80211_dynamic_vap_setup(struct ieee80211com *ic, struct ieee80211vap *vap,
                            struct vdev_mlme_obj *vdev_mlme);
int ieee80211_vap_setup(struct ieee80211com *ic, struct ieee80211vap *vap,
                        struct vdev_mlme_obj *vdev_mlme,
                        int opmode, int scan_priority_base, u_int32_t flags,
                        const u_int8_t bssid[QDF_MAC_ADDR_SIZE]);

void ieee80211_proc_vattach(struct ieee80211vap *vap);
int ieee80211_ifattach(struct ieee80211com *ic, IEEE80211_REG_PARAMETERS *);
void ieee80211_ifdetach(struct ieee80211com *ic);

int ieee80211_vap_attach(struct ieee80211vap *vap);
void ieee80211_vap_detach(struct ieee80211vap *vap);
void ieee80211_vap_free(struct ieee80211vap *vap);

int ieee80211_vap_update_superag_cap(struct ieee80211vap *vap, int en_superag);
int ieee80211_vap_match_ssid(struct ieee80211vap *vap, const u_int8_t *ssid, u_int8_t ssidlen);

int ieee80211_vap_register_events(struct ieee80211vap *vap, wlan_event_handler_table *evtab);
int ieee80211_vap_register_mlme_events(struct ieee80211vap *vap, os_handle_t oshandle, wlan_mlme_event_handler_table *evtab);
int ieee80211_vap_unregister_mlme_events(struct ieee80211vap *vap,os_handle_t oshandle, wlan_mlme_event_handler_table *evtab);
int ieee80211_vap_register_misc_events(struct ieee80211vap *vap, os_handle_t oshandle, wlan_misc_event_handler_table *evtab);
int ieee80211_vap_unregister_misc_events(struct ieee80211vap *vap,os_handle_t oshandle, wlan_misc_event_handler_table *evtab);
int ieee80211_vap_register_ccx_events(struct ieee80211vap *vap, os_if_t osif, wlan_ccx_handler_table *evtab);
void ieee80211_vap_mlme_inact_erp_timeout(struct ieee80211com *ic);
int ieee80211_vap_update_wnm_bss_cap(struct ieee80211vap *vap, bool enable);
ieee80211_aplist_config_t ieee80211_vap_get_aplist_config(struct ieee80211vap *vap);
systime_t ieee80211_get_last_data_timestamp(wlan_if_t vaphandle);
systime_t ieee80211_get_directed_frame_timestamp(wlan_if_t vaphandle);
systime_t ieee80211_get_last_ap_frame_timestamp(wlan_if_t vaphandle);

int ieee80211_vendorie_vdetach(wlan_if_t vaphandle);

int ieee80211_aplist_config_vattach(ieee80211_aplist_config_t *pconfig,
        osdev_t osdev);

int ieee80211_aplist_config_vdetach(ieee80211_aplist_config_t *pconfig);

void ieee80211_aplist_config_init(ieee80211_aplist_config_t aplist_config);

bool ieee80211_aplist_get_accept_any_bssid(ieee80211_aplist_config_t pconfig);

int ieee80211_aplist_get_desired_bssid_count(ieee80211_aplist_config_t pconfig);

int ieee80211_aplist_get_desired_bssid(ieee80211_aplist_config_t pconfig,
                  int index, uint8_t **bssid);

uint32_t ieee80211_channel_frequency(struct ieee80211_ath_channel *chan);

int ieee80211_dfs_cac_start(struct ieee80211vap *vap);

int ieee80211_dfs_cac_cancel(struct ieee80211com *ic);

void ieee80211_dfs_cac_timer_start_async(void *data);

void ieee80211_dfs_cac_stop(struct ieee80211vap *vap,
                            int force);

int ieee80211_dfs_vaps_in_dfs_wait(struct ieee80211com *ic,
	                           struct ieee80211vap *curr_vap);

void ieee80211_mark_dfs(struct ieee80211com *ic,
                        uint8_t ieee,
                        uint16_t freq,
                        uint16_t ic_vhtop_freq_seg2,
                        uint64_t ic_flags);

QDF_STATUS ieee80211_start_csa(struct ieee80211com *ic,
                         uint8_t ieeeChan,
                         uint16_t freq,
                         uint16_t cfreq_80_mhz_seg2,
                         uint64_t flags);

void ieee80211_dfs_start_tx_rcsa_and_waitfor_rx_csa(struct ieee80211com *ic);

void ieee80211_dfs_rx_rcsa(struct ieee80211com *ic);

uint8_t *ieee80211_add_nol_ie(uint8_t *frm, struct ieee80211vap *vap,
			      struct ieee80211com *ic);

void ieee80211_dfs_cancel_waitfor_csa_timer(struct ieee80211com *ic);

bool ieee80211_dfs_start_rcsa(struct ieee80211com *ic);

OS_TIMER_FUNC(ieee80211_dfs_tx_rcsa_task);

OS_TIMER_FUNC(ieee80211_dfs_waitfor_csa_task);

#if ATH_SUPPORT_DFS && QCA_DFS_NOL_VAP_RESTART
OS_TIMER_FUNC(ieee80211_dfs_nochan_vap_restart);
#endif

void ieee80211_dfs_channel_mark_radar(struct ieee80211com *ic,
                                      struct ieee80211_ath_channel *chan);

void ieee80211_dfs_clist_update(struct ieee80211com *ic,
                                void *nollist,
                                int nentries);

/*
 * IC-based functions that gather information from all VAPs.
 */

/*
 * ieee80211_get_vap_opmode_count
 *     returns number of VAPs of each type currently active in an IC.
 */
void
ieee80211_get_vap_opmode_count(struct ieee80211com *ic,
                               struct ieee80211_vap_opmode_count *vap_opmode_count);

/*
 * ieee80211_sta_assoc_in_progress
 *     returns 1 if STA vap is not in associated state else 0
 */
u_int8_t ieee80211_sta_assoc_in_progress(struct ieee80211com *ic);

int ieee80211_regdmn_reset(struct ieee80211com *ic);

#define MBSS_CACHE_ENTRY_POS  0
#define MBSS_CACHE_ENTRY_CAP  1
#define MBSS_CACHE_ENTRY_SSID 2
#define MBSS_CACHE_ENTRY_IDX  3
#define MBSS_INVALID_NODE_IDX 0xff
int ieee80211_mbssid_setup(struct ieee80211vap *vap);
uint8_t ieee80211_mbssid_get_num_vaps_in_mbss_cache(struct ieee80211com  *ic);
int ieee80211_mbssid_update_mbssie_cache(struct ieee80211vap *vap, bool add);
void ieee80211_mbssid_update_mbssie_cache_entry(struct ieee80211vap *vap, uint8_t entry);
int ieee80211_mbssid_txvap_reset(struct ieee80211vap *vap);
int ieee80211_mbssid_txvap_set(struct ieee80211vap *vap);
int ieee80211_mbssid_txvap_is_active(struct ieee80211vap *vap);
bool ieee80211_mbss_is_beaconing_ap(struct ieee80211vap *vap);
int ieee80211_mbssid_beacon_control(struct ieee80211vap *vap, uint8_t ctrl);
bool wlan_mbss_beaconing_vdev_up(struct wlan_objmgr_vdev *vdev);

/* Derive non-tx vap node_idx using access-strategy defined as
 *
 * node_idx(tx-vap)     = f(ema_max_ap, rf)
 *                      = ema_max_ap  – rf – 1
 *
 * node_idx(non-tx vap) = f(node_idx_tx_vap, bssidx)
 *                      = (node_idx_tx_vap + bssidx) % ema_max_ap
 *                      = (ema_max_ap – rf – 1 + bssidx) % ema_max_ap
 *
 * ema_max_ap is nothing but user configurable INI to set the max
 * no of EMA APs which <= 2^n
 */
static INLINE uint32_t
ieee80211_mbssid_get_non_tx_vap_node_idx(struct ieee80211vap *vap,
                                        uint8_t ema_ap_num_max_vaps) {
    struct ieee80211com *ic = vap->iv_ic;
    struct vdev_mlme_obj *vdev_mlme = vap->vdev_mlme;
    bool is_mbssid_enabled = wlan_pdev_nif_feat_cap_get(ic->ic_pdev_obj,
                                WLAN_PDEV_F_MBSS_IE_ENABLE);
    bool is_non_tx_vap = IEEE80211_VAP_IS_MBSS_NON_TRANSMIT_ENABLED(vap);
    uint32_t node_idx = MBSS_INVALID_NODE_IDX;

    if (is_mbssid_enabled && is_non_tx_vap) {
        node_idx = (ema_ap_num_max_vaps -
                    ic->ic_mbss.rot_factor +
                    vdev_mlme->mgmt.mbss_11ax.profile_idx - 1) %
                    ema_ap_num_max_vaps;
    }

    mbss_debug("<<node_idx: %d", node_idx);
    return node_idx;
}

/* Derive Tx-vap node_idx using access-strategy defined as
 *
 * node_idx(tx-vap)     = f(ema_max_ap, rf)
 *                      = ema_max_ap  – rf – 1
 *
 * ema_max_ap is nothing but user configurable INI to set the max
 * no of EMA APs which <= 2^n
 */
static INLINE uint8_t
ieee80211_mbssid_get_tx_vap_node_idx(struct ieee80211com *ic,
                                     uint8_t ema_ap_num_max_vaps) {
    return (ema_ap_num_max_vaps - ic->ic_mbss.rot_factor - 1);
}

/*
 * ieee80211_flush_vap_mgmt_baclkog_queue : Flush mgmt frames for
 * underlying vap from host backlog queue.
 * @vap: vap whose frames shall be flushed
 * @force: whether forcefully flush mgmt_txrx queue or not.
 *
 * Return: void
 */
void
ieee80211_flush_vap_mgmt_queue(struct ieee80211vap *vap,
        bool force);

/*
 * ieee80211_flush_peer_mgmt_queue : Flush mgmt frames for peer
 * from host backlog queue.
 * @ni: node information
 *
 * Return: void
 */
void
ieee80211_flush_peer_mgmt_queue(struct ieee80211_node *ni);

int
ieee80211_has_weptkipaggr(struct ieee80211_node *ni);

bool

wlan_bsteering_set_inact_params(struct ieee80211com *ic,
                                u_int16_t inact_check_interval,
                                u_int16_t inact_normal,
                                u_int16_t inact_overload);

void wlan_bsteering_set_overload_param(struct ieee80211com *ic,bool overload);
void wlan_bsteering_send_null(struct ieee80211com *ic,
                              u_int8_t *macaddr,
                              struct ieee80211vap *vap);
bool wlan_bsteering_direct_attach_enable(struct ieee80211com *ic,bool enable);


static INLINE osdev_t
ieee80211com_get_oshandle(struct ieee80211com *ic)
{
     return ic->ic_osdev;
}

static INLINE void
ieee80211com_set_flags(struct ieee80211com *ic, u_int32_t cap)
{
    ic->ic_flags |= cap;
    wlan_ic_psoc_set_flag(ic, cap);
    wlan_ic_pdev_set_flag(ic, cap);
}

static INLINE void
ieee80211com_set_flags_ext(struct ieee80211com *ic, u_int32_t cap)
{
    ic->ic_flags_ext |= cap;
    wlan_ic_psoc_set_extflag(ic, cap);
    wlan_ic_pdev_set_extflag(ic, cap);
}

static INLINE void
ieee80211com_clear_flags(struct ieee80211com *ic, u_int32_t cap)
{
    ic->ic_flags &= ~cap;
    wlan_ic_psoc_clear_flag(ic, cap);
    wlan_ic_pdev_clear_flag(ic, cap);
}

static INLINE void
ieee80211com_clear_flags_ext(struct ieee80211com *ic, u_int32_t cap)
{
    ic->ic_flags_ext &= ~cap;
    wlan_ic_psoc_clear_extflag(ic, cap);
    wlan_ic_pdev_clear_extflag(ic, cap);
}

/*
 * Capabilities
 */
static INLINE void
ieee80211com_set_cap(struct ieee80211com *ic, u_int32_t cap)
{
    ic->ic_caps |= cap;
    wlan_ic_psoc_set_cap(ic, cap);
    wlan_ic_pdev_set_cap(ic, cap);
}

static INLINE void
ieee80211com_clear_cap(struct ieee80211com *ic, u_int32_t cap)
{
    ic->ic_caps &= ~cap;
    wlan_ic_psoc_clear_cap(ic, cap);
    wlan_ic_pdev_clear_cap(ic, cap);
}

static INLINE int
ieee80211com_has_cap(struct ieee80211com *ic, u_int32_t cap)
{
    return ((ic->ic_caps & cap) != 0);
}

int
ieee80211com_init_netlink(struct ieee80211com *ic);
/*
 * Extended Capabilities
 */
static INLINE void
ieee80211com_set_cap_ext(struct ieee80211com *ic, u_int32_t cap)
{
    ic->ic_caps_ext |= cap;
    wlan_ic_psoc_set_extcap(ic, cap);
    wlan_ic_pdev_set_extcap(ic, cap);
}

static INLINE void
ieee80211com_clear_cap_ext(struct ieee80211com *ic, u_int32_t cap)
{
    ic->ic_caps_ext &= ~cap;
    wlan_ic_psoc_clear_extcap(ic, cap);
    wlan_ic_pdev_clear_extcap(ic, cap);
}

static INLINE int
ieee80211com_has_cap_ext(struct ieee80211com *ic, u_int32_t cap_ext)
{
    return ((ic->ic_caps_ext & cap_ext) != 0);
}

/*
 * Cipher Capabilities
 */
static INLINE void
ieee80211com_set_ciphercap(struct ieee80211com *ic, u_int32_t ciphercap)
{
    ic->ic_cipher_caps |= ciphercap;
}

static INLINE void
ieee80211com_clear_ciphercap(struct ieee80211com *ic, u_int32_t ciphercap)
{
    ic->ic_cipher_caps &= ~ciphercap;
}

static INLINE int
ieee80211com_has_ciphercap(struct ieee80211com *ic, u_int32_t ciphercap)
{
    return ((ic->ic_cipher_caps & ciphercap) != 0);
}

/*
 * Atheros Capabilities
 */
static INLINE void
ieee80211com_set_athcap(struct ieee80211com *ic, u_int32_t athcap)
{
    ic->ic_ath_cap |= athcap;
}

static INLINE void
ieee80211com_clear_athcap(struct ieee80211com *ic, u_int32_t athcap)
{
    ic->ic_ath_cap &= ~athcap;
}

static INLINE int
ieee80211com_has_athcap(struct ieee80211com *ic, u_int32_t athcap)
{
    return ((ic->ic_ath_cap & athcap) != 0);
}

/*
 * Atheros State machine Roaming capabilities
 */


static INLINE void
ieee80211com_set_roaming(struct ieee80211com *ic, u_int8_t roaming)
{
    ic->ic_roaming = roaming;
}

static INLINE u_int8_t
ieee80211com_get_roaming(struct ieee80211com *ic)
{
    return ic->ic_roaming;
}

#define wlan_pdev_set_athextcap(pdev, val) /**/
#define wlan_pdev_clear_athextcap(pdev, val) /**/
/*
 * Atheros Extended Capabilities
 */
static INLINE void
ieee80211com_set_athextcap(struct ieee80211com *ic, u_int32_t athextcap)
{
    ic->ic_ath_extcap |= athextcap;
    wlan_pdev_set_athextcap(ic->ic_pdev_obj, athextcap);
}

static INLINE void
ieee80211com_clear_athextcap(struct ieee80211com *ic, u_int32_t athextcap)
{
    ic->ic_ath_extcap &= ~athextcap;
    wlan_pdev_clear_athextcap(ic->ic_pdev_obj, athextcap);
}

static INLINE int
ieee80211com_has_athextcap(struct ieee80211com *ic, u_int32_t athextcap)
{
    return ((ic->ic_ath_extcap & athextcap) != 0);
}

/* to check if node need, extra delimeter fix */
static INLINE int
ieee80211com_has_extradelimwar(struct ieee80211com *ic)
{
    return (ic->ic_ath_extcap & IEEE80211_ATHEC_EXTRADELIMWAR) ;
}

static INLINE int
ieee80211com_has_pn_check_war(struct ieee80211com *ic)
{
    return (ic->ic_ath_extcap & IEEE80211_ATHEC_PN_CHECK_WAR) ;
}


/*
 * 11n
 */
static INLINE void
ieee80211com_set_htcap(struct ieee80211com *ic, u_int16_t htcap)
{
    ic->ic_htcap |= htcap;
}

static INLINE void
ieee80211com_clear_htcap(struct ieee80211com *ic, u_int16_t htcap)
{
    ic->ic_htcap &= ~htcap;
}

static INLINE int
ieee80211com_has_htcap(struct ieee80211com *ic, u_int16_t htcap)
{
    return ((ic->ic_htcap & htcap) != 0);
}

static INLINE int
ieee80211com_get_ldpccap(struct ieee80211com *ic)
{
    return ic->ic_ldpccap;
}

static INLINE void
ieee80211com_set_ldpccap(struct ieee80211com *ic, u_int16_t ldpccap)
{
    if (ldpccap == 0) {
        ic->ic_ldpccap = 0;
        return;
    }

    if (ldpccap & IEEE80211_HTCAP_C_LDPC_TX)
        ic->ic_ldpccap |= IEEE80211_HTCAP_C_LDPC_TX;

    if (ldpccap & IEEE80211_HTCAP_C_LDPC_RX)
        ic->ic_ldpccap |= IEEE80211_HTCAP_C_LDPC_RX;
}

static INLINE void
ieee80211com_clear_ldpccap(struct ieee80211com *ic, u_int16_t ldpccap)
{
    if (ldpccap & IEEE80211_HTCAP_C_LDPC_TX)
        ic->ic_ldpccap &= ~IEEE80211_HTCAP_C_LDPC_TX;

    if (ldpccap & IEEE80211_HTCAP_C_LDPC_RX)
        ic->ic_ldpccap &= ~IEEE80211_HTCAP_C_LDPC_RX;
}

static INLINE void
ieee80211com_set_htextcap(struct ieee80211com *ic, u_int16_t htextcap)
{
    ic->ic_htextcap |= htextcap;
}

static INLINE void
ieee80211com_clear_htextcap(struct ieee80211com *ic, u_int16_t htextcap)
{
    ic->ic_htextcap &= ~htextcap;
}

static INLINE int
ieee80211com_has_htextcap(struct ieee80211com *ic, u_int16_t htextcap)
{
    return ((ic->ic_htextcap & htextcap) != 0);
}

static INLINE void
ieee80211com_set_htflags(struct ieee80211com *ic, u_int16_t htflags)
{
    ic->ic_htflags |= htflags;
}

static INLINE void
ieee80211com_clear_htflags(struct ieee80211com *ic, u_int16_t htflags)
{
    ic->ic_htflags &= ~htflags;
}

static INLINE int
ieee80211com_has_htflags(struct ieee80211com *ic, u_int16_t htflags)
{
    return ((ic->ic_htflags & htflags) != 0);
}

static INLINE void
ieee80211com_set_maxampdu(struct ieee80211com *ic, u_int8_t maxampdu)
{
    ic->ic_maxampdu = maxampdu;
}

static INLINE u_int8_t
ieee80211com_get_mpdudensity(struct ieee80211com *ic)
{
    return ic->ic_mpdudensity;
}

static INLINE void
ieee80211com_set_mpdudensity(struct ieee80211com *ic, u_int8_t mpdudensity)
{
    ic->ic_mpdudensity = mpdudensity;
}

static INLINE u_int8_t
ieee80211com_get_weptkipaggr_rxdelim(struct ieee80211com *ic)
{
    return (ic->ic_weptkipaggr_rxdelim);
}

static INLINE void
ieee80211com_set_weptkipaggr_rxdelim(struct ieee80211com *ic, u_int8_t weptkipaggr_rxdelim)
{
    ic->ic_weptkipaggr_rxdelim = weptkipaggr_rxdelim;
}

static INLINE u_int16_t
ieee80211com_get_channel_switching_time_usec(struct ieee80211com *ic)
{
    return (ic->ic_channelswitchingtimeusec);
}

static INLINE void
ieee80211com_set_channel_switching_time_usec(struct ieee80211com *ic, u_int16_t channel_switching_time_usec)
{
    ic->ic_channelswitchingtimeusec = channel_switching_time_usec;
}

/*
 * PHY type
 */
static INLINE enum ieee80211_phytype
ieee80211com_get_phytype(struct ieee80211com *ic)
{
    return ic->ic_phytype;
}

static INLINE void
ieee80211com_set_phytype(struct ieee80211com *ic, enum ieee80211_phytype phytype)
{
    ic->ic_phytype = phytype;
}

/*
 * 11ac
 */
static INLINE void
ieee80211com_set_vhtcap(struct ieee80211com *ic, u_int32_t vhtcap)
{
    ic->ic_vhtcap |= vhtcap;
}

static INLINE void
ieee80211com_clear_vhtcap(struct ieee80211com *ic, u_int32_t vhtcap)
{
    ic->ic_vhtcap &= ~vhtcap;
}

static INLINE int
ieee80211com_has_vhtcap(struct ieee80211com *ic, u_int32_t vhtcap)
{
    return ((ic->ic_vhtcap & vhtcap) != 0);
}

static INLINE void
ieee80211com_set_vht_mcs_map(struct ieee80211com *ic, u_int32_t mcs_map)
{
    ic->ic_vhtcap_max_mcs.rx_mcs_set.mcs_map =
    ic->ic_vhtcap_max_mcs.tx_mcs_set.mcs_map =
                                    (mcs_map & IEEE80211_VHT_LOWER_MCS_MAP);
    if(mcs_map & IEEE80211_VHT_MCS10_11_SUPP) {
        ic->ic_vhtcap_max_mcs.rx_mcs_set.higher_mcs_supp =
        ic->ic_vhtcap_max_mcs.tx_mcs_set.higher_mcs_supp =
                      ((mcs_map & IEEE80211_VHT_HIGHER_MCS_MAP) >>
                                        IEEE80211_VHT_HIGHER_MCS_S);
    }
}

static INLINE void
ieee80211com_set_vht_high_data_rate(struct ieee80211com *ic, u_int16_t datarate)
{
        ic->ic_vhtcap_max_mcs.rx_mcs_set.data_rate =
        ic->ic_vhtcap_max_mcs.tx_mcs_set.data_rate = datarate;
}

static INLINE void
ieee80211com_set_vhtop_basic_mcs_map(struct ieee80211com *ic, u_int16_t basic_mcs_map)
{
    ic->ic_vhtop_basic_mcs = basic_mcs_map;
}

static INLINE void
ieee80211com_set_rx_amsdu(struct ieee80211com *ic, u_int8_t tid, bool flag)
{
    u_int8_t mask = 1 << tid;
    ic->ic_rx_amsdu = (ic->ic_rx_amsdu & ~mask) | ((flag << tid) & mask);
}

static INLINE bool
ieee80211com_get_rx_amsdu(struct ieee80211com *ic, u_int8_t tid)
{
    return (ic->ic_rx_amsdu & (1 << tid));
}

static INLINE void
ieee80211vap_set_flag(struct ieee80211vap *vap, u_int32_t cap)
{
    vap->iv_flags |= cap;
    wlan_vap_vdev_set_flag(vap, cap);
}

static INLINE void
ieee80211vap_clear_flag(struct ieee80211vap *vap, u_int32_t cap)
{
    vap->iv_flags &= ~cap;
    wlan_vap_vdev_clear_flag(vap, cap);
}

static INLINE void
ieee80211vap_set_flag_ext(struct ieee80211vap *vap, u_int32_t cap)
{
    vap->iv_flags_ext |= cap;
    wlan_vap_vdev_set_extflag(vap, cap);
}

static INLINE void
ieee80211vap_clear_flag_ext(struct ieee80211vap *vap, u_int32_t cap)
{
    vap->iv_flags_ext &= ~cap;
    wlan_vap_vdev_clear_extflag(vap, cap);
}

static INLINE void
ieee80211vap_set_flag_ext2(struct ieee80211vap *vap, u_int32_t cap)
{
    vap->iv_flags_ext2 |= cap;
    wlan_vap_vdev_set_ext2flag(vap, cap);
}

static INLINE void
ieee80211vap_clear_flag_ext2(struct ieee80211vap *vap, u_int32_t cap)
{
    vap->iv_flags_ext2 &= ~cap;
    wlan_vap_vdev_clear_ext2flag(vap, cap);
}

static INLINE void
ieee80211vap_set_flag_ext22(struct ieee80211vap *vap, u_int32_t cap)
{
    vap->iv_flags_ext2 |= cap;
    wlan_vap_vdev_set_ext22flag(vap, cap);
}

static INLINE void
ieee80211vap_clear_flag_ext22(struct ieee80211vap *vap, u_int32_t cap)
{
    vap->iv_flags_ext2 &= ~cap;
    wlan_vap_vdev_clear_ext2flag(vap, cap);
}

static INLINE void
ieee80211vap_set_flag_ext222(struct ieee80211vap *vap, u_int32_t cap)
{
    vap->iv_flags_ext |= cap;
    wlan_vap_vdev_set_ext222flag(vap, cap);
}

static INLINE void
ieee80211vap_clear_flag_ext222(struct ieee80211vap *vap, u_int32_t cap)
{
    vap->iv_flags_ext &= ~cap;
    wlan_vap_vdev_clear_ext222flag(vap, cap);
}

static INLINE void
ieee80211vap_set_cap(struct ieee80211vap *vap, u_int32_t cap)
{
    vap->iv_caps |= cap;
    wlan_vap_vdev_set_cap(vap, cap);
}

static INLINE void
ieee80211vap_clear_cap(struct ieee80211vap *vap, u_int32_t cap)
{
    vap->iv_caps &= ~cap;
    wlan_vap_vdev_clear_cap(vap, cap);
}

static INLINE void
ieee80211vap_get_insctd_mcsnssmap(struct ieee80211vap *vap,
                   uint16_t rxmcsnssmap[HEHANDLE_CAP_TXRX_MCS_NSS_SIZE],
                   uint16_t txmcsnssmap[HEHANDLE_CAP_TXRX_MCS_NSS_SIZE]) {
    struct ieee80211com *ic  = vap->iv_ic;
    struct ieee80211_he_handle *ic_he = &ic->ic_he;

    if (vap->iv_he_tx_mcsnssmap != HE_INVALID_MCSNSSMAP) {
        /* User sends HE MCS-NSS info for less than equal to 80MHz encoded
         * in the lower 16 bits */
        txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80]    =
                        HECAP_TXRX_MCS_NSS_GET_LT80_INFO(vap->iv_he_tx_mcsnssmap);
        /* User sends HE MCS-NSS info for greater than 80MHz encoded in the
         * upper 16 bits */
        txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_160]   =
                        HECAP_TXRX_MCS_NSS_GET_GT80_INFO(vap->iv_he_tx_mcsnssmap);
        /* User sends HE MCS-NSS info for greater than 80MHz encoded in the
         * upper 16 bits */
        txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80_80] =
                        HECAP_TXRX_MCS_NSS_GET_GT80_INFO(vap->iv_he_tx_mcsnssmap);

        /* intersect user configured values with target capabilities */
        txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80]    =
            INTERSECT_11AX_MCSNSS_MAP(txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80],
                    ic_he->hecap_txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80]);

        txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_160]    =
            INTERSECT_11AX_MCSNSS_MAP(txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_160],
                    ic_he->hecap_txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_160]);

        txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80_80]    =
            INTERSECT_11AX_MCSNSS_MAP(txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80_80],
                    ic_he->hecap_txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80_80]);

    } else {
        txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80] =
                        ic_he->hecap_txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80];
        txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_160] =
                        ic_he->hecap_txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_160];
        txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80_80] =
                        ic_he->hecap_txmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80_80];
    }

    if (vap->iv_he_rx_mcsnssmap != HE_INVALID_MCSNSSMAP) {
        /* User sends HE MCS-NSS info for less than equal to 80MHz encoded
         * in the lower 16 bits */
        rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80]    =
                        HECAP_TXRX_MCS_NSS_GET_LT80_INFO(vap->iv_he_rx_mcsnssmap);
        /* User sends HE MCS-NSS info for greater than 80MHz encoded in the
         * upper 16 bits */
        rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_160]   =
                        HECAP_TXRX_MCS_NSS_GET_GT80_INFO(vap->iv_he_rx_mcsnssmap);
        /* User sends HE MCS-NSS info for greater than 80MHz encoded in the
         * upper 16 bits */
        rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80_80] =
                        HECAP_TXRX_MCS_NSS_GET_GT80_INFO(vap->iv_he_rx_mcsnssmap);

        /* intersect user configured values with target capabilities */
        rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80]    =
            INTERSECT_11AX_MCSNSS_MAP(rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80],
                    ic_he->hecap_rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80]);

        rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_160]    =
            INTERSECT_11AX_MCSNSS_MAP(rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_160],
                    ic_he->hecap_rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_160]);

        rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80_80]    =
            INTERSECT_11AX_MCSNSS_MAP(rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80_80],
                    ic_he->hecap_rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80_80]);
    } else {
        rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80] =
                        ic_he->hecap_rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80];
        rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_160] =
                        ic_he->hecap_rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_160];
        rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80_80] =
                        ic_he->hecap_rxmcsnssmap[HECAP_TXRX_MCS_NSS_IDX_80_80];
    }
}

/**
* @brief        Checks if 160 MHz capability bit is set in vhtcap
*
* @param vhtcap: vhtcap info
*
* @return true if capability bit is set, false othersie.
*/
static INLINE bool ieee80211_get_160mhz_vhtcap(u_int32_t vhtcap)
{
    return ((vhtcap & IEEE80211_VHTCAP_SUP_CHAN_WIDTH_160) != 0);
}

/**
* @brief        Checks if 160 and 80+80 MHz capability bit is set in vhtcap
*
* @param vhtcap: vhtcap info
*
* @return true if capability bit is set, false othersie.
*/
static INLINE bool
ieee80211_get_80p80_160mhz_vhtcap(u_int32_t vhtcap)
{
    return ((vhtcap & IEEE80211_VHTCAP_SUP_CHAN_WIDTH_80_160) != 0);
}

/**
* @brief        Checks if 160 MHz short GI is enabled in vhtcap
*
* @param vhtcap: vhtcap info
*
* @return true if capability bit is set, false othersie.
*/
static INLINE bool
ieee80211_get_160mhz_shortgi_vhtcap(u_int32_t vhtcap)
{
    return ((vhtcap & IEEE80211_VHTCAP_SHORTGI_160) != 0);
}


/*
 * XXX these need to be here for IEEE80211_F_DATAPAD
 */

/*
 * Return the space occupied by the 802.11 header and any
 * padding required by the driver.  This works for a
 * management or data frame.
 */
static INLINE int
ieee80211_hdrspace(struct ieee80211com *ic, const void *data)
{
    int size = ieee80211_hdrsize(data);

    if (ic->ic_flags & IEEE80211_F_DATAPAD)
        size = roundup(size, sizeof(u_int32_t));

    return size;
}

/*
 * Like ieee80211_hdrspace, but handles any type of frame.
 */
static INLINE int
ieee80211_anyhdrspace(struct wlan_objmgr_pdev *pdev, const void *data)
{
    int size = ieee80211_anyhdrsize(data);
    const struct ieee80211_frame *wh = (const struct ieee80211_frame *)data;

    if ((wh->i_fc[0]&IEEE80211_FC0_TYPE_MASK) == IEEE80211_FC0_TYPE_CTL) {
        switch (wh->i_fc[0] & IEEE80211_FC0_SUBTYPE_MASK) {
            case IEEE80211_FC0_SUBTYPE_CTS:
            case IEEE80211_FC0_SUBTYPE_ACK:
                return size;
        }
    }

    if (wlan_pdev_nif_feat_cap_get(pdev, WLAN_PDEV_F_DATAPAD))
        size = roundup(size, sizeof(u_int32_t));

    return size;
}

static INLINE struct wmeParams *
ieee80211com_wmm_chanparams(struct ieee80211com *ic, int ac)
{
    ASSERT(ac < WME_NUM_AC);
    return &ic->ic_wme.wme_chanParams.cap_wmeParams[ac];
}

/*
 * chainmask
 */
static INLINE void
ieee80211com_set_tx_chainmask(struct ieee80211com *ic, u_int8_t chainmask)
{
    ic->ic_tx_chainmask = chainmask;
    wlan_objmgr_update_txchainmask_to_allvdevs(ic);
}

static INLINE void
ieee80211com_set_rx_chainmask(struct ieee80211com *ic, u_int8_t chainmask)
{
    ic->ic_rx_chainmask = chainmask;
    wlan_objmgr_update_rxchainmask_to_allvdevs(ic);
}

static INLINE u_int8_t
ieee80211com_get_tx_chainmask(struct ieee80211com *ic)
{
    return ic->ic_tx_chainmask;
}

static INLINE u_int8_t
ieee80211com_get_rx_chainmask(struct ieee80211com *ic)
{
    return ic->ic_rx_chainmask;
}

static INLINE void
ieee80211com_set_spatialstreams(struct ieee80211com *ic, u_int8_t stream)
{
    ic->ic_spatialstreams = stream;
}

static INLINE u_int8_t
ieee80211com_get_spatialstreams(struct ieee80211com *ic)
{
    return ic->ic_spatialstreams;
}

static INLINE void
ieee80211com_set_num_tx_chain(struct ieee80211com *ic, u_int8_t num_chain)
{
    ic->ic_num_tx_chain = num_chain;
}

static INLINE void
ieee80211com_set_num_rx_chain(struct ieee80211com *ic, u_int8_t num_chain)
{
    ic->ic_num_rx_chain = num_chain;
}

#if ATH_SUPPORT_WAPI
static INLINE void
ieee80211com_set_wapi_max_tx_chains(struct ieee80211com *ic, u_int8_t num_chain)
{
    ic->ic_num_wapi_tx_maxchains = num_chain;
}

static INLINE void
ieee80211com_set_wapi_max_rx_chains(struct ieee80211com *ic, u_int8_t num_chain)
{
    ic->ic_num_wapi_rx_maxchains = num_chain;
}
#endif

static INLINE u_int16_t
ieee80211com_get_txpowerlimit(struct ieee80211com *ic)
{
    return ic->ic_txpowlimit;
}

static INLINE void
ieee80211com_set_txpowerlimit(struct ieee80211com *ic, u_int16_t txpowlimit)
{
    ic->ic_txpowlimit = txpowlimit;
}

/*
 * Channel
 */
static INLINE void
ieee80211com_set_curchanmaxpwr(struct ieee80211com *ic, u_int8_t maxpower)
{
    ic->ic_curchanmaxpwr = maxpower;
}

static INLINE u_int8_t
ieee80211com_get_curchanmaxpwr(struct ieee80211com *ic)
{
    return ic->ic_curchanmaxpwr;
}

static INLINE  struct ieee80211_ath_channel*
ieee80211com_get_curchan(struct ieee80211com *ic)
{
    return ic->ic_curchan;   /* current channel */
}

static INLINE void
ieee80211_set_tspecActive(struct ieee80211com *ic, u_int8_t val)
{
    ic->ic_tspec_active = val;
}

static INLINE int
ieee80211_is_tspecActive(struct ieee80211com *ic)
{
    return ic->ic_tspec_active;
}

static INLINE u_int32_t
ieee80211_get_tsf32(struct ieee80211com *ic)
{
    return ic->ic_get_TSF32(ic);
}

static INLINE int
ieee80211_get_bw_nss_mapping(struct ieee80211vap *vap, struct ieee80211_bwnss_map *nssmap, u_int8_t chainmask)
{
    struct ieee80211com *ic = vap->iv_ic;

    if(ic->ic_get_bw_nss_mapping)
        return ic->ic_get_bw_nss_mapping(vap, nssmap, chainmask);

    return -EINVAL;
}

/*
 * Pre-conditions for ForcePPM to be enabled.
 */
#define ieee80211com_can_enable_force_ppm(_ic)  0

/*
 * internal macro to iterate through vaps.
 * Modify the duplicate definition used in
 * ieee80211_ioctl.h if below defintions change
 */
#if ATH_SUPPORT_AP_WDS_COMBO
#define IEEE80211_MAX_VAPS 16
#elif ATH_SUPPORT_WRAP
#define IEEE80211_MAX_VAPS 32
#elif ATH_PERF_PWR_OFFLOAD
#define IEEE80211_MAX_VAPS 17
#else
#define IEEE80211_MAX_VAPS 16
#endif
/*
 * Need nt_nodelock since iv_bss could have changed.
 * TBD: make ic_lock a read/write lock to reduce overhead in input_all
 */
#define ieee80211_iterate_vap_list_internal(ic,iter_func,arg,vaps_count)             \
do {                                                                                 \
    struct ieee80211vap *_vap;                                                       \
    struct ieee80211_node * ni;                                                      \
    struct ieee80211_node *bss_node[IEEE80211_MAX_VAPS];                             \
    u_int16_t    idx;                                                                \
    vaps_count=0;                                                                    \
    IEEE80211_COMM_LOCK(ic);                                                         \
    TAILQ_FOREACH(_vap, &ic->ic_vaps, iv_next) {                                     \
        if (ieee80211_vap_deleted_is_set(_vap))                                      \
            continue;                                                                \
        if (vaps_count >= IEEE80211_MAX_VAPS) {                                      \
            IEEE80211_DPRINTF(_vap, IEEE80211_MSG_DEBUG,                             \
                    "Max Vap count %s reached\n", __func__);                         \
            break;                                                                   \
        }                                                                            \
        if ((ni = ieee80211_try_ref_bss_node(_vap, WLAN_MLME_SB_ID)) != NULL)                         \
            bss_node[vaps_count++] = ni;                                             \
    }                                                                                \
    IEEE80211_COMM_UNLOCK(ic);                                                       \
    for (idx=0; idx<vaps_count; ++idx) {                                             \
         if (bss_node[idx]) {                                                        \
             iter_func(arg, bss_node[idx]->ni_vap, (idx == (vaps_count -1)));        \
             ieee80211_free_node(bss_node[idx], WLAN_MLME_SB_ID);                                     \
        }                                                                            \
    }                                                                                \
} while(0)


/*
 * Key update synchronization methods.  XXX should not be visible.
 */
static INLINE void
ieee80211_key_update_begin(struct ieee80211vap *vap)
{
}
static INLINE void
ieee80211_key_update_end(struct ieee80211vap *vap)
{
}

/*
 * Return the bssid of a frame.
 */
static INLINE const u_int8_t *
ieee80211vap_getbssid(struct ieee80211vap *vap, const struct ieee80211_frame *wh)
{
    if (vap->iv_opmode == IEEE80211_M_STA)
        return wh->i_addr2;
    if ((wh->i_fc[1] & IEEE80211_FC1_DIR_MASK) != IEEE80211_FC1_DIR_NODS)
        return wh->i_addr1;
    if ((wh->i_fc[0] & IEEE80211_FC0_SUBTYPE_MASK) == IEEE80211_FC0_SUBTYPE_PS_POLL)
        return wh->i_addr1;
    return wh->i_addr3;
}

/*
 * Operation Mode
 */
static INLINE enum ieee80211_opmode
ieee80211vap_get_opmode(struct ieee80211vap *vap)
{
    return vap->iv_opmode;
}

/*
 * Misc
 */

static INLINE struct ieee80211_node *
ieee80211vap_get_bssnode(struct ieee80211vap *vap)
{
    return vap->iv_bss;
}

static INLINE void
ieee80211vap_get_macaddr(struct ieee80211vap *vap, u_int8_t *macaddr)
{
    /* the same as IC for extensible STA mode */
    IEEE80211_ADDR_COPY(macaddr, vap->iv_myaddr);
}

static INLINE void
ieee80211vap_set_macaddr(struct ieee80211vap *vap, const u_int8_t *macaddr)
{
    /* Normally shouldn't be called for a station */
    IEEE80211_ADDR_COPY(vap->iv_myaddr, macaddr);
}

static INLINE void *
ieee80211vap_get_private_data(struct ieee80211vap *vap)
{
    return vap->iv_priv;
}

static INLINE void
ieee80211vap_set_private_data(struct ieee80211vap *vap , void *priv_data)
{
    vap->iv_priv = priv_data;
}

static INLINE IEEE80211_VEN_IE *
ieee80211vap_get_venie(struct ieee80211vap *vap)
{
    return vap->iv_venie;
}

static INLINE IEEE80211_VEN_IE *
ieee80211vap_init_venie(struct ieee80211vap *vap)
{
    vap->iv_venie = (IEEE80211_VEN_IE *) OS_MALLOC(vap->iv_ic->ic_osdev,
     					sizeof(IEEE80211_VEN_IE), GFP_KERNEL);
    return vap->iv_venie;
}

static INLINE void ieee80211vap_delete_venie(struct ieee80211vap *vap)
{
    if(vap->iv_venie) {
        OS_FREE(vap->iv_venie);
        vap->iv_venie = NULL;
    }
}

/**
 * set(register) input filter management function callback.
 * @param vap                        : pointer to vap
 * @param mgmt_filter_function       : input management filter function calback.
 * @return the value of old filter function.
 *  the input management function is called for every received management frame.
 *  if the call back function returns true frame will be dropped.
 *  if the call back function returns false then hte frame will be passed down to mlme.
 * *** NOTE: the call back function is called even if the vap is not active.
 */
static INLINE  ieee80211_vap_input_mgmt_filter
ieee80211vap_set_input_mgmt_filter(struct ieee80211vap *vap , ieee80211_vap_input_mgmt_filter mgmt_filter_func)
{
     ieee80211_vap_input_mgmt_filter old_filter=vap->iv_input_mgmt_filter;
     vap->iv_input_mgmt_filter = mgmt_filter_func;
     return old_filter;
}

/**
 * set(register) output filter management function callback.
 * @param vap                        : pointer to vap
 * @param mgmt_filter_function       : output management filter function calback.
 * @return the value of old filter function.
 *  the output management function is called for every transimitted management frame.
 *   just before handing over the frame to lmac.
 *  if the call back function returns true frame will be dropped.
 *  if the call back function returns false then hte frame will be passed down to lmac.
 */
static INLINE  ieee80211_vap_output_mgmt_filter
ieee80211vap_set_output_mgmt_filter_func(struct ieee80211vap *vap , ieee80211_vap_output_mgmt_filter mgmt_output_filter_func)
{
     ieee80211_vap_output_mgmt_filter old_func=vap->iv_output_mgmt_filter;
     vap->iv_output_mgmt_filter = mgmt_output_filter_func;
     return old_func;
}

static INLINE int
ieee80211vap_has_pssta(struct ieee80211vap *vap)
{
    return (vap->iv_ps_sta != 0);
}

static INLINE bool is_weptkip_htallowed(struct ieee80211vap *vap, struct ieee80211_node *ni)
{
    if (!(vap || ni))
	return 0;
    if (vap)
        return wlan_crypto_is_htallowed(vap->vdev_obj, NULL);
    else
        return wlan_crypto_is_htallowed(NULL, ni->peer_obj);
}

static INLINE u_int8_t
ieee80211vap_11ng_vht_interopallowed (struct ieee80211vap *vap)
{
    return ieee80211_vap_11ng_vht_interop_is_set(vap);
}

static INLINE bool
ieee80211vap_ishemode(struct ieee80211vap *vap)
{
    if (vap->iv_des_mode >= IEEE80211_MODE_11AXA_HE20)
        return true;

    return false;
}

static INLINE int
ieee80211vap_vht_or_above_allowed(struct ieee80211vap *vap)
{
    return (ieee80211vap_vhtallowed(vap) | ieee80211vap_heallowed(vap));
}

static INLINE bool
ieee80211vap_muedca_is_enabled(struct ieee80211vap *vap)
{
    if(vap->iv_he_muedca) {
        return 1;
    }

    return 0;
}


#if ATH_SUPPORT_UORA
static INLINE bool
ieee80211vap_uora_is_enabled(struct ieee80211vap *vap)
{
    struct ieee80211com *ic = vap->iv_ic;
    u_int32_t ic_hecap_mac_low;
    int val;
    struct ieee80211_he_handle *ic_he = &ic->ic_he;
    qdf_mem_copy(&ic_hecap_mac_low, &ic_he->hecap_macinfo[HECAP_MACBYTE_IDX0], sizeof(ic_hecap_mac_low));

    val = HECAP_MAC_OFDMARA_GET_FROM_IC(ic_hecap_mac_low);
    if(vap->iv_he_ofdma_ra && val)
        return 1;
    return 0;
}
#endif

static INLINE void
ieee80211vap_set_htflags(struct ieee80211vap *vap, u_int16_t htflags)
{
    vap->iv_htflags |= htflags;
}

static INLINE void
ieee80211vap_clear_htflags(struct ieee80211vap *vap, u_int16_t htflags)
{
    vap->iv_htflags &= ~htflags;
}

static INLINE int
ieee80211vap_has_htflags(struct ieee80211vap *vap, u_int16_t htflags)
{
    return ((vap->iv_htflags & htflags) != 0);
}

#define printf  qdf_info

#define CHK_IEEE80211_MSG_BASE(_ctxt, _prefix, _m)          \
    ((_ctxt)->_prefix.category_mask[(_m) >> 3] &            \
     (1 << ((_m) & 0x7)))
#define ieee80211_msg_ic(_ic, _m)               \
    CHK_IEEE80211_MSG_BASE(_ic, ic_print, _m)
#define ieee80211_msg(_vap, _m)                 \
    CHK_IEEE80211_MSG_BASE(_vap, iv_print, _m)
#define ieee80211_msg_dumppkts(_vap) \
        ieee80211_msg(_vap, IEEE80211_MSG_DUMPPKTS)
/*
 * if os does not define the
 * debug temp buf size, define
 * a default size.
 */
#ifndef OS_TEMP_BUF_SIZE
#define OS_TEMP_BUF_SIZE 256
#endif

#define SCANREQ_GIVE_ALL_SCAN_ENTRIES 0
#define SCANREQ_GIVE_ONLY_DESSIRED_SSID 1
#define SCANREQ_GIVE_EXCEPT_DESSIRED_SSID 2

struct scanreq
{
    struct ieee80211req_scan_result *sr;
    size_t space;
    struct ieee80211vap *vap;
    u_int8_t scanreq_type;
};

struct stainforeq
{
    wlan_if_t vap;
    struct ieee80211req_sta_info *si;
    size_t  space;
};


void ieee80211com_note(struct ieee80211com *ic, int msgtype, const char *fmt, ...);
void ieee80211_note(struct ieee80211vap *vap, int msgtype, const char *fmt, ...);
void ieee80211_note_frame(struct ieee80211vap *vap, int msgtype,
                                    struct ieee80211_frame *wh,const  char *fmt, ...);
void ieee80211_note_mac(struct ieee80211vap *vap, int msgtype, u_int8_t *mac, const  char *fmt, ...);
void ieee80211_discard_ie(struct ieee80211vap *vap, int msgtype, const char *type, const char *fmt, ...);
void ieee80211_discard_frame(struct ieee80211vap *vap, int msgtype,
                             const struct ieee80211_frame *wh,
                             const char *type, const char *fmt, ...);
void ieee80211_discard_mac(struct ieee80211vap *vap, int msgtype, u_int8_t *mac,
                           const char *type, const char *fmt, ...);

int ieee80211_set_igtk_key(struct ieee80211vap *vap, u_int16_t keyix, ieee80211_keyval *kval);

int ieee80211_cmac_calc_mic(struct ieee80211_key *key, u_int8_t *aad,
                                  u_int8_t *pkt, u_int32_t pktlen , u_int8_t *mic);

int ieee80211_gmac_calc_mic(struct ieee80211_key *key, u_int8_t *aad,
                                  u_int8_t *pkt, u_int32_t pktlen , u_int8_t *mic,u_int8_t *nounce);

uint16_t ieee80211_getCurrentCountry(struct ieee80211com *ic);

/**
 * ieee80211_build_countryie_all() - Build country IE for all vaps in the radio
 * @ic: Pointer to ieee80211com structure.
 * @ctry_iso: Pointer to country_iso.
 *
 * Return: void.
 */
void ieee80211_build_countryie_all(struct ieee80211com *ic, uint8_t *ctry_iso);

uint16_t ieee80211_getCurrentCountryISO(struct ieee80211com *ic, char *str);

uint32_t ieee80211_get_regdomain(struct ieee80211com *ic);

void ieee80211_set_country_code_assoc_sm(void *data);

int ieee80211_set_ctry_code_continue(struct ieee80211com *ic,
                             bool no_chanchange);

/**
 * ieee80211_regdmn_get_chan_params() - Get channel related parameters.
 * @ic: Pointer to ieee80211com structure.
 * @ch_params: Pointer to channel params structure.
 */
void ieee80211_regdmn_get_chan_params(struct ieee80211com *ic,
        struct ch_params *ch_params);

/**
 * ieee80211_regdmn_get_des_chan_params() - Get channel related parameters.
 * @ic: Pointer to ieee80211vap structure.
 * @ch_params: Pointer to channel params structure.
 */
void ieee80211_regdmn_get_des_chan_params(struct ieee80211vap *vap,
        struct ch_params *ch_params);

/**
 * ieee80211_get_target_channel_mode() - Get phymode from the target channel
 * @ic: Pointer to ieee80211com structure.
 * @ch_params: Pointer to channel params structure.
 *
 * Return: Return phymode.
 */
enum ieee80211_phymode ieee80211_get_target_channel_mode(
        struct ieee80211com *ic,
        struct ch_params *ch_params);

/**
 * ieee80211_dfs_reset - Reset DFS structure members.
 * @ic: Pointer to ic structure
 */
void ieee80211_dfs_reset(struct ieee80211com *ic);

/**
 * ieee80211_dfs_nol_reset - Reset DFS NOL structure data and NOL timers.
 * @ic: Pointer to ic structure
 */
void ieee80211_dfs_nol_reset(struct ieee80211com *ic);
/**
 * ieee80211_dfs_reset_precaclists - Reset DFS preCAC structure.
 * @ic: Pointer to ic structure.
 */
void ieee80211_dfs_reset_precaclists(struct ieee80211com *ic);

/**
 * ieee80211_proc_spoof_success - Process spoof success status from FW.
 * @ic: Pointer to ic structure.
 */
#if defined(WLAN_DFS_PARTIAL_OFFLOAD) && defined(HOST_DFS_SPOOF_TEST)
void ieee80211_proc_spoof_success(struct ieee80211com *ic);
#endif

enum bring_updown_mode {
    ALL_AP_VDEVS,
    ALL_VDEVS,
};

QDF_STATUS wlan_pdev_wait_to_bringdown_all_vdevs(struct ieee80211com *ic,
                                                 enum bring_updown_mode mode);
int ieee80211_bringdown_all_vaps(struct ieee80211com *ic, enum bring_updown_mode mode);
int ieee80211_bringup_all_vaps(struct ieee80211com *ic, enum bring_updown_mode mode);

extern void
ieee80211_set_vht_rates(struct ieee80211com *ic, struct ieee80211vap  *vap);

/* UMAC global category bit -> name translation */
static const struct asf_print_bit_spec ieee80211_msg_categories[] = {
    {IEEE80211_MSG_ACS, "ACS"},
    {IEEE80211_MSG_SCAN_SM, "scan state machine"},
    {IEEE80211_MSG_SCANENTRY, "scan entry"},
    {IEEE80211_MSG_WDS, "WDS"},
    {IEEE80211_MSG_ACTION, "action"},
    {IEEE80211_MSG_ROAM, "STA roaming"},
    {IEEE80211_MSG_INACT, "inactivity"},
    {IEEE80211_MSG_DOTH, "11h"},
    {IEEE80211_MSG_IQUE, "IQUE"},
    {IEEE80211_MSG_WME, "WME"},
    {IEEE80211_MSG_ACL, "ACL"},
    {IEEE80211_MSG_WPA, "WPA/RSN"},
    {IEEE80211_MSG_RADKEYS, "dump 802.1x keys"},
    {IEEE80211_MSG_RADDUMP, "dump radius packet"},
    {IEEE80211_MSG_RADIUS, "802.1x radius client"},
    {IEEE80211_MSG_DOT1XSM, "802.1x state machine"},
    {IEEE80211_MSG_DOT1X, "802.1x authenticator"},
    {IEEE80211_MSG_POWER, "power save"},
    {IEEE80211_MSG_STATE, "state"},
    {IEEE80211_MSG_OUTPUT, "output"},
    {IEEE80211_MSG_SCAN, "scan"},
    {IEEE80211_MSG_AUTH, "authentication"},
    {IEEE80211_MSG_ASSOC, "association"},
    {IEEE80211_MSG_NODE, "node"},
    {IEEE80211_MSG_ELEMID, "element ID"},
    {IEEE80211_MSG_XRATE, "rate"},
    {IEEE80211_MSG_INPUT, "input"},
    {IEEE80211_MSG_CRYPTO, "crypto"},
    {IEEE80211_MSG_DUMPPKTS, "dump packet"},
    {IEEE80211_MSG_DEBUG, "debug"},
    {IEEE80211_MSG_MLME, "mlme"},
};

void IEEE80211_DPRINTF_IC(struct ieee80211com *ic, unsigned verbose, unsigned category, const char *
fmt, ...);

void IEEE80211_DPRINTF_IC_CATEGORY(struct ieee80211com *ic, unsigned category, const char *fmt, ...);

#define ieee80211_dprintf_ic_init(_ic)                                  \
    do {                                                                \
        (_ic)->ic_print.name = "IEEE80211_IC";                          \
        (_ic)->ic_print.verb_threshold = IEEE80211_VERBOSE_LOUD;        \
        (_ic)->ic_print.num_bit_specs =                                 \
            IEEE80211_MSG_LIST_LENGTH(ieee80211_msg_categories);        \
        (_ic)->ic_print.bit_specs = ieee80211_msg_categories;           \
        (_ic)->ic_print.custom_ctxt = NULL;                             \
        (_ic)->ic_print.custom_print = NULL;                            \
        asf_print_mask_set(&(_ic)->ic_print, IEEE80211_DEBUG_DEFAULT, 1); \
        if (IEEE80211_DEBUG_DEFAULT < ASF_PRINT_MASK_BITS) {            \
            asf_print_mask_set(&(_ic)->ic_print, IEEE80211_MSG_ANY, 1); \
        }                                                               \
        asf_print_ctrl_register(&(_ic)->ic_print);                      \
    } while (0)
#define ieee80211_dprintf_ic_deregister(_ic)    \
    asf_print_ctrl_unregister(&(_ic)->ic_print)

void IEEE80211_DPRINTF(struct ieee80211vap *vap, unsigned category, const char *
fmt, ...);

void IEEE80211_DPRINTF_VB(struct ieee80211vap *vap, unsigned verbose, unsigned category, const char *
fmt, ...);

#define ieee80211_dprintf_init(_vap)                                    \
    do {                                                                \
        (_vap)->iv_print.name = "IEEE80211";                            \
        (_vap)->iv_print.verb_threshold = IEEE80211_VERBOSE_LOUD;      \
        (_vap)->iv_print.num_bit_specs =                                \
            IEEE80211_MSG_LIST_LENGTH(ieee80211_msg_categories);        \
        (_vap)->iv_print.bit_specs = ieee80211_msg_categories;          \
        (_vap)->iv_print.custom_ctxt = NULL;                            \
        (_vap)->iv_print.custom_print = NULL;                           \
        asf_print_mask_set(&(_vap)->iv_print, IEEE80211_DEBUG_DEFAULT, 1); \
        if (IEEE80211_DEBUG_DEFAULT < ASF_PRINT_MASK_BITS) {            \
            asf_print_mask_set(&(_vap)->iv_print, IEEE80211_MSG_ANY, 1); \
        }                                                               \
        asf_print_ctrl_register(&(_vap)->iv_print);                     \
    } while (0)
#define ieee80211_dprintf_deregister(_vap)          \
    asf_print_ctrl_unregister(&(_vap)->iv_print)

#define IEEE80211_MSG_LIST_LENGTH(_list) ARRAY_LENGTH(_list)

#if ATH_DEBUG
#if DBG_LVL_MAC_FILTERING
#define IEEE80211_NOTE(_vap, _m, _ni, _fmt, ...) do {                   \
        if (ieee80211_msg(_vap, _m)) {                                   \
            if (!(_vap)->iv_print.dbgLVLmac_on) {                        \
                ieee80211_note_mac(_vap, _m, (_ni)->ni_macaddr, _fmt, ##__VA_ARGS__); \
            } else if ((_ni)->ni_dbgLVLmac_on ||                                      \
                    dbgmac_peer_find((_ni)->ni_macaddr, (_vap)->dbgmac_peer_list)) {\
                ieee80211_note_mac(_vap, _m, (_ni)->ni_macaddr, _fmt, ##__VA_ARGS__); \
            }                                                          \
        }                                                              \
    } while (0)
#define IEEE80211_NOTE_MAC(_vap, _m, _mac, _fmt, ...) do {               \
        if (ieee80211_msg(_vap, _m)) {                                   \
            if (!(_vap)->iv_print.dbgLVLmac_on) {                        \
                ieee80211_note_mac(_vap, _m,  _mac, _fmt, ##__VA_ARGS__);   \
            } else if (dbgmac_peer_find(_mac, (_vap)->dbgmac_peer_list)) { \
                ieee80211_note_mac(_vap, _m,  _mac, _fmt, ##__VA_ARGS__);   \
            }                                                               \
        }                                                                   \
    } while (0)
#else
#define IEEE80211_NOTE(_vap, _m, _ni, _fmt, ...) do {                   \
        if (ieee80211_msg(_vap, _m))                                   \
                ieee80211_note_mac(_vap, _m, (_ni)->ni_macaddr, _fmt, ##__VA_ARGS__); \
    } while (0)
#define IEEE80211_NOTE_MAC(_vap, _m, _mac, _fmt, ...) do {              \
        if (ieee80211_msg(_vap, _m)) 					\
            ieee80211_note_mac(_vap, _m,  _mac, _fmt, ##__VA_ARGS__);        \
    } while (0)
#endif /*DBG_LVL_MAC_FILTERING*/
#define IEEE80211_NOTE_FRAME(_vap, _m, _wh, _fmt, ...) do {             \
        if (ieee80211_msg(_vap, _m))                                    \
            ieee80211_note_frame(_vap, _m, _wh, _fmt, ##__VA_ARGS__);       \
    } while (0)
#else
#define IEEE80211_NOTE(_vap, _m, _ni, _fmt, ...)
#define IEEE80211_NOTE_MAC(_vap, _m, _mac, _fmt, ...)
#define IEEE80211_NOTE_FRAME(_vap, _m, _wh, _fmt, ...)
#endif /* ATH_DEBUG */

#define IEEE80211_DISCARD(_vap, _m, _wh, _type, _fmt, ...) do {         \
    if (ieee80211_msg((_vap), (_m)))                                    \
        ieee80211_discard_frame(_vap, _m, _wh, _type, _fmt, __VA_ARGS__);   \
    } while (0)
#define IEEE80211_DISCARD_MAC(_vap, _m, _mac, _type, _fmt, ...) do {    \
    if (ieee80211_msg((_vap), (_m)))                                    \
        ieee80211_discard_mac(_vap, _m, _mac, _type, _fmt, __VA_ARGS__);    \
    } while (0)
#define IEEE80211_DISCARD_IE(_vap, _m, _type, _fmt, ...) do {           \
    if (ieee80211_msg((_vap), (_m)))                                    \
        ieee80211_discard_ie(_vap, _m,  _type, _fmt, __VA_ARGS__);           \
    } while (0)

#ifdef ATH_CWMIN_WORKAROUND

#else
#define IEEE80211_VI_NEED_CWMIN_WORKAROUND_INIT(_v)
#define VAP_NEED_CWMIN_WORKAROUND(_v) (0)
#endif /* #ifdef ATH_CWMIN_WORKAROUND */

#if ATH_SUPPORT_DFS && ATH_SUPPORT_STA_DFS
#define IEEE80211_IC_DFS_DFSMASTER    0x01 /* DFS Master */
#define IEEE80211_IC_DFS_RADARCAPABLE 0x02 /* Radar Capable */
#define IEEE80211_IC_DFS_CACREQUIRED  0x04 /* CAC (Channel Availability Check) Required */

#define IEEE80211_IC_DFS_DFSMASTER_SET(_ic)         ((_ic)->ic_dfs_flags |=  IEEE80211_IC_DFS_DFSMASTER)
#define IEEE80211_IC_DFS_DFSMASTER_CLR(_ic)         ((_ic)->ic_dfs_flags &= ~IEEE80211_IC_DFS_DFSMASTER)
#define IEEE80211_IC_DFS_DFSMASTER_IS_SET(_ic)      ((_ic)->ic_dfs_flags &   IEEE80211_IC_DFS_DFSMASTER)

#define IEEE80211_IC_DFS_RADARCAPABLE_SET(_ic)      ((_ic)->ic_dfs_flags |=  IEEE80211_IC_DFS_RADARCAPABLE)
#define IEEE80211_IC_DFS_RADARCAPABLE_CLR(_ic)      ((_ic)->ic_dfs_flags &= ~IEEE80211_IC_DFS_RADARCAPABLE)
#define IEEE80211_IC_DFS_RADARCAPABLE_IS_SET(_ic)   ((_ic)->ic_dfs_flags &   IEEE80211_IC_DFS_RADARCAPABLE)

#define IEEE80211_IC_DFS_CACREQUIRED_SET(_ic)       ((_ic)->ic_dfs_flags |=  IEEE80211_IC_DFS_CACREQUIRED)
#define IEEE80211_IC_DFS_CACREQUIRED_CLR(_ic)       ((_ic)->ic_dfs_flags &= ~IEEE80211_IC_DFS_CACREQUIRED)
#define IEEE80211_IC_DFS_CACREQUIRED_IS_SET(_ic)    ((_ic)->ic_dfs_flags &   IEEE80211_IC_DFS_CACREQUIRED)
#endif

#define IEEE80211_MAX_32BIT_UNSIGNED_VALUE          (0xFFFFFFFFU)

#if DBDC_REPEATER_SUPPORT
#define ROOTAP_ACCESS_MASK 0x0F
#define STAVAP_CONNECTION_MASK 0xF0
#endif

/**
 * ieee80211_dfs_rebuild_chan_list_with_non_dfs_channels() - Rebuild channel
 * list with non-DFS channels.
 * @ic: Pointer to ieee80211com structure.
 *
 * Return: 1 or 0 on success else failure.
 */
#if defined(WLAN_DFS_PARTIAL_OFFLOAD) && defined(HOST_DFS_SPOOF_TEST)
int ieee80211_dfs_rebuild_chan_list_with_non_dfs_channels(struct ieee80211com *ic);
#endif /* HOST_DFS_SPOOF_TEST */

void ieee80211_csa_interop_phy_update(struct ieee80211_node *ni, int rx_bw);

typedef enum event_type {
    TYPE_SENT_EVENT = 0,
    TYPE_MSG_RESP   = 1,
} event_type;

struct nl_event_info{
    int type;
    enum event_type ev_type;
    uint32_t ev_len;
    uint32_t pid;
    uint32_t seq;
    uint32_t flags;
    void *event;
};

int nl_ev_fill_info(struct sk_buff *skb, struct net_device *dev,
                    struct nl_event_info *info);

/*
 *  *  Quality of the link
 *   */
struct  ieee80211_quality
{
    uint8_t qual;       /* link quality (%retries, SNR,
                           %missed beacons or better...) */
    uint8_t level;      /* signal level (dBm) */
    uint8_t noise;      /* noise level (dBm) */
    uint8_t updated;    /* Flags to know if updated */
};

/* Bcast Probe response enable bit */
#define WLAN_BCAST_PRB_RSP_ENABLE_BIT 31
#define WLAN_BCAST_PRB_RSP_PERIOD_MASK 0xFFFF

/* Macros for RNR 6Ghz support */

#define RNR_6GHZ_EN 1
#define RNR_6GHZ_DIS 0
#define RNR_6GHZ_DRIVER_MODE 2

#define WLAN_RNR_FRM_MAX 0x7
#define WLAN_RNR_MODE_MAX 2

/* APIs to set, clear and check 6Ghz RNR advertisement
 * frame wise.
 *
 * ic_6ghz_rnr_enable is 8 bit val.
 *
 * Upper 4 bit to select user or driver mode.
 * User mode - 1
 * Driver mode - 0
 *
 * Lower 4 bits are for frame type.
 * B0 - Beacon, B1 - PrbRsp, B2 - FILS/20TUprb
 *
 * If in user mode, driver to peek into frame selection bit
 * to decide if RNR to be added.
 * If in driver mode, Host will decide if RNR is to be added
 *
 */
#define WLAN_EN_USER_RNR_ADV    0x10

#define WLAN_RNR_IN_BCN         0x01
#define WLAN_RNR_IN_PRB         0x02
#define WLAN_RNR_IN_FILS_20TU   0x04

#define WLAN_6GHZ_ADV_USER_SET(old_frm_sel, new_frm_sel) do { \
    old_frm_sel = (old_frm_sel | new_frm_sel); \
    } while (0)

#define WLAN_6GHZ_ADV_USER_CLEAR(old_frm_sel, new_frm_sel) do { \
    old_frm_sel = (old_frm_sel & ~(new_frm_sel)); \
    } while (0)

#define WLAN_6GHZ_RNR_ADV_BCN_IS_SET(val) (val & WLAN_RNR_IN_BCN)
#define WLAN_6GHZ_RNR_ADV_PRB_IS_SET(val) (val & WLAN_RNR_IN_PRB)
#define WLAN_6GHZ_RNR_ADV_FILS_20TU_IS_SET(val) (val & WLAN_RNR_IN_FILS_20TU)

#define WLAN_6GHZ_RNR_USR_MODE_SET(val) do { \
    val = val | (WLAN_EN_USER_RNR_ADV); \
    } while (0)

/* Setting Driver mode is same as clearing user mode bit
 */
#define WLAN_6GHZ_RNR_DRIVER_MODE_SET(val) do { \
    val = val & ~(WLAN_EN_USER_RNR_ADV); \
    } while (0)

#define WLAN_6GHZ_RNR_USR_MODE_IS_SET(val) (val & WLAN_EN_USER_RNR_ADV)

#define WLAN_USER_RNR_BCN_UPDATE_CHECK(val1,val2) \
    ((val1 & WLAN_RNR_IN_BCN) ^ (val2 & WLAN_RNR_IN_BCN))

/**
 * ieee80211_get_rnr_count() - Get rnr count
 * @ic: Pointer to ieee80211com structure.
 *
 * Return: Return rnr count.
 */
u_int32_t ieee80211_get_rnr_count(struct ieee80211com *ic);

/**
 * ieee80211_display_rnr_stats() - display rnr stats
 * @ic: Pointer to ieee80211com structure.
 *
 * Return: void.
 */
void ieee80211_display_rnr_stats(struct ieee80211com *ic);

struct wlan_objmgr_vdev *wlan_find_vdev_from_psocs_by_macaddr(uint8_t* bssid);
void wlan_fetch_inter_soc_rnr_cache(void *rnr);
int wlan_remove_vap_from_6ghz_rnr_cache(struct ieee80211vap *vap);
int wlan_update_6ghz_rnr_cache(struct ieee80211vap *vap, uint8_t vap_change);
uint8_t *ieee80211_add_oob_rnr_ie(uint8_t *frm, struct ieee80211vap *vap, u_int8_t *ssid,
                                  u_int8_t ssid_len, int subtype, bool *rnr_filled);
uint8_t *ieee80211_add_selective_rnr(uint8_t *frm, struct ieee80211vap *vap, u_int8_t *ssid,
                                  u_int8_t ssid_len, int subtype, bool *rnr_filled);
/**
 * ieee80211_add_user_rnr_ie() - Add user rnr data to beacon and
 * probe response frame.
 * @frm - frm buffer
 * @vap - Pointer to struct ieee80211vap
 * @rnr_offset - offset in frame buffer for RNR IE
 * @rnr_filled - Flag to indicate RNR IE_1 is filled
 * @subtype - Frame type
 */
uint8_t *ieee80211_add_user_rnr_ie(uint8_t *frm, struct ieee80211vap *vap, u_int8_t *rnr_offset,
                                   bool rnr_filled, int subtype);
uint8_t *ieee80211_add_6ghz_rnr_ie(struct ieee80211_node *ni, struct ieee80211_beacon_offsets *bo,
                 uint8_t *frm, uint8_t **temp_bo, int subtype, bool is_20tu_prb);

/**
 * ieee80211_reset_user_rnr_list() - Reset uid copy status after
 * IE is filled.
 * @ic - Pointer to struct ieee80211com
 */
void ieee80211_reset_user_rnr_list(struct ieee80211com *ic);

/**
 * ieee80211_user_rnr_free() - Remove the user RNR list
 * @ic - Pointer to struct ieee80211com
 */
void ieee80211_user_rnr_free(struct ieee80211com *ic);

bool ieee80211_is_chan_radar(struct ieee80211com *ic, struct ieee80211_ath_channel *chan);
bool ieee80211_is_chan_nol_history(struct ieee80211com *ic, struct ieee80211_ath_channel *chan);

void heop_param_set(u_int32_t *heop_param, u_int8_t idx, u_int8_t tot_bits,
                 u_int32_t value);
u_int32_t heop_param_get(u_int32_t heop_param, u_int8_t idx, u_int8_t tot_bits);
void heop_bsscolor_set(u_int8_t *bsscolor, u_int8_t idx, u_int8_t tot_bits,
                 u_int32_t value);
u_int32_t heop_bsscolor_get(u_int8_t bsscolor, u_int8_t idx, u_int8_t tot_bits);

#define RNR_MAX_VAP_ADVERTISED 16
#define RNR_TBTT_OFFSET_UNKNOWN 255
/* Space reserved in RNR IE for 6Ghz 16 vaps
 */
#define RESERVED_6GHZ_RNR 212
/* Set bit 0 and bit 1 to enable RNR in
 * beacon and probe resp */
#define IEEE80211_BCN_PROBERSP_RNR_EN 0x3
/* TBTT Information Length subfield value */
#define TBTT_INFO_LEN_1 1
#define TBTT_INFO_LEN_2 2
#define TBTT_INFO_LEN_5 5
#define TBTT_INFO_LEN_6 6
#define TBTT_INFO_LEN_7 7
#define TBTT_INFO_LEN_8 8
#define TBTT_INFO_LEN_9 9
#define TBTT_INFO_LEN_11 11
#define TBTT_INFO_LEN_12 12
#define TBTT_INFO_LEN_13 13
#define TBTT_INFO_LEN_14 14

/* Get IE count from IE size. 257 is the total
 * IE size (IE TAG + IE LEN + 255 payload)
 */
#define GET_RNR_IE_COUNT_FROM_SIZE(val) qdf_ceil(val, 257)

/*
 * mbss_mode_t:
 * Defines the different types of multi-VAP modes, the driver is capable
 * of dynamically switching between.
 */
typedef enum {
    MBSS_MODE_COHOSTED,
    MBSS_MODE_MBSSID_EMA,
} mbss_mode_t;

/*
 * mbss_switch_status_t:
 * Defines the various return values during the MBSSID switch operation
 */
typedef enum {
   MBSS_SWITCH_SUCCESS,
   MBSS_SWITCH_E_RECOVER,
   MBSS_SWITCH_E_NORECOVER_NOMEM,
   MBSS_SWITCH_E_NORECOVER_INVAL,
   MBSS_SWITCH_GOTO_ROLLBACK,
} mbss_switch_status_t;

/*
 * ieee80211_mbss_get_first_ap_vap:
 * Find the first AP VAP capable of beaconing in the list or the first AP
 * VAP which is marked as UP in the netdevice structure.
 *
 * @ic: Pointer to the ic structure
 * @find_first_up: Flag to find the first UP VAP.
 * NOTE: If find_first_up is selected and all VAPs are down, the first VAP
 *       will be returned instead of NULL
 *
 * Return:
 * NULL: Could not find any VAP in the list
 * Else: Found either first UP VAP or the first VAP in the list
 */
struct ieee80211vap *ieee80211_mbss_get_first_ap_vap(struct ieee80211com *ic,
                                         bool find_first_up);

/*
 * ieee80211_mbss_switch_mode:
 * Switch the mode between the different MBSSID modes defined in
 * mbss_mode_t.
 *
 * @ic: Pointer to the ic structure
 * @target_mode: Value of the target_mode
 *
 * Return:
 * MBSS_SWITCH_SUCCESS: Switch completed successfully
 * MBSS_SWITCH_E_RECOVER: Switch failed but recovered successfully
 * MBSS_SWITCH_E_NORECOVER_INVAL: Switch and recovery failed (invalid operation)
 * MBSS_SWITCH_E_NORECOVER_NOMEM: Switch and recover failed (no memory)
 */
mbss_switch_status_t ieee80211_mbss_switch_mode(struct ieee80211com *ic,
                                                mbss_mode_t target_mode);

/*
 * ieee80211_mbss_mode_switch_sanity:
 * Check the sanity for MBSS mode switch in the caller (before switching mode)
 *
 * @ic: Pointer to the ic structure
 * @value: Value of the target mode (typecasted to type mbss_mode_t)
 *
 * Return:
 * QDF_STATUS_SUCCESS: Successful sanity
 * QDF_STATUS_E_INVAL: Sanity failed. Switch not possible
 */
QDF_STATUS ieee80211_mbss_mode_switch_sanity(struct ieee80211com *ic,
                                             uint32_t value);

/*
 * ieee80211_mbss_handle_mode_switch:
 * Wrapper API to handle error handling for mode switching
 *
 * @ic: Pointer to the ic structure
 * @target_mode: Value of the target_mode (typecasted to type mbss_mode_t)
 *
 * Return:
 * QDF_STATUS_SUCCESS: Successful sanity
 * QDF_STATUS_E_INVAL: Sanity failed. Switch not possible
 */
QDF_STATUS ieee80211_mbss_handle_mode_switch(struct ieee80211com *ic,
                                             mbss_mode_t target_mode);

/*
 * ieee80211_get_num_beacon_ap_vaps:
 * API to return the total number of AP capable of beaconing. This includes all
 * HOSTAP VAPs except for special_mode_vap and smart_ap_monitor VAPs
 *
 * @ic: Pointer to the ic structure
 *
 * Return:
 * Number of VAPs created
 */
uint8_t ieee80211_get_num_beacon_ap_vaps(struct ieee80211com *ic);

/*
 * mbss_iter_func_purpose_t:
 * Types of iteration routines for MBSS operation
 */
typedef enum {
    MBSS_ITER_FUNC_PURPOSE_CACHE_INVALIDATION,
    MBSS_ITER_FUNC_PURPOSE_MODE_SWITCH_EMA,
    MBSS_ITER_FUNC_PURPOSE_MODE_SWITCH_COHOSTED,
    MBSS_ITER_FUNC_PURPOSE_INVALID,
} mbss_iter_func_purpose_t;

/*
 * mbss_iter_func_mode_switch_ema_args:
 * Sub-structure for arguments required for EMA mode switch
 */
struct mbss_iter_func_mode_switch_ema_args {
    bool                 is_cache_empty;
    bool                 is_first_vap;
    uint32_t             tx_key;
    struct ieee80211vap *tx_vap;
};

/*
 * mbss_iter_func_args:
 * Structure of arguments used for the MBSS iteration function
 */
struct mbss_iter_func_args {
    mbss_iter_func_purpose_t purpose;
    QDF_STATUS               retval;
    void                     *args;
};

/*
 * ieee80211_check_driver_tx_cmn_ie: Check TxVAP common size for driver-IEs
 *
 * @vap             : VAP handle
 * @box             : Beacon offset for the IE
 * @frm             : frm pointer
 * @remaining_space : Remainig space variable to check remaining space
 * @ftype           : Frame type for which TxVAP common size is checked
 *
 * Return: 0, if enough size available; -ENOMEM, elsewhere
 */
static INLINE int ieee80211_check_driver_tx_cmn_ie(struct ieee80211vap *vap,
      uint8_t *bo_x, uint8_t **frm, int32_t *remaining_space,
      ieee80211_frame_type ftype)
{
#if QCA_SUPPORT_EMA_EXT
  return (IS_MBSSID_EMA_EXT_ENABLED(vap->iv_ic) &&
          ieee80211_check_and_add_tx_cmn_ie(vap, bo_x, frm,
              remaining_space, ftype));
#else
  return 0;
#endif
}

uint16_t ieee80211_get_max_user_rnr_size_allowed(struct ieee80211com *ic);

void ieee80211_intersect_mcsnssmap(struct ieee80211vap *vap,
                                  struct ieee80211_node *ni);
#endif /* end of _ATH_STA_IEEE80211_VAR_H */
