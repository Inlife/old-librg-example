#include <uv.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <librg/core/client.h>

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
        librg::network::client("127.0.0.1", 27010);
    }
}

void ontick(double dt)
{
    // printf("tick?\n");
}

int main(int argc, char** argv) {
    uv_tty_t tty;

    uv_tty_init(uv_default_loop(), &tty, 0, 1);
    uv_tty_set_mode(&tty, UV_TTY_MODE_NORMAL);

    // setup reading callback
    uv_read_start((uv_stream_t*)&tty, tty_alloc, on_console_message);

    std::string test = "";

    test.append("==========          CLIENT         ===============\n");
    test.append("==                                              ==\n");
    test.append("==                 ¯\\_(ツ)_/¯                   ==\n");
    test.append("==                                              ==\n");
    test.append("==================================================\n");

    printf("%s\n\n", test.c_str());

    librg::core::set_mode(librg::core::mode_client);
    librg::core::set_tick_cb(ontick);

    librg::entities_initialize();
    librg::events_initialize();
    librg::network_initialize();
    librg::resources_initialize();

    // a game ticker
    while (true) {
        uv_sleep(10); // 10 ms gameloop tick

        // game code
        // game code
        // game code
        // game code

        librg::core::client_tick();
    }

    librg::entities_terminate();
    librg::events_terminate();
    librg::network_terminate();
    librg::resources_terminate();

    librg::core::client_terminate();

    return 0;
}
