#ifndef __INVASION_WINDOW__
#define __INVASION_WINDOW__

int command_invasion_window(char *text, int len);
void destroy_invasion_window(void);
void update_play_invasion_window(void);
#ifdef JSON_FILES
void save_invasion_window(void);
void load_invasion_window(void);
#endif

#endif