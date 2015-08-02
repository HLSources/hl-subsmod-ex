/*
Copyright (C) 2014-2015, Subtitles MOD Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

This product contains software technology licensed from Id Software, Inc. ("Id Technology").
1997-2001 Id Software, Inc.
*/

#include "extdll.h"
#include "basemenu.h"
#include "utils.h"
#include "keydefs.h"
#include "menu_btnsbmp_table.h"
#include "menu_strings.h"

#define ART_MINIMIZE_N	"gfx/shell/min_n"
#define ART_MINIMIZE_F	"gfx/shell/min_f"
#define ART_MINIMIZE_D	"gfx/shell/min_d"
#define ART_CLOSEBTN_N	"gfx/shell/cls_n"
#define ART_CLOSEBTN_F	"gfx/shell/cls_f"
#define ART_CLOSEBTN_D	"gfx/shell/cls_d"


#define ID_BACKGROUND		0

// Main menu buttons
#define ID_BTN_CONSOLE			1
#define ID_BTN_RESUME			2
#define ID_BTN_NEWGAME			3
#define ID_BTN_OPTIONS			4
#define ID_BTN_SAVERESTORE		5	
#define ID_BTN_LANGUAGE			6	// Fograin92: Change language button
//#define ID_BTN_CREDITS			7
#define ID_BTN_EXTRAS			8
#define ID_BTN_QUIT				9

// New Game dialog
#define ID_NEWGAME_WINDOW		20
#define ID_NEWGAME_TEXT			21
#define ID_NEWGAME_BTN_YES	 	22
#define ID_NEWGAME_BTN_NO		23

// Quit Game dialog
#define ID_QUIT_WINDOW	 	30
#define ID_QUIT_TEXT		31
#define ID_QUIT_BTN_YES	 	32
#define ID_QUIT_BTN_NO		33




// Main menu structure
typedef struct
{
	menuFramework_s	menu;
	menuBitmap_s	background;

	// Main menu buttons
	menuPicButton_s	console;
	menuPicButton_s	resumeGame;
	menuPicButton_s	newGame;
	menuPicButton_s	configuration;
	menuPicButton_s	saveRestore;
	menuPicButton_s	customGame;
	//menuPicButton_s	credits;
	//menuPicButton_s	extras;
	menuPicButton_s	quit;

	// New Game dialog
	menuAction_s	newgameDialog;
	menuAction_s	newgameMessage;
	menuPicButton_s	newgameBtnYes;
	menuPicButton_s	newgameBtnNo;

	// Quit Game dialog
	menuAction_s	quitDialog;
	menuAction_s	quitMessage;
	menuPicButton_s	quitBtnYes;
	menuPicButton_s	quitBtnNo;

} uiMain_t;
static uiMain_t		uiMain;


//============================
// Function to over-draw menu
//============================
static void UI_Background_Ownerdraw( void *self )
{
	menuCommon_s	*item = (menuCommon_s *)self;

	// map has background
	if( CVAR_GET_FLOAT( "cl_background" ))
		return;

	UI_DrawBackground_Callback( self );

	//if (uiStatic.m_fHaveSteamBackground || uiStatic.m_fDisableLogo)
	//	return; // no logos for steam background
}


//==================================
// Enable/Disable main menu buttons
//==================================
static void UI_MainMenu_ToggleButtons( int iMenuID )
{
	// Enable/Disable menu buttons
	uiMain.console.generic.flags		^= QMF_INACTIVE|QMF_GRAYED;
	uiMain.resumeGame.generic.flags		^= QMF_INACTIVE|QMF_GRAYED;
	uiMain.newGame.generic.flags		^= QMF_INACTIVE|QMF_GRAYED;
	uiMain.configuration.generic.flags	^= QMF_INACTIVE|QMF_GRAYED;
	uiMain.saveRestore.generic.flags	^= QMF_INACTIVE|QMF_GRAYED;
	uiMain.customGame.generic.flags		^= QMF_INACTIVE|QMF_GRAYED;
	//uiMain.credits.generic.flags		^= QMF_INACTIVE|QMF_GRAYED;
	//uiMain.extras.generic.flags		^= QMF_INACTIVE|QMF_GRAYED;
	uiMain.quit.generic.flags			^= QMF_INACTIVE|QMF_GRAYED;


	// Show/Hide proper popup window/sub-menu
	if (iMenuID == ID_NEWGAME_WINDOW)
	{
		uiMain.newgameDialog.generic.flags ^= QMF_HIDDEN;
		uiMain.newgameMessage.generic.flags ^= QMF_HIDDEN;
		uiMain.newgameBtnNo.generic.flags ^= QMF_HIDDEN;
		uiMain.newgameBtnYes.generic.flags ^= QMF_HIDDEN;
	}
	else if (iMenuID == ID_QUIT_WINDOW)
	{
		uiMain.quitDialog.generic.flags ^= QMF_HIDDEN;
		uiMain.quitMessage.generic.flags ^= QMF_HIDDEN;
		uiMain.quitBtnNo.generic.flags ^= QMF_HIDDEN;
		uiMain.quitBtnYes.generic.flags ^= QMF_HIDDEN;
	}

}


//================================
// UI WINDOW -> NEW GAME DIALOG
//================================
static void UI_Window_NewGame( void *self )
{
	menuCommon_s	*item = (menuCommon_s *)self;

	// Draw border
	UI_DrawRectangleExt( item->x, item->y, item->width, item->height, 0xFFDBA500, 1 );
	// Draw center
	UI_FillRect( item->x, item->y, item->width, item->height, 0xCC000000 );
}

//================================
// UI WINDOW -> QUIT GAME DIALOG
//================================
static void UI_Window_Quit( void *self )
{
	menuCommon_s	*item = (menuCommon_s *)self;

	// Draw border
	UI_DrawRectangleExt( item->x, item->y, item->width, item->height, 0xFFDBA500, 1 );
	// Draw center
	UI_FillRect( item->x, item->y, item->width, item->height, 0xCC000000 );
}


//=======================
// Capture keybord input
//=======================
static const char *UI_Main_KeyFunc( int key, int down )
{
	// ESC key has been pressed
	if( down && key == K_ESCAPE )
	{
		if ( CL_IsActive())
		{
			//if( (!( uiMain.dlgMessage1.generic.flags & QMF_HIDDEN )) || (!( uiMain.quitMessage.generic.flags & QMF_HIDDEN )) )
			if( (!( uiMain.quitMessage.generic.flags & QMF_HIDDEN )) )
				UI_MainMenu_ToggleButtons(ID_QUIT_WINDOW);
			else
				UI_CloseMenu();
		}
		else 
			UI_MainMenu_ToggleButtons(ID_QUIT_WINDOW);
		return uiSoundNull;
	}
	return UI_DefaultKey( &uiMain.menu, key, down );
}


//============================================
// Activate buttons under certain conditions
//============================================
static void UI_Main_ActivateFunc( void )
{
	// Resume game should only be avaible when player is ingame
	if ( !CL_IsActive( ))
		uiMain.resumeGame.generic.flags |= QMF_HIDDEN;


	// Fograin92: We enable developer console always
	uiMain.console.generic.y = CL_IsActive() ? 180 : 230;
	UI_ScaleCoords( NULL, &uiMain.console.generic.y, NULL, NULL );
}


//=================================
// Buttons action/event handler
//=================================
static void UI_Main_Callback( void *self, int event )
{
	menuCommon_s	*item = (menuCommon_s *)self;

	if( event != QM_ACTIVATED )
		return;

	switch( item->id )
	{
		case ID_BTN_CONSOLE:
			UI_SetActiveMenu( FALSE );
			KEY_SetDest( KEY_CONSOLE );
		break;
	
		case ID_BTN_RESUME:
			UI_CloseMenu();
		break;
	
		case ID_BTN_NEWGAME:
			UI_MainMenu_ToggleButtons(ID_NEWGAME_WINDOW);
		break;

		case ID_BTN_OPTIONS:
			UI_Options_Menu();
		break;
	
		case ID_BTN_SAVERESTORE:
			if( CL_IsActive( ))
				UI_SaveLoad_Menu();
			else
				UI_LoadGame_Menu();
		break;
	
		case ID_BTN_LANGUAGE:
			UI_CustomGame_Menu();
		break;
	
		case ID_BTN_QUIT:
			UI_MainMenu_ToggleButtons(ID_QUIT_WINDOW);
		break;

	
		case ID_QUIT_BTN_YES:
			if( !( uiMain.quitMessage.generic.flags & QMF_HIDDEN ))
				CLIENT_COMMAND( FALSE, "quit\n" );
		break;
	
		case ID_QUIT_BTN_NO:
				UI_MainMenu_ToggleButtons(ID_QUIT_WINDOW);
		break;

		case ID_NEWGAME_BTN_YES:
			if( !( uiMain.newgameMessage.generic.flags & QMF_HIDDEN ))

				// Fograin92: temporary code
			if( CVAR_GET_FLOAT( "host_serverstate" ) && CVAR_GET_FLOAT( "maxplayers" ) > 1 )
		HOST_ENDGAME( "end of the game" );

	CVAR_SET_FLOAT( "skill", 1.0f );
	CVAR_SET_FLOAT( "deathmatch", 0.0f );
	CVAR_SET_FLOAT( "teamplay", 0.0f );
	CVAR_SET_FLOAT( "pausable", 1.0f ); // singleplayer is always allowing pause
	CVAR_SET_FLOAT( "coop", 0.0f );

	BACKGROUND_TRACK( NULL, NULL );

	CLIENT_COMMAND( FALSE, "hazardcourse\n" );
		break;
	
		case ID_NEWGAME_BTN_NO:
				UI_MainMenu_ToggleButtons(ID_NEWGAME_WINDOW);
		break;
	}
}

/*
=================
UI_Main_Init
=================
*/
static void UI_Main_Init( void )
{
	memset( &uiMain, 0, sizeof( uiMain_t ));

	uiMain.menu.vidInitFunc = UI_Main_Init;
	uiMain.menu.keyFunc = UI_Main_KeyFunc;
	uiMain.menu.activateFunc = UI_Main_ActivateFunc;

	// Fograin92: Set background
	uiMain.background.generic.id = ID_BACKGROUND;
	uiMain.background.generic.type = QMTYPE_BITMAP;
	uiMain.background.generic.flags = QMF_INACTIVE;
	uiMain.background.generic.x = 0;
	uiMain.background.generic.y = 0;
	uiMain.background.generic.width = 1024;
	uiMain.background.generic.height = 768;
	uiMain.background.pic = ART_BACKGROUND;
	uiMain.background.generic.ownerdraw = UI_Background_Ownerdraw;

	// Fograin92: Console button
	uiMain.console.generic.id = ID_BTN_CONSOLE;
	uiMain.console.generic.type = QMTYPE_BM_BUTTON;
	uiMain.console.generic.name = "Console";
	uiMain.console.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW;
	uiMain.console.generic.x = 72;
	uiMain.console.generic.y = CL_IsActive() ? 180 : 230;
	uiMain.console.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.console, PC_CONSOLE );

	// Fograin92: Resume game button
	uiMain.resumeGame.generic.id = ID_BTN_RESUME;
	uiMain.resumeGame.generic.type = QMTYPE_BM_BUTTON;
	uiMain.resumeGame.generic.name = "Resume game";
	uiMain.resumeGame.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.resumeGame.generic.x = 72;
	uiMain.resumeGame.generic.y = 230;
	uiMain.resumeGame.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.resumeGame, PC_RESUME_GAME );

	// Fograin92: New game button
	uiMain.newGame.generic.id = ID_BTN_NEWGAME;
	uiMain.newGame.generic.type = QMTYPE_BM_BUTTON;
	uiMain.newGame.generic.name = "New game";
	uiMain.newGame.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.newGame.generic.x = 72;
	uiMain.newGame.generic.y = 280;
	uiMain.newGame.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.newGame, PC_NEW_GAME );




	// Fograin92: Hazard Course / Boot Camp button
	/*
	uiMain.hazardCourse.generic.id = ID_HAZARDCOURSE;
	uiMain.hazardCourse.generic.type = QMTYPE_BM_BUTTON;
	uiMain.hazardCourse.generic.name = "Hazard course";
	uiMain.hazardCourse.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.hazardCourse.generic.x = 72;
	uiMain.hazardCourse.generic.y = 330;
	uiMain.hazardCourse.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.hazardCourse, PC_HAZARD_COURSE );
	*/

	// Fograin92: SAVE or LOAD game buttons
	uiMain.saveRestore.generic.id = ID_BTN_SAVERESTORE;
	uiMain.saveRestore.generic.type = QMTYPE_BM_BUTTON;
	uiMain.saveRestore.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	if( CL_IsActive( ))
	{
		uiMain.saveRestore.generic.name = "Save\\Load Game";
		UI_UtilSetupPicButton(&uiMain.saveRestore,PC_SAVE_LOAD_GAME);
	}
	else
	{
		uiMain.saveRestore.generic.name = "Load Game";
		uiMain.resumeGame.generic.flags |= QMF_HIDDEN;
		UI_UtilSetupPicButton( &uiMain.saveRestore, PC_LOAD_GAME );
	}
	uiMain.saveRestore.generic.x = 72;
	uiMain.saveRestore.generic.y = 380;
	uiMain.saveRestore.generic.callback = UI_Main_Callback;

	// Fograin92: Options button
	uiMain.configuration.generic.id = ID_BTN_OPTIONS;
	uiMain.configuration.generic.type = QMTYPE_BM_BUTTON;
	uiMain.configuration.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.configuration.generic.name = "Configuration";
	uiMain.configuration.generic.x = 72;
	uiMain.configuration.generic.y = 430;
	uiMain.configuration.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.configuration, PC_CONFIG );

	// Fograin92: Change language button
	uiMain.customGame.generic.id = ID_BTN_LANGUAGE;
	uiMain.customGame.generic.type = QMTYPE_BM_BUTTON;
	uiMain.customGame.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.customGame.generic.name = "Custom Game";
	uiMain.customGame.generic.x = 72;
	uiMain.customGame.generic.y = 480;
	uiMain.customGame.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.customGame, PC_CUSTOM_GAME );

	// Fograin92: Quit game button
	uiMain.quit.generic.id = ID_BTN_QUIT;
	uiMain.quit.generic.type = QMTYPE_BM_BUTTON;
	uiMain.quit.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.quit.generic.name = "Quit";
	uiMain.quit.generic.x = 72;
	uiMain.quit.generic.y = 530;
	uiMain.quit.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.quit, PC_QUIT );





	// New Game dialog
	uiMain.newgameDialog.generic.id = ID_NEWGAME_WINDOW;
	uiMain.newgameDialog.generic.type = QMTYPE_ACTION;
	uiMain.newgameDialog.generic.flags = QMF_INACTIVE|QMF_HIDDEN;
	uiMain.newgameDialog.generic.ownerdraw = UI_Window_NewGame; // just a fill rectangle
	uiMain.newgameDialog.generic.x = 192;
	uiMain.newgameDialog.generic.y = 256;
	uiMain.newgameDialog.generic.width = 640;
	uiMain.newgameDialog.generic.height = 256;

	uiMain.newgameMessage.generic.id = ID_NEWGAME_WINDOW;
	uiMain.newgameMessage.generic.type = QMTYPE_ACTION;
	uiMain.newgameMessage.generic.flags = QMF_INACTIVE|QMF_DROPSHADOW|QMF_HIDDEN|QMF_CENTER_JUSTIFY;
	uiMain.newgameMessage.generic.name = "Start new game?";
	uiMain.newgameMessage.generic.x = 192;
	uiMain.newgameMessage.generic.y = 280;
	uiMain.newgameMessage.generic.width = 640;
	uiMain.newgameMessage.generic.height = 256;

	uiMain.newgameBtnYes.generic.id = ID_NEWGAME_BTN_YES;
	uiMain.newgameBtnYes.generic.type = QMTYPE_BM_BUTTON;
	uiMain.newgameBtnYes.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	uiMain.newgameBtnYes.generic.name = "Ok";
	uiMain.newgameBtnYes.generic.x = 380;
	uiMain.newgameBtnYes.generic.y = 460;
	uiMain.newgameBtnYes.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.newgameBtnYes, PC_OK );

	uiMain.newgameBtnNo.generic.id = ID_NEWGAME_BTN_NO;
	uiMain.newgameBtnNo.generic.type = QMTYPE_BM_BUTTON;
	uiMain.newgameBtnNo.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	uiMain.newgameBtnNo.generic.name = "Cancel";
	uiMain.newgameBtnNo.generic.x = 530;
	uiMain.newgameBtnNo.generic.y = 460;
	uiMain.newgameBtnNo.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.newgameBtnNo, PC_CANCEL );




	// Quit Game dialog
	uiMain.quitDialog.generic.id = ID_QUIT_WINDOW;
	uiMain.quitDialog.generic.type = QMTYPE_ACTION;
	uiMain.quitDialog.generic.flags = QMF_INACTIVE|QMF_HIDDEN;
	uiMain.quitDialog.generic.ownerdraw = UI_Window_Quit; // just a fill rectangle
	uiMain.quitDialog.generic.x = 192;
	uiMain.quitDialog.generic.y = 256;
	uiMain.quitDialog.generic.width = 640;
	uiMain.quitDialog.generic.height = 256;

	uiMain.quitMessage.generic.id = ID_QUIT_WINDOW;
	uiMain.quitMessage.generic.type = QMTYPE_ACTION;
	uiMain.quitMessage.generic.flags = QMF_INACTIVE|QMF_DROPSHADOW|QMF_HIDDEN|QMF_CENTER_JUSTIFY;
	uiMain.quitMessage.generic.name = "Are you sure?";
	uiMain.quitMessage.generic.x = 192;
	uiMain.quitMessage.generic.y = 280;
	uiMain.quitMessage.generic.width = 640;
	uiMain.quitMessage.generic.height = 256;

	uiMain.quitBtnYes.generic.id = ID_QUIT_BTN_YES;
	uiMain.quitBtnYes.generic.type = QMTYPE_BM_BUTTON;
	uiMain.quitBtnYes.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	uiMain.quitBtnYes.generic.name = "Ok";
	uiMain.quitBtnYes.generic.x = 380;
	uiMain.quitBtnYes.generic.y = 460;
	uiMain.quitBtnYes.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.quitBtnYes, PC_OK );

	uiMain.quitBtnNo.generic.id = ID_QUIT_BTN_NO;
	uiMain.quitBtnNo.generic.type = QMTYPE_BM_BUTTON;
	uiMain.quitBtnNo.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	uiMain.quitBtnNo.generic.name = "Cancel";
	uiMain.quitBtnNo.generic.x = 530;
	uiMain.quitBtnNo.generic.y = 460;
	uiMain.quitBtnNo.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.quitBtnNo, PC_CANCEL );









	// Fograin92: Add items
	UI_AddItem( &uiMain.menu, (void *)&uiMain.background );
	if ( gpGlobals->developer )
		UI_AddItem( &uiMain.menu, (void *)&uiMain.console );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.resumeGame );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.newGame );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.saveRestore );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.configuration );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.customGame );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.quit );

	UI_AddItem( &uiMain.menu, (void *)&uiMain.newgameDialog );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.newgameMessage );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.newgameBtnNo );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.newgameBtnYes );
	
	UI_AddItem( &uiMain.menu, (void *)&uiMain.quitDialog );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.quitMessage );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.quitBtnNo );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.quitBtnYes );
}

/*
=================
UI_Main_Precache
=================
*/
void UI_Main_Precache( void )
{
	PIC_Load( ART_BACKGROUND );
	PIC_Load( ART_MINIMIZE_N );
	PIC_Load( ART_MINIMIZE_F );
	PIC_Load( ART_MINIMIZE_D );
	PIC_Load( ART_CLOSEBTN_N );
	PIC_Load( ART_CLOSEBTN_F );
	PIC_Load( ART_CLOSEBTN_D );
}

/*
=================
UI_Main_Menu
=================
*/
void UI_Main_Menu( void )
{
	UI_Main_Precache();
	UI_Main_Init();
	UI_PushMenu( &uiMain.menu );
}