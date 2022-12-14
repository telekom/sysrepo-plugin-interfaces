#include <signal.h>
#include <sysrepo.h>
#include <unistd.h>

#include "plugin.h"
#include "plugin/common.h"

#include <srpc.h>

volatile int exit_application = 0;

static void sigint_handler(__attribute__((unused)) int signum);

int main(void)
{
    int error = SR_ERR_OK;
    sr_conn_ctx_t* connection = NULL;
    sr_session_ctx_t* session = NULL;
    void* private_data = NULL;

    sr_log_stderr(SR_LL_INF);

    /* connect to sysrepo */
    SRPC_SAFE_CALL_ERR(error, sr_connect(SR_CONN_DEFAULT, &connection), out);
    SRPC_SAFE_CALL_ERR(error, sr_session_start(connection, SR_DS_RUNNING, &session), out);

    /* init plugin */
    SRPC_SAFE_CALL_ERR(error, sr_plugin_init_cb(session, &private_data), out);

    /* loop until ctrl-c is pressed / SIGINT is received */
    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, SIG_IGN);
    while (!exit_application) {
        sleep(1);
    }

out:
    /* cleanup plugin */
    sr_plugin_cleanup_cb(session, private_data);
    sr_disconnect(connection);

    return error ? -1 : 0;
}

static void sigint_handler(__attribute__((unused)) int signum)
{
    SRPLG_LOG_INF(PLUGIN_NAME, "Sigint called, exiting...");
    exit_application = 1;
}