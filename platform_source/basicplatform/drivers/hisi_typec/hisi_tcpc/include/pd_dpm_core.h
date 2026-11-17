/*
 * Copyright (c) 2020 Huawei Technologies Co., Ltd.
 *
 * Copyright (C) 2016 Richtek Technology Corp.
 * Author: TH <tsunghan_tsai@richtek.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef PD_DPM_CORE_H
#define PD_DPM_CORE_H

#include "include/tcpci.h"
#include "include/pd_core.h"
#include "include/pd_tcpm.h"
#include "include/pd_dpm_pdo_select.h"
#include "include/pd_process_evt.h"

#define DPM_READY_REACTION_BUSY		(0xff)
uint8_t hisi_pd_dpm_get_ready_reaction_v800(pd_port_t *pd_port);

/* ---- MISC ---- */
int hisi_pd_dpm_core_init_v800(pd_port_t *pd_port);
int hisi_pd_dpm_enable_vconn_v800(pd_port_t *pd_port, bool en);
int hisi_pd_dpm_send_sink_caps_v800(pd_port_t *pd_port);
int hisi_pd_dpm_send_source_caps_v800(pd_port_t *pd_port);

/* ---- SNK ---- */
bool pd_dpm_update_tcp_request_v800(pd_port_t *pd_port,
		struct tcp_dpm_pd_request *pd_req);

bool pd_dpm_update_tcp_request_ex_v800(pd_port_t *pd_port,
		struct tcp_dpm_pd_request_ex *pd_req);

bool pd_dpm_update_tcp_request_again_v800(pd_port_t *pd_port);
void pd_dpm_dfp_extract_cable_vdo_v800(pd_port_t *pd_port,
	int vdo_num, int vdo_ext_num);

bool hisi_pd_dpm_send_request_v800(pd_port_t *pd_port, int mv, int ma);
void hisi_pd_dpm_snk_evaluate_caps_v800(pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_snk_standby_power_v800(pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_snk_transition_power_v800(pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_snk_hard_reset_v800(pd_port_t *pd_port, pd_event_t *pd_event);
#ifdef CONFIG_KIRIN_PD_REV30_PPS_SINK
void hisi_pd_dpm_pps_request_heart_beat_v800(pd_port_t *pd_port);
#endif
/* ---- SRC ---- */
void hisi_pd_dpm_src_evaluate_request_v800(pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_src_transition_power_v800(pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_src_hard_reset_v800(pd_port_t *pd_port);
void hisi_pd_dpm_src_inform_cable_vdo_v800(pd_port_t *pd_port, pd_event_t *pd_event);

/* ---- UFP : Evaluate VDM Request ---- */
void hisi_pd_dpm_ufp_request_id_info_v800(pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_ufp_request_svid_info_v800(
		pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_ufp_request_mode_info_v800(
		pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_ufp_request_enter_mode_v800(
		pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_ufp_request_exit_mode_v800(
		pd_port_t *pd_port, pd_event_t *pd_event);

/* ---- UFP : Response VDM Request ---- */
int hisi_pd_dpm_ufp_response_id_v800(pd_port_t *pd_port, pd_event_t *pd_event);
int hisi_pd_dpm_ufp_response_svids_v800(pd_port_t *pd_port, pd_event_t *pd_event);
int hisi_pd_dpm_ufp_response_modes_v800(pd_port_t *pd_port, pd_event_t *pd_event);

/* ---- Defult : Response Hw PD Request ---- */
int hisi_pd_dpm_is_during_dp_v800(pd_port_t *pd_port);

/* ---- UFP : DP Only ---- */
#ifdef CONFIG_KIRIN_USB_PD_ALT_MODE_SUPPORT
int hisi_pd_dpm_ufp_request_dp_status_v800(pd_port_t *pd_port, pd_event_t *pd_event);
int hisi_pd_dpm_ufp_request_dp_config_v800(pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_ufp_send_dp_attention_v800(pd_port_t *pd_port, pd_event_t *pd_event);
#endif

/* ---- DFP : Inform VDM Result ---- */
void hisi_pd_dpm_dfp_inform_id_v800(pd_port_t *pd_port,
		pd_event_t *pd_event, bool ack);
void hisi_pd_dpm_dfp_inform_svids_v800(pd_port_t *pd_port,
		pd_event_t *pd_event, bool ack);
void hisi_pd_dpm_dfp_inform_modes_v800(pd_port_t *pd_port,
		pd_event_t *pd_event, bool ack);
void hisi_pd_dpm_dfp_inform_enter_mode_v800(pd_port_t *pd_port,
		pd_event_t *pd_event, bool ack);
void hisi_pd_dpm_dfp_inform_exit_mode_v800(pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_dfp_inform_attention_v800(pd_port_t *pd_port, pd_event_t *pd_event);

void hisi_pd_dpm_dfp_inform_cable_vdo_v800(pd_port_t *pd_port, pd_event_t *pd_event);

/* ---- DFP : DP Only  ---- */
#ifdef CONFIG_KIRIN_USB_PD_ALT_MODE_DFP_SUPPORT
void hisi_pd_dpm_dfp_send_dp_status_update_v800(
		pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_dfp_inform_dp_status_update_v800(
		pd_port_t *pd_port, pd_event_t *pd_event, bool ack);

void hisi_pd_dpm_dfp_send_dp_configuration_v800(
		pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_dfp_inform_dp_configuration_v800(
		pd_port_t *pd_port, pd_event_t *pd_event, bool ack);
#endif

/* ---- UVDM  ---- */
#ifdef CONFIG_KIRIN_USB_PD_UVDM_SUPPORT
void hisi_pd_dpm_ufp_recv_uvdm_v800(pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_dfp_send_uvdm_v800(pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_dfp_inform_uvdm_v800(pd_port_t *pd_port,
		pd_event_t *pd_event, bool ack);
#endif

/* ---- DRP : Inform PowerCap ---- */
void hisi_pd_dpm_dr_inform_sink_cap_v800(pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_dr_inform_source_cap_v800(pd_port_t *pd_port, pd_event_t *pd_event);

/* ---- DRP : Data Role Swap ---- */
void hisi_pd_dpm_drs_evaluate_swap_v800(pd_port_t *pd_port, uint8_t role);
void hisi_pd_dpm_drs_change_role_v800(pd_port_t *pd_port, uint8_t role);

/* ---- DRP : Power Role Swap ---- */
void hisi_pd_dpm_prs_evaluate_swap_v800(pd_port_t *pd_port, uint8_t role);
void hisi_pd_dpm_prs_turn_off_power_sink_v800(pd_port_t *pd_port);
void hisi_pd_dpm_prs_enable_power_source_v800(pd_port_t *pd_port, bool en);
void hisi_pd_dpm_prs_change_role_v800(pd_port_t *pd_port, uint8_t role);

/* ---- DRP : Vconn Swap ---- */
void hisi_pd_dpm_vcs_evaluate_swap_v800(pd_port_t *pd_port);
void hisi_pd_dpm_vcs_enable_vconn_v800(pd_port_t *pd_port, bool en);

#ifdef CONFIG_KIRIN_PD_REV30
int hisi_pd_dpm_send_alert_v800(pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_inform_alert_v800(pd_port_t *pd_port, pd_event_t *pd_event);
void hisi_pd_dpm_inform_status_v800(pd_port_t *pd_port, pd_event_t *pd_event);
int hisi_pd_dpm_send_source_cap_ext_v800(pd_port_t *pd_port);
#ifdef CONFIG_KIRIN_PD_REV30_STATUS_LOCAL
int hisi_pd_dpm_send_status_v800(pd_port_t *pd_port);
#endif
void hisi_pd_dpm_inform_sink_cap_ext_v800(pd_port_t *pd_port, pd_event_t *pd_event);
int hisi_pd_dpm_send_sink_cap_ext_v800(pd_port_t *pd_port);
#ifdef CONFIG_KIRIN_PD_REV30_BAT_CAP_LOCAL
int hisi_pd_dpm_send_battery_cap_v800(pd_port_t *pd_port);
#endif

#ifdef CONFIG_KIRIN_PD_REV30_BAT_CAP_REMOTE
void hisi_pd_dpm_inform_battery_cap_v800(pd_port_t *pd_port, pd_event_t *pd_event);
#endif

#ifdef CONFIG_KIRIN_PD_REV30_BAT_STATUS_LOCAL
int hisi_pd_dpm_send_battery_status_v800(pd_port_t *pd_port);
#endif

#ifdef CONFIG_KIRIN_PD_REV30_BAT_STATUS_REMOTE
void hisi_pd_dpm_inform_battery_status_v800(pd_port_t *pd_port, pd_event_t *pd_event);
#endif

#ifdef CONFIG_KIRIN_PD_REV30_MFRS_INFO_LOCAL
int hisi_pd_dpm_send_mfrs_info_v800(pd_port_t *pd_port);
#endif

#ifdef CONFIG_KIRIN_PD_REV30_MFRS_INFO_REMOTE
void hisi_pd_dpm_inform_mfrs_info_v800(pd_port_t *pd_port, pd_event_t *pd_event);
#endif

#ifdef CONFIG_KIRIN_PD_REV30_COUNTRY_CODE_REMOTE
void hisi_pd_dpm_inform_country_codes_v800(pd_port_t *pd_port, pd_event_t *pd_event);
#endif

#ifdef CONFIG_KIRIN_PD_REV30_COUNTRY_CODE_LOCAL
int hisi_pd_dpm_send_country_codes_v800(pd_port_t *pd_port, pd_event_t *pd_event);
#endif

#ifdef CONFIG_KIRIN_PD_REV30_COUNTRY_INFO_REMOTE
void hisi_pd_dpm_inform_country_info_v800(pd_port_t *pd_port, pd_event_t *pd_event);
#endif

#ifdef CONFIG_KIRIN_PD_REV30_COUNTRY_INFO_LOCAL
int hisi_pd_dpm_send_country_info_v800(pd_port_t *pd_port);
#endif

#ifdef CONFIG_KIRIN_PD_REV30_PPS_SINK
void hisi_pd_dpm_inform_pps_status_v800(pd_port_t *pd_port, pd_event_t *pd_event);
#endif

void hisi_pd_dpm_inform_not_support_v800(pd_port_t *pd_port);
void hisi_pd_dpm_inform_revision_v800(pd_port_t *pd_port, pd_event_t *pd_event);
int hisi_pd_dpm_send_revision_v800(pd_port_t *pd_port);
#ifdef CONFIG_KIRIN_PD_REV30_SRC_CAP_EXT_REMOTE
void hisi_pd_dpm_inform_source_cap_ext_v800(pd_port_t *pd_port, pd_event_t *pd_event);
#endif
#endif



/* PE : Notify DPM */

int hisi_pd_dpm_notify_pe_startup_v800(pd_port_t *pd_port, pd_event_t *pd_event);
int hisi_pd_dpm_notify_pe_hardreset_v800(pd_port_t *pd_port);
int hisi_pd_dpm_notify_pe_ready_v800(pd_port_t *pd_port, pd_event_t *pd_event);
int pd_dpm_ufp_attempt_discover_cable_v800(pd_port_t *pd_port, pd_event_t *pd_event);
#ifdef CONFIG_USB_PD_DFP_FLOW_DELAY
int hisi_pd_dpm_notify_dfp_delay_done_v800(
		pd_port_t *pd_port, pd_event_t *pd_event);
#endif

#ifdef CONFIG_USB_PD_UFP_FLOW_DELAY
int hisi_pd_dpm_notify_ufp_delay_done_v800(
		pd_port_t *pd_port, pd_event_t *pd_event);
#endif

/* TCPCI - VBUS Control */

static inline int pd_dpm_check_vbus_valid(pd_port_t *pd_port)
{
	return tcpci_check_vbus_valid(pd_port->tcpc_dev);
}

static inline int pd_dpm_sink_vbus(pd_port_t *pd_port, bool en)
{
	int mv = en ? TCPC_VBUS_SINK_5V : TCPC_VBUS_SINK_0V;

	return tcpci_sink_vbus(pd_port->tcpc_dev,
			TCP_VBUS_CTRL_REQUEST, mv, -1);
}

static inline int pd_dpm_source_vbus(pd_port_t *pd_port, bool en)
{
	int mv = en ? TCPC_VBUS_SOURCE_5V : TCPC_VBUS_SOURCE_0V;

	return tcpci_source_vbus(pd_port->tcpc_dev,
			TCP_VBUS_CTRL_REQUEST, mv, -1);
}

/* Mode Operations */

#ifdef CONFIG_KIRIN_USB_PD_ALT_MODE_SUPPORT
#ifdef CONFIG_KIRIN_USB_PD_ALT_MODE_DFP_SUPPORT

bool hisi_dp_dfp_u_notify_discover_id_v800(pd_port_t *pd_port,
		svdm_svid_data_t *svid_data, pd_event_t *pd_event, bool ack);

bool hisi_dp_dfp_u_notify_discover_svid_v800(
		pd_port_t *pd_port, svdm_svid_data_t *svid_data, bool ack);

bool hisi_dp_dfp_u_notify_discover_modes_v800(
		pd_port_t *pd_port, svdm_svid_data_t *svid_data, bool ack);


bool hisi_dp_dfp_u_notify_enter_mode_v800(pd_port_t *pd_port,
		svdm_svid_data_t *svid_data, uint8_t ops, bool ack);

bool hisi_dp_dfp_u_notify_exit_mode_v800(
		pd_port_t *pd_port, svdm_svid_data_t *svid_data, uint8_t ops);

bool hisi_dp_dfp_u_notify_attention_v800(pd_port_t *pd_port,
		svdm_svid_data_t *svid_data, pd_event_t *pd_event);
#endif

void hisi_dp_ufp_u_request_enter_mode_v800(
		pd_port_t *pd_port, svdm_svid_data_t *svid_data, uint8_t ops);

void hisi_dp_ufp_u_request_exit_mode_v800(
		pd_port_t *pd_port, svdm_svid_data_t *svid_data, uint8_t ops);

#ifdef CONFIG_KIRIN_USB_PD_ALT_MODE_DFP_SUPPORT
bool hisi_dp_dfp_u_notify_pe_startup_v800(
		pd_port_t *pd_port, svdm_svid_data_t *svid_data);

int hisi_dp_dfp_u_notify_pe_ready_v800(pd_port_t *pd_port,
		svdm_svid_data_t *svid_data, pd_event_t *pd_event);
#endif

bool hisi_dp_reset_state_v800(pd_port_t *pd_port, svdm_svid_data_t *svid_data);

#endif

#endif /* PD_DPM_CORE_H */
