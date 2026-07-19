/* @ 0x00015d98 (20B): call three functions, tail-call the fourth.
 *   call 0x3eda6; call 0x355d8; call 0x3ee21; jmp 0x35538 */
extern void draw_mouse_pointer(void);
extern void blit_backbuffer(void);
extern void end_cursor_draw(void);
extern void bulk_dword_copy(void);
void present_frame(void)
{
    draw_mouse_pointer();
    blit_backbuffer();
    end_cursor_draw();
    bulk_dword_copy();
}
