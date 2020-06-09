#pragma once

/**
 * Scale to be used for all time-related constants
 * 
 */
const int TIME_SCALE = 1;
/**
 * Default size of network buffers
 * 
 */
const int BUFFER_SIZE = 8192;
/**
 * Max buffer size of network buffers
 * 
 */
const int MAX_BUFFER_SIZE = BUFFER_SIZE * 4;
/**
 * Max allowed packet size
 * 
 */
const int MAX_PACKET_SIZE = BUFFER_SIZE * 8;
/**
 * Timeout for the socket recv() operation
 * 
 */
const int SOCKET_TIMEOUT = 10000 * TIME_SCALE;
/**
 * Ping interval for the client
 * 
 */
const int CLIENT_PING_INTERVAL = 5000 * TIME_SCALE;
