#include "..\Menu.h"
#include "..\..\SDK\ISurface.h"
#include "..\..\Utils\Color.h"
#include "..\..\Utils\GlobalVars.h"
bool dont_recieve_input = false;

// yes just paste shit in here not in render manager oh right forgot we dont even have a proper render manager we just stuck a couple 
// of fucking functions in g_pSurface and in other random places of the source good job gamers -swoopae
void GradientV( int x, int y, int w, int h, Color c1, Color c2 )
{
	g_pSurface->FilledRect( x, y, w, h, c1 );
	BYTE first = c2.red;
	BYTE second = c2.green;
	BYTE third = c2.blue;
	for ( int i = 0; i < h; i++ )
	{
		float fi = i, fh = h;
		float a = fi / fh;
		DWORD ia = a * 255;
		g_pSurface->FilledRect( x, y + i, w, 1, Color( first, second, third, ia ) );
	}
}

void GradientVEnd( int x, int y, int w, int h, Color c1, Color c2 )
{
	//g_pSurface->FilledRect(x, y, w, h, c1);
	BYTE first = c2.red;
	BYTE second = c2.green;
	BYTE third = c2.blue;
	for ( float i = 0; i < h / 1.5; i++ )
	{
		float fi = i, fh = h / 1.5;
		float a = fi / fh;
		DWORD ia = a * 255;
		g_pSurface->FilledRect( x, y + i, w, 2, Color( first, second, third, ia ) );
	}
}

void GradientH( int x, int y, int w, int h, Color c1, Color c2 )
{
	g_pSurface->FilledRect( x, y, w, h, c1 );
	BYTE first = c2.red;
	BYTE second = c2.green;
	BYTE third = c2.blue;
	for ( int i = 0; i < w; i++ )
	{
		float fi = i, fw = w;
		float a = fi / fw;
		DWORD ia = a * 255;
		g_pSurface->FilledRect( x + i, y, 1, h, Color( first, second, third, ia ) );
	}
}

void c_menu::checkbox( std::string name, bool* item, std::string tooltip ) {
	if ( groupbox_bottom <= y_offset + 16 )
		return;

	if ( groupbox_top >= y_offset + 16 )
		return;

	int size = 20;

	static bool pressed = false;

	auto text_size = get_text_size( g::fonts::menu_font, name.c_str( ) );

	if ( g_Menu.menu_opened ) {
		if ( mouse_in_params( x_offset + 7, y_offset, size + text_size.right, size ) )
			g_Menu.tooltip = tooltip;

		if ( !GetAsyncKeyState( VK_LBUTTON ) && mouse_in_params( x_offset + 7, y_offset, size + text_size.right, size ) )
		{
			if ( pressed )
				* item = !*item;
			pressed = false;
		}

		if ( GetAsyncKeyState( VK_LBUTTON ) && mouse_in_params( x_offset + 7, y_offset, size + text_size.right, size ) && !pressed )
			pressed = true;
	}

	g_pSurface->FilledRect(x_offset + 8, y_offset, size, size, Color(62, 62, 62, 255));

	if ( *item == true )
		g_pSurface->FilledRect( x_offset + 9, y_offset + 1, size - 2, size - 2, g_Menu.Config.AccentColour);

	g_pSurface->OutlinedRect( x_offset + 8, y_offset, size, size, Color( 32, 32, 32, 145 / 2 ) );
	g_pSurface->DrawT( x_offset + 32, y_offset, Color( 255, 255, 255, 255 ), g::fonts::menu_font, false, name.c_str( ) );

	y_offset += 24;
}

void c_menu::text(std::string name) {
	if (groupbox_bottom <= y_offset + 16)
		return;

	if (groupbox_top >= y_offset + 16)
		return;

	g_pSurface->DrawT(x_offset + 9, y_offset, Color(255, 255, 255, 255), g::fonts::menu_font, false, name.c_str());

	y_offset += 24;
}

void c_menu::button( std::string name, bool* item ) {
	if ( groupbox_bottom <= y_offset + 16 )
		return;

	if ( groupbox_top >= y_offset + 16 )
		return;

	int size = 20;

	static bool pressed = false;

	auto text_size = get_text_size( g::fonts::menu_font, name.c_str( ) );

	if ( g_Menu.menu_opened ) {

		if ( !GetAsyncKeyState( VK_LBUTTON ) && mouse_in_params( x_offset + 7, y_offset, 2 + text_size.right, size ) )
		{
			if ( pressed )
				* item = !*item;
			pressed = false;
		}

		if ( GetAsyncKeyState( VK_LBUTTON ) && mouse_in_params( x_offset + 7, y_offset, 2 + text_size.right, size ) && !pressed )
			pressed = true;
	}

	if ( *item == true )
		g_pSurface->FilledRect( x_offset + 8, y_offset, 2 + text_size.right, size, Color( 82, 82, 82, 255 ) );
	else
		g_pSurface->FilledRect( x_offset + 8, y_offset, 2 + text_size.right, size, Color( 62, 62, 62, 255 ) );

	g_pSurface->OutlinedRect( x_offset + 8, y_offset, 2 + text_size.right, size, Color( 32, 32, 32, 145 / 2 ) );
	g_pSurface->DrawT( x_offset + 8 + text_size.right / 2 + 1, y_offset, Color( 255, 255, 255, 255 ), g::fonts::menu_font, true, name.c_str( ) );

	y_offset += 24;
}

void c_menu::groupbox( int x, int y, int w, int h, std::string name ) {
	g_pSurface->FilledRect( _pos.x + x, _pos.y + y, w, h, Color( 35, 35, 35, 255 ) );
	g_pSurface->OutlinedRect( _pos.x + x, _pos.y + y, w, h, Color( 25, 25, 25, 255 ) );

	y_offset = ( _pos.y + ( y + 12 ) );
	x_offset = _pos.x + x;
	groupbox_bottom = ( _pos.y + ( y + h ) );
	groupbox_top = ( _pos.y + y );
	groupbox_width = ( _pos.x + ( x + w ) );
}

void c_menu::slider( int max, std::string name, int* item, std::string suffix, int step )
{
	if ( groupbox_bottom <= y_offset + 30 )
		return;

	float slider_size = 390 - 16; // 163

	float _pixel_value = max / slider_size;

	if ( g_Menu.menu_opened ) {
		if ( GetAsyncKeyState( VK_LBUTTON ) && mouse_in_params( x_offset + 8, y_offset + 19, slider_size + 1, 15 ) )
			* item = abs( mouse_pos( ).x - ( x_offset + 8 ) ) * _pixel_value;

		static bool pressed_subtract = false;
		static bool pressed_add = false;

		if ( *item > max ) {
			*item = max;
		}

		if ( *item < 0 ) {
			*item = 0;
		}
	}

	std::string namedisplay;

	namedisplay += name.c_str( );
	namedisplay += " (";
	namedisplay += std::to_string( *item );
	namedisplay += ")";

	g_pSurface->DrawT( x_offset + 8, y_offset - 3, Color( 255, 255, 255, 255 ), g::fonts::menu_font, false, namedisplay.c_str( ) );

	g_pSurface->FilledRect( x_offset + 8, y_offset + 19, slider_size, 15, Color( 62, 62, 62, 255 ) );

	if ( *item )
	{
		if ( *item == max )
			g_pSurface->FilledRect( x_offset + 8, y_offset + 19, slider_size, 15, g_Menu.Config.AccentColour);
		else
			g_pSurface->FilledRect( x_offset + 8, y_offset + 19, ( *item / _pixel_value ), 15, g_Menu.Config.AccentColour);
	}

	g_pSurface->OutlinedRect( x_offset + 8, y_offset + 19, slider_size, 15, Color( 32, 32, 32, 145 / 2 ) );

	y_offset += 38;
}

void c_menu::combobox( int size, std::string name, std::string * itemname, int* item )
{
	if ( groupbox_bottom <= y_offset + 41 )
		return;

	y_offset += 2;

	bool pressed = false;
	bool open = false;
	static bool selected_opened = false;
	static bool click_rest;
	static bool rest;
	static std::string name_selected;

	int ComboSize = 390 - 16; // 163

	if ( g_Menu.menu_opened ) {
		if ( GetAsyncKeyState( VK_LBUTTON ) && mouse_in_params( x_offset + 8, y_offset + 14, ComboSize, 17 ) && !click_rest )
		{
			name_selected = name;
			pressed = true;
			click_rest = true;
		}
		else if ( !GetAsyncKeyState( VK_LBUTTON ) && mouse_in_params( x_offset + 8, y_offset + 14, ComboSize, 17 ) )
			click_rest = false;

		if ( pressed )
		{
			if ( !rest )
				selected_opened = !selected_opened;
			rest = true;
		}
		else
			rest = false;

		if ( name_selected == name )
			open = selected_opened;
	}

	g_pSurface->DrawT( x_offset + 8, y_offset - 5, Color( 255, 255, 255, 255 ), g::fonts::menu_font, false, name.c_str( ) );
	g_pSurface->FilledRect( x_offset + 8, y_offset + 17, ComboSize, 17, Color( 62, 62, 62, 255 ) );
	g_pSurface->OutlinedRect( x_offset + 8, y_offset + 17, ComboSize, 17, Color( 32, 32, 32, 145 / 2 ) );

	if ( open )
	{
		g_pSurface->FilledRect( x_offset + 8, y_offset + 17 + 19, ComboSize, ( size * 23 ), Color( 62, 62, 62, 255 ) );
		g_pSurface->OutlinedRect( x_offset + 8, y_offset + 17 + 19, ComboSize, ( size * 23 ), Color( 32, 32, 32, 145 / 2 ) );

		for ( int i = 0; i < size; i++ )
		{
			if ( GetAsyncKeyState( VK_LBUTTON ) && mouse_in_params( x_offset + 8, y_offset + 34 + i * 23, ComboSize, 16 ) )
				* item = i;

			if ( i == *item )
				g_pSurface->DrawT( x_offset + 8 + 10, y_offset + 37 + i * 23, Color( 255, 255, 255, 255 ), g::fonts::menu_font, false, itemname[ i ].c_str( ) );
			else
				g_pSurface->DrawT( x_offset + 8 + 10, y_offset + 37 + i * 23, Color( 215, 215, 215, 255 ), g::fonts::menu_font, false, itemname[ i ].c_str( ) );
		}
	}

	g_pSurface->DrawT( x_offset + 17, y_offset + 16, Color( 255, 255, 255, 255 ), g::fonts::menu_font, false, itemname[ *item ].c_str( ) );

	if ( open ) // i added 1 more to 42 bec the space between the main box and the drop down
		y_offset += 43 + ( size * 23 );
	else
		y_offset += 42;
}

void spectrum( int x, int y, int w, int h ) {
	g_pSurface->FilledRect( x, y, w, h - 3, Color(62, 62, 62, 145) );
	g_pSurface->OutlinedRect( x - 1, y - 1, w + 2, h - 1, Color(32, 32, 32, 145 / 2) );
	g_pSurface->FilledRect( x + 1, y + 1, w - 2, h - 2 - 3, Color(32, 32, 32, 255) );

	for ( int i = 1; i < 60; i++ ) {
		if ( i <= 10 )
			g_pSurface->horizontal_gradient_rect( x + 2, y + i + 2, w - 4, 1, Color( 255, 25.5f * ( i ), 0 ), Color( 255, 255, 255 ) );
		if ( i > 10 && i <= 20 )
			g_pSurface->horizontal_gradient_rect( x + 2, y + i + 2, w - 4, 1, Color( 255 - ( 25.5f * ( i ) ), 255, 0 ), Color( 255, 255, 255 ) );
		if ( i > 20 && i <= 30 )
			g_pSurface->horizontal_gradient_rect( x + 2, y + i + 2, w - 4, 1, Color( 0, 255, 25.5f * ( i ) ), Color( 255, 255, 255 ) );
		if ( i > 30 && i <= 40 )
			g_pSurface->horizontal_gradient_rect( x + 2, y + i + 2, w - 4, 1, Color( 0, 255 - ( 25.5f * ( i ) ), 255 ), Color( 255, 255, 255 ) );
		if ( i > 40 && i <= 50 )
			g_pSurface->horizontal_gradient_rect( x + 2, y + i + 2, w - 4, 1, Color( 25.5f * ( i ), 0, 255 ), Color( 255, 255, 255 ) );
		if ( i > 50 && i <= 60 )
			g_pSurface->horizontal_gradient_rect( x + 2, y + i + 2, w - 4, 1, Color( 255, 0, 255 - ( 25.5f * ( i ) ) ), Color( 255, 255, 255 ) );
	}
}

bool old_key_states[ 256 ];

std::string keys_list[ ] = {
"error", "left mouse", "right mouse", "break", "middle mouse", "mouse 4", "mouse 5",
"error", "backspace", "tab", "error", "error", "error", "enter", "error", "error", "shift",
"ctrl", "alt","pause","caps lock", "error", "error", "error", "error", "error", "error",
"error", "error", "error", "error", "error", "spacebar","pg up", "pg down", "end", "home", "left",
"up", "right", "down", "error", "print", "error", "print screen", "insert","delete", "error", "0", "1",
"2", "3", "4", "5", "6", "7", "8", "9", "error", "error", "error", "error", "error", "error",
"error", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u",
"v", "w", "x","y", "z", "left windows", "right windows", "error", "error", "error", "num 0", "num 1",
"num 2", "num 3", "num 4", "num 5", "num 6","num 7", "num 8", "num 9", "*", "+", "_", "-", ".", "/", "f1", "f2", "f3",
"f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12","f13", "f14", "f15", "f16", "f17", "f18", "f19", "f20", "f21",
"f22", "f23", "f24", "error", "error", "error", "error", "error","error", "error", "error",
"num lock", "scroll lock", "error", "error", "error", "error", "error", "error", "error",
"error", "error","error", "error", "error", "error", "error", "left shift", "right shift", "left control",
"right control", "left menu", "right menu", "error","error", "error","error", "error", "error", "error",
"error", "error", "error", "next track", "previous track", "stop", "play/pause", "error", "error",
"error", "error", "error", "error", ";", "+", ",", "-", ".", "/?", "~", "error", "error",
"error", "error","error", "error", "error", "error", "error", "error", "error",
"error", "error", "error", "error", "error", "error", "error", "error","error",
"error", "error", "error", "error", "error", "error", "[{", "\\|", "}]", "'\"", "error",
"error", "error", "error","error", "error", "error", "error", "error", "error",
"error", "error", "error", "error", "error", "error", "error", "error", "error",
"error", "error", "error", "error", "error", "error", "error", "error", "error",
"error", "error"
};

void c_menu::update_input( ) {
	for ( int i = 0; i < 256; i++ ) {
		if ( GetAsyncKeyState( i ) ) old_key_states[ i ] = true;
		else old_key_states[ i ] = false;
	}
}

bool c_menu::is_key_pressed( int vkey ) {
	return old_key_states[ vkey ];
}
bool c_menu::is_key_held( int vkey ) {
	return old_key_states[ vkey ] && GetAsyncKeyState( vkey );
}
bool c_menu::is_key_released( int vkey ) {
	if ( GetAsyncKeyState( vkey ) ) return false;
	return old_key_states[ vkey ];
}

void c_menu::keybind( int& item, std::string name ) {
	bool pressed = false;

	static bool open = false;
	static bool selected_opened = false;
	static bool click_rest;
	static bool rest;

	static std::string name_selected;

	int x_position = groupbox_width - 20;
	int y_position = y_offset - 24;

	std::string something;

	if ( is_keybind_selected && ( keybind_selected_name != name ) )
		return;

	if ( g_Menu.menu_opened ) {
		if ( GetAsyncKeyState( VK_LBUTTON ) && mouse_in_params( x_position - 40, y_position - 1, 48, 20 ) && !click_rest ) {
			name_selected = name;
			pressed = true;
			click_rest = true;
		}
		else if ( !GetAsyncKeyState( VK_LBUTTON ) && mouse_in_params( x_position - 40, y_position - 1, 48, 20 ) ) {
			click_rest = false;
		}
		if ( pressed )
		{
			if ( !rest )
				selected_opened = !selected_opened;
			rest = true;
		}
		else
			rest = false;

		if ( name_selected == name ) {
			open = selected_opened;
			is_keybind_selected = open;
			keybind_selected_name = name;
		}
	}

	if (open && (keybind_selected_name == name)) {
		something = "key...";
		for (int i = 0; i < 256; i++) {
			if (is_key_pressed(i)) {
				if (i == VK_ESCAPE) {
					something = keys_list[item];
					open = false;
					selected_opened = false;
					is_keybind_selected = false;
					keybind_selected_name = "transexualplaceholder";
				}
				else {
					if (keys_list[i] != "error") {
						if (keys_list[i] == keys_list[1]) {
							// left mouse button
							if (click_rest)
								continue;
						}
						item = i;
						something = keys_list[i];
						open = false;
						selected_opened = false;
						is_keybind_selected = false;
						keybind_selected_name = "transexualplaceholder";
					}
				}
			}
		}
	}
	else {
		if ( item != 0 ) something = keys_list[ item ];
		else something = "undefined";
	}

	std::string thing_to_draw = "[ " + something + " ]";

	RECT text_size = g_Menu.get_text_size( g::fonts::menu_font, thing_to_draw.c_str( ) );

	g_pSurface->DrawT( x_position - text_size.right + 3, y_position, Color( 185, 185, 185, 230 ), g::fonts::menu_font, false, thing_to_draw.c_str( ) );
}

void c_menu::color_selector( std::string name, Color &colour)
{
	bool pressed = false;
	bool open = false;

	static bool selected_opened = false;
	static bool click_rest;
	static bool rest;
	static std::string name_selected;

	int x_position = groupbox_width - 29;
	int y_position = y_offset - 19;

	g_pSurface->FilledRect(x_position - 1, y_position - 1, 25, 13, Color(35, 35, 35, 255));
	g_pSurface->FilledRect(x_position, y_position, 23, 11, Color(colour.red, colour.green, colour.blue, 255));
	g_pSurface->vertical_gradient_rect(x_position, y_position + 5, 23, 6, Color(255, 255, 255, 0), Color(32, 32, 32, 255));

	if (menu_opened) {
		if (GetAsyncKeyState(VK_LBUTTON) && mouse_in_params(x_position - 1, y_position - 1, 25, 13) && !click_rest) {
			name_selected = name;
			pressed = true;
			click_rest = true;
		}
		else if (!GetAsyncKeyState(VK_LBUTTON) && mouse_in_params(x_position - 1, y_position - 1, 25, 13)) {
			click_rest = false;
		}
		if (pressed)
		{
			if (!rest) {
				selected_opened = !selected_opened;
			}
			rest = true;
		}
		else
			rest = false;

		if (name_selected == name)
			open = selected_opened;
	}

	if (open) {
		spectrum(x_position + 25, y_position - 4, 68, 68); // draw spectrum

		// handle input
		if (GetAsyncKeyState(VK_LBUTTON) && mouse_in_params(x_position + 27, y_position - 4 + 3, 60, 60)) { // idk should work right???
			HDC hdc = GetDC(nullptr);

			COLORREF selected_colour;
			selected_colour = GetPixel(hdc, mouse_pos().x, mouse_pos().y);

			colour.red = GetRValue(selected_colour);
			colour.green = GetGValue(selected_colour);
			colour.blue = GetBValue(selected_colour);
			colour.alpha = 255.f;
		}
	}
}