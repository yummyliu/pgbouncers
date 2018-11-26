/*
 * multi process pgboncers
 *
 *
 */
#include <unistd.h>
#define PGB_INVALID_PID  -1
#define pgb_getpid   getpid
#define PGB_MAX_PROCESSES         1024
typedef pid_t pgb_pid_t;

typedef int  pgb_socket_t;
typedef intptr_t        pgb_int_t;
typedef void (*pgb_spawn_proc_pt) (void *data);

typedef struct {
    pgb_pid_t           pid;
    int                 status;
    pgb_socket_t        channel[2];

    pgb_spawn_proc_pt   proc;
    void               *data;
    char               *name;

    unsigned            respawn:1;
    unsigned            just_spawn:1;
    unsigned            detached:1;
    unsigned            exiting:1;
    unsigned            exited:1;
} pgb_process_t;



pgb_int_t        pgb_process_slot;
pgb_socket_t     pgb_channel;
pgb_int_t        pgb_last_process;
pgb_process_t    pgb_processes[PGB_MAX_PROCESSES];


void pgb_master_loop(void);
pgb_pid_t
pgb_spawn_process(pgb_spawn_proc_pt proc,
		void *data,
		const char *name);

