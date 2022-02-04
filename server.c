/*
    Server demo program
    source: https://docs.kubos.com/1.2.0/apis/libcsp/csp_docs/example.html
*/

#include <csp/csp.h>
#include <csp/interfaces/csp_if_can.h>

/*---------------------------Setup-------------------------------------------*/

/* CAN configuration struct for SocketCAN interface "can0" */
struct csp_can_config can_conf = {.ifc = "can0"};

/* Init buffer system with 10 packets of maximum 320 bytes each */
csp_buffer_init(10, 320);

/* Init CSP with address 1 */
csp_init(1);

/* Init the CAN interface with hardware filtering */
csp_can_init(CSP_CAN_MASKED, &can_conf)

/* Setup default route to CAN interface */
csp_route_set(CSP_DEFAULT_ROUTE, &csp_can_tx, CSP_HOST_MAC);

/* Start router task with 500 word stack, OS task priority 1 */
csp_route_start_task(500, 1);


/*---------------------------Server------------------------------------------*/

void csp_task(void *parameters) {
    /* Create socket without any socket options */
    csp_socket_t *sock = csp_socket(CSP_SO_NONE);

    /* Bind all ports to socket */
    csp_bind(sock, CSP_ANY);

    /* Create 10 connections backlog queue */
    csp_listen(sock, 10);

    /* Pointer to current connection and packet */
    csp_conn_t *conn;
    csp_packet_t *packet;

    /* Process incoming connections */
    while (1) {
        /* Wait for connection, 10000 ms timeout */
        if ((conn = csp_accept(sock, 10000)) == NULL)
            continue;

        /* Read packets. Timout is 1000 ms */
        while ((packet = csp_read(conn, 1000)) != NULL) {
            switch (csp_conn_dport(conn)) {
                case MY_PORT:
                    /* Process packet here */
                default:
                    /* Let the service handler reply pings, buffer use, etc. */
                    csp_service_handler(conn, packet);
                    break;
            }
        }

        /* Close current connection, and handle next */
        csp_close(conn);
    }
}