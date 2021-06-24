call SET_GOC_SPEED.bat
call SET_COM.bat
m3_ice -y -s %COM% goc -d %GOC_DELAY% -V3 -g %GOC_SPEED% message A000002C 000D278F
;set ICE_DEBUG=1
