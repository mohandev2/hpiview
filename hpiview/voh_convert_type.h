#ifndef __VOH_CONVERT_TYPE_H__
#define __VOH_CONVERT_TYPE_H__

typedef struct VtValNameMap {
	guint		m_value;
	const gchar	*m_name;
	gpointer	data;
} VtValNameMapT;


static const gchar *vt_value_to_string(VtValNameMapT *map,
				       guint value,
				       const gchar *def);



const gchar *vt_convert_boolean(guint b);



const gchar *vt_convert_ctrl_type(guint type);
const gchar *vt_convert_ctrl_output_type(guint type);
const gchar *vt_convert_ctrl_mode(guint mode);
const gchar *vt_convert_ctrl_state_digital(guint state);

const gchar *vt_convert_watchdog_timer_use(guint t_use);
const gchar *vt_convert_watchdog_action(guint action);
const gchar *vt_convert_watchdog_action_event(guint action);
const gchar *vt_convert_watchdog_pretimer_interrupt(guint inter);
const gchar *vt_convert_watchdog_exp_flags(guint flags);
const gchar *vt_convert_domain_capabilities(guint cap);


const gchar *vt_convert_event_state(guint evs);
const gchar *vt_convert_severity(guint sev);

const gchar *vt_convert_status_cond_type(guint type);


const gchar *vt_convert_event_log_overflow_action(guint act);
const gchar *vt_convert_event_type(guint type);
const gchar *vt_convert_resource_event_type(guint type);
const gchar *vt_convert_domain_event_type(guint type);
const gchar *vt_convert_sensor_type(guint type);
const gchar *vt_convert_hs_state(guint state);
const gchar *vt_convert_hpi_sw_event_type(guint type);


const gchar *vt_convert_time(gfloat time);
const gchar *vt_convert_watchdog_counter(guint counter);
gboolean vt_get_dmyhms(gfloat time,
		       guint *year,
		       guint *month,
		       guint *day,
		       guint *hour,
		       guint *min,
		       guint *sec);
gdouble vt_dmyhms_to_time(guint year,
			 guint month,
			 guint day,
			 guint hour,
			 guint min,
			 guint sec);

#endif /* __VOH_CONVERT_TYPE_H__ */

