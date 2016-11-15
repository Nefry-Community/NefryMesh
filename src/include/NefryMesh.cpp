/*
  NefryMesh.cpp - Mesh network node
  Sets up a Mesh Node which acts as a router, creating a Mesh Network with other nodes. All information
  is passed in both directions, but it is up to the user what the data sent is and how it is dealt with.

  Copyright (c) 2015 Julian Fell. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/



#include "NefryMesh.h"

#define SSID_PREFIX      		"NefryMesh"
#define SERVER_IP_ADDR			"192.168.4.1"
#define SERVER_PORT				4011

NefryMesh::NefryMesh(uint32_t chip_id, std::function<String(String)> handler, char mode)
  : _server(SERVER_PORT)
{
  _chip_id = chip_id;
  _ssid = String( String( SSID_PREFIX ) + mode + String( _chip_id ) );
  _ssid_prefix = String( SSID_PREFIX );
  _handler = handler;
  _mode = mode;
}

void NefryMesh::begin()
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP( _ssid.c_str() );
  _server.begin();

}

/**
   Wait for a WiFiClient to connect

   @returns: True if the client is ready, false otherwise.

*/
bool NefryMesh::searchClient(WiFiClient _accessClient, int max_wait)
{
  int wait = max_wait;
  while (_accessClient.connected() && !_accessClient.available() && wait--)
    Nefry.ndelay(3);

  /* Return false if the client isn't ready to communicate */
  if (WiFi.status() == WL_DISCONNECTED || !_accessClient.connected())
    return false;

  return true;
}


/**
   Send the supplied message then read back the other node's response
   and pass that to the user-supplied handler.

   @target_ssid The name of the AP the other node has set up.
   @message The string to send to the node.
   @returns: True if the exchange was a succes, false otherwise.

*/
bool NefryMesh::exchangeInfo(String message, WiFiClient _accessClient)
{
  _accessClient.println( message.c_str() );

  if (!searchClient(_accessClient, 1000))
    return false;

  return true;
}

/**
   Connect to the AP at ssid, send them a message then disconnect.

   @target_ssid The name of the AP the other node has set up.
   @message The string to send to the node.

*/
void NefryMesh::connectToNode(String target_ssid, String message)
{
  WiFiClient _accessClient;
  WiFi.begin( target_ssid.c_str() );

  int wait = 1500;
  while ((WiFi.status() == WL_DISCONNECTED) && wait--)
    Nefry.ndelay(3);

  /* If the connection timed out */
  if (WiFi.status() != 3)
    return;

  /* Connect to the node's server */
  if (!_accessClient.connect(SERVER_IP_ADDR, SERVER_PORT))
    return;
  _accessClient.println(message.c_str());
    String request = _hostClient.readStringUntil('\r');
  _hostClient.readStringUntil('\n');
  _handler(request);
  
  //if (!exchangeInfo(message, _accessClient))
    //return;

  _accessClient.stop();
  WiFi.disconnect();
}

void NefryMesh::sendMessage(String message)
{
  if (_mode != 'c') {
    /* Scan for APs */
    int n = WiFi.scanNetworks();

    for (int i = 0; i < n; ++i) {
      String current_ssid = WiFi.SSID(i);
      int index = current_ssid.indexOf( _ssid_prefix + "c");
      uint32_t target_chip_id = (current_ssid.substring(index + _ssid_prefix.length())).toInt();

      /* Connect to any _suitable_ APs which contain _ssid_prefix */
      if (index >= 0 && (target_chip_id < _chip_id)) {

        WiFi.mode(WIFI_STA);
        Nefry.ndelay(100);
        connectToNode(current_ssid, message);
        WiFi.mode(WIFI_AP_STA);
        Nefry.ndelay(100);
      }
    }
  }
}

void NefryMesh::loop()
{
	while (true) {
		_hostClient = _server.available();
		if (!_hostClient)
			break;

		if (!searchClient(_hostClient, 1500)) {
			continue;
		}


		String response = _handler(getMessage());

		sendPacket(response);
	}
}



void NefryMesh::sendPacket(String _message)
{
	/* Send the response back to the client */
	if (_hostClient.connected()){
		_hostClient.println(_message);
	}
}

String NefryMesh::getMessage(void)
{
	String request = _hostClient.readStringUntil('\r');
	_hostClient.readStringUntil('\n');
	if (request.length() <= 2)
		return "";
	return request;

}
