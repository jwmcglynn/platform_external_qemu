/* This file is autogenerated by tracetool, do not edit. */

#include "qemu/osdep.h"
#include "trace.h"

uint16_t _TRACE_USER_SETUP_FRAME_DSTATE;
uint16_t _TRACE_USER_SETUP_RT_FRAME_DSTATE;
uint16_t _TRACE_USER_DO_RT_SIGRETURN_DSTATE;
uint16_t _TRACE_USER_DO_SIGRETURN_DSTATE;
uint16_t _TRACE_USER_FORCE_SIG_DSTATE;
uint16_t _TRACE_USER_HANDLE_SIGNAL_DSTATE;
uint16_t _TRACE_USER_HOST_SIGNAL_DSTATE;
uint16_t _TRACE_USER_QUEUE_SIGNAL_DSTATE;
uint16_t _TRACE_USER_S390X_RESTORE_SIGREGS_DSTATE;
TraceEvent _TRACE_USER_SETUP_FRAME_EVENT = {
    .id = 0,
    .vcpu_id = TRACE_VCPU_EVENT_NONE,
    .name = "user_setup_frame",
    .sstate = TRACE_USER_SETUP_FRAME_ENABLED,
    .dstate = &_TRACE_USER_SETUP_FRAME_DSTATE 
};
TraceEvent _TRACE_USER_SETUP_RT_FRAME_EVENT = {
    .id = 0,
    .vcpu_id = TRACE_VCPU_EVENT_NONE,
    .name = "user_setup_rt_frame",
    .sstate = TRACE_USER_SETUP_RT_FRAME_ENABLED,
    .dstate = &_TRACE_USER_SETUP_RT_FRAME_DSTATE 
};
TraceEvent _TRACE_USER_DO_RT_SIGRETURN_EVENT = {
    .id = 0,
    .vcpu_id = TRACE_VCPU_EVENT_NONE,
    .name = "user_do_rt_sigreturn",
    .sstate = TRACE_USER_DO_RT_SIGRETURN_ENABLED,
    .dstate = &_TRACE_USER_DO_RT_SIGRETURN_DSTATE 
};
TraceEvent _TRACE_USER_DO_SIGRETURN_EVENT = {
    .id = 0,
    .vcpu_id = TRACE_VCPU_EVENT_NONE,
    .name = "user_do_sigreturn",
    .sstate = TRACE_USER_DO_SIGRETURN_ENABLED,
    .dstate = &_TRACE_USER_DO_SIGRETURN_DSTATE 
};
TraceEvent _TRACE_USER_FORCE_SIG_EVENT = {
    .id = 0,
    .vcpu_id = TRACE_VCPU_EVENT_NONE,
    .name = "user_force_sig",
    .sstate = TRACE_USER_FORCE_SIG_ENABLED,
    .dstate = &_TRACE_USER_FORCE_SIG_DSTATE 
};
TraceEvent _TRACE_USER_HANDLE_SIGNAL_EVENT = {
    .id = 0,
    .vcpu_id = TRACE_VCPU_EVENT_NONE,
    .name = "user_handle_signal",
    .sstate = TRACE_USER_HANDLE_SIGNAL_ENABLED,
    .dstate = &_TRACE_USER_HANDLE_SIGNAL_DSTATE 
};
TraceEvent _TRACE_USER_HOST_SIGNAL_EVENT = {
    .id = 0,
    .vcpu_id = TRACE_VCPU_EVENT_NONE,
    .name = "user_host_signal",
    .sstate = TRACE_USER_HOST_SIGNAL_ENABLED,
    .dstate = &_TRACE_USER_HOST_SIGNAL_DSTATE 
};
TraceEvent _TRACE_USER_QUEUE_SIGNAL_EVENT = {
    .id = 0,
    .vcpu_id = TRACE_VCPU_EVENT_NONE,
    .name = "user_queue_signal",
    .sstate = TRACE_USER_QUEUE_SIGNAL_ENABLED,
    .dstate = &_TRACE_USER_QUEUE_SIGNAL_DSTATE 
};
TraceEvent _TRACE_USER_S390X_RESTORE_SIGREGS_EVENT = {
    .id = 0,
    .vcpu_id = TRACE_VCPU_EVENT_NONE,
    .name = "user_s390x_restore_sigregs",
    .sstate = TRACE_USER_S390X_RESTORE_SIGREGS_ENABLED,
    .dstate = &_TRACE_USER_S390X_RESTORE_SIGREGS_DSTATE 
};
TraceEvent *linux_user_trace_events[] = {
    &_TRACE_USER_SETUP_FRAME_EVENT,
    &_TRACE_USER_SETUP_RT_FRAME_EVENT,
    &_TRACE_USER_DO_RT_SIGRETURN_EVENT,
    &_TRACE_USER_DO_SIGRETURN_EVENT,
    &_TRACE_USER_FORCE_SIG_EVENT,
    &_TRACE_USER_HANDLE_SIGNAL_EVENT,
    &_TRACE_USER_HOST_SIGNAL_EVENT,
    &_TRACE_USER_QUEUE_SIGNAL_EVENT,
    &_TRACE_USER_S390X_RESTORE_SIGREGS_EVENT,
  NULL,
};

static void trace_linux_user_register_events(void)
{
    trace_event_register_group(linux_user_trace_events);
}
trace_init(trace_linux_user_register_events)
