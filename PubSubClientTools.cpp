#include "PubSubClientTools.h"

PubSubClientTools::PubSubClientTools(PubSubClient& _pubSub) {
	pubSub = &_pubSub;
	pubSub->setCallback(mqtt_callback);
};

bool PubSubClientTools::connect(String clientId) {
	char client_char[CLIENTID_BUFFER_SIZE];
	clientId.toCharArray(client_char, CLIENTID_BUFFER_SIZE);
	return pubSub->connect(client_char);
}

bool PubSubClientTools::connect(String clientId, String willTopic, int willQoS, bool willRetain, String willMessage) {
	char client_char[CLIENTID_BUFFER_SIZE];
	char topic_char[TOPIC_BUFFER_SIZE];
	char msg_char[MESSAGE_BUFFER_SIZE];

	clientId.toCharArray(client_char, CLIENTID_BUFFER_SIZE);
	willTopic.toCharArray(topic_char, TOPIC_BUFFER_SIZE);
	willMessage.toCharArray(msg_char, MESSAGE_BUFFER_SIZE);

	return pubSub->connect(client_char, topic_char, willQoS, willRetain, msg_char);
}

void PubSubClientTools::publish(String topic, String message) {
	this->publish(topic, message, false);
}
void PubSubClientTools::publish(String topic, String message, bool retained) {
	topic = prefix_publish+topic;

	char topic_char[TOPIC_BUFFER_SIZE];
	char msg_char[MESSAGE_BUFFER_SIZE];

	topic.toCharArray(topic_char, TOPIC_BUFFER_SIZE);
	message.toCharArray(msg_char, MESSAGE_BUFFER_SIZE);

	pubSub->publish(topic_char, msg_char, retained);
}

void PubSubClientTools::subscribe(String topic, CALLBACK_SIGNATURE) {
	topic = prefix_subscribe+topic;

	char topic_char[TOPIC_BUFFER_SIZE];
	topic.toCharArray(topic_char, TOPIC_BUFFER_SIZE);

	callbackList[callbackCount].topic = topic;
	callbackList[callbackCount].callback = callback;
	callbackCount++;

	pubSub->subscribe(topic_char);
}

void PubSubClientTools::_callback(char* topic_char, byte* payload, unsigned int length) {
	String topic = String(topic_char);
	String message = "";
	for (int i = 0; i < length; i++) {
		message += (char)payload[i];
	}

	for (int i = 0; i < callbackCount; i++) {
		if ( topic == callbackList[i].topic ) {
			(*callbackList[i].callback)(topic,message);
		}
	}
}

int PubSubClientTools::resubscribe() {
	int count = 0;

	pubSub->setCallback(mqtt_callback);

	for (int i = 0; i < callbackCount; i++) {
		char topic_char[TOPIC_BUFFER_SIZE];
		callbackList[i].topic.toCharArray(topic_char, TOPIC_BUFFER_SIZE);

		if ( pubSub->subscribe(topic_char) ) {
			count++;
		}
	}
	return count;
}

void PubSubClientTools::setSubscribePrefix(String prefix) {
	prefix_subscribe = prefix;
}
void PubSubClientTools::setPublishPrefix(String prefix) {
	prefix_publish = prefix;
}
