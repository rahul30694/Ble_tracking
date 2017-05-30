#include "mqtt.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <mosquitto.h>

// https://mosquitto.org/api/files/mosquitto-h.html#Functions

#define WILL_TOPIC_NAME			"/will"
#define WILL_MESSAGE			"I am Offline"
#define WILL_QOS				2

#define USERNAME 				"abhiarora"
#define PASSWORD 				"123456789"

#define HOST_IP					"52.66.8.146"
#define HOST_PORT				1883
#define KEEPALIVE_TIME_IN_SEC	10

#define TOPIC_NAME 				"/test001"
#define TOPIC_QOS				2

#define TOPIC_PUBLISH_NAME		"/test002"
#define TOPIC_PUBLISH_QOS		2

void on_connect(struct mosquitto *mq, void *obj, int rc)
{
	printf("%s\n", __func__);
}

void on_disconnect(struct mosquitto *mq, void *obj, int rc)
{
	printf("%s\n", __func__);
}

void on_publish(struct mosquitto *mq, void *obj, int mid)
{
	printf("%s\n", __func__);
}

void on_message(struct mosquitto *mq, void *obj, const struct mosquitto_message *message)
{
	printf("%s\n", __func__);
}

void on_unsubscribe(struct mosquitto *mq, void *obj, int mid)
{
	printf("%s\n", __func__);
}


int main(int argc, char *argv[])
{
	int mid=0;

	// This Function is not Thread Safe!
	if (mosquitto_lib_init() != MOSQ_ERR_SUCCESS)
		return -1;

	if (mosquitto_lib_cleanup() != MOSQ_ERR_SUCCESS)
		goto CLEANUP;

	// First parameter is Client ID. For signup, client id should be NULL
	// Second Session is clean_session.	Set to true to instruct the broker to 
	// 			clean all messages and subscriptions on disconnect, false to instruct it to keep them.

	// See mosquitto_user_data_set(mq, NULL)
	struct mosquitto *mq = mosquitto_new(NULL, true, NULL);
	if (mq == NULL)
		goto CLEANUP;


	if (mosquitto_will_set(mq, WILL_TOPIC_NAME, strlen(WILL_MESSAGE), WILL_MESSAGE, WILL_QOS, true) != MOSQ_ERR_SUCCESS)
		goto CLEANUP;
	// mosquitto_will_clear(mq);

	if (mosquitto_username_pw_set(mq, USERNAME, PASSWORD) != MOSQ_ERR_SUCCESS)
		goto DESTROY;


	if (mosquitto_connect(mq, HOST_IP, HOST_PORT, KEEPALIVE_TIME_IN_SEC) != MOSQ_ERR_SUCCESS)
		goto DESTROY;

	// mosquitto_reconnect(mq);

	if (mosquitto_subscribe(mq, NULL, TOPIC_NAME, TOPIC_QOS) != MOSQ_ERR_SUCCESS)
		goto DESTROY;
	// mosquitto_unsubscribe(mq, NULL, TOPIC_NAME);

	mosquitto_connect_callback_set(mq, on_connect);
	mosquitto_disconnect_callback_set(mq, on_disconnect);
	mosquitto_publish_callback_set(mq, on_publish);
	mosquitto_message_callback_set(mq, on_message);
	mosquitto_unsubscribe_callback_set(mq, on_unsubscribe);

	// For QoS 1 and 2. It is the maximum number of infight messages.
	mosquitto_max_inflight_messages_set(mq, 10);

	// Retry for publish message for Qos > 0!
	mosquitto_message_retry_set(mq, 10);


	while (true) {
		if (mosquitto_loop(mq, -1, 1) != MOSQ_ERR_SUCCESS)
			break;
		int t = 5;
		//usleep(100000);
		printf("Sending....\n");
		if (mosquitto_publish(mq, NULL, TOPIC_PUBLISH_NAME, sizeof(t), &t, TOPIC_PUBLISH_QOS, true))
			break;
	}

	if (mosquitto_disconnect(mq) != MOSQ_ERR_SUCCESS)
		goto DESTROY;
DESTROY:
	mosquitto_destroy(mq);
	return 0;

CLEANUP:
	if (mosquitto_lib_cleanup() != MOSQ_ERR_SUCCESS)
		return -1;
	return -1;
}
