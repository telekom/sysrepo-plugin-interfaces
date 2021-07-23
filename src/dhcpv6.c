#include <sysrepo.h>
#include "utils/dhcpv6_client/dhcpv6_client/dhcpv6_client.h"

volatile int exit_application = 0;

int sr_plugin_init_cb(sr_session_ctx_t *session, void **private_data)
{
	int error = SR_ERR_OK;
	sr_conn_ctx_t *connection = NULL;
	sr_session_ctx_t *startup_session = NULL;
	sr_subscription_ctx_t *subscription = NULL;

	*private_data = NULL;

	SRP_LOG_INF("start session to startup datastore");

	connection = sr_session_get_connection(session);

	error = sr_session_start(connection, SR_DS_STARTUP, &startup_session);
	if (error) {
		SRP_LOG_ERR("sr_session_start error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	*private_data = startup_session;

	SRP_LOG_INF("initialize dhcpv6-client");

	/* initialize dhcpv6-client; load datastore if it's empty */
	error = dhcpv6_client_init(session, startup_session);
	if (error) {
		SRP_LOG_ERR("dhcpv6_client_init error");
		goto error_out;
	}

	SRP_LOG_INF("subscribing to 'dhcpv6-client' module changes");

	/* subscribe to dhcpv6-client changes */
	error = dhcpv6_client_subscribe(session, private_data, &subscription);
	if (error) {
		SRP_LOG_ERR("dhcpv6_client_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	SRP_LOG_INF("plugin init done");

    sr_unsubscribe(subscription);

	goto out;

error_out:
	if (subscription != NULL) {
		sr_unsubscribe(subscription);
	}
out:
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

void sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *private_data)
{
	sr_session_ctx_t *startup_session = (sr_session_ctx_t *) private_data;

	exit_application = 1;

	if (startup_session) {
		sr_session_stop(startup_session);
	}

	// empty the client_config_list
	dhcpv6_client_list_free(&client_config_list);

	SRP_LOG_INF("plugin cleanup finished");
}

#ifndef PLUGIN
#include <signal.h>
#include <unistd.h>

static void sigint_handler(__attribute__((unused)) int signum);

int main(void)
{
	int error = SR_ERR_OK;
	sr_conn_ctx_t *connection = NULL;
	sr_session_ctx_t *session = NULL;
	void *private_data = NULL;

	sr_log_stderr(SR_LL_DBG);

	error = sr_connect(SR_CONN_DEFAULT, &connection);
	if (error) {
		SRP_LOG_ERR("sr_connect error (%d): %s", error, sr_strerror(error));
		goto out;
	}

	error = sr_session_start(connection, SR_DS_RUNNING, &session);
	if (error) {
		SRP_LOG_ERR("sr_session_start error (%d): %s", error, sr_strerror(error));
		goto out;
	}

	error = sr_plugin_init_cb(session, &private_data);
	if (error) {
		SRP_LOG_ERR("sr_plugin_init_cb error");
		goto out;
	}

	/* loop until ctrl-c is pressed / SIGINT is received */
	signal(SIGINT, sigint_handler);
	signal(SIGPIPE, SIG_IGN);
	while (!exit_application) {
		sleep(1);
	}

out:
	sr_plugin_cleanup_cb(session, private_data);
	sr_disconnect(connection);

	return error ? -1 : 0;
}

static void sigint_handler(__attribute__((unused)) int signum)
{
	SRP_LOG_INF("Sigint called, exiting...");
	exit_application = 1;
}

#endif
