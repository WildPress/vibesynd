/* call + tail-call @ 0x00025338 (10B): call 0x254a8 ; jmp 0x28b88 */
extern void keyboard_hook_install(void);
extern void mouse_init_int33(void);
void init_input_subsystem(void)
{
    keyboard_hook_install();
    mouse_init_int33();
}
