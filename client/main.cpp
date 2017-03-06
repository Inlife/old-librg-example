
// #include <librg/server.h>
#include <uv.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include <RakNetTypes.h>  // MessageID

#include <librg/Shared/BuildVersion.h>
#include <librg/Shared/MessageID.h>

#if WIN32
inline void uv_sleep(int milis) {
    Sleep(milis);
}
#else
#include <unistd.h>
inline void uv_sleep(int milis) {
    usleep(milis * 1000);
}
#endif

/**
 * Main loop ticker
 * @param handle [description]
 */
void timed_loop(uv_timer_t* handle)
{
    printf("hey\n");
}

/**
 * Alloc callback for allocating input memory
 * @param handle         tty handle
 * @param suggested_size suggensted size by uv (65536 in most cases)
 * @param buf            buffer, where data will be written to, and read from by us
 */
static void tty_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
    buf->base = new char[1024];
    buf->len = 1024;
}







bool networkinit = false;
RakNet::RakPeerInterface *peer;
RakNet::Packet *packet;
RakNet::SocketDescriptor sd;








/**
 * On user console message
 * @param stream tty handle
 * @param nread  size of string
 * @param buf    buffer with data
 */
void on_console_message(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
    buf->base[nread] = '\0';

    if (strncmp(buf->base, "conn", 4) == 0) {
        std::string ipAddress = "127.0.0.1";

        peer = RakNet::RakPeerInterface::GetInstance();
        peer->Startup(1, &sd, 1);

        printf("Connecting to %s:%d...\n", ipAddress.c_str(), 27010);
        peer->Connect(ipAddress.c_str(), 27010, 0, 0);

        networkinit = true;
    }
}



void update_network() {
    for (packet=peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
        {
            switch (packet->data[0])
            {
            case ID_REMOTE_DISCONNECTION_NOTIFICATION:
                printf("Another client has disconnected.\n");
                break;
            case ID_REMOTE_CONNECTION_LOST:
                printf("Another client has lost the connection.\n");
                break;
            case ID_REMOTE_NEW_INCOMING_CONNECTION:
                printf("Another client has connected.\n");
                break;
            case ID_NEW_INCOMING_CONNECTION:
                printf("A connection is incoming.\n");
                break;
            case ID_NO_FREE_INCOMING_CONNECTIONS:
                printf("The server is full.\n");
                break;
            case ID_DISCONNECTION_NOTIFICATION:
                printf("We have been disconnected.\n");
                break;
            case ID_CONNECTION_LOST:
                printf("Connection lost.\n");
                break;
            case ID_CONNECTION_REQUEST_ACCEPTED:
                {
                    printf("Our connection request has been accepted.\n");
                    printf("Sending OnClientConnect packet\n");
                    /**
                     * This data-packet is used to validate
                     * game mod compability, and add client to server list
                     *
                     * Data template
                     * @param int NETWORK_PLATFORM_ID
                     * @param int NETWORK_PROTOCOL_VERSION
                     * @param int NETWORK_BUILD_VERSION
                     * @param string Client Name
                     */
                    RakNet::BitStream data;
                    data.Write((RakNet::MessageID)MessageID::CONNECTION_INIT);
                    data.Write(NETWORK_PLATFORM_ID);
                    data.Write(NETWORK_PROTOCOL_VERSION);
                    data.Write(NETWORK_BUILD_VERSION);
                    data.Write("Test Player");
                    data.Write("4555ASDASD4555ASDASD4555");
                    peer->Send(&data, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
                }
                break;
            case MessageID::CONNECTION_ACCEPTED:
                {
                    printf("Successfuly connected to server.\n");
                }
                break;
            default:
                printf("Message with identifier %i has arrived.\n", packet->data[0]);
                break;
            }
        }
}

int main(int argc, char** argv) {
    uv_timer_t timer_req;
    uv_tty_t tty;

    uv_tty_init(uv_default_loop(), &tty, 0, 1);
    uv_tty_set_mode(&tty, UV_TTY_MODE_NORMAL);

    // setup reading callback
    uv_read_start((uv_stream_t*)&tty, tty_alloc, on_console_message);

    // a game ticker
    while (true) {
        uv_sleep(10); // 10 ms gameloop tick

        // game code
        // game code
        // game code
        // game code

        // run libuv one-time iteration
        uv_run(uv_default_loop(), UV_RUN_NOWAIT);
        if (networkinit) {
            update_network();
        }
    }

    if (networkinit) {
        RakNet::RakPeerInterface::DestroyInstance(peer);
    }

    return 0;
}
