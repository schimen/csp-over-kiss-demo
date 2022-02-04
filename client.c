/*
    Client demo program
    source: https://docs.kubos.com/1.2.0/apis/libcsp/csp_docs/example.html
*/

#include <csp/csp.h>
#include <csp/interfaces/csp_if_can.h>

/*---------------------------Setup-------------------------------------------*/

/* CAN configuration struct for SocketCAN interface "can0" */
struct csp_can_config can_conf = {.ifc = "can0"};

/* Init buffer system with 10 packets of maximum 320 bytes each */
csp_buffer_init(10, 320);

/* Init CSP with address 2 */
csp_init(2);

/* Init the CAN interface with hardware filtering */
csp_can_init(CSP_CAN_MASKED, &can_conf)

/* Setup default route to CAN interface */
csp_route_set(CSP_DEFAULT_ROUTE, &csp_can_tx, CSP_HOST_MAC);

/* Start router task with 500 word stack, OS task priority 1 */
csp_route_start_task(500, 1);


/*---------------------------Client------------------------------------------*/
int send_packet(void) {

    /* Get packet buffer for data */
    csp_packet_t *packet = csp_buffer_get(data_size);
    if (packet == NULL) {
        /* Could not get buffer element */
        printf("Failed to get buffer element\\n");
        return -1;
    }

    /* Connect to host HOST, port PORT with regular UDP-like protocol and 1000 ms timeout */
    csp_conn_t *conn = csp_connect(CSP_PRIO_NORM, HOST, PORT, 1000, CSP_O_NONE);
    if (conn == NULL) {
        /* Connect failed */
        printf("Connection failed\\n");
        /* Remember to free packet buffer */
        csp_buffer_free(packet);
        return -1;
    }

    /* Copy message to packet */
    char *msg = "HELLO";
    strcpy(packet->data, msg);

    /* Set packet length */
    packet->length = strlen(msg);

    /* Send packet */
    if (!csp_send(conn, packet, 1000)) {
        /* Send failed */
        printf("Send failed\\n");
        csp_buffer_free(packet);
    }

    /* Close connection */
    csp_close(conn);

    return 0
}