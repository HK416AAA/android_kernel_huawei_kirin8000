/*
 * Copyright (c) 2020 Huawei Technologies Co., Ltd.
 *
 * Copyright (C) 2016 Richtek Technology Corp.
 * Author: TH <tsunghan_tsai@richtek.com>
 *
 * Power Delivery Policy Engine for UFP
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#define LOG_TAG "[PE_UFP]"

#include "include/pd_core.h"
#include "include/pd_tcpm.h"
#include "include/pd_dpm_core.h"
#include "include/tcpci.h"
#include "include/pd_policy_engine.h"

void hisi_pe_ufp_vdm_get_identity_v800_entry(pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_dpm_ufp_request_id_info_v800(pd_port, pd_event);
}

void hisi_pe_ufp_vdm_send_identity_v800_entry(pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_dpm_ufp_response_id_v800(pd_port, pd_event);
	pd_free_pd_event(pd_port, pd_event);
}

void hisi_pe_ufp_vdm_get_identity_nak_v800_entry(pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_reply_svdm_request_v800_simply(pd_port, pd_event, CMDT_RSP_NAK);
	pd_free_pd_event(pd_port, pd_event);
}

/*
 * [PD2.0] Figure 8-59 UFP Structured VDM Discover SVIDs State Diagram
 */

void hisi_pe_ufp_vdm_get_svids_v800_entry(pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_dpm_ufp_request_svid_info_v800(pd_port, pd_event);
}

void hisi_pe_ufp_vdm_send_svids_v800_entry(pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_dpm_ufp_response_svids_v800(pd_port, pd_event);
	pd_free_pd_event(pd_port, pd_event);
}

void hisi_pe_ufp_vdm_get_svids_nak_v800_entry(pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_reply_svdm_request_v800_simply(pd_port, pd_event, CMDT_RSP_NAK);
	pd_free_pd_event(pd_port, pd_event);
}

void hisi_pe_ufp_vdm_get_modes_v800_entry(pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_dpm_ufp_request_mode_info_v800(pd_port, pd_event);
}

void hisi_pe_ufp_vdm_send_modes_v800_entry(pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_dpm_ufp_response_modes_v800(pd_port, pd_event);
	pd_free_pd_event(pd_port, pd_event);
}

void hisi_pe_ufp_vdm_get_modes_nak_v800_entry(pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_reply_svdm_request_v800_simply(pd_port, pd_event, CMDT_RSP_NAK);
	pd_free_pd_event(pd_port, pd_event);
}

void hisi_pe_ufp_vdm_evaluate_mode_entry_v800_entry(
		pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_dpm_ufp_request_enter_mode_v800(pd_port, pd_event);
}

void hisi_pe_ufp_vdm_mode_entry_ack_v800_entry(pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_reply_svdm_request_v800_simply(pd_port, pd_event, CMDT_RSP_ACK);
	pd_free_pd_event(pd_port, pd_event);
}

void hisi_pe_ufp_vdm_mode_entry_nak_v800_entry(pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_reply_svdm_request_v800_simply(pd_port, pd_event, CMDT_RSP_NAK);
	pd_free_pd_event(pd_port, pd_event);
}

void hisi_pe_ufp_vdm_mode_exit_v800_entry(pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_dpm_ufp_request_exit_mode_v800(pd_port, pd_event);
}

void hisi_pe_ufp_vdm_mode_exit_ack_v800_entry(pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_reply_svdm_request_v800_simply(pd_port, pd_event, CMDT_RSP_ACK);
	pd_free_pd_event(pd_port, pd_event);
}

void hisi_pe_ufp_vdm_mode_exit_nak_v800_entry(pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_reply_svdm_request_v800_simply(pd_port, pd_event, CMDT_RSP_NAK);
	pd_free_pd_event(pd_port, pd_event);
}

void hisi_pe_ufp_vdm_attention_request_v800_entry(
		pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	switch (pd_port->mode_svid) {
#ifdef CONFIG_KIRIN_USB_PD_ALT_MODE_SUPPORT
	case USB_SID_DISPLAYPORT:
		hisi_pd_dpm_ufp_send_dp_attention_v800(pd_port, pd_event);
		break;
#endif

	default:
		pd_send_vdm_attention(pd_port, TCPC_TX_SOP,
			pd_port->mode_svid, pd_port->mode_obj_pos);
		break;
	}

	pd_free_pd_event(pd_port, pd_event);
}

#ifdef CONFIG_KIRIN_USB_PD_ALT_MODE_SUPPORT
void hisi_pe_ufp_vdm_dp_status_update_v800_entry(pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_dpm_ufp_request_dp_status_v800(pd_port, pd_event);
	pd_free_pd_event(pd_port, pd_event);
}

void hisi_pe_ufp_vdm_dp_configure_v800_entry(pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_dpm_ufp_request_dp_config_v800(pd_port, pd_event);
	pd_free_pd_event(pd_port, pd_event);
}

#endif

#ifdef CONFIG_KIRIN_USB_PD_UVDM_SUPPORT

void hisi_pe_ufp_uvdm_recv_v800_entry(pd_port_t *pd_port, pd_event_t *pd_event)
{
	D("\n");
	hisi_pd_dpm_ufp_recv_uvdm_v800(pd_port, pd_event);
	pd_free_pd_event(pd_port, pd_event);
}

#endif
