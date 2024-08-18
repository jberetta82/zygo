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
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED ANDz
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <jo/jo.h>
#include "hamster.h"

#include <string.h>
#include "levels.h"
jo_sidescroller_physics_params  physics;
jo_palette                  image_pal;

static int map_pos_x = WORLD_DEFAULT_X;
static int map_pos_y = WORLD_DEFAULT_Y;
static int mapf_pos_x = WORLD_DEFAULT_X;
static int mapff_pos_x = WORLD_DEFAULT_X;
static int map_max_x = 1504;
static int map_min_x = WORLD_DEFAULT_X;
static int player_def_x = 96;
static int level = 1;
static int select_level = 1;
static int enemy_pos_y = 0;
static int enemy_pos_x = WORLD_DEFAULT_X;
static jo_sound     h_jump;
static jo_sound     h_health;
static jo_sound     h_die;
static jo_sound     h_splash;
static jo_sound     h_flap;
static bool     is_cd_playing = false;
static int bg_x = -184;
static float bg_x2 = -184;
static int bg_width = 184;
static int bg_sprite;
static int bg_sprite2;
static int bg_adj;
static int enemy_dead_x = -184;
static int enemy_dead_y = 0;
static bool water;
static int enemy_min_x;
static int enemy_max_x;
static int enemy_walk_x;
static bool enemy_left;
static int	enemy_total;
static int	powerup_total;
static int powerup_gem_total;
unsigned char TILE_TYPE;
static int level_end;
static int l_select_title;
static int l_select_level_name1;
static int l_select_level_name2;
static int l_select_level_name3;
static int l_select_l1;
static int l_select_l2;
static int l_select_l3;
static int l_select_banner;
static int logo_sprite;
static int bg_direction = 1;
static int splash_x;
static int splash_y;
static int level_title_sprite;
static int level_banner_sprite;



game_params game;
enemy 	enemies[50];
powerup	powerups[200];

void getTime(jo_datetime* currentTime);
unsigned int getSeconds();

//from XL2//
void ztClearText(void)
{
    Uint16 i;
    for (i=0; i<64; ++i)
        jo_nbg2_printf(0, i,"                                                                ");
}

void restart_level(void)
{
int i;

	for(i = 0; i < enemy_total; i++)
	{
		if(enemies[i].type != 3)
		enemies[i].alive = true;
	}
	
	for(i = 0; i < powerup_total; i++)
	{
		if(powerups[i].type == 5)
		powerups[i].used = powerups[i].used_saved;
		else
		powerups[i].used = false;
	}
	
	switch(level)
    {
        case 1:
           player.gems = game.level_1_gems;
            break;
		case 2:
           player.gems = game.level_2_gems;
            break;
	}
	
	
}

static void    reset_demo(void)
{
map_pos_x = WORLD_DEFAULT_X;
mapf_pos_x = WORLD_DEFAULT_X;
mapff_pos_x = WORLD_DEFAULT_X;
map_pos_y = WORLD_DEFAULT_Y;
player.x = 96;
player.y = 128;
player.can_jump = true;
player.flip_player = false;
player.health = 3;
enemy_pos_y = 0;
enemy_pos_x = WORLD_DEFAULT_X;
enemy_dead_x = -184;
enemy_dead_y = 0;
player.type = 1;
player.height = HAMSTER_HEIGHT;
player.height2 = HAMSTER_HEIGHT_2;
player.width = HAMSTER_WIDTH;
player.width2 = HAMSTER_WIDTH_2;
restart_level();
player.can_shoot = true;
player.rotate_player = false;
player.frame_timer = 0;
player.pup_timer = 0;
player.on_ladder = false;
game.pressed_start = false;
game.pressed_up = false;
game.pressed_down = false;
game.pressed_X = false;
player.in_water = false;
enemy_min_x = -200;
enemy_max_x = 200;
enemy_walk_x = 0;
enemy_left = true;
player.can_be_hurt = true;
player.mutate = false;
game.start_time = getSeconds();
game.debug = false;
game.pause_time = 0;
player.can_splash = true;
splash_x = 350;
splash_y = 300;
game.level_start_timer = 0;
game.title_pos_x = 320;
game.title_pos_y = -240;
jo_audio_stop_cd();
is_cd_playing = false;
map_min_x = WORLD_DEFAULT_X;
game.game_frame = 0;

}

void			transition_to_end_level(void)
{
	int i;
		game.game_state = GAMESTATE_END_LEVEL;
		game.end_time = game.current_time;
		for(i = 0; i < powerup_total; i++)
	{
		if(powerups[i].type == 5)
		powerups[i].used_saved = powerups[i].used;
		
	}
		ztClearText();
		jo_nbg2_printf(9, 20, "*");
		
		
}

void			transition_to_level_select(void)
{		

//jo_sprite_free_from(bg_sprite);
		ztClearText();
		jo_clear_background(JO_COLOR_Black);
		bg_sprite = jo_sprite_add_tga("BG", "LSEL.TGA", JO_COLOR_Transparent);
		l_select_title = jo_sprite_add_tga("TEX", "SELT1.TGA", JO_COLOR_Red);
		l_select_level_name1 = jo_sprite_add_tga("TEX", "SELN1.TGA", JO_COLOR_Red);
		l_select_level_name2 = jo_sprite_add_tga("TEX", "SELN2.TGA", JO_COLOR_Red);
		l_select_level_name3 = jo_sprite_add_tga("TEX", "SELN3.TGA", JO_COLOR_Red);
		
		l_select_l1 = jo_sprite_add_tga("BG", "L1.TGA", JO_COLOR_Transparent);
		l_select_l2 = jo_sprite_add_tga("BG", "L2.TGA", JO_COLOR_Transparent);
		l_select_l3 = jo_sprite_add_tga("BG", "L3.TGA", JO_COLOR_Transparent);
		l_select_banner = jo_sprite_add_tga("TEX", "SELBAN.TGA", JO_COLOR_Transparent);
		game.game_state = GAMESTATE_LEVEL_SELECT;
		
}

bool has_vertical_collision(void)
{
    player.can_jump = false;
    if (physics.speed_y < 0.0f)
    {
        physics.is_in_air = true;
        return false;
    }
    int dist = jo_map_per_pixel_vertical_collision(WORLD_MAP_ID, map_pos_x + player.x + player.width2, map_pos_y + player.y + player.height, &TILE_TYPE);
	
    if (dist == JO_MAP_NO_COLLISION || dist > 0 || TILE_TYPE == 2)
        {
            if (dist != JO_MAP_NO_COLLISION && dist < PLAYER_JUMP_PER_PIXEL_TOLERANCE && TILE_TYPE != 2)
            player.can_jump = true;
        physics.is_in_air = true;
        return false;
        }

    if(dist < 0 && jo_is_float_equals_zero(physics.speed_y)) //player going up hill
        player.y += dist;
    player.can_jump = true;
    physics.is_in_air = false;
	//jo_nbg2_printf(0, 5, "> tile type  : %11d",TILE_TYPE);
    return true;



}



static inline void            player_bounceback(void)
{

    if (jo_physics_is_going_on_the_right(&physics))
    {
        player.x -= player.width;
		
    }

    else if (jo_physics_is_going_on_the_left(&physics))
    {
       player.x += player.width;
	  
    }
    else
		player.can_jump = false;
        jo_physics_jump(&physics);
		
		


}

static inline void            player_bounce(void)
{
    
        player.can_jump = false;
        jo_physics_jump(&physics);
			

}

static inline bool      has_horizontal_collision(int width2, int y)
{
    int         next_pixel;
	

    next_pixel = jo_physics_is_going_on_the_right(&physics) ? player.x + 4 :
                 jo_physics_is_going_on_the_left(&physics) ? player.x - 4 :
                 player.x;
    int attr = jo_map_hitbox_detection_custom_boundaries(WORLD_MAP_ID, map_pos_x + next_pixel + width2, map_pos_y + y, 4, 20);
    if (attr == JO_MAP_NO_COLLISION)
        return false;

    if (attr != MAP_TILE_BLOCK_ATTR)
	{if(attr == MAP_TILE_BLOCK_ATTR_CEILING && player.type == 1)
		return true;
	else
        return false;
	}

    return true;
}

static inline bool      has_ceiling_collision(int width, int y)
{
   
    int attr = jo_map_hitbox_detection_custom_boundaries(WORLD_MAP_ID, map_pos_x + player.x , map_pos_y + y, width,0);
    if (attr == JO_MAP_NO_COLLISION)
    {return false;
	}

    if (attr != MAP_TILE_BLOCK_ATTR_CEILING)
    {return false;
	}
	
	if(physics.speed_y >= 0.0f)
    {return false;
	}

return true;
}

static inline bool      ceiling_too_low(int width, int y)
{
   
    int attr = jo_map_hitbox_detection_custom_boundaries(WORLD_MAP_ID, map_pos_x + player.x , map_pos_y + y, width,0);
    if (attr == JO_MAP_NO_COLLISION)
        return false;

    if (attr != MAP_TILE_BLOCK_ATTR_CEILING)
        return false;
	
	//if(physics.speed_y >= 0.0f)
   // return false;

return true;
}

static inline bool		has_water_collision(void)
{
	if (water)
	{
		if (!jo_map_hitbox_detection_custom_boundaries(MAP_WATER_ID, map_pos_x + player.width2, map_pos_y + player.y, 4, 20))
			return true;
		else
		return false;
	}
	else
	return false;
	
}


void player_collision_handling(void)
{
    if (has_vertical_collision())

        physics.speed_y = 0.0f;
    else
    {

        if (!player.on_ladder)
        {jo_physics_apply_gravity(&physics);
        player.y += physics.speed_y;
        }
		
		if (has_ceiling_collision(player.width,player.y))
	{physics.speed_y = 0.0f;
		jo_physics_apply_gravity(&physics);
	player.y += physics.speed_y;

	}



    }
	
	
	
	 
	
    /*jo_nbg2_printf(0, 0, "> physics speed: %d  ", physics.speed_y);*/

    if (has_horizontal_collision(player.width2,player.y))
    {
        if (player.type == 2 && jo_is_pad1_key_pressed(JO_KEY_RIGHT))
        {
            player.on_ladder = true;
            player.rotate_player = -90;
            map_pos_y -= physics.speed;
            enemy_pos_y += physics.speed;
        }
        else if (player.type == 2 && jo_is_pad1_key_pressed(JO_KEY_LEFT))
        {   player.on_ladder = true;
            player.rotate_player = 90;
            map_pos_y += physics.speed;
            enemy_pos_y -= physics.speed;
        }

            else
            {physics.speed = 0.0f;

            }



        }

    else if (physics.speed > 0.0f && jo_is_pad1_key_pressed(JO_KEY_RIGHT) && (map_pos_x < map_max_x && player.x == player_def_x))
        {map_pos_x += physics.speed;
        mapf_pos_x += physics.speed + 1;
        mapff_pos_x += physics.speed + 2;
		player.on_ladder = false;
        {if (bg_x == -184 )
            bg_x = 0;
            else
                bg_x -=1;
        }
		
		{if (bg_x2 == -184 )
            bg_x2 = 0;
            else
                bg_x2 -=0.5;
        }
		
        }
    else if (physics.speed > 0.0f && jo_is_pad1_key_pressed(JO_KEY_RIGHT) && ((map_pos_x >= map_max_x && player.x < 300)|| player.x < player_def_x ))
	{ player.x += physics.speed;
		player.on_ladder = false;
	}

    else if (physics.speed < 0.0f && jo_is_pad1_key_pressed(JO_KEY_LEFT) && (map_pos_x > map_min_x && player.x == player_def_x ))
        {map_pos_x += physics.speed;
        mapf_pos_x += physics.speed - 1;
        mapff_pos_x += physics.speed - 2;
		player.on_ladder = false;
        {if (bg_x == 0 )
            bg_x = -184;
            else
                bg_x +=1;
        }
		{if (bg_x2 == 0 )
            bg_x2 = -184;
            else
                bg_x2 +=0.5;
        }
		
		
		
        }
    else if (physics.speed < 0.0f && jo_is_pad1_key_pressed(JO_KEY_LEFT) && (map_pos_x <= map_min_x || player.x > player_def_x)&& player.x >= 0)
	{ player.x += physics.speed;
	player.on_ladder = false;
	}

    else
        {physics.speed = 0.0f;
        player.on_ladder = false;}
		
	if (has_water_collision())
	{
		if(player.type<4)
		{player.type = 1;
		player.height = HAMSTER_HEIGHT;
		player.height2 = HAMSTER_HEIGHT_2;
        player.width = HAMSTER_WIDTH;
        player.width2 = HAMSTER_WIDTH_2;
        player.can_shoot = true;
		}
		player.in_water = true;
		if(physics.speed_y > 0)
		physics.speed_y -=0.18f;
		else if (physics.speed_y < 0)
		physics.speed_y +=0.2f;
		
	}
	else
	{	
	player.in_water = false;
	//player.can_shoot = true;
	}
	

}

void 		stop_sounds()
{
jo_audio_stop_sound(&h_health);
jo_audio_stop_sound(&h_die);
jo_audio_stop_sound(&h_splash);
jo_audio_stop_sound(&h_jump);
jo_audio_stop_sound(&h_flap);

	
}

static inline void     camera_handling(int prev_y)
{
    int         delta;

    delta = JO_ABS(player.y - prev_y);
	
	
    if (player.y > 100)
    {
        map_pos_y += delta;
        player.y -= delta;
        enemy_pos_y -= delta;
    }
    else if (player.y < 50)
    {
        map_pos_y -= delta;
        player.y += delta;
        enemy_pos_y += delta;
    }
}


// CREATE A NEW POWERUP 
void		create_powerup(powerup* power,int type,int x,int y, int lev) 
{
		power->used = false;
		power->used_saved = false;
		power->type = type;
		power->x = x;
		power->y = y;
		power->lev = lev;
		
}

// DISPLAY POWERUP
void		draw_powerup(powerup* go)
{
	if(go->type == 3 && go->used == true && player.mutate == false)
				{
					go->used = false;
				}
	if(go->type == 4 && go->used == true && player.mutate == false)
				{
					go->used = false;
				}
	
	if (go->x - map_pos_x > -50 && go->x - map_pos_x <350 && go->y - map_pos_y > -50 && go->y - map_pos_y <270)
	{
	
	if (!go->used && go->lev == level)
	{
			//health powerup
			if(go->type == 1)
			{
		
			jo_sprite_draw3D2(jo_get_anim_sprite(game.pup_health_anim_id),go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y, 400);
			jo_start_sprite_anim_loop(game.pup_health_anim_id);

			if (jo_hitbox_detection_custom_boundaries(game.pup_health_anim_id,go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y,player.x + player.width2,player.y + player.height2, 16,16) && player.health < 3)
				{
				stop_sounds();
				jo_audio_play_sound_on_channel(&h_health, 0);
				player.health +=1;
				//powerupused
				go->used = true;
				jo_reset_sprite_anim(game.stars_anim_id);
			   }
			}
			
			//Super health powerup
			if(go->type == 2)
			{
			jo_sprite_draw3D2(jo_get_anim_sprite(game.pup_super_health_anim_id),go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y, 400);
			jo_start_sprite_anim_loop(game.pup_super_health_anim_id);
													  
			if (jo_hitbox_detection_custom_boundaries(game.pup_super_health_anim_id,go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y,player.x + player.width2,player.y + player.height2, 16,16) && player.health < 3)
				{
				stop_sounds();
				jo_audio_play_sound_on_channel(&h_health, 0);
				player.health =3;
				//powerupused
				go->used = true;
				jo_reset_sprite_anim(game.stars_anim_id);
				}
			}
			
			//Spider Powerup - require spider ability type 6
			if(go->type == 3 && player.spider_ability)
			{
				
		
			jo_sprite_draw3D2(jo_get_anim_sprite(game.pup_spider_anim_id),go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y, 400);
			jo_start_sprite_anim_loop(game.pup_spider_anim_id);

			if (jo_hitbox_detection_custom_boundaries(game.pup_spider_anim_id,go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y,player.x + player.width2,player.y + player.height2, 16,16) && player.type != 2)
				{
				stop_sounds();
				jo_audio_play_sound_on_channel(&h_health, 0);
				player.pup_timer = 0;
				player.mutate = true;
				player.type = 2;
				player.height = SPIDER_HEIGHT;
				player.height2 = SPIDER_HEIGHT_2;
				player.width = SPIDER_WIDTH;
				player.width2 = SPIDER_WIDTH_2;
				player.can_shoot = false;
				//powerupused
				go->used = true;
				jo_reset_sprite_anim(game.stars_anim_id);
				}
			}
			
			//Bat Powerup - require bat ability type 7
			if(go->type == 4 && player.bat_ability)
			{
			
			jo_sprite_draw3D2(jo_get_anim_sprite(game.pup_bat_anim_id),go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y, 400);
			jo_start_sprite_anim_loop(game.pup_bat_anim_id);

			if (jo_hitbox_detection_custom_boundaries(game.pup_bat_anim_id,go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y,player.x + player.width2,player.y + player.height2, 16,16) && player.type != 3)
			{
				stop_sounds();
				jo_audio_play_sound_on_channel(&h_health, 0);
				player.type = 3;
				player.pup_timer = 0;
				player.mutate = true;
				player.height = BAT_HEIGHT;
				player.height2 = BAT_HEIGHT_2;
				player.width = BAT_WIDTH;
				player.width2 = BAT_WIDTH_2;
				player.can_shoot = false;
				//powerupused
				go->used = true;
				jo_reset_sprite_anim(game.stars_anim_id);
				}
			}
			
			
				
			
			//gem powerup
			if(go->type == 5)
			{
		
			jo_sprite_draw3D2(jo_get_anim_sprite(game.pup_gem_anim_id),go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y, 400);
			jo_start_sprite_anim_loop(game.pup_gem_anim_id);

			if (jo_hitbox_detection_custom_boundaries(game.pup_gem_anim_id,go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y,player.x + player.width2,player.y + player.height2, 16,16) && player.gems < 25)
				{
				stop_sounds();
				jo_audio_play_sound_on_channel(&h_health, 0);
				player.gems +=1;
				//powerupused
				go->used = true;
				jo_reset_sprite_anim(game.stars_anim_id);
			   }
			}
			
			//Spider ability
			if(go->type == 6 && !player.spider_ability)
			{
				
			jo_sprite_change_sprite_scale(2);
			jo_sprite_draw3D2(jo_get_anim_sprite(game.pup_spider_anim_id),go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y, 400);
			jo_start_sprite_anim_loop(game.pup_spider_anim_id);
			jo_sprite_restore_sprite_scale();
			
			if (jo_hitbox_detection_custom_boundaries(game.pup_spider_anim_id,go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y,player.x + player.width2,player.y + player.height2, 16,16) && player.type != 2)
				{
				stop_sounds();
				jo_audio_play_sound_on_channel(&h_health, 0);
		
				//powerupused
				go->used = true;
				player.spider_ability = true;
				jo_reset_sprite_anim(game.stars_anim_id);
				//help_text = 1;
				//game.game_state = GAMESTATE_HELP_TEXT;
				}
			}
			
				//Bat ability
			if(go->type == 7 && !player.bat_ability)
			{
				
			jo_sprite_change_sprite_scale(2);
			jo_sprite_draw3D2(jo_get_anim_sprite(game.pup_bat_anim_id),go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y, 400);
			jo_start_sprite_anim_loop(game.pup_bat_anim_id);
			jo_sprite_restore_sprite_scale();
			
			if (jo_hitbox_detection_custom_boundaries(game.pup_bat_anim_id,go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y,player.x + player.width2,player.y + player.height2, 16,16) && player.type != 2)
				{
				stop_sounds();
				jo_audio_play_sound_on_channel(&h_health, 0);
		
				//powerupused
				go->used = true;
				player.bat_ability = true;
				jo_reset_sprite_anim(game.stars_anim_id);
				//help_text = 1;
				//game.game_state = GAMESTATE_HELP_TEXT;
				}
			}
			
			
			
	}else
	{
	jo_sprite_draw3D2(jo_get_anim_sprite(game.stars_anim_id),go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y, 400);
	jo_start_sprite_anim(game.stars_anim_id);
	//jo_reset_sprite_anim(game.stars_anim_id);
	}
	}
	
}


// CREATE A NEW ENEMY 
void		create_enemy_spider(enemy* spider,int type,int x,int y,int min_x,int max_x,int speed_x, int hits)
{
		spider->alive = true;
		spider->x = x;
		spider->y = y;
		spider->min_x = min_x;
		spider->max_x = max_x;
		spider->speed_x = speed_x;
		spider->x_limit = 0;
		spider->y_limit = 0;
		spider->type = type;
		spider->up = false;
		spider->max_y = 200;
		spider->rotation = 0;
		spider->hits = hits;
		spider->hurt = false;
		spider->hurt_timer = 0;
		spider->can_be_hurt = true;
				
				
		switch(type)
    {
        case 1:
            spider->height = SPIDER_HEIGHT;
			spider->height2 = SPIDER_HEIGHT_2;
			spider->width = SPIDER_WIDTH;
			spider->width2 = SPIDER_WIDTH_2;
            break;

        case 2:
            spider->height = BAT_HEIGHT;
			spider->height2 = BAT_HEIGHT_2;
			spider->width = BAT_WIDTH;
			spider->width2 = BAT_WIDTH_2;
            break;
			
		case 3:
            spider->height = SPIDER_BOSS_HEIGHT ;
			spider->height2 = SPIDER_BOSS_HEIGHT_2;
			spider->width = SPIDER_BOSS_WIDTH;
			spider->width2 = SPIDER_BOSS_WIDTH_2;
            break;
			
		case 4:
            spider->height = FISH_HEIGHT ;
			spider->height2 = FISH_HEIGHT_2;
			spider->width = FISH_WIDTH;
			spider->width2 = FISH_WIDTH_2;
            break;
			
		case 5:
            spider->height = BAT_BOSS_HEIGHT ;
			spider->height2 = BAT_BOSS_HEIGHT_2;
			spider->width = BAT_BOSS_WIDTH;
			spider->width2 = BAT_BOSS_WIDTH_2;
            break;
			
		case 6:
            spider->height = FROG_HEIGHT ;
			spider->height2 = FROG_HEIGHT_2;
			spider->width = FROG_WIDTH;
			spider->width2 = FROG_WIDTH_2;
            break;

        default:
            spider->height = SPIDER_HEIGHT;
			spider->height2 = SPIDER_HEIGHT_2;
			spider->width = SPIDER_WIDTH;
			spider->width2 = SPIDER_WIDTH_2;
            break;
    }
		
		
}


// DISPLAY ENEMY
void		draw_enemy_spider(enemy* go)
{
	
	if(go->type == 5 && player.bat_ability)
		go->alive = false;
	
	if(go->type == 3 && player.spider_ability)
		go->alive = false;
	
	//hurt timeout
	if (!player.can_be_hurt)
		player.hurt_timer ++;
	
	if (player.hurt_timer >= PLAYER_HURT_TIMER_2)
	{
		go->can_be_hurt = true;
	}
	
	if (player.hurt_timer >= PLAYER_HURT_TIMER)
	{
		player.hurt_timer = 0;
		player.can_be_hurt = true;
	}
	//enemy hurt timeout
	
	if (go->hurt)
		go->hurt_timer ++;
	
	if (go->hurt_timer >= ENEMY_HURT_TIMER)
	{
		go->hurt_timer = 0;
		go->hurt = false;
	}
	
	if (go->x - map_pos_x > -50 && go->x - map_pos_x <350 && go->y - map_pos_y > -50 && go->y - map_pos_y <270)
	{
		
	
	if(go->alive)
	{
		if (go->hurt)
	{
	jo_sprite_enable_half_transparency();
	jo_sprite_enable_gouraud_shading();
	jo_set_gouraud_shading_colors(JO_COLOR_Red, JO_COLOR_Red, JO_COLOR_Red, JO_COLOR_Red);
	}				
		//flip enemy
		if (go->left)
		jo_sprite_enable_horizontal_flip();
		
		if (go->type ==1)
		{
		jo_sprite_draw3D_and_rotate(jo_get_anim_sprite(game.enemy_spider_anim_id),go->x - map_pos_x - 124,go->y + enemy_pos_y - WORLD_DEFAULT_Y - 104, 400,go->rotation);
		jo_start_sprite_anim_loop(game.enemy_spider_anim_id);
	
		jo_sprite_draw3D_and_rotate(jo_get_anim_sprite(player.spider_blink_anim_id), go->x - map_pos_x - 124,go->y + enemy_pos_y - WORLD_DEFAULT_Y - 104 -4, 380, go->rotation);
			
			player.frame_timer ++;
			if (player.frame_timer >= SPIDER_BLINK_TIMER)
			{
			player.frame_timer = 0;
			jo_start_sprite_anim(player.spider_blink_anim_id);
			}
			if (jo_is_sprite_anim_stopped(player.spider_blink_anim_id))
            jo_set_sprite_anim_frame(player.spider_blink_anim_id, 0);
		
		
		}
		if (go->type ==2)
		{jo_sprite_draw3D_and_rotate(jo_get_anim_sprite(game.enemy_bat_anim_id),go->x - map_pos_x - 124,go->y + enemy_pos_y - WORLD_DEFAULT_Y - 104, 400,go->rotation);
		jo_start_sprite_anim_loop(game.enemy_bat_anim_id);		
		}
		if (go->type ==3)
		{jo_sprite_change_sprite_scale(4);

		
		jo_sprite_draw3D_and_rotate(jo_get_anim_sprite(game.enemy_spider_anim_id),go->x - map_pos_x - 124,go->y + enemy_pos_y - WORLD_DEFAULT_Y - 104 - go->height2, 400,go->rotation);
		jo_start_sprite_anim_loop(game.enemy_spider_anim_id);
		
		jo_sprite_draw3D_and_rotate(jo_get_anim_sprite(player.spider_blink_anim_id), go->x - map_pos_x - 124,go->y + enemy_pos_y - WORLD_DEFAULT_Y - 104 - go->height2 - 16, 380, go->rotation);
			
			player.frame_timer ++;
			if (player.frame_timer >= SPIDER_BLINK_TIMER)
			{
			player.frame_timer = 0;
			jo_start_sprite_anim(player.spider_blink_anim_id);
			}
			if (jo_is_sprite_anim_stopped(player.spider_blink_anim_id))
            jo_set_sprite_anim_frame(player.spider_blink_anim_id, 0);
		
		jo_sprite_restore_sprite_scale();

		}
		if (go->type ==4)
		{
		//go->rotation = 90;
		jo_sprite_draw3D_and_rotate(game.enemy_fish_sprite_id,go->x - map_pos_x - 124,go->y + enemy_pos_y - WORLD_DEFAULT_Y - 104, 400,go->rotation);
		}
		
		if (go->type ==5)
		{jo_sprite_change_sprite_scale(4);

		
		jo_sprite_draw3D_and_rotate(jo_get_anim_sprite(game.enemy_bat_anim_id),go->x - map_pos_x - 124,go->y + enemy_pos_y - WORLD_DEFAULT_Y - 104 - go->height2, 400,go->rotation);
		jo_start_sprite_anim_loop(game.enemy_bat_anim_id);
		jo_sprite_restore_sprite_scale();

		}
		
		if (go->type ==6)
		{jo_sprite_draw3D_and_rotate(game.enemy_frog_sprite_id_1,go->x - map_pos_x - 124,go->y + enemy_pos_y - WORLD_DEFAULT_Y - 104, 400,go->rotation);
		//jo_start_sprite_anim_loop(game.enemy_bat_anim_id);		
		}
		
		jo_sprite_disable_horizontal_flip();
		jo_sprite_disable_gouraud_shading();
		jo_sprite_disable_half_transparency();
		
				
		//move enemy left and right within boundries
	
		if(go->type != 6)
		{
			if (go->x_limit <= go->min_x)
				go->left = false;
			else if (go->x_limit >= go->max_x)
			go->left = true;
		
			if (go->left)
				{go->x -= go->speed_x;
				go->x_limit -= go->speed_x;
				}
			else
			{go->x += go->speed_x;
			go->x_limit += go->speed_x;
			}
		}else
		{
		if(	(go->x - map_pos_x - 124)<player.x)
		{
			go->left = false;
		}else
		{
			go->left = true;
		}
			
		}
		// move flying enemy up and down within boundries
		
		if (go->type == 2)
		{if (go->y_limit <= 0)
				go->up = false;
			else if (go->y_limit >= go->max_y)
			go->up = true;
		}
		
		if (go->type == 4)
		{if (go->y_limit <= 0)
				go->up = false;
			else if (go->y_limit >= go->max_y)
			go->up = true;
		}
		
		
		
	
    //jo_sprite_draw3D2(col_test_sprite_id2,go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y,200);
	//jo_sprite_draw3D2(col_test_sprite_id,player.x,player.y,200);
	//jo_nbg2_printf(0, 3, ">health id %3d  ",jo_sprite_get_width(jo_get_anim_sprite(game.enemy_spider_anim_id)));
	//jo_nbg2_printf(0, 4, ">super health id %3d  ",game.pup_super_health_anim_id);
	//jo_nbg2_printf(0, 5, ">player sprite id %3d  ",player.health_sprite_id_3);	//jo_sprite_get_width(game.enemy_spider_anim_id)
		
		if (jo_hitbox_detection_custom_boundaries(jo_get_anim_sprite(game.enemy_spider_anim_id),go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y,player.x,player.y, player.width,player.height) && player.can_be_hurt)
    {
        //lose health

          player_bounceback();
		  player.can_be_hurt = false;
		  go->can_be_hurt = false;
          player.health -=1;
         if (player.health == 0)
		  { //game.game_state = GAMESTATE_DEAD;
          reset_demo();
		  }
          

    }
	//stomp on enemy	
	else if (jo_hitbox_detection_custom_boundaries(jo_get_anim_sprite(game.enemy_spider_anim_id),go->x - map_pos_x,go->y + enemy_pos_y - 10 - WORLD_DEFAULT_Y,player.x,player.y, player.width,player.height) && go->can_be_hurt && physics.is_in_air)//&& player.can_be_hurt
    {
        
		if (go->hits > 1)
		{
			go->hurt = true;
			go->hits --;
			stop_sounds();
			jo_audio_play_sound_on_channel(&h_die, 1);
			player_bounce();
			if(go->type == 3)
				{//spider boss
					//go->y = go->y -100;
					//create_enemy_spider(&enemies[enemy_total],1,go->x,go->y,-100,100,1,1);enemy_total++;
					
			
				}
			if(go->type == 5)
				{//bat boss
					//go->y = go->y -100;
					//create_enemy_spider(&enemies[enemy_total],2,go->x,go->y+300,-100,100,1,1);enemy_total++;
					
			
				}
		}
		else
		{/*enemy dead*/
		jo_reset_sprite_anim(game.enemy_splat_sprite_id);
		stop_sounds();
        jo_audio_play_sound_on_channel(&h_die, 1);
		player_bounce();
		
			if(go->type == 3 || go->type == 5)
				{
				//jo_nbg2_printf(0, 10, "BOSS DEAD");
				map_max_x = level_end;
				jo_audio_stop_cd();
				is_cd_playing = false;
				}
		go->alive = false;
        }
		
			
    }
    

	
	}
	
	//gravity
	int dist = jo_map_per_pixel_vertical_collision(WORLD_MAP_ID, go->x + go->width2, go->y + go->height, JO_NULL);
    if (dist == JO_MAP_NO_COLLISION || dist > 0)
        {
       // if (dist != JO_MAP_NO_COLLISION) // colliding with map going down hill
        //jo_nbg2_printf(0, 4, "> SPIDER TOUCHING FLOOR  : TRUE");
		//else // dist = -2147483647 (jo_map_no_collision)
			if (!go->up)
			{
			go->y += 4; 
			go->y_limit +=4;
			}
			else
			{
			go->y -= 2; 
			go->y_limit -=2;
			}
			
        }
	else if(dist < 0)// negative distance means going up hill so add to y position
	{//jo_nbg2_printf(0, 4, "> SPIDER TOUCHING FLOOR  : TRUE");
		go->y += dist;
	}
	
	//rotate spider
	int side;
		
	
	if (go->left)
		side = go->x;
	else
		side = go->x + go->width;		
	
	
	int r_dist = jo_map_per_pixel_vertical_collision(WORLD_MAP_ID, side, go->y + go->height, JO_NULL);
	
	
    if (r_dist == JO_MAP_NO_COLLISION || r_dist > 0)
        {
       // if (dist != JO_MAP_NO_COLLISION) // colliding with map going down hill
        //jo_nbg2_printf(0, 4, "> SPIDER TOUCHING FLOOR  : TRUE");
		//else // dist = -2147483647 (jo_map_no_collision)
			if (r_dist > 0)
			{
			go->rotation = -r_dist; 
			//go->y_limit +=4;
			}
			else
			{
			
			go->rotation = 0; 
			//go->y_limit -=2;
			}
        }
	else if(r_dist < 0)// negative distance means going up hill so add to y position
	{//jo_nbg2_printf(0, 4, "> SPIDER TOUCHING FLOOR  : TRUE");
	go->rotation = r_dist;
	}
	
	//jo_nbg2_printf(0, 4, "> r_dist %10d  : ",r_dist);
	
	
	//horizontal collision
	
	 
    int attr = jo_map_hitbox_detection_custom_boundaries(WORLD_MAP_ID, side, go->y, 4, 20);
    if (attr != JO_MAP_NO_COLLISION && attr != 0 && attr != 2)
	{
	if(go->left)
			go->left = false;
		else
			go->left = true;
	}
	
	if (!go->alive)
	{
		if(go->type == 3 || go->type == 5)
		jo_sprite_change_sprite_scale(4);
		
		
		
	go->up = false;
	jo_sprite_draw3D2(jo_get_anim_sprite(game.enemy_splat_sprite_id),go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y, 500);
	jo_start_sprite_anim(game.enemy_splat_sprite_id);
	//if((go->type == 2 || go->type == 5)&& jo_is_sprite_anim_stopped)
	if(go->type == 2 || go->type == 5)
	jo_sprite_draw3D2(game.bat_corpse_sprite_id,go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y + 24, 550);
	else //if (jo_is_sprite_anim_stopped)
	jo_sprite_draw3D2(game.spider_corpse_sprite_id,go->x - map_pos_x,go->y + enemy_pos_y - WORLD_DEFAULT_Y + 12, 550);
	
	
	
	jo_sprite_restore_sprite_scale();
	}
	
	}
	jo_sprite_disable_gouraud_shading();
	
	
	//jo_nbg2_printf(0, 2, "> GO_X: %4d",go->x + SPIDER_WIDTH_2 - map_pos_x);
	//jo_nbg2_printf(20, 2, "> GO_Y: %4d",go->y);
	//jo_nbg2_printf(0, 1, "> MAP_X: %4d",map_pos_x);
	//jo_nbg2_printf(20, 1, "> MAP_Y: %4d",enemy_pos_y);
}


void						draw_waterfall(void)
{
		
jo_sprite_change_sprite_scale_xy(2.5,3);

	jo_sprite_enable_screen_doors_filter();
	if(level==1)
	{jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),4992 - map_pos_x,416 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),4992 - map_pos_x,560 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),4992 - map_pos_x,704 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),4992 - map_pos_x,848 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),4992 - map_pos_x,992 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_splash_anim_id),4992 - map_pos_x,344 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),5280 - map_pos_x,416 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),5280 - map_pos_x,560 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),5280 - map_pos_x,704 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),5280 - map_pos_x,848 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),5280 - map_pos_x,992 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_splash_anim_id),5280 - map_pos_x,344 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	
	
	}else if(level==2)
	{
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),936 - map_pos_x,944 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),936 - map_pos_x,800 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_splash_anim_id),936 - map_pos_x,1008 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),1128 - map_pos_x,944 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),1128 - map_pos_x,800 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_splash_anim_id),1128 - map_pos_x,1008 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),1152 - map_pos_x,2216 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),1152 - map_pos_x,2072 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_splash_anim_id),1152 - map_pos_x,2280 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),1904 - map_pos_x,2216 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_anim_id),1904 - map_pos_x,2072 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_sprite_draw3D2(jo_get_anim_sprite(game.wfall_splash_anim_id),1904 - map_pos_x,2280 + enemy_pos_y - WORLD_DEFAULT_Y,300);
	
	
	
	}
	
	jo_start_sprite_anim_loop(game.wfall_anim_id);
	jo_start_sprite_anim_loop(game.wfall_splash_anim_id);
	
	jo_sprite_restore_sprite_scale();
		
	//make splash when entering water
	
	if(player.in_water)
	{
		jo_sprite_draw3D2(jo_get_anim_sprite(game.player_splash_anim_id),splash_x - map_pos_x,splash_y + enemy_pos_y - WORLD_DEFAULT_Y,300);
	jo_start_sprite_anim(game.player_splash_anim_id);
		if(player.can_splash)
		{
		
			jo_reset_sprite_anim(game.player_splash_anim_id);
			stop_sounds();
			jo_audio_play_sound_on_channel(&h_splash, 3);
			player.can_splash = false;
	
		}
		
	
	}
	else
	{player.can_splash = true;
		splash_x = map_pos_x + player.x;
		splash_y = map_pos_y + player.y - player.height2;
	}
	
	
	
	jo_sprite_disable_screen_doors_filter();
}



static inline void			my_draw(void)
{

    if (game.game_state != GAMESTATE_GAMEPLAY)
       return;

    int               prev_y;
	int				 mins;
    int        		secs;
	
	game.current_time = getSeconds() - game.start_time - game.pause_time;
	
	// wait a bit before starting the music track
    if(game.game_frame >= 30)
    {  
	
	if(map_pos_x > game.boss_start_x && map_pos_y < game.boss_start_y && map_max_x != level_end)
	{map_min_x = game.boss_start_x;
		if(!game.boss)
		{
			jo_audio_stop_cd();
			is_cd_playing = false;
			game.boss = true;
			game.game_frame = 0;
		}
	 if (!is_cd_playing)
        {
             jo_audio_play_cd_track(game.boss_music_track, game.boss_music_track, true);
            is_cd_playing = true;
        }
	}else
		{
	 if (!is_cd_playing)
        {
             jo_audio_play_cd_track(game.music_track, game.music_track, true);
            is_cd_playing = true;
        }
		}
	}
	game.game_frame++;
	
	//jo_fixed_point_time();

    jo_map_draw(WORLD_MAP_ID, map_pos_x, map_pos_y);
	jo_map_draw(MAP_FORE1_ID, map_pos_x, map_pos_y);
	
	//waterfall
	draw_waterfall();

   jo_map_draw(MAP_FORE3_ID, mapff_pos_x, map_pos_y);
    if (water)
        {
        //jo_sprite_enable_half_transparency();
		jo_sprite_enable_screen_doors_filter();
        jo_map_draw(MAP_WATER_ID, map_pos_x, map_pos_y);
        jo_start_sprite_anim_loop(game.wave_anim_id);
		//jo_sprite_disable_half_transparency();
		jo_sprite_disable_screen_doors_filter();
				
        }
    else
    jo_map_draw(MAP_FORE2_ID, mapf_pos_x, map_pos_y);
	jo_sprite_enable_horizontal_flip();
	jo_sprite_disable_horizontal_flip();

    //background

    jo_sprite_draw3D2(bg_sprite, bg_x, 0, 1000);
	jo_sprite_draw3D2(bg_sprite, bg_x + bg_width, 0, 1000);
	jo_sprite_draw3D2(bg_sprite, bg_x + (bg_width + bg_width), 0, 1000);
	
	//if(level == 1)
	//{
	jo_sprite_draw3D2(bg_sprite2, bg_x2, bg_adj, 1500);
	jo_sprite_draw3D2(bg_sprite2, bg_x2 + bg_width, bg_adj, 1500);
	jo_sprite_draw3D2(bg_sprite2, bg_x2 + (bg_width + bg_width), bg_adj, 1500);	
	//}
	

    prev_y = player.y;
	player_collision_handling();
	camera_handling(prev_y);
	hamster_animation_handling();
	display_player();
	display_hud();
	
	
	
	// draw enemies
	int i;
	for(i = 0; i < enemy_total; i++)
	{draw_enemy_spider(&enemies[i]);
	}
	
	// draw powerups
	int i2;
	for(i2 = 0; i2 < powerup_total; i2++)
	{draw_powerup(&powerups[i2]);
	}
	//powerup timer
	if (player.pup_timer >= PUP_TIMER && player.mutate == true && !ceiling_too_low(HAMSTER_WIDTH,player.y-26) && !has_horizontal_collision(HAMSTER_WIDTH_2,player.y))
				{
			player.type = 1;
				player.height = HAMSTER_HEIGHT;
				player.height2 = HAMSTER_HEIGHT_2;
				player.width = HAMSTER_WIDTH;
				player.width2 = HAMSTER_WIDTH_2;
				player.can_shoot = false;
				player.y = player.y -10;
				player.mutate = false;
			}
			else
			player.pup_timer ++;
	
	
	
	
	if(game.debug)
	{jo_nbg2_printf(0, 1, "* DYNAMIC MEMORY USAGE: %d%%  ", jo_memory_usage_percent());
	jo_nbg2_printf(0, 2, "* SPRITE MEMORY USAGE: %d%%  ", jo_sprite_usage_percent());
	jo_nbg2_printf(0, 3, "* MAP.X %4d    MAP.Y %4d  ", map_pos_x,map_pos_y);
	jo_nbg2_printf(0, 4, "* PLAYER.X %4d    PLAYER.Y %4d  ", player.x,player.y);
	//jo_nbg2_printf(0, 5, "* CEILING COLLISION %d  ", ceiling_too_low(HAMSTER_WIDTH,player.y-26));
	jo_nbg2_printf(0, 5, "* PLAYER.IN_WATER %d  ", player.in_water);
	//jo_nbg2_printf(0, 6, "* PLAYER.CAN_SPLASH %d  ", player.can_splash);
	
	
	}
	else
	{
	jo_nbg2_printf(0, 1, "                                     ");
	jo_nbg2_printf(0, 2, "                                     ");
	jo_nbg2_printf(0, 3, "                                     ");
	jo_nbg2_printf(0, 4, "                                     ");
	jo_nbg2_printf(0, 5, "                                     ");
	}

	mins = game.current_time / 60;
            secs = game.current_time % 60;
			
	jo_nbg2_printf(11, 24, "x%2d", player.gems);
	jo_nbg2_printf(14, 24, "/25");
	jo_nbg2_printf(11, 27, "%02d:%02d", mins,secs);
	
	if(map_pos_x == level_end && player.x >= 200)
	{	
		transition_to_end_level();
		
	}



	return;

}



static inline void            player_jump(void)
{

    player.can_jump = false;
	stop_sounds();
    jo_audio_play_sound_on_channel(&h_jump, 2);
	
    jo_physics_jump(&physics);

}

static inline void            player_flap(void)
{

    //flying bat
	stop_sounds();
	jo_audio_play_sound_on_channel(&h_flap, 2);
    physics.speed_y = -6.0f;

}


static inline void			my_gamepad(void)
{

    if (game.game_state != GAMESTATE_GAMEPLAY)
    {return;
	}

	if (!jo_is_pad1_available())
	{return ;
	}
	if (jo_is_pad1_key_pressed(JO_KEY_LEFT) )//&& player.can_be_hurt
		{
            physics.speed = -4.0f;
            player.flip_player = true;

		}

	if (jo_is_pad1_key_pressed(JO_KEY_RIGHT) )//&& player.can_be_hurt
		{
		    physics.speed = 4.0f;
            player.flip_player = false;

		}



		 // did player one pause the game?
    if (jo_is_pad1_key_down(JO_KEY_START))
    {
  
            game.game_state = GAMESTATE_PAUSED;
			game.pause_start_time = getSeconds();
            game.pause_menu = 0;
			jo_audio_stop_cd();
            is_cd_playing = false;
            jo_nbg2_printf(9, 12, "*");
   
    }
    



		if (player.can_jump && jo_is_pad1_key_down(JO_KEY_A) && player.type < 3)
		player_jump();

		if (jo_is_pad1_key_down(JO_KEY_A) && player.type == 3)
        {
            player.frame_timer = 0;
            player_flap();
        }
		
		if (jo_is_pad1_key_down(JO_KEY_A) && player.type == 4 && player.in_water)
        {
            player.frame_timer = 0;
            player_flap();
        }

if(game.debug)
{
		/*if (jo_is_pad1_key_down(JO_KEY_Z))
    {
        if (!is_cd_playing)
        {
             jo_audio_play_cd_track(3, 3, true);
            is_cd_playing = true;
        }
        else
        {
            jo_audio_stop_cd();
            is_cd_playing = false;
        }
    }*/
        /*if (jo_is_pad1_key_down(JO_KEY_B) && player.can_shoot)

           {//jo_audio_play_sound_on_channel(&h_health, 1);

            if (player.flip_player)
            shoot_left();
            else
            shoot_right();
           }*/
		  // change player type - need to put this in it's own function
        if (jo_is_pad1_key_pressed(JO_KEY_X))
		{
			
			if(game.pressed_X == false)
			{
			player.type = 1;
			player.y -= (HAMSTER_HEIGHT - player.height);
			player.height = HAMSTER_HEIGHT;
			player.height2 = HAMSTER_HEIGHT_2;
			player.width = HAMSTER_WIDTH;
			player.width2 = HAMSTER_WIDTH_2;
			player.can_shoot = true;
			
			}
			game.pressed_X = true;
			
			

		}
		else
			game.pressed_X = false;
		
		
		if (jo_is_pad1_key_pressed(JO_KEY_Y))
		{player.type = 2;
		player.height = SPIDER_HEIGHT;
		player.height2 = SPIDER_HEIGHT_2;
        player.width = SPIDER_WIDTH;
        player.width2 = SPIDER_WIDTH_2;
        player.can_shoot = false;

		}
		if (jo_is_pad1_key_pressed(JO_KEY_Z))
		{player.type = 3;
		player.height = BAT_HEIGHT;
		player.height2 = BAT_HEIGHT_2;
        player.width = BAT_WIDTH;
        player.width2 = BAT_WIDTH_2;
        player.can_shoot = false;

		}
		
		if (jo_is_pad1_key_pressed(JO_KEY_C))
		{player.type = 4;
		player.height = FISH_HEIGHT;
		player.height2 = FISH_HEIGHT_2;
        player.width = FISH_WIDTH;
        player.width2 = FISH_WIDTH_2;
        player.can_shoot = false;

		}
}
		if (jo_is_pad1_key_down(JO_KEY_L) && jo_is_pad1_key_down(JO_KEY_R))
		{
			game.debug ^= true;
		}
		
		if (jo_is_pad1_key_down(JO_KEY_L) && game.debug == true)
		{
			//player.spider_ability = true;
			//player.bat_ability = true;
			player.health = 3;
			//transition_to_end_level();
		}
		

}
void			clear_level(void)
{
	game.game_state = GAMESTATE_UNINITIALIZED;
	//clear_anim();		
	jo_sprite_free_from(bg_sprite);
	
	//if(level == 1)
	//{
		jo_sprite_free_from(bg_sprite2);
	//}
	
	jo_map_free(MAP_FORE3_ID);
	if (water)
	jo_map_free(MAP_WATER_ID);
	else
	jo_map_free(MAP_FORE2_ID);

	jo_map_free(MAP_FORE1_ID);
	jo_map_free(WORLD_MAP_ID);
	jo_sprite_free_from(first_map_sprite_id);
	jo_sprite_free_from(level_title_sprite);
	jo_sprite_free_from(level_banner_sprite);
	
	
			
	enemy_total = 0;
	powerup_total = 0;
	transition_to_level_select();
	
}



void            end_level(void)
{

if (game.game_state != GAMESTATE_END_LEVEL)
    return;

int				 mins;
    int        		secs;
	mins = game.current_time / 60;
            secs = game.current_time % 60;
	//ztClearText();
	jo_audio_stop_cd();
    is_cd_playing = false;
	
	
	switch(level)
    {
        case 1:
            game.level_1_gems = player.gems;
			if(game.current_time < game.level_1_time || game.level_1_time == 0)
				game.level_1_time = game.current_time;
			
				jo_nbg2_printf(4, 5, "YOU CAN NOW TURN INTO A SPIDER");
				jo_nbg2_printf(4, 6, "FOR A SHORT TIME BY COLLECTING");
				jo_nbg2_printf(4, 7, "WALK UP WALLS AND GET THROUGH ");
				jo_nbg2_printf(4, 8, "SMALL GAPS TO FIND NEW AREAS!");
				jo_sprite_draw3D2(jo_get_anim_sprite(game.pup_spider_anim_id),280,44, 400);
				jo_start_sprite_anim_loop(game.pup_spider_anim_id);
				
				
            break;
		 case 2:
            game.level_2_gems = player.gems;
			if(game.current_time < game.level_2_time || game.level_2_time == 0)
				game.level_2_time = game.current_time;
			
				jo_nbg2_printf(4, 5, "YOU CAN NOW TURN INTO A BAT");
				jo_nbg2_printf(4, 6, "FOR A SHORT TIME BY COLLECTING");
				jo_nbg2_printf(4, 7, "FLAP YOUR WINGS AND FLY");
				jo_nbg2_printf(4, 8, "IN THE SKY TO FIND NEW AREAS!");
				
				jo_sprite_draw3D2(jo_get_anim_sprite(game.pup_bat_anim_id),280,44, 400);
				jo_start_sprite_anim_loop(game.pup_bat_anim_id);
            break;
		
	}
	
	
	

	jo_nbg2_printf(10, 3, "LEVEL COMPLETED!");
	jo_nbg2_printf(4, 11,  "TIME: %02d:%02d", mins,secs);
	jo_nbg2_printf(22, 11, "TOTAL GEMS: %2d ", player.gems);
	
if (game.end_time < 50)
{
	jo_nbg2_printf(4, 13, "COMPLETE IN 50 SECONDS......DONE");

}
else
{
	jo_nbg2_printf(4, 13, "COMPLETE IN 50 SECONDS.........X");
}
	if (player.gems < 5)
{
jo_nbg2_printf(4, 14, "COLLECT 5 GEMS..............X");

}
else
{
	jo_nbg2_printf(4, 14, "COLLECT 5 GEMS...........DONE");
}

if (player.gems < 10)
{
jo_nbg2_printf(4, 15, "COLLECT 10 GEMS..............X");

}
else
{
	jo_nbg2_printf(4, 15, "COLLECT 10 GEMS...........DONE");
}

if (player.gems < 25)
{
jo_nbg2_printf(4, 16, "COLLECT 25 GEMS..............X");

}
else
{
	jo_nbg2_printf(4, 16, "COLLECT 25 GEMS...........DONE");
}
	
	
	jo_nbg2_printf(10, 20, "CONTINUE");
    jo_nbg2_printf(10, 21, "RESTART LEVEL");
 
    //jo_nbg2_printf(9, 20, "*");

    jo_sprite_enable_gouraud_shading();
	jo_set_gouraud_shading_colors(JO_COLOR_RGB(173,0,253), JO_COLOR_RGB(173,0,253), JO_COLOR_RGB(56,2,112), JO_COLOR_RGB(56,2,112));
	
    jo_sprite_draw3D2(bg_sprite, bg_x, 0, 500);
	jo_sprite_draw3D2(bg_sprite, bg_x + bg_width, 0, 500);
	jo_sprite_draw3D2(bg_sprite, bg_x + (bg_width + bg_width), 0, 500);
	
	jo_sprite_disable_gouraud_shading();

		

    if (jo_is_pad1_key_pressed(JO_KEY_UP))
    {
        if(game.pressed_up == false)
        {

            jo_nbg2_printf(9, 20, " ");
            jo_nbg2_printf(9, 21, " ");

            if (game.end_level_menu == 0)
                game.end_level_menu = END_LEVEL_MENU_MAX;
            else
                game.end_level_menu --;

            jo_nbg2_printf(9, 20 + game.end_level_menu, "*");

        }
        game.pressed_up = true;
    }
    else
    {
        game.pressed_up = false;
    }

    if (jo_is_pad1_key_pressed(JO_KEY_DOWN))
    {
        if(game.pressed_down == false)
        {
            jo_nbg2_printf(9, 20, " ");
            jo_nbg2_printf(9, 21, " ");

            if (game.end_level_menu == END_LEVEL_MENU_MAX)
                game.end_level_menu = 0;
            else
                game.end_level_menu ++;

            jo_nbg2_printf(9, 20 + game.end_level_menu, "*");
        }
        game.pressed_down = true;
    }
    else
    {
        game.pressed_down = false;
    }

    if (jo_is_pad1_key_pressed(JO_KEY_START))
    {
        if(game.pressed_start == false)
        {
           
            if (game.end_level_menu == 1)
            {
				ztClearText();
                game.game_state = GAMESTATE_LEVEL_START;
                reset_demo();
            }

            else if (game.end_level_menu == 0)
            {
                clear_level();


            }
           // else
           //     game.game_state = GAMESTATE_GAMEPLAY;


        }
        game.pressed_start = true;
    }
    else
    {
        game.pressed_start = false;
    }

}

void            pause_game(void)
{

if (game.game_state != GAMESTATE_PAUSED)
    return;

	jo_nbg2_printf(10, 10, "PAUSED");
    jo_nbg2_printf(10, 12, "CONTINUE");
    jo_nbg2_printf(10, 13, "RESTART LEVEL");
    jo_nbg2_printf(10, 14, "LEVEL SELECT");

    jo_sprite_enable_gouraud_shading();
	
	jo_set_gouraud_shading_colors(JO_COLOR_RGB(0,109,105), JO_COLOR_RGB(0,109,105), JO_COLOR_RGB(0,109,105), JO_COLOR_RGB(0,109,105));

    jo_map_draw(WORLD_MAP_ID, map_pos_x, map_pos_y);
    jo_map_draw(MAP_FORE1_ID, map_pos_x, map_pos_y);
    jo_map_draw(MAP_FORE2_ID, mapf_pos_x, map_pos_y);
    jo_map_draw(MAP_FORE3_ID, mapff_pos_x, map_pos_y);
	
	jo_sprite_disable_gouraud_shading();
    //background

    jo_sprite_enable_gouraud_shading();
	jo_set_gouraud_shading_colors(JO_COLOR_RGB(0,0,0), JO_COLOR_RGB(0,0,0), JO_COLOR_RGB(0,109,105), JO_COLOR_RGB(0,109,105));
	
    jo_sprite_draw3D2(bg_sprite, bg_x, 0, 500);
	jo_sprite_draw3D2(bg_sprite, bg_x + bg_width, 0, 500);
	jo_sprite_draw3D2(bg_sprite, bg_x + (bg_width + bg_width), 0, 500);
	
	jo_sprite_disable_gouraud_shading();

	display_player();
	display_hud();
	
	// draw enemies
	//int i;
	//for(i = 0; i < enemy_total; i++)
	//{draw_enemy_spider(&enemies[i]);
	//}
	
	// draw powerups
	int i2;
	for(i2 = 0; i2 < powerup_total; i2++)
	{draw_powerup(&powerups[i2]);
	}
	
	

    if (jo_is_pad1_key_pressed(JO_KEY_UP))
    {
        if(game.pressed_up == false)
        {

            jo_nbg2_printf(9, 12, " ");
            jo_nbg2_printf(9, 13, " ");
            jo_nbg2_printf(9, 14, " ");

            if (game.pause_menu == 0)
                game.pause_menu = PAUSE_MENU_MAX;
            else
                game.pause_menu --;



            jo_nbg2_printf(9, 12 + game.pause_menu, "*");


        }
        game.pressed_up = true;
    }
    else
    {
        game.pressed_up = false;
    }

    if (jo_is_pad1_key_pressed(JO_KEY_DOWN))
    {
        if(game.pressed_down == false)
        {
            jo_nbg2_printf(9, 12, " ");
            jo_nbg2_printf(9, 13, " ");
            jo_nbg2_printf(9, 14, " ");

            if (game.pause_menu == PAUSE_MENU_MAX)
                game.pause_menu = 0;
            else
                game.pause_menu ++;



            jo_nbg2_printf(9, 12 + game.pause_menu, "*");
        }
        game.pressed_down = true;
    }
    else
    {
        game.pressed_down = false;
    }

    // did player one pause the game?
    if (jo_is_pad1_key_pressed(JO_KEY_START))
    {
        if(game.pressed_start == false)
        {
            jo_nbg2_printf(10, 10, "                  ");
            jo_nbg2_printf(9, 12, "                  ");
            jo_nbg2_printf(9, 13, "                  ");
            jo_nbg2_printf(9, 14, "                  ");
            if (game.pause_menu == 1)
            {
                game.game_state = GAMESTATE_LEVEL_START;
                reset_demo();
            }

            else if (game.pause_menu == 2)
            {
    
			clear_level();
             
            }
            else
			{ 
			game.pause_time += getSeconds() - game.pause_start_time;
			
			game.game_state = GAMESTATE_GAMEPLAY;
		
		
			}


        }
        game.pressed_start = true;
    }
    else
    {
        game.pressed_start = false;
    }



}

void            title_screen(void)
{
	
	

if (game.game_state != GAMESTATE_TITLE_SCREEN)
    return;

//jo_nbg2_printf(0, 24, "> Dynamic memory usage: %d%%  ", jo_memory_usage_percent());
//	jo_nbg2_printf(0, 25, "> sprite memory usage: %d%%  ", jo_sprite_usage_percent());

 if (!is_cd_playing)
        {
             jo_audio_play_cd_track(4, 4, true);
            is_cd_playing = true;
        }

jo_sprite_draw3D2(bg_sprite, bg_x, 0, 500);
	jo_sprite_enable_horizontal_flip();
	jo_sprite_draw3D2(bg_sprite, bg_x + bg_width, 0, 500);
	jo_sprite_disable_horizontal_flip();
jo_sprite_draw3D2(bg_sprite, bg_x + (bg_width + bg_width), 0, 500);
	
	
	if (bg_x == -184 )
	{ bg_direction = 1;
	}
    if(bg_x == 0)
	{bg_direction = -1;
	}
			
	bg_x += bg_direction;


	jo_sprite_change_sprite_scale(game.title_size);
	
	jo_sprite_draw3D2(logo_sprite, 76, 36, 500);
	
	jo_sprite_restore_sprite_scale();

	
    //jo_nbg2_printf(10, 10, "MUTATOR V1.0");
	jo_nbg2_printf(14, 28, "PRESS START");


 if (jo_is_pad1_key_down(JO_KEY_START))
    {
        if(game.pressed_start == false)
        {
		game.start_timer = true;
		jo_audio_play_sound_on_channel(&h_health, 1);


        }
       game.pressed_start = true;
    }
    else
    {
        game.pressed_start = false;
   }
    
        //level title
	if(game.start_timer)
	{	if(game.title_size < 10)
		{game.title_size ++;
		}
		
	game.title_screen_timer ++;
	
	
	
	
	
	}
	
	if(game.title_screen_timer >= TITLE_SCREEN_TIMER)
	{
 	
		
        game.game_state = GAMESTATE_UNINITIALIZED;
		 		ztClearText();
		jo_clear_background(JO_COLOR_Purple);
	jo_sprite_free_from(bg_sprite);
	jo_sprite_free_from(logo_sprite);
		transition_to_level_select();
		
		
		
	}
	
	


        return;
}
void			clear_level_select(void)
{		
		jo_audio_stop_cd();
    is_cd_playing = false;
		ztClearText();
		jo_clear_background(JO_COLOR_Black);
		jo_sprite_free_from(l_select_banner);
		jo_sprite_free_from(l_select_l3);
		jo_sprite_free_from(l_select_l2);
		jo_sprite_free_from(l_select_l1);
		jo_sprite_free_from(l_select_level_name3);
		jo_sprite_free_from(l_select_level_name2);
		jo_sprite_free_from(l_select_level_name1);
		jo_sprite_free_from(l_select_title);
		jo_sprite_free_from(bg_sprite);
		
		
		
		
		
		
		
}
void			load_level(void)
{
	
		
        if (level == 1)
        //LEVEL1
		{
		jo_nbg2_printf(0, 1, "LOADING MAP TILES......[  ]");
		
		first_map_sprite_id = jo_sprite_add_tga_tileset("TEX", "LEV1.TGA", JO_COLOR_Red, Level_1_Tileset, JO_TILE_COUNT(Level_1_Tileset));
		
		jo_nbg2_printf(0, 1, "LOADING MAP TILES......[OK]");
		jo_nbg2_printf(0, 2, "LOADING MAPS...........[  ]");
		level_1_map();		
		jo_nbg2_printf(0, 2, "LOADING MAPS...........[OK]");
        
        //load bg
        jo_nbg2_printf(0, 3, "LOADING BACKGROUND.....[  ]");
        bg_sprite = jo_sprite_add_tga("BG", "TREESF.TGA", JO_COLOR_Blue);
		bg_sprite2 = jo_sprite_add_tga("BG", "TREESB.TGA", JO_COLOR_Transparent);
		bg_adj = 0;
		jo_nbg2_printf(0, 3, "LOADING BACKGROUND.....[OK]");
		
		jo_nbg2_printf(0, 4, "LOADING TITLE.....[  ]");
        level_title_sprite = jo_sprite_add_tga("TEX", "L1TIT.TGA", JO_COLOR_Red);
		level_banner_sprite = jo_sprite_add_tga("TEX", "SELBAN.TGA", JO_COLOR_Transparent);
		jo_nbg2_printf(0, 4, "LOADING TITLE.....[OK]");
		
		
        }
        
        else if (level == 2)
        //LEVEL2
        {
		jo_nbg2_printf(0, 1, "LOADING MAP TILES......[  ]");
		
        first_map_sprite_id = jo_sprite_add_tga_tileset("TEX", "LEV2.TGA", JO_COLOR_Red, Level_2_Tileset, JO_TILE_COUNT(Level_2_Tileset));
		
		jo_nbg2_printf(0, 1, "LOADING MAP tiles......[OK]");
		jo_nbg2_printf(0, 2, "LOADING MAPS...........[  ]");
		level_2_map();
		jo_nbg2_printf(0, 2, "LOADING MAPS...........[OK]");
       
        //load bg
		jo_nbg2_printf(0, 3, "LOADING BACKGROUND.....[  ]");
        bg_sprite = jo_sprite_add_tga("BG", "CAVEF.TGA", JO_COLOR_Blue);
		bg_sprite2 = jo_sprite_add_tga("BG", "CAVEB.TGA", JO_COLOR_Transparent);
		bg_adj = 16;
		jo_nbg2_printf(0, 3, "LOADING BACKGROUND.....[OK]");
		
		jo_nbg2_printf(0, 4, "LOADING TITLE.....[  ]");
        level_title_sprite = jo_sprite_add_tga("TEX", "L2TIT.TGA", JO_COLOR_Red);
		level_banner_sprite = jo_sprite_add_tga("TEX", "SELBAN.TGA", JO_COLOR_Transparent);
		jo_nbg2_printf(0, 4, "LOADING TITLE.....[OK]");
		
        }else if (level == 3)
        //LEVEL2
        {
		jo_nbg2_printf(0, 1, "LOADING MAP TILES......[  ]");
		
        first_map_sprite_id = jo_sprite_add_tga_tileset("TEX", "LEV2.TGA", JO_COLOR_Red, Level_2_Tileset, JO_TILE_COUNT(Level_2_Tileset));
		
		jo_nbg2_printf(0, 1, "LOADING MAP tiles......[OK]");
		jo_nbg2_printf(0, 2, "LOADING MAPS...........[  ]");
		level_2_map();
		jo_nbg2_printf(0, 2, "LOADING MAPS...........[OK]");
       
        //load bg
		jo_nbg2_printf(0, 3, "LOADING BACKGROUND.....[  ]");
        bg_sprite = jo_sprite_add_tga("BG", "WATERF.TGA", JO_COLOR_Blue);
		bg_sprite2 = jo_sprite_add_tga("BG", "WATERC.TGA", JO_COLOR_Transparent);
		bg_adj = 16;
		jo_nbg2_printf(0, 3, "LOADING BACKGROUND.....[OK]");
		
		jo_nbg2_printf(0, 4, "LOADING TITLE.....[  ]");
        level_title_sprite = jo_sprite_add_tga("TEX", "L2TIT.TGA", JO_COLOR_Red);
		level_banner_sprite = jo_sprite_add_tga("TEX", "SELBAN.TGA", JO_COLOR_Transparent);
		jo_nbg2_printf(0, 4, "LOADING TITLE.....[OK]");
		
        }
		
}

void        init_level(void)

{
	int e = 0;
	int p = 0;
	
	p = powerup_gem_total;

     if (level == 1)
        //LEVEL1
        {
		//create_enemy_spider(enemy number (max 100),type,pos_x,pos_y,min_x,max_x,speed, number of hits)
		//1=spider, 2=bat, 3=spider boss, 4=fish, 5=bat boss
		enemy_total = 0;
		create_enemy_spider(&enemies[e],1,496,1008,-100,100,1,1);e++;
		create_enemy_spider(&enemies[e],1,1168,960,-300,100,1,1);e++;
		create_enemy_spider(&enemies[e],1,1592,864,-300,400,1,1);e++;
		create_enemy_spider(&enemies[e],2,264,225,-200,100,1,1);e++;
		create_enemy_spider(&enemies[e],2,2216,408,-200,100,1,1);e++;
		create_enemy_spider(&enemies[e],2,2544,712,-100,100,1,1);e++;
		create_enemy_spider(&enemies[e],2,2080,768,-100,100,1,1);e++;
		create_enemy_spider(&enemies[e],1,2080,1008,-200,100,1,1);e++;
		create_enemy_spider(&enemies[e],1,2432,960,-200,100,1,1);e++;
		create_enemy_spider(&enemies[e],1,3168,760,-90,90,1,1);e++;
		create_enemy_spider(&enemies[e],1,3480,856,-90,90,1,1);e++;
		create_enemy_spider(&enemies[e],1,4160,1000,-180,180,1,1);e++;
		create_enemy_spider(&enemies[e],1,4720,944,-200,60,1,1);e++;
		create_enemy_spider(&enemies[e],1,5800,864,-200,0,1,1);e++;
		create_enemy_spider(&enemies[e],2,3792,520,-90,90,1,1);e++;
		create_enemy_spider(&enemies[e],2,4280,160,-90,90,1,1);e++;
		create_enemy_spider(&enemies[e],2,4984,136,-50,50,1,1);e++;
		create_enemy_spider(&enemies[e],2,5280,136,-50,50,1,1);e++;
		create_enemy_spider(&enemies[e],4,4992,1040,-100,100,1,1);e++;;
		create_enemy_spider(&enemies[e],4,5280,1040,-100,100,1,1);e++;
		create_enemy_spider(&enemies[e],1,3520,296,-50,50,1,1);e++;
		create_enemy_spider(&enemies[e],1,6192,344,-50,50,1,1);e++;
		create_enemy_spider(&enemies[e],2,5568,488,-50,50,1,1);e++;
		create_enemy_spider(&enemies[e],2,6672,128,-50,50,1,1);e++;
		create_enemy_spider(&enemies[e],1,7168,912,-100,100,1,1);e++;
		create_enemy_spider(&enemies[e],1,6160,680,-100,100,1,1);e++;
		
		create_enemy_spider(&enemies[e],6,100,800,0,0,1,1);e++;
		
		//create spider boss
		create_enemy_spider(&enemies[e],3,8000,896,-200,100,1,6);e++;
		
			
		
		
		//create_powerup(powerup number (max 100),type,pos_x,pos_y); 1=health, 2=super health, 3=spider, 4=bat, 5=gem, 6=spider_ability, 7=bat_ability
		create_powerup(&powerups[p],1,1096,696,1);p++;
		create_powerup(&powerups[p],4,168,258,1);p++;
		create_powerup(&powerups[p],1,1848,1008,1);p++;
		create_powerup(&powerups[p],1,2014,529,1);p++;
		create_powerup(&powerups[p],1,3080,768,1);p++;
		create_powerup(&powerups[p],1,4184,824,1);p++;
		create_powerup(&powerups[p],1,3392,864,1);p++;
		create_powerup(&powerups[p],1,5688,624,1);p++;
		create_powerup(&powerups[p],2,5152,264,1);p++;
		create_powerup(&powerups[p],3,1288,760,1);p++;
		create_powerup(&powerups[p],3,2616,968,1);p++;
		create_powerup(&powerups[p],3,5776,872,1);p++;
		create_powerup(&powerups[p],3,2960,776,1);p++;
		create_powerup(&powerups[p],2,2920,776,1);p++;
		create_powerup(&powerups[p],6,8288,1000,1);p++;
		create_powerup(&powerups[p],3,232,1008,1);p++;
		create_powerup(&powerups[p],3,3800,760,1);p++;
		create_powerup(&powerups[p],4,4520,728,1);p++;
		create_powerup(&powerups[p],4,7071,585,1);p++;
		create_powerup(&powerups[p],2,7112,800,1);p++;
		create_powerup(&powerups[p],4,3613,120,1);p++;
		
		
		//WORLD_DEFAULT_X = 0;
		//WORLD_DEFAULT_Y = 850;
		map_max_x = 7944;
        water = false;
		game.music_track = 3;
        game.boss_start_x = 7624;
		game.boss_start_y = 1000;
        }

        if (level == 2)
        //LEVEL2
        {
		//enemies
		create_enemy_spider(&enemies[e],1,408,904,-100,100,1,1);e++;
		create_enemy_spider(&enemies[e],2,600,800,-200,100,1,1);e++;
		create_enemy_spider(&enemies[e],2,800,800,-200,100,1,1);e++;
		create_enemy_spider(&enemies[e],1,1124,1400,-50,50,1,1);e++;
		create_enemy_spider(&enemies[e],2,744,1384,-200,100,1,1);e++;
		create_enemy_spider(&enemies[e],2,1048,2104,-200,100,1,1);e++;
		create_enemy_spider(&enemies[e],2,1776,22096,-200,100,1,1);e++;
		create_enemy_spider(&enemies[e],1,560,2256,-100,100,1,1);e++;
		create_enemy_spider(&enemies[e],1,416,1736,-50,50,1,1);e++;
		create_enemy_spider(&enemies[e],4,1032,2360,-10,200,1,1);e++;
		create_enemy_spider(&enemies[e],4,1240,2360,-10,200,1,1);e++;
		create_enemy_spider(&enemies[e],1,2832,1728,-50,50,1,1);e++;
		create_enemy_spider(&enemies[e],2,1968,896,-200,100,1,1);e++;
		
		//create bat boss
		create_enemy_spider(&enemies[e],5,2952,900,-50,100,1,6);e++;
		
		//powerups
		create_powerup(&powerups[p],1,1044,948,2);p++;		
		create_powerup(&powerups[p],1,1184,1328,2);p++;
		create_powerup(&powerups[p],7,3232,1016,2);p++;
		create_powerup(&powerups[p],3,64,1912,2);p++;
		create_powerup(&powerups[p],1,1912,2200,2);p++;
		create_powerup(&powerups[p],1,3107,1777,2);p++;
		create_powerup(&powerups[p],2,2168,1024,2);p++;
		create_powerup(&powerups[p],2,432,1744,2);p++;
		create_powerup(&powerups[p],4,608,1736,2);p++;
		create_powerup(&powerups[p],3,2296,1696,2);p++;
		create_powerup(&powerups[p],4,1920,952,2);p++;
		create_powerup(&powerups[p],4,3040,1448,2);p++;
		create_powerup(&powerups[p],3,2128,800,2);p++;
		create_powerup(&powerups[p],4,2584,2256,2);p++;
		create_powerup(&powerups[p],4,432,912,2);p++;
		//create_powerup(&powerups[p],5,1568,1728,2);p++;//fish powerup
		
		map_max_x = 2840;
		water = true;
		game.music_track = 5;
		game.boss_start_x = 2520;
		game.boss_start_y = 950;
        }
		if (level == 3)
        //LEVEL2
        {
		//enemies
		create_enemy_spider(&enemies[e],1,408,904,-100,100,1,1);e++;
		create_enemy_spider(&enemies[e],2,600,800,-200,100,1,1);e++;
		create_enemy_spider(&enemies[e],2,800,800,-200,100,1,1);e++;
		create_enemy_spider(&enemies[e],1,1124,1400,-50,50,1,1);e++;
		create_enemy_spider(&enemies[e],2,744,1384,-200,100,1,1);e++;
		create_enemy_spider(&enemies[e],2,1048,2104,-200,100,1,1);e++;
		create_enemy_spider(&enemies[e],2,1776,22096,-200,100,1,1);e++;
		create_enemy_spider(&enemies[e],1,560,2256,-100,100,1,1);e++;
		create_enemy_spider(&enemies[e],1,416,1736,-50,50,1,1);e++;
		create_enemy_spider(&enemies[e],4,1032,2360,-10,200,1,1);e++;
		create_enemy_spider(&enemies[e],4,1240,2360,-10,200,1,1);e++;
		create_enemy_spider(&enemies[e],1,2832,1728,-50,50,1,1);e++;
		create_enemy_spider(&enemies[e],2,1968,896,-200,100,1,1);e++;
		
		//create bat boss
		create_enemy_spider(&enemies[e],5,2952,900,-50,100,1,6);e++;
		
		//powerups
		create_powerup(&powerups[p],1,1044,948,2);p++;		
		create_powerup(&powerups[p],1,1184,1328,2);p++;
		create_powerup(&powerups[p],7,3232,1016,2);p++;
		create_powerup(&powerups[p],3,64,1912,2);p++;
		create_powerup(&powerups[p],1,1912,2200,2);p++;
		create_powerup(&powerups[p],1,3107,1777,2);p++;
		create_powerup(&powerups[p],2,2168,1024,2);p++;
		create_powerup(&powerups[p],2,432,1744,2);p++;
		create_powerup(&powerups[p],4,608,1736,2);p++;
		create_powerup(&powerups[p],3,2296,1696,2);p++;
		create_powerup(&powerups[p],4,1920,952,2);p++;
		create_powerup(&powerups[p],4,3040,1448,2);p++;
		create_powerup(&powerups[p],3,2128,800,2);p++;
		create_powerup(&powerups[p],4,2584,2256,2);p++;
		create_powerup(&powerups[p],4,432,912,2);p++;
		//create_powerup(&powerups[p],5,1568,1728,2);p++;//fish powerup
		
		map_max_x = 2840;
		water = true;
		game.music_track = 5;
		game.boss_start_x = 2520;
		game.boss_start_y = 950;
        }
		
		level_end = map_max_x + 196;
		if(level==1 && player.spider_ability)
			map_max_x = level_end;

		if(level==2 && player.bat_ability)
			map_max_x = level_end;
		
		if(level==3 && player.bat_ability)
			map_max_x = level_end;
		
        enemy_total = e;	
		powerup_total = p;
		game.boss_music_track = 6;
		
}

void			level_start(void)
{
	if (game.game_state != GAMESTATE_LEVEL_START)
    return;

physics.speed_y = 0.0f;
physics.speed = 0.0f;

if (!is_cd_playing)
        {
             jo_audio_play_cd_track(game.music_track, game.music_track, true);
            is_cd_playing = true;
        }


	    jo_map_draw(WORLD_MAP_ID, map_pos_x, map_pos_y);
	jo_map_draw(MAP_FORE1_ID, map_pos_x, map_pos_y);
	
	//waterfall
	draw_waterfall();

   jo_map_draw(MAP_FORE3_ID, mapff_pos_x, map_pos_y);
    if (water)
        {
        //jo_sprite_enable_half_transparency();
		jo_sprite_enable_screen_doors_filter();
        jo_map_draw(MAP_WATER_ID, map_pos_x, map_pos_y);
        
		//jo_sprite_disable_half_transparency();
		jo_sprite_disable_screen_doors_filter();
				
        }
    else
    jo_map_draw(MAP_FORE2_ID, mapf_pos_x, map_pos_y);
	jo_sprite_enable_horizontal_flip();
	jo_sprite_disable_horizontal_flip();

    //background

    jo_sprite_draw3D2(bg_sprite, bg_x, 0, 1000);
	jo_sprite_draw3D2(bg_sprite, bg_x + bg_width, 0, 1000);
	jo_sprite_draw3D2(bg_sprite, bg_x + (bg_width + bg_width), 0, 1000);
	
	//if(level == 1)
	//{
	jo_sprite_draw3D2(bg_sprite2, bg_x2, bg_adj, 1500);
	jo_sprite_draw3D2(bg_sprite2, bg_x2 + bg_width, bg_adj, 1500);
	jo_sprite_draw3D2(bg_sprite2, bg_x2 + (bg_width + bg_width), bg_adj, 1500);	
	//}
	
	

    display_player();
	display_hud();
	//player_collision_handling();
	hamster_animation_handling();
	
	//level title
	
	if(game.title_pos_y < 90)
	{game.title_pos_y +=10;
	}
	if(game.title_pos_x > 111)
	{game.title_pos_x -=10;
	}
	
	
	jo_sprite_draw3D2(level_title_sprite, game.title_pos_x, 87,100);
	jo_sprite_change_sprite_scale_xy(5,5);
	jo_sprite_enable_half_transparency();
	jo_sprite_draw3D2(level_banner_sprite, 115, game.title_pos_y,110);
	jo_sprite_disable_half_transparency();
	jo_sprite_restore_sprite_scale();
	
	
	
	if(game.level_start_timer >= LEVEL_START_TIMER)
	game.game_state = GAMESTATE_GAMEPLAY;
	else
		game.level_start_timer++;
	
}


void            level_select(void)
{
int banner_y;//58,112,166

game.total_gems = game.level_1_gems + game.level_2_gems;

if (game.game_state != GAMESTATE_LEVEL_SELECT)
    return;

switch(select_level)
    {
        case 1:
            banner_y = 58;
            break;
		 case 2:
            banner_y = 112;
            break;
		case 3:
            banner_y = 166;
            break;
		default:
            banner_y = 58;
            break;
	}
	
	jo_sprite_draw3D2(bg_sprite, bg_x, 0, 500);
	jo_sprite_draw3D2(bg_sprite, bg_x + bg_width, 0, 500);
	jo_sprite_draw3D2(bg_sprite, bg_x + (bg_width + bg_width), 0, 500);
	if (bg_x == -184 )
            bg_x = 0;
            else
                bg_x -=1;
			
	jo_sprite_draw3D2(l_select_title, 118, 21, 200);
	jo_sprite_draw3D2(l_select_level_name1, 4, 76, 200);
	jo_sprite_draw3D2(l_select_l1, 90, 60, 200);
	jo_sprite_draw3D2(l_select_level_name2, 4, 132, 200);
	jo_sprite_draw3D2(l_select_l2, 90, 114, 200);
	jo_sprite_draw3D2(l_select_level_name3, 4, 188, 200);
	jo_sprite_draw3D2(l_select_l3, 90, 168, 200);
	
	jo_sprite_change_sprite_scale_xy(40,1);
	jo_sprite_enable_half_transparency();
	jo_sprite_draw3D2(l_select_banner, 156, banner_y, 400);
	jo_sprite_disable_half_transparency();
	jo_sprite_restore_sprite_scale();
        
    //leve11
	jo_sprite_draw3D2(player.gems_sprite_id_1,158, 58, 200);
	jo_sprite_draw3D2(player.clock_sprite_id,158, 84, 200);
	jo_nbg2_printf(23, 9, "x%2d", game.level_1_gems);
	jo_nbg2_printf(26, 9, "/25");
	jo_nbg2_printf(23, 12, "%02d:%02d", game.level_1_time / 60,game.level_1_time % 60);
	//level2
	if(game.total_gems >= LEVEL_2_GEMS_TO_UNLOCK)
	{
	game.level_2_locked = false;
	jo_sprite_draw3D2(player.gems_sprite_id_1,158, 114, 200);
	jo_sprite_draw3D2(player.clock_sprite_id,158, 140, 200);
	jo_nbg2_printf(23, 16, "x%2d", game.level_2_gems);
	jo_nbg2_printf(26, 16, "/25");
	jo_nbg2_printf(23, 19, "%02d:%02d", game.level_2_time / 60,game.level_2_time % 60);
	}
	else
	{
	game.level_2_locked = true;
	jo_nbg2_printf(21, 15, "*******************");
	jo_nbg2_printf(21, 16, "FIND %d",LEVEL_2_GEMS_TO_UNLOCK - game.total_gems);
	jo_nbg2_printf(21, 17, "MORE GEMS TO UNLOCK");
	jo_nbg2_printf(21, 18, "*******************");
	}
	//level3
	//level2
	if(game.total_gems >= LEVEL_3_GEMS_TO_UNLOCK)
	{
	game.level_2_locked = false;
	jo_sprite_draw3D2(player.gems_sprite_id_1,158, 170, 200);
	jo_sprite_draw3D2(player.clock_sprite_id,158, 196, 200);
	jo_nbg2_printf(23, 25, "x%2d", game.level_3_gems);
	jo_nbg2_printf(26, 25, "/25");
	jo_nbg2_printf(23, 28, "%02d:%02d", game.level_3_time / 60,game.level_3_time % 60);
	}
	else
	{
	game.level_3_locked = true;
	jo_nbg2_printf(21, 24, "*******************");
	jo_nbg2_printf(21, 25, "FIND %d",LEVEL_3_GEMS_TO_UNLOCK - game.total_gems);
	jo_nbg2_printf(21, 26, "MORE GEMS TO UNLOCK");
	jo_nbg2_printf(21, 27, "*******************");
	}

	
		


    if (jo_is_pad1_key_down(JO_KEY_UP))
    {
        //if(game.pressed_up == false)
       // {

            if (select_level == 1)
                select_level = LEVEL_MENU_MAX;
            else
                select_level --;


        }
       // game.pressed_up = true;
   // }
   // else
   // {
   //     game.pressed_up = false;
   // }

    if (jo_is_pad1_key_down(JO_KEY_DOWN))
    {
        //if(game.pressed_down == false)
        //{

            if (select_level == LEVEL_MENU_MAX)
                select_level = 1;
            else
                select_level ++;


        }
       // game.pressed_down = true;
   // }
   // else
   // {
    //    game.pressed_down = false;
   // }




         // did player one pause the game?
    if (jo_is_pad1_key_down(JO_KEY_START))
    {
        if(game.pressed_start == false)
        {
            level = select_level;
        jo_nbg2_printf(10, 10, "                  ");
		//set level gems
		switch(level)
    {
        case 1:
            player.gems = game.level_1_gems;
			game.level_locked = false;
            break;
		 case 2:
            player.gems = game.level_2_gems;
			game.level_locked = game.level_2_locked;
            break;
		
	}
	if(!game.level_locked)
		
		{
		clear_level_select();
        load_level();
		init_level();
        reset_demo();
		ztClearText();

        game.game_state = GAMESTATE_LEVEL_START;
		}
        }
       game.pressed_start = true;
    }
    else
    {
        game.pressed_start = false;
   }


        return;
}


void	add_gems(void)
{
int p = 0;
//level1
		create_powerup(&powerups[p],5,136,832,1);p++;
		create_powerup(&powerups[p],5,1096,160,1);p++;
		create_powerup(&powerups[p],5,5936,1016,1);p++;
		create_powerup(&powerups[p],5,4528,856,1);p++;
		create_powerup(&powerups[p],5,180,252,1);p++;
		create_powerup(&powerups[p],5,768,616,1);p++;
		create_powerup(&powerups[p],5,1848,816,1);p++;
		create_powerup(&powerups[p],5,6431,261,1);p++;
		create_powerup(&powerups[p],5,5932,504,1);p++;
		create_powerup(&powerups[p],5,16,0,1);p++;
		create_powerup(&powerups[p],5,392,176,1);p++;
		create_powerup(&powerups[p],5,5144,120,1);p++;
		create_powerup(&powerups[p],5,5000,832,1);p++;
		create_powerup(&powerups[p],5,6697,929,1);p++;
		create_powerup(&powerups[p],5,5288,336,1);p++;
		create_powerup(&powerups[p],5,6608,264,1);p++;
		create_powerup(&powerups[p],5,8280,696,1);p++;
		create_powerup(&powerups[p],5,5152,672,1);p++;
		create_powerup(&powerups[p],5,4000,656,1);p++;
		create_powerup(&powerups[p],5,4000,440,1);p++;
		create_powerup(&powerups[p],5,4544,400,1);p++;
		create_powerup(&powerups[p],5,7034,120,1);p++;
		create_powerup(&powerups[p],5,4480,88,1);p++;
		create_powerup(&powerups[p],5,5000,408,1);p++;
		create_powerup(&powerups[p],5,5288,408,1);p++;//25
//level2
		create_powerup(&powerups[p],5,1124,1032,2);p++;
		create_powerup(&powerups[p],5,1332,1400,2);p++;
		create_powerup(&powerups[p],5,1800,1728,2);p++;
		create_powerup(&powerups[p],5,56,1560,2);p++;
		create_powerup(&powerups[p],5,592,1536,2);p++;
		create_powerup(&powerups[p],5,1608,2336,2);p++;
		create_powerup(&powerups[p],5,3088,2112,2);p++;
		create_powerup(&powerups[p],5,2600,1728,2);p++;
		create_powerup(&powerups[p],5,2296,1752,2);p++;
		create_powerup(&powerups[p],5,600,1680,2);p++;
		create_powerup(&powerups[p],5,896,1560,2);p++;
		create_powerup(&powerups[p],5,1136,1680,2);p++;
		create_powerup(&powerups[p],5,1256,1680,2);p++;
		create_powerup(&powerups[p],5,1392,1680,2);p++;
		create_powerup(&powerups[p],5,1688,1840,2);p++;
		create_powerup(&powerups[p],5,1840,1840,2);p++;
		create_powerup(&powerups[p],5,2176,1952,2);p++;
		create_powerup(&powerups[p],5,1387,1905,2);p++;
		create_powerup(&powerups[p],5,1392,1680,2);p++;
		create_powerup(&powerups[p],5,3064,1152,2);p++;
		create_powerup(&powerups[p],5,2640,1200,2);p++;
		create_powerup(&powerups[p],5,2096,1160,2);p++;
		create_powerup(&powerups[p],5,3091,1441,2);p++;
		create_powerup(&powerups[p],5,865,2369,2);p++;
		create_powerup(&powerups[p],5,1529,840,2);p++;//25
		
		powerup_gem_total = p;

}


void            load_title_screen()
{
   	
	bg_sprite = jo_sprite_add_tga("BG", "TITL.TGA", JO_COLOR_Transparent);
	logo_sprite = jo_sprite_add_tga("BG", "ZYGO.TGA", JO_COLOR_Black);

}


void            load_sound(void)
{
    jo_audio_load_pcm("JUMP.PCM", JoSoundMono16Bit, &h_jump);
    h_jump.sample_rate = 27086;
    jo_audio_load_pcm("DIE.PCM", JoSoundMono16Bit, &h_die);
    h_die.sample_rate = 27086;
    jo_audio_load_pcm("LIGHT.PCM", JoSoundMono16Bit, &h_health);
    h_health.sample_rate = 27086;
    jo_audio_load_pcm("SPLASH.PCM", JoSoundMono16Bit, &h_splash);
    h_splash.sample_rate = 27086;
	jo_audio_load_pcm("FLAP.PCM", JoSoundMono16Bit, &h_flap);
    //h_flap.sample_rate = 27086;
	}

void getTime(jo_datetime* currentTime)
{
    SmpcDateTime *time = NULL;

    slGetStatus();

    time = &(Smpc_Status->rtc);

    currentTime->day = slDec2Hex(time->date);
    currentTime->year = slDec2Hex(time->year);
    currentTime->month = time->month & 0x0f;

    currentTime->hour = (char)slDec2Hex(time->hour);
    currentTime->minute = (char)slDec2Hex(time->minute);
    currentTime->second = (char)slDec2Hex(time->second);
}

unsigned int getSeconds()
{
    jo_datetime now = {0};
    unsigned int numSeconds = 0;

    getTime(&now);

    numSeconds = now.second + (now.minute * 60) + (now.hour * (60*60)) + (now.day * (24*60*60));

    return numSeconds;
}

jo_palette          *my_tga_palette_handling(void)
{
    // We create a new palette for each image. It's not optimal but OK for a demo.
    jo_create_palette(&image_pal);
    return (&image_pal);
}

void                    load_nbg2_font(void)
{
    jo_img_8bits        img;

    img.data = NULL;
    jo_tga_8bits_loader(&img, "TEX", "FONT.TGA", 2);
    jo_vdp2_set_nbg2_8bits_font(&img, " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!\"?=%&',.()*+-/", image_pal.id, false, true);
    jo_free_img(&img);
}

void			jo_main(void)
{
    player.loaded = false;

    // extend the heap to use LWRAM as well otherwise our sprites won't fit in
    // main memory
   jo_add_memory_zone((unsigned char *)LWRAM, LWRAM_HEAP_SIZE);
	
    game.game_state = GAMESTATE_UNINITIALIZED;

	jo_core_init(JO_COLOR_Black);
	jo_core_set_screens_order(JO_NBG2_SCREEN, JO_NBG0_SCREEN, JO_SPRITE_SCREEN);
	//slPriorityNbg3(7);
	
	jo_set_tga_palette_handling(my_tga_palette_handling);
	load_nbg2_font();
	
	load_player_and_enemies();
	load_hud();
	//load_enemy();
	load_powerups();
	load_sound();
	load_waterfall();
	load_title_screen();
	add_gems();
game.title_screen_timer = 0;
game.title_size = 1;
game.boss = false;
        //jo_malloc(level_sprites_1,40000);

       jo_physics_init_for_sonic(&physics);
		
		 // title screen
        jo_core_add_callback(title_screen);

        // level select
        jo_core_add_callback(level_select);
		
		//level start
		jo_core_add_callback(level_start);
		
		 // gameplay
        jo_core_add_callback(my_draw);
        jo_core_add_callback(my_gamepad);

        // paused
        jo_core_add_callback(pause_game);
		
		// end_level
        jo_core_add_callback(end_level);

        game.game_state = GAMESTATE_TITLE_SCREEN;


   // jo_nbg2_printf(10, 0, "MUTATOR v1.0");
	jo_core_run();
}

/*
** END OF FILE
*/
