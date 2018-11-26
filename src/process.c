/*
 * multi process pgboncers
 *
 *
 */
#include <string.h>

#include "bouncer.h"

extern int ischild;
static void sig_parent_handler(int signo)
{
	switch (signo) {
		case SIGCHLD:
			printf("caught SIGCHLD: %d\n", signo);
			break;
		case SIGINT:
			printf("caught SIGINT: %d\n", signo);
			break;
		default:
			printf("caught UnKnown: %d\n", signo);
			break;
	}
}
static void sig_child_handler(int signo)
{
	switch (signo) {
		case SIGCHLD:
			printf("caught child SIGCHLD: %d\n", signo);
			break;
		case SIGINT:
			printf("caught child SIGINT: %d\n", signo);
			break;
		default:
			printf("caught child UnKnown: %d\n", signo);
			break;
	}
}
static void sig_handler(int signo)
{
	if (ischild) {
		sig_child_handler(signo);
	} else {
		sig_parent_handler(signo);
	}
}

static void
pgb_worker_loop (void* data){
	/* Initialize libevent. */
	if (!event_init())
		fatal("event_init() failed");

	while (cf_shutdown < 2) {
		int err;
    	reset_time_cache();

    	err = event_loop(EVLOOP_ONCE);
    	if (err < 0) {
    	    if (errno != EINTR)
    	        log_warning("event_loop failed: %s", strerror(errno));
    	}
    	pam_poll();
    	per_loop_maint();
    	reuse_just_freed_objects();
    	rescue_timers();
    	per_loop_pooler_maint();


    	if (adns)
    	    adns_per_loop(adns);
	}
}
pgb_pid_t
pgb_spawn_process(pgb_spawn_proc_pt proc,
		void *data,
		const char *name)
{

    pgb_pid_t pid = fork();
	pgb_pid_t pgb_worker_pid;
    switch (pid) {

		case -1:
    	    log_error("fork() failed while spawning \"%s\"", name);
    	    return PGB_INVALID_PID;

    	case 0:
			ischild = 1;
    	    pgb_worker_pid = pgb_getpid();
			strncpy(pgb_os_argv[0],"pgbworker",pgb_argc);
			log_info("pgbouncer worker start: %d", pgb_worker_pid);
    	    proc(data);
    	    break;

    	default:
			log_info("in parent");
    	    break;
    }


	log_info("start worker process: %s", name);

	return pid;
}

static void
pgb_start_worker_processes()
{
	int i;
    for (i = 0; i < cf_process_count; i++) {

		log_info("process : %d", i);
        pgb_spawn_process(pgb_worker_loop,
				(void *) (intptr_t) i,
				"pgb-worker");

    }
}

void pgb_master_loop(void) {

	struct sigaction abc;
	abc.sa_handler = sig_handler;
	sigemptyset(&abc.sa_mask);
	abc.sa_flags = 0;

	sigaction(SIGCHLD, &abc, NULL);
	sigaction(SIGINT,  &abc, NULL);
	sigaction(SIGHUP,  &abc, NULL);

	/* XXX
	 * fork new pgbworker*/
	pgb_start_worker_processes();

	while (cf_shutdown < 2) {
		int err;
    	reset_time_cache();

    	err = event_loop(EVLOOP_ONCE);
    	if (err < 0) {
    	    if (errno != EINTR)
    	        log_warning("event_loop failed: %s", strerror(errno));
    	}
    	pam_poll();
    	per_loop_maint();
    	reuse_just_freed_objects();
    	rescue_timers();
    	per_loop_pooler_maint();


    	if (adns)
    	    adns_per_loop(adns);
	}
}
