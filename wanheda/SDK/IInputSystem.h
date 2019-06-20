class i_inputsytem {
public:
	void enable_input( bool bEnable )
	{
		using original_fn = void( __thiscall* )( void*, bool );
		return ( *( original_fn * * )this )[ 11 ]( this, bEnable );
	}
};

extern i_inputsytem* g_pInputSystem;