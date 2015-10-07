#undef TRACE_SYSTEM
#define TRACE_SYSTEM gianfar

#if !defined(_TRACE_GIANFAR_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_GIANFAR_H_

#include <linux/netdevice.h>
#include <linux/tracepoint.h>
#include <linux/ftrace.h>

TRACE_EVENT(gianfar_poll_tx_error,

	TP_PROTO(int q, int skb_dirtytx, void *base,
		 void *bdp, void *lbdp, u32 tstat, u32 tbptr),

	TP_ARGS(q, skb_dirtytx, base, bdp, lbdp, tstat, tbptr),

	TP_STRUCT__entry(
		__field(int,    q)
		__field(int,    skb_dirtytx)
		__field(void *, base)
		__field(void *, bdp)
		__field(void *, lbdp)
		__field(u32,    tstat)
		__field(u32,    tbptr)
		__field(u32,    bdp_lstatus)
		__field(u32,    bdp_bufPtr)
		__field(u32,    lbdp_lstatus)
		__field(u32,    lbdp_bufPtr)
	),

	TP_fast_assign(
		__entry->q = q;
		__entry->skb_dirtytx = skb_dirtytx;
		__entry->base = base;
		__entry->bdp = bdp;
		__entry->bdp_lstatus = ((u32 *)bdp)[0];
		__entry->bdp_bufPtr = ((u32 *)bdp)[1];
		__entry->lbdp = lbdp;
		__entry->lbdp_lstatus = ((u32 *)lbdp)[0];
		__entry->lbdp_bufPtr = ((u32 *)lbdp)[1];
		__entry->tstat = tstat;
		__entry->tbptr = tbptr;
	),

	TP_printk("q:%d  skb_dirtytx %u  base %p  "
		  "bdp %p (%08x %08x)  "
		  "lbdp %p (%08x %08x)  "
		  "tstat:%08x  (TBPTRn:%08x)",
		__entry->q,
		__entry->skb_dirtytx,
		__entry->base,
		__entry->bdp,
		__entry->bdp_lstatus,
		__entry->bdp_bufPtr,
		__entry->lbdp,
		__entry->lbdp_lstatus,
		__entry->lbdp_bufPtr,
		__entry->tstat,
		__entry->tbptr)
);

TRACE_EVENT(gianfar_poll_tx_complete,

	TP_PROTO(int dummy),

	TP_ARGS(dummy),

	TP_STRUCT__entry(
		__field( int,	dummy	)
	),

	TP_fast_assign(
		__entry->dummy		= dummy;
	),

	TP_printk("%s", "")
);

TRACE_EVENT(gianfar_poll_tx_working,

	TP_PROTO(int mask),

	TP_ARGS(mask),

	TP_STRUCT__entry(
		__field(	int,	mask)
	),

	TP_fast_assign(
		__entry->mask = mask;
	),

	TP_printk("mask:%08x",
		__entry->mask)
);

TRACE_EVENT(gianfar_poll_tx_kick,

	TP_PROTO(int mask, int mask2, int mask3),

	TP_ARGS(mask, mask2, mask3),

	TP_STRUCT__entry(
		__field(	int,	mask)
		__field(	int,	mask2)
		__field(	int,	mask3)
	),

	TP_fast_assign(
		__entry->mask = mask;
		__entry->mask2 = mask2;
		__entry->mask3 = mask3;
	),

	TP_printk("mask:%08x mask2:%08x mask3:%08x",
		__entry->mask,
		__entry->mask2,
		__entry->mask3)
);

TRACE_EVENT(gianfar_tx_no_bds,

	    TP_PROTO(int left, int needed),

	    TP_ARGS(left, needed),

	TP_STRUCT__entry(
		__field(	int,	left)
		__field(	int,	needed)
	),

	TP_fast_assign(
		__entry->left = left;
		__entry->needed = needed;
	),

	TP_printk("left:%d  needed:%d",
		  __entry->left, __entry->needed)
);

#endif /* _TRACE_GIANFAR_H_ */

/* This part must be outside protection */
#include <trace/define_trace.h>
