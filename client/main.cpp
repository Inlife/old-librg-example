#include <librg/server.h>
#include <uv.h>
#include <unistd.h>


/**
 * Main loop ticker
 * @param handle [description]
 */
void timed_loop(uv_timer_t* handle)
{
    printf("hey\n");
}


int main(int argc, char** argv) {
    uv_timer_t timer_req;

    uv_timer_init(uv_default_loop(), &timer_req);
    uv_timer_start(&timer_req, timed_loop, 16, 16);

    while (true) {
        // custom code

        uv_run(uv_default_loop(), UV_RUN_NOWAIT);


        // custom code
        sleep(1);
    }

    return 0;
}
