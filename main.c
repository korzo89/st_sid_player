#include <system/system.h>
#include <gui/screens/screens.h>
#include <sid/sid_player.h>

//----------------------------------------------

int main(void)
{
    system_init();

    sid_player_init();

    gui_show_screen(&screen_file_list);

    while (1)
    {
        gui_process();
    }
}
