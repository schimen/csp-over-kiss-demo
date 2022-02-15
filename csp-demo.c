#include <stdio.h>
#include <stdlib.h>

#include <csp/csp.h>
#include <csp/drivers/usart.h>
#include <csp/interfaces/csp_if_kiss.h>
#include <csp/drivers/can_socketcan.h>
#include <csp/arch/csp_thread.h>

// array representing all server addresses (fixed size of 10)
static uint8_t server_addresses[10] = {0};

CSP_DEFINE_TASK(task_client) {
    // variable to represent current server-address
    uint8_t server_address;
	while (1) {
        for (uint8_t i = 0; i < 10; i++) {
            server_address = server_addresses[i]; // set server address to next address in array
            if (server_address == 0) continue; // skip loop if there is no server address here

            csp_sleep_ms(1000);
            
            // ping server address and show the response time:
            
            printf("Pinging address %d: ", server_address);
            int result = csp_ping(server_address, 100, 1, CSP_O_NONE); // ping with 1 byte
            printf("Ping result %d [ms]\r\n", result);
            
            csp_sleep_ms(1000);
            
            // send a packet to the server:
            
            // get a packet-buffer of 100 bytes
            csp_packet_t * packet = csp_buffer_get(100);
            // connect to server (on port 10)
            csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, server_address, 10, 1000, CSP_O_NONE);
            if (conn == NULL) { // if connection failed, skip to next
                printf("could not connect to %d\n", server_address);
                csp_buffer_free(packet);
                continue;
            }
            // copy message to packet and set packet length
            const char *msg = "Hello World";
            strcpy((char *) packet->data, msg);
            packet->length = strlen(msg);
            // send packet
            if (csp_send(conn, packet, 1000)){
                printf("Sent '%s' to address %d\n", msg, csp_conn_dst(conn));
                csp_close(conn); // close connection after sending
            }
            else {
                printf("Sending to address %d failed\n", server_address);
                csp_buffer_free(packet);
            }
        }
	}
	return CSP_TASK_RETURN;
}

CSP_DEFINE_TASK(task_server) {
    // create socket, bind it to all ports and create a queue for 10 connections
    csp_socket_t * sock = csp_socket(CSP_SO_NONE);
    csp_bind(sock, CSP_ANY);
    csp_listen(sock, 10);

    // pointer to current connection and packet
    csp_conn_t   * conn;
    csp_packet_t * packet;

    // process incoming connections
    while (1) {
            // wait for connection, 1 s timeout
            if ((conn = csp_accept(sock, 10000)) == NULL)
                    continue;

            // read packets, timeout is 100 ms
            while ((packet = csp_read(conn, 100)) != NULL) {
        switch (csp_conn_dport(conn)) { // check the destination port of the connection
            case 10:
                // when a message is sent to port 10, show the content
                printf("Packet from address %d: '%s'\n", 
                        csp_conn_src(conn), (char *) packet->data);
                csp_buffer_free(packet); // free buffer for next message
                break;
            default:
                // if it was sent to an uninteresting port, call normal service handler
                csp_service_handler(conn, packet);
                break;
        }
            }
            // close current connection
            csp_close(conn);
    }
    return CSP_TASK_RETURN;
}

void csp_setup( uint8_t address ) {
    printf("Setting up CSP with address: %d\n", address);
    
    // csp configuration and initialization
    csp_conf_t csp_conf;
    csp_conf_get_defaults(&csp_conf);
    csp_conf.address = address;
    csp_init(&csp_conf);

    // initialize buffer
    csp_buffer_init(5, 300);
}

void kiss_setup( char              * device
               , char              * name
               , uint8_t             address
               , uint8_t             netmask
               , csp_iface_t       * kiss_iface_p
               , csp_kiss_handle_t * kiss_driver_p
               , usart_callback_t    callback_func ) {
    printf("Setting up KISS interface %s with device: %s\n", name, device);

    // usart configuration
    struct usart_conf conf;
    conf.device = device;
    conf.baudrate = 115200;
    
    usart_init(&conf);

    csp_kiss_init(kiss_iface_p, kiss_driver_p, usart_putc,
                  usart_insert, name);
    
    // function to catch incoming chars. 
    usart_set_callback(callback_func);

    // set route for kiss interface
    csp_rtable_set(address, netmask, kiss_iface_p, CSP_NODE_MAC);
}

void can_setup(char * device, int bitrate, uint8_t address, uint8_t netmask) {

    //setup can
    csp_iface_t * can_iface = csp_can_socketcan_init(device, bitrate, false);

    // set route for can  interface
    csp_rtable_set(address, netmask, can_iface, CSP_NODE_MAC);
}

void csp_start( csp_thread_return_t (* thread_task)(void *), char * name ) {
    printf("Starting %s task\n", name);

    // start router task with 1000 bytes of stack    
    csp_route_start_task(1000, 0);

    // create the thread with our task
    csp_thread_handle_t thread_handle;
    csp_thread_create(thread_task, name, 1000, NULL, 0, &thread_handle);
}

void setup_debug() {
    printf("Debug enabled\r\n");
	csp_debug_toggle_level(3);
	csp_debug_toggle_level(4);

	printf("Route table\r\n");
	csp_route_print_table();

	printf("Interfaces\r\n");
	csp_route_print_interfaces();
}

void parse_addresses_inplace(char * address_string, uint8_t * addresses) {
    // parse comma separated addresses and put them in server_addresses array
    char * pt;
    uint8_t i = 0;
    pt = strtok(address_string,",");
    while (pt != NULL) {
        uint8_t a = atoi(pt);
        addresses[i] = a;
        pt = strtok(NULL, ",");
        ++i;
    }
}

bool is_server() {
    // check if this program is supposed to run as a server
    for (uint8_t i = 0; i < 10; i++) {
        if (server_addresses[i] != 0) return false;
    }
    return true;
}

int main(int argc, char * argv[]) {
    // default values
    uint8_t address = 1;
    char * can_device = NULL;
    char * kiss_device = NULL;

    // parse arguments
    int opt;
    while ((opt = getopt(argc, argv, "a:r:k:c:h")) != -1) {
        switch (opt) {
            case 'a':
                address = atoi(optarg);
                break;
            case 'r':
                parse_addresses_inplace(optarg, server_addresses);
                break;
            case 'k':
                kiss_device = optarg;
                break;
            case 'c':
                can_device = optarg;
                break;
            default:
                printf("Usage:\n"
                       " -a  <address>     local CSP address\n"
                       " -r  <addresses>   run client against server addresses \n"
                       "                   (seperated with comma and maximum 10)\n"
                       " -k  <kiss-device> add KISS device (serial)\n"
                       " -c  <can-device>  add CAN device\n");
                exit(1);
                break;
        }
    }
                                                                           
    csp_setup(address);                                                                      

    // initialize kiss interface (these values must be available for main function)
    csp_iface_t kiss_iface;
    csp_kiss_handle_t kiss_driver;
    void kiss_callback_func(uint8_t * buf, int len, void * pxTaskWoken) {
        // callback function for receiving over kiss 
        // (this function must be available for main function)
        csp_kiss_rx(&kiss_iface, buf, len, pxTaskWoken);
    }
    // setup kiss
    if (kiss_device != NULL) {
        kiss_setup(kiss_device, "KISS interface", 0, 1, &kiss_iface, &kiss_driver, kiss_callback_func);
    }
    //setup can
    if (can_device != NULL) {
        can_setup(can_device, 0, 16, 1);
    }                                                                           

    if (is_server()) { // if there are no addresses in server_addresses
        csp_start(task_server, "server");
    }
    else {
        csp_start(task_client, "client");
    }                                                                                                                                         

    setup_debug(); // show interesting debug info

    while(1) {                                                                               
        csp_sleep_ms(1000);                                                                  
    }                                                                           
    return 0;                                                                                
}    