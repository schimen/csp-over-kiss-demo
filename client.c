#include <stdio.h>
#include <stdlib.h>

#include <csp/csp.h>
#include <csp/drivers/usart.h>
#include <csp/interfaces/csp_if_kiss.h>
#include <csp/arch/csp_thread.h>

// server address must be global for client task to read it:
static uint8_t server_address;

CSP_DEFINE_TASK(task_client) {
    // pointer to connection and packet
    csp_packet_t * packet;
	csp_conn_t   * conn;

	while (1) {
        csp_sleep_ms(2000);
        
        // ping server address and show the response time:
        
        printf("Pinging address %d: ", server_address);
		int result = csp_ping(server_address, 100, 1, CSP_O_NONE); // ping with 1 byte
		printf("Ping result %d [ms]\r\n", result);
        
        csp_sleep_ms(2000);
        
        // send a packet to the server:
        
        // get a packet-buffer of 100 bytes
		packet = csp_buffer_get(100);
        // connect to server (on port 10)
        conn = csp_connect(CSP_PRIO_NORM, server_address, 10, 1000, CSP_O_NONE);
		// copy message to packet and set packet length
		const char *msg = "This message was sent over KISS";
		strcpy((char *) packet->data, msg);
		packet->length = strlen(msg);
		// send packet
		csp_send(conn, packet, 1000);
        printf("Sent '%s' to address %d\n", msg, csp_conn_dst(conn));
        // close connection after sending
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
    csp_route_set(CSP_DEFAULT_ROUTE, kiss_iface_p, CSP_NODE_MAC);
}

void csp_start( csp_thread_return_t (* thread_task)(void *), char * name ) {
    printf("Starting %s task\n", name);

    // start router task with 1000 bytes of stack    
    csp_route_start_task(1000, 0);

    // create the thread with our task
    csp_thread_handle_t thread_handle;
    csp_thread_create(thread_task, name, 1000, NULL, 0, &thread_handle);
}

void show_debug() {
    printf("Debug enabled\r\n");
	csp_debug_toggle_level(3);
	csp_debug_toggle_level(4);

	printf("Conn table\r\n");
	csp_conn_print_table();

	printf("Route table\r\n");
	csp_route_print_table();

	printf("Interfaces\r\n");
	csp_route_print_interfaces();
}

int main(int argc, char * argv[]) {
    // parse command line arguments
    uint8_t address;
    char * kiss_device;
    if (argc == 4) {
        address = atoi(argv[1]);
        kiss_device = argv[2];
        server_address = atoi(argv[3]);
    }
    else  {
        printf("Usage:\n"
               " client <client-csp-id> <kiss-device> <server-csp-id>\n");
       exit(1);
    }
    
    csp_setup(address);
    
    // initialize kiss interface (these values must be available for all of main function)
    csp_iface_t kiss_iface;
    csp_kiss_handle_t kiss_driver;
    // callback function when receiving over kiss
    void kiss_callback_func(uint8_t * buf, int len, void * pxTaskWoken) {
        csp_kiss_rx(&kiss_iface, buf, len, pxTaskWoken);
    }
    // setup kiss
    kiss_setup(kiss_device, "KISS interface", &kiss_iface, &kiss_driver, kiss_callback_func);

    csp_start(task_client, "client");
    
    //show_debug(); // uncomment this line for debug info

    while(1) {
    	csp_sleep_ms(1000);
    }
    return 0;
}