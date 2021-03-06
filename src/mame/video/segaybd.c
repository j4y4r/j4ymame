/***************************************************************************

    Sega Y-board hardware

***************************************************************************/

#include "emu.h"
#include "video/segaic16.h"
#include "includes/segas16.h"


/*************************************
 *
 *  Video startup
 *
 *************************************/

VIDEO_START( yboard )
{
	segas1x_state *state = machine.driver_data<segas1x_state>();

	/* compute palette info */
	segaic16_palette_init(0x2000);

	/* allocate a bitmap for the yboard layer */
	state->m_tmp_bitmap = auto_bitmap_ind16_alloc(machine, 512, 512);

	/* initialize the rotation layer */
	segaic16_rotate_init(machine, 0, SEGAIC16_ROTATE_YBOARD, 0x000);

	state->save_item(NAME(*state->m_tmp_bitmap));
}



/*************************************
 *
 *  Video update
 *
 *************************************/

SCREEN_UPDATE_IND16( yboard )
{
	segas1x_state *state = screen.machine().driver_data<segas1x_state>();
	rectangle yboard_clip;

	/* if no drawing is happening, fill with black and get out */
	if (!segaic16_display_enable)
	{
		bitmap.fill(get_black_pen(screen.machine()), cliprect);
		return 0;
	}

	/* draw the yboard sprites */
	yboard_clip.set(0, 511, 0, 511);
	segaic16_sprites_draw(screen, *state->m_tmp_bitmap, yboard_clip, 1);

	/* apply rotation */
	segaic16_rotate_draw(screen.machine(), 0, bitmap, cliprect, state->m_tmp_bitmap);

	/* draw the 16B sprites */
	segaic16_sprites_draw(screen, bitmap, cliprect, 0);
	return 0;
}
