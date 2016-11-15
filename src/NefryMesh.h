/*

*/

#ifndef __NEFRYMESHPAKET_H__
#define __NEFRYMESHPAKET_H__

//#include "../include/aJson/aJSON.h"
//#include "../include/aJson/stringbuffer.h"
#include<Nefry.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <WiFiServer.h>
class NefryMesh {


	private:
		String _ssid;
		String _ssid_prefix;
		uint32_t _chip_id;

		std::function<String(String)> _handler;

		char _mode;
		WiFiServer  _server;

		WiFiClient  _client;

		void connectToNode(String target_ssid, String message);
		bool exchangeInfo(String message, WiFiClient curr_client);
		bool waitForClient(WiFiClient curr_client, int max_wait);

		void sendMessage(String _message);
		String getMessage(void);

	public:

		/**
		* WiFiMesh Constructor method. Creates a WiFi Mesh Node, ready to be initialised.
		*
		* @chip_id A unique identifier number for the node.
		* @handler The callback handler for dealing with received messages. Takes a string as an argument which
		*          is the string received from another node and returns the string to send back.
		*
		*/
		NefryMesh(uint32_t chip_id, std::function<String(String)> handler,char mode);

		/**
		* Initialises the node.
		*/
		void begin();

		/**
		* Scan for other nodes, and exchange the chosen message with any that are found.
		*
		* @message The message to send to all other nodes.
		*
		*/
		void attemptScan(String message);

		/**
		* If any clients are connected, accept their requests and call the hander function for each one.
		*/
		void acceptRequest();
		
	};

#endif
