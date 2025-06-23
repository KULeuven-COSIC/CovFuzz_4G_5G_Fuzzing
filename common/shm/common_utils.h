#pragma once

#define MSG_MAX_SIZE 4090

#define SHM_CLIENT 0
#define SHM_SERVER 1

#define SHM_NAME "srsRAN_SHM"

#define DIRECTION_UPLINK 0
#define DIRECTION_DOWNLINK 1

typedef enum
{
    SHM_MUTEX_RRC,
    SHM_MUTEX_MAC,
    SHM_MUTEX_MAX
} EnumMutex;

enum CHANNEL {
    BCCH_BCH = 0,
    BCCH_DL_SCH,
    PCCH,
    DL_CCCH,
    UL_CCCH,
    DL_DCCH,
    UL_DCCH,
    UNKNOWN
};

/* radioType */
#define FDD_RADIO 1
#define TDD_RADIO 2

/* Direction */
#define DIRECTION_UPLINK 0
#define DIRECTION_DOWNLINK 1

/* rntiType */
#define WS_NO_RNTI 0
#define WS_P_RNTI 1
#define WS_RA_RNTI 2
#define WS_C_RNTI 3
#define WS_SI_RNTI 4
#define WS_SPS_RNTI 5
#define WS_M_RNTI 6
#define WS_SL_BCH_RNTI 7
#define WS_SL_RNTI 8
#define WS_SC_RNTI 9
#define WS_G_RNTI 10

#define MAC_LTE_RNTI_TAG 0x02
/* 2 bytes, network order */

#define MAC_LTE_UEID_TAG 0x03
/* 2 bytes, network order */

#define MAC_LTE_FRAME_SUBFRAME_TAG 0x04
/* 2 bytes, network order, SFN is stored in 12 MSB and SF in 4 LSB */

#define MAC_LTE_PREDEFINED_DATA_TAG 0x05
/* 1 byte */

#define MAC_LTE_RETX_TAG 0x06
/* 1 byte */

#define MAC_LTE_CRC_STATUS_TAG 0x07
/* 1 byte */

#define MAC_LTE_EXT_BSR_SIZES_TAG 0x08
/* 0 byte */

#define MAC_LTE_SEND_PREAMBLE_TAG 0x09
/* 2 bytes, RAPID value (1 byte) followed by RACH attempt number (1 byte) */

#define MAC_LTE_CARRIER_ID_TAG 0x0A
/* 1 byte */

#define MAC_LTE_PHY_TAG 0x0B
/* variable length, length (1 byte) then depending on direction
   in UL: modulation type (1 byte), TBS index (1 byte), RB length (1 byte),
          RB start (1 byte), HARQ id (1 byte), NDI (1 byte)
   in DL: DCI format (1 byte), resource allocation type (1 byte), aggregation level (1 byte),
          MCS index (1 byte), redundancy version (1 byte), resource block length (1 byte),
          HARQ id (1 byte), NDI (1 byte), TB (1 byte), DL reTx (1 byte) */

#define MAC_LTE_SIMULT_PUCCH_PUSCH_PCELL_TAG 0x0C
/* 0 byte */

#define MAC_LTE_SIMULT_PUCCH_PUSCH_PSCELL_TAG 0x0D
/* 0 byte */

#define MAC_LTE_CE_MODE_TAG 0x0E
/* 1 byte containing mac_lte_ce_mode enum value */

#define MAC_LTE_NB_MODE_TAG 0x0F
/* 1 byte containing mac_lte_nb_mode enum value */

#define MAC_LTE_N_UL_RB_TAG 0x10
/* 1 byte containing the number of UL resource blocks: 6, 15, 25, 50, 75 or 100 */

#define MAC_LTE_SR_TAG 0x11
/* 2 bytes for the number of items, followed by that number of ueid, rnti (2 bytes each) */

/* MAC PDU. Following this tag comes the actual MAC PDU (there is no length, the PDU
   continues until the end of the frame) */
#define MAC_LTE_PAYLOAD_TAG 0x01

#define T_ENB_MAC_UE_DL_SDU (20)
#define T_ENB_MAC_UE_UL_SCHEDULE (21)
#define T_ENB_MAC_UE_UL_SCHEDULE_RETRANSMISSION (22)
#define T_ENB_MAC_UE_UL_PDU (23)
#define T_ENB_MAC_UE_UL_PDU_WITH_DATA (24)
#define T_ENB_MAC_UE_UL_SDU (25)
#define T_ENB_MAC_UE_UL_SDU_WITH_DATA (26)
#define T_ENB_MAC_UE_UL_CE (27)
#define T_ENB_MAC_UE_DL_PDU_WITH_DATA (28)
#define T_ENB_MAC_SCHEDULING_REQUEST (29)
#define T_ENB_MAC_UE_DL_RAR_PDU_WITH_DATA (30)

/* NR */
#define NR_NO_RNTI 0
#define NR_RA_RNTI 2
#define NR_C_RNTI  3
#define NR_SI_RNTI 4

#define W_GNB_MAC_UE_DL_SIB   (201)
#define W_GNB_PHY_INITIATE_RA_PROCEDURE (202)
#define W_GNB_MAC_UE_DL_RAR_PDU_WITH_DATA (203)
#define W_GNB_MAC_UE_DL_PDU_WITH_DATA (204)
#define W_GNB_MAC_UE_UL_PDU_WITH_DATA (205)

#define MAC_NR_PAYLOAD_TAG    0x01
#define MAC_NR_RNTI_TAG       0x02
#define MAC_NR_UEID_TAG       0x03
#define MAC_NR_FRAME_SLOT_TAG 0x07

#define NR_FDD_RADIO 1
#define NR_TDD_RADIO 2



