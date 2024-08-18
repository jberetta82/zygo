/*
** Jo Sega Saturn Engine
** Copyright (c) 2012-2017, Johannes Fetz (johannesfetz@gmail.com)
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the Johannes Fetz nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL Johannes Fetz BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <jo/jo.h>
#include "hamster.h"

player_params player;
extern jo_sidescroller_physics_params  physics;

inline void     hamster_animation_handling(void)
{

    if (player.type == 1)
{
     if (jo_physics_is_standing(&physics) || physics.is_in_air)
    {
        if (!jo_is_sprite_anim_stopped(player.hamster_anim_id))
            jo_reset_sprite_anim(player.hamster_anim_id);
    }
    else
    {
        if (jo_is_sprite_anim_stopped(player.hamster_anim_id))
            jo_start_sprite_anim_loop(player.hamster_anim_id);

    }

}
   //if (player.type == "spider")
   if (player.type == 2)
{
     if ((jo_physics_is_standing(&physics)|| physics.is_in_air)&& !player.on_ladder)
    {
        if (!jo_is_sprite_anim_stopped(player.spider_anim_id))
            jo_reset_sprite_anim(player.spider_anim_id);
    }
    else
    {
        if (jo_is_sprite_anim_stopped(player.spider_anim_id))
            jo_start_sprite_anim_loop(player.spider_anim_id);

    }

player.frame_timer ++;
    if (player.frame_timer >= SPIDER_BLINK_TIMER)
      {
        player.frame_timer = 0;
        jo_start_sprite_anim(player.spider_blink_anim_id);


      }
      if (jo_is_sprite_anim_stopped(player.spider_blink_anim_id))
            jo_set_sprite_anim_frame(player.spider_blink_anim_id, 0);


        //jo_printf_with_color(0, 0, JO_COLOR_INDEX_Yellow, "> timer: %d  ", player.frame_timer);



}


}

inline void     display_player(void)
{


if (player.health == 0)
		  { 
			//player death anim
			jo_sprite_draw3D2(jo_get_anim_sprite(game.enemy_splat_sprite_id), player.x, player.y, 400);
			jo_reset_sprite_anim(game.enemy_splat_sprite_id);
			
		  }else
		  {

	if (!player.can_be_hurt)
		{
		jo_sprite_enable_gouraud_shading();
		jo_set_gouraud_shading_colors(JO_COLOR_Red, JO_COLOR_Red, JO_COLOR_Red, JO_COLOR_Red);
		}
	
	
	

        if (player.flip_player)
        jo_sprite_enable_horizontal_flip();

            if (player.type == 1)
                jo_sprite_draw3D2(jo_get_anim_sprite(player.hamster_anim_id), player.x, player.y, 400);
            else if (player.type == 2)
               {
                jo_sprite_draw3D_and_rotate(jo_get_anim_sprite(player.spider_anim_id), player.x - 124,player.y - 104, 400, player.rotate_player);
				if (player.rotate_player == 0)
                jo_sprite_draw3D_and_rotate(jo_get_anim_sprite(player.spider_blink_anim_id), player.x - 124,player.y - 104 -4, 380, player.rotate_player);
				else
				jo_sprite_draw3D_and_rotate(jo_get_anim_sprite(player.spider_blink_anim_id), player.x - 124 + 4,player.y - 104, 380, player.rotate_player);

               }
               else if (player.type ==3)
                {if (physics.speed_y >= 0)
                jo_sprite_draw3D2(player.bat_sprite_id, player.x, player.y, 400);
               else
                jo_sprite_draw3D2(player.bat_sprite_flap_id, player.x, player.y, 400);
                }
				else if (player.type ==4)
					{if (physics.speed_y >= 0)
                jo_sprite_draw3D2(player.fish_sprite_id, player.x, player.y, 400);
               else
                jo_sprite_draw3D2(player.fish_sprite_id, player.x, player.y, 400);
                }

        jo_sprite_disable_horizontal_flip();
        player.rotate_player = 0;
		jo_sprite_disable_gouraud_shading();
		
		  }

}

void            load_player_and_enemies(void)
{ 
int hamster_sprite_id;
int spider_sprite_id;
int esplat_id;

		hamster_sprite_id =jo_sprite_add_tga_tileset("TEX", "HAM.TGA", JO_COLOR_Red, HAM_Tileset, 4);
		spider_sprite_id = jo_sprite_add_tga_tileset("TEX","ENEMY.TGA",JO_COLOR_Red,SPIDER_Tileset,14);
		player.bat_sprite_id = spider_sprite_id + 10;
        player.bat_sprite_flap_id = spider_sprite_id + 11;
		player.fish_sprite_id = jo_sprite_add_tga("TEX", "EFISH.TGA", JO_COLOR_Green);
		game.enemy_fish_sprite_id = player.fish_sprite_id;
        player.spider_anim_id = jo_create_sprite_anim(spider_sprite_id, 4, 4);
		game.enemy_spider_anim_id = jo_create_sprite_anim(spider_sprite_id, 4, 4);
		game.enemy_bat_anim_id = jo_create_sprite_anim(spider_sprite_id + 10, 2, 4);
		player.hamster_anim_id = jo_create_sprite_anim(hamster_sprite_id, 4, 4);
        player.spider_blink_anim_id = jo_create_sprite_anim(spider_sprite_id + 4, 6, 4);
		//game.enemy_frog_anim_id = jo_create_sprite_anim(spider_sprite_id + 12, 2, 4);
		game.enemy_frog_sprite_id_1 = spider_sprite_id + 12;
		game.enemy_frog_sprite_id_2 = spider_sprite_id + 13;
		player.frog_sprite_id_1 = spider_sprite_id + 12;
		player.frog_sprite_id_2 = spider_sprite_id + 13;

		esplat_id = jo_sprite_add_tga_tileset("TEX","EDEAD.TGA",JO_COLOR_Green,EDEAD_Tileset,6);
		game.spider_corpse_sprite_id = esplat_id + 4;
		game.bat_corpse_sprite_id = esplat_id + 5;
		game.enemy_splat_sprite_id = jo_create_sprite_anim(esplat_id, 4, 6);

}



inline void     display_hud(void)
{
        jo_sprite_enable_half_transparency();
        if (player.health == 3)
            jo_sprite_draw3D2(player.health_sprite_id_3,10, 182, 100);
        else if (player.health == 2)
            jo_sprite_draw3D2(player.health_sprite_id_2,10, 182, 100);
        else if (player.health == 1)
            jo_sprite_draw3D2(player.health_sprite_id_1,10, 182, 100);
		
		
		jo_sprite_draw3D2(player.gems_sprite_id_1,60, 182, 100);
		jo_sprite_draw3D2(player.clock_sprite_id,60, 208, 100);
		
		
        jo_sprite_disable_half_transparency();


}
/* 5 tiles */
static const jo_tile    HUD_Tileset[] =
{
	{0, 0, 48, 48},
	{48, 0, 48, 48},
	{96, 0, 48, 48},
	{144, 0, 24, 24},
	{144, 24, 24, 24}
};

void            load_hud(void)
{
	int hud_sprite_id;
	player.health = 3;
	player.gems = 0;
	
	hud_sprite_id = jo_sprite_add_tga_tileset("TEX", "HUD.TGA",JO_COLOR_Green,HUD_Tileset,5);
    player.health_sprite_id_1 = hud_sprite_id;
    player.health_sprite_id_2 = hud_sprite_id + 1;
    player.health_sprite_id_3 = hud_sprite_id + 2;
	player.gems_sprite_id_1 = hud_sprite_id + 3;
    player.clock_sprite_id = hud_sprite_id + 4;

}

void        load_powerups(void)

{
    int pup_sprite_id;
	
		pup_sprite_id = jo_sprite_add_tga_tileset("TEX", "PUP.TGA",JO_COLOR_Green,PUP_Tileset,20);
		        
		game.pup_health_anim_id = jo_create_sprite_anim(pup_sprite_id, 3, 4);
		game.pup_super_health_anim_id = jo_create_sprite_anim(pup_sprite_id + 3, 3, 4);
		game.pup_spider_anim_id = jo_create_sprite_anim(pup_sprite_id + 6, 3, 4);
		game.pup_bat_anim_id = jo_create_sprite_anim(pup_sprite_id + 9, 3, 4);
		game.pup_gem_anim_id = jo_create_sprite_anim(pup_sprite_id + 12, 3, 4);
		
		game.stars_anim_id = jo_create_sprite_anim(pup_sprite_id + 15 , 5, 3);
		
		       


}

void		load_waterfall(void)

{
		int water_sprite_id;
				
		water_sprite_id = jo_sprite_add_tga_tileset("TEX", "WATER.TGA",JO_COLOR_Red,WATER_Tileset,13);
		game.wfall_anim_id = jo_create_sprite_anim(water_sprite_id, 4, 6);	
		
		game.wave_anim_id = jo_create_sprite_anim(water_sprite_id + 6, 4, 6);	
		game.wfall_splash_anim_id = jo_create_sprite_anim(water_sprite_id + 4, 2, 6);
		game.player_splash_anim_id = jo_create_sprite_anim(water_sprite_id + 10, 3, 6);
		
		
	
}

void            replace_anim(void)
{
int spider_sprite_id;
int esplat_id;
int water_sprite_id;
		//jo_clear_all_sprite_anim();
		jo_sprite_free_from(game.spider_corpse_sprite_id);    
		jo_sprite_free_from(game.bat_corpse_sprite_id);
		jo_sprite_free_from(game.enemy_fish_sprite_id);
        jo_sprite_free_from(game.enemy_frog_sprite_id_1);
		jo_sprite_free_from(player.fish_sprite_id);        
		jo_sprite_free_from(game.enemy_frog_sprite_id_2);
		jo_sprite_free_from(player.frog_sprite_id_1);
		jo_sprite_free_from(player.frog_sprite_id_2);
		jo_sprite_free_from(game.wfall_anim_id);
		jo_sprite_free_from(game.wave_anim_id);
		jo_sprite_free_from(game.wfall_splash_anim_id);
		jo_sprite_free_from(game.player_splash_anim_id);


		spider_sprite_id = jo_sprite_add_tga_tileset("TEX","ENEMY.TGA",JO_COLOR_Red,SPIDER_Tileset,14);
		player.bat_sprite_id = spider_sprite_id + 10;
        player.bat_sprite_flap_id = spider_sprite_id + 11;
		player.fish_sprite_id = jo_sprite_add_tga("TEX", "EFISH.TGA", JO_COLOR_Green);
		game.enemy_fish_sprite_id = player.fish_sprite_id;
        player.spider_anim_id = jo_replace_sprite_anim(player.spider_anim_id , spider_sprite_id, 4, 4);	
		game.enemy_spider_anim_id = jo_replace_sprite_anim(game.enemy_spider_anim_id , spider_sprite_id, 4, 4);
		//done up to here
		game.enemy_bat_anim_id = jo_replace_sprite_anim(game.enemy_bat_anim_id, spider_sprite_id + 10, 2, 4);
		//player.hamster_anim_id = jo_replace_sprite_anim(player.hamster_anim_id, hamster_sprite_id, 4, 4);
        player.spider_blink_anim_id = jo_replace_sprite_anim(player.spider_blink_anim_id, spider_sprite_id + 4, 6, 4);
		//game.enemy_frog_anim_id = jo_create_sprite_anim(spider_sprite_id + 12, 2, 4);
		game.enemy_frog_sprite_id_1 = spider_sprite_id + 12;
		game.enemy_frog_sprite_id_2 = spider_sprite_id + 13;
		player.frog_sprite_id_1 = spider_sprite_id + 12;
		player.frog_sprite_id_2 = spider_sprite_id + 13;

		esplat_id = jo_sprite_add_tga_tileset("TEX","EDEAD.TGA",JO_COLOR_Green,EDEAD_Tileset,6);
		game.spider_corpse_sprite_id = esplat_id + 4;
		game.bat_corpse_sprite_id = esplat_id + 5;
		game.enemy_splat_sprite_id = jo_replace_sprite_anim(game.enemy_splat_sprite_id, esplat_id, 4, 6);
		
		water_sprite_id = jo_sprite_add_tga_tileset("TEX", "WATER.TGA",JO_COLOR_Red,WATER_Tileset,13);
		game.wfall_anim_id = jo_replace_sprite_anim(game.wfall_anim_id, water_sprite_id, 4, 6);	
		
		game.wave_anim_id = jo_replace_sprite_anim(game.wave_anim_id, water_sprite_id + 6, 4, 6);	
		game.wfall_splash_anim_id = jo_replace_sprite_anim(game.wfall_splash_anim_id, water_sprite_id + 4, 2, 6);
		game.player_splash_anim_id = jo_replace_sprite_anim(game.player_splash_anim_id, water_sprite_id + 10, 3, 6);
		
}



/*
** END OF FILE
*/
