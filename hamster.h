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

#ifndef __HAMSTER_H__
# define __HAMSTER_H__

#define MAP_BACK1_ID                    (4)
#define WORLD_MAP_ID		            (0)
#define MAP_FORE1_ID	                (1)
#define MAP_FORE2_ID	                (2)
#define MAP_FORE3_ID	                (3)
#define MAP_WATER_ID	                (4)

# define WORLD_DEFAULT_X                (0)
# define WORLD_DEFAULT_Y                (850)//850

# define MAP_TILE_BLOCK_ATTR            (1)
# define MAP_TILE_BLOCK_ATTR_CEILING    (3)

# define HAMSTER_WIDTH                   (40)
# define HAMSTER_WIDTH_2                 (20)
# define HAMSTER_HEIGHT                  (58)
# define HAMSTER_HEIGHT_2                (28)

# define PLAYER_HURT_TIMER				(300)
# define PLAYER_HURT_TIMER_2			(180)
# define ENEMY_HURT_TIMER				(300)

# define FISH_WIDTH                   (56)
# define FISH_WIDTH_2                 (28)
# define FISH_HEIGHT                  (32)
# define FISH_HEIGHT_2                (16)

# define SPIDER_WIDTH                   (56)
# define SPIDER_WIDTH_2                 (28)
# define SPIDER_HEIGHT                  (32)
# define SPIDER_HEIGHT_2                (16)

# define FROG_WIDTH                   (28)
# define FROG_WIDTH_2                 (28)
# define FROG_HEIGHT                  (28)
# define FROG_HEIGHT_2                (16)

# define SPIDER_BOSS_WIDTH                   (224)//224
# define SPIDER_BOSS_WIDTH_2                 (112)//112
# define SPIDER_BOSS_HEIGHT                  (48)//128
# define SPIDER_BOSS_HEIGHT_2                (24)//64

# define SPIDER_BLINK_TIMER             (380)
# define PUP_TIMER             (380)

# define BAT_WIDTH                   (56)
# define BAT_WIDTH_2                 (28)
# define BAT_HEIGHT                  (56)
# define BAT_HEIGHT_2                (28)

# define BAT_BOSS_WIDTH                   (224)//224
# define BAT_BOSS_WIDTH_2                 (112)//112
# define BAT_BOSS_HEIGHT                  (48)//128
# define BAT_BOSS_HEIGHT_2                (24)//64

# define BAT_FLAP_TIMER              (50)
# define LEVEL_START_TIMER              (120)
# define TITLE_SCREEN_TIMER              (120)
# define DEATH_TIMER              (120)

//define game states
#define GAMESTATE_UNINITIALIZED         (0)
#define GAMESTATE_TITLE_SCREEN          (1)
#define GAMESTATE_LEVEL_SELECT          (2)
#define GAMESTATE_GAMEPLAY              (3)
#define GAMESTATE_PAUSED                (4)
#define GAMESTATE_END_LEVEL             (5)
#define GAMESTATE_GAME_OVER             (6)
#define GAMESTATE_VICTORY               (7)
#define GAMESTATE_LEVEL_START           (8)


//define menu options
#define END_LEVEL_MENU_MAX               (1)
#define PAUSE_MENU_MAX               (2)
#define LEVEL_MENU_MAX               (3)

//total gems needed to unlock levels
#define LEVEL_2_GEMS_TO_UNLOCK             (10)
#define LEVEL_3_GEMS_TO_UNLOCK             (30)

//make unused ram available
#define LWRAM                           (0x00200000) // start of LWRAM memory. Doesn't appear to be used
#define LWRAM_HEAP_SIZE                 (0x15000) // number of bytes to extend heap by




/* If sonic almost touch the ground we allow the user to jump */
# define PLAYER_JUMP_PER_PIXEL_TOLERANCE (10)

//static int spider_sprite_id;




typedef struct  _PLAYER
{
	int			fish_sprite_id;
    int         bat_sprite_id;
    int         bat_sprite_flap_id;
   // int         hamster_sprite_id;
    int         spider_anim_id;
    int         hamster_anim_id;
    int         spider_blink_anim_id;
	int			frog_anim_id;
	int			frog_sprite_id_1;
	int			frog_sprite_id_2;
    int         type;
    int         x;
    int         y;
    bool        flip_player;
    bool        can_jump;
    int         health;
    int         health_sprite_id_1;
    int         health_sprite_id_2;
    int         health_sprite_id_3;
	int			gems;
    int         gems_sprite_id_1;
    int         clock_sprite_id;
    int         height;
	int			height2;
    int         width;
    int         width2;
    bool        can_shoot;
    int        rotate_player;
    int         frame_timer;
	int         pup_timer;
    bool        on_ladder;
    bool        loaded;
	bool		in_water;
	int			hurt_timer;
	bool		can_be_hurt;
	bool		mutate;
	bool		spider_ability;
	bool		bat_ability;
	bool		fish_ability;
	bool		can_splash;
	


}               player_params;
extern player_params player;


typedef struct	_ENEMY
{
	int			type;
	int			x;
	int			y;
	int         height;
	int			height2;
    int         width;
    int         width2;
	int			min_x;
	int			max_x;
	int			min_y;
	int			max_y;
	bool		alive;
	int			hits;
	bool		hurt;
	bool		can_be_hurt;
	int			hurt_timer;
	float			speed_y;
	int			speed_x;
	bool		left;
	int			x_limit;
	int			y_limit;
	int			up;
	int			rotation;
	bool		enemy_jumping;

	
}				enemy;
extern enemy 	enemies[50];

typedef struct	_POWERUP
{

	int			type;
	int			x;
	int			y;
	bool		used;
	bool		used_saved;
	int			lev;

	
}				powerup;
extern powerup	powerups[200];

typedef struct  _GAME
{
    // game state variables
    int         game_state;
    int         pause_menu;
	int			end_level_menu;
    bool        pressed_start;
    bool        pressed_up;
    bool        pressed_down;
	bool        pressed_X;
	int			start_time;
	int			current_time;
	int			pause_start_time;
	int			pause_time;
	int			end_time;
	bool		debug;
	int			music_track;
	int			total_gems;
	int			level_1_gems;
	int			level_2_gems;
	int			level_3_gems;
	int			total_time;
	int			level_1_time;
	int			level_2_time;
	int			level_3_time;
	bool		level_locked;
	bool		level_2_locked;
	bool		level_3_locked;
	int			enemy_spider_anim_id;
	int			enemy_bat_anim_id;
	int			enemy_frog_anim_id;
	int			enemy_frog_sprite_id_1;
	int			enemy_frog_sprite_id_2;
	int			enemy_splat_sprite_id;
	int			enemy_fish_sprite_id;
	int 		spider_corpse_sprite_id;
	int 		bat_corpse_sprite_id;
	int 		pup_health_anim_id;
	int 		pup_super_health_anim_id;
	int 		stars_anim_id;
	int 		pup_spider_anim_id;
	int 		pup_bat_anim_id;
	int 		pup_gem_anim_id;
	int 		wfall_anim_id;
	int 		wave_anim_id;
	int 		player_splash_anim_id;
	int 		wfall_splash_anim_id;
	int			level_start_timer;
	int			title_screen_timer;
	bool			start_timer;
	int			title_pos_x;
	int 		title_pos_y;
	int			title_size;
	int			boss_start_x;
	int			boss_start_y;
	int			boss_music_track;
	bool			boss;
	int			game_frame;

	
}               game_params;

extern game_params game;


/* 4 tiles */
static const jo_tile    HAM_Tileset[] =
{
	{0, 0, 40, 56},
	{40, 0, 40, 56},
	{80, 0, 40, 56},
	{120, 0, 40, 56}
};

/* 12 tiles */
static const jo_tile    SPIDER_Tileset[] =
{
	{0, 0, 56, 32},//w1
	{56, 0, 56, 32},//w2
	{0, 32, 56, 32},//w3
	{56, 32, 56, 32},//w4
	{0, 64, 32, 24},//b1
	{32, 64, 32, 24},//b2
	{64, 64, 32, 24},//b3
	{0, 88, 32, 24},//b4
	{32, 88, 32, 24},//b5	
	{64, 88, 32, 24},//b6
	{0, 112, 56, 56},//BAT1
	{56, 112, 56, 56},//BAT2
	{112, 0, 56, 56},//FROG1
	{112, 56, 56, 56}//FROG2
	
};

/* 12 tiles */
static const jo_tile    SPIDER2_Tileset[] =
{
	{0, 0, 56, 32},//w1
	{56, 0, 56, 32},//w2
	{0, 32, 56, 32},//w3
	{56, 32, 56, 32},//w4
	{0, 64, 32, 24},//b1
	{32, 64, 32, 24},//b2
	{64, 64, 32, 24},//b3
	{0, 88, 32, 24},//b4
	{32, 88, 32, 24},//b5	
	{64, 88, 32, 24},//b6
	{0, 112, 56, 56},//BAT1
	{56, 112, 56, 56},//BAT2
	{112, 0, 56, 56},//FROG1
	{112, 56, 56, 56}//FROG2
	
};

/* 6 tiles */
static const jo_tile    EDEAD_Tileset[] =
{
	{0, 0, 56, 40}, //ESPID_D4
	{0, 40, 56, 40},//ESPID_D5
	{0, 80, 56, 40},//ESPID_D6
	{0, 172, 8, 1},//BLANK
	{0, 120, 56, 20},//ESPID_D7
	{0, 140, 56, 32}//EBAT_D
	
};

/* 20 tiles */
static const jo_tile    PUP_Tileset[] =
{
	{0, 0, 16, 16},//HEALTH1
	{16, 0, 16, 16},//HEALTH2
	{32, 0, 16, 16},//HEALTH3
	{0, 16, 16, 16},//SUPERHEALTH1
	{16, 16, 16, 16},//SUPERHEALTH2
	{32, 16, 16, 16},//SUPERHEALTH3
	{0, 32, 16, 16},//SPIDER1
	{16, 32, 16, 16},//SPIDER2
	{32, 32, 16, 16},//SPIDER3
	{0, 48, 16, 16},//BAT1
	{16, 48, 16, 16},//BAT2
	{32, 48, 16, 16},//BAT3
	{0, 64, 24, 24},//GEM1
	{24, 64, 24, 24},//GEM2
	{0, 88, 24, 24},//GEM3
	{24, 88, 16, 16},//STARS1
	{0, 112, 16, 16},//STARS2
	{16, 112, 16, 16},//STARS3
	{32, 112, 16, 16},//STARS4
	{24, 102, 8, 1}//BLANK
	
};


/* 13 tiles */
static const jo_tile    WATER_Tileset[] =
{
	{0, 0, 48, 48},
	{0, 48, 48, 48},
	{48, 0, 48, 48},
	{48, 48, 48, 48},
	{96, 0, 48, 16},
	{96, 16, 48, 16},
	{96, 32, 48, 8},
	{96, 40, 48, 8},
	{96, 48, 48, 8},
	{96, 56, 48, 8},
	{144, 0, 40, 48},
	{144, 48, 40, 48},
	{96, 0, 8, 1}
};

void            hamster_animation_handling();

void            display_player(void);
void            load_player_and_enemies(void);

void            display_hud(void);
void            load_hud(void);
void            load_powerups(void);
void            load_waterfall(void);
void            clear_anim(void);

#endif /* !__HAMSTER_H__ */

/*
** END OF FILE
*/
