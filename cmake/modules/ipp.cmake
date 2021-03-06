SET(IPP_FOUND 0)

IF( "$ENV{IPPROOT}" STREQUAL "" )
	MESSAGE(STATUS "IPPROOT environment variable not set." )
	MESSAGE(STATUS "This can be done in your user .bashrc file by appending the corresponding line, e.g:" )
	MESSAGE(STATUS "export IPPROOT=/opt/intel/ipp/6.0.1.071/ia32/" )
ELSE( "$ENV{IPPROOT}" STREQUAL "" )
	IF ($ENV{IPPROOT} MATCHES .*composer.*)	#IPP 7.X
		SET(IPP_LIBS -lippcore -lippcc -lippi -lipps -lippdc -lippm -lpthread -lippcc -lippcv -lippi -lippm -lippcv -liomp5)
		SET(IPP_LIBS_PATH -L$ENV{IPPROOT}/sharedlib/ -L$ENV{IPPROOT}/lib/ -L$ENV{IPPROOT}/ipp/lib/intel64 )
		SET( LIBS ${LIBS} ${IPP_LIBS} -L$ENV{IPPROOT}/../compiler/lib/ia32 -L$ENV{IPPROOT}/../compiler/lib/intel64 -L$ENV{IPPROOT}/ipp/lib/intel64/ -L$ENV{IPPROOT}/lib/intel64/ -L$ENV{IPPROOT}/ipp/lib/ia32/ -L$ENV{IPPROOT}/lib/ia32/ )
		INCLUDE_DIRECTORIES( "$ENV{IPPROOT}/ipp/include/")
		INCLUDE_DIRECTORIES( "$ENV{IPPROOT}/include/")
		MESSAGE (STATUS "IPPROOT (IPP 7): $ENV{IPPROOT}")
	ELSE ($ENV{IPPROOT} MATCHES .*composer.*)
                MESSAGE (STATUS "IPPROOT (IPP 6): $ENV{IPPROOT}")

		IF( $ENV{IPPROOT} MATCHES .*[6][4].*)
			SET(IPP_LIBS -lippcoreem64t -lippcvem64t -lippccem64t -lippiem64t -lippsem64t -lippmem64t -liomp5 -lpthread -lippscem64t -lippsrem64t)
		ELSE( $ENV{IPPROOT} MATCHES .*[6][4].*)
			IF( $ENV{IPPROOT} MATCHES .*/lp32.*)
				SET(IPP_LIBS -lippcore -lippcvmerged -lippcc -lippi -lipps -lippdc -lippm -lguide -lpthread -lippcc -lippcv -lippi -lippm -lippsr -lippcv -limf -liomp5 -lsvml -lirc )
			ELSE( $ENV{IPPROOT} MATCHES .*/lp32.*)
				SET(IPP_LIBS -lippcore -lippcvmerged -lippcc -lippi -lipps -lippdc -lippm -lguide -lpthread -lippcc -lippcv -lippi -lippm -lippsr -lippcvt7 -limf -liomp5 -lsvml -lirc )
			ENDIF( $ENV{IPPROOT} MATCHES .*/lp32.*)
		ENDIF( $ENV{IPPROOT} MATCHES .*[6][4].*)

		SET( LIBS ${LIBS} ${IPP_LIBS} -L$ENV{IPPROOT}/sharedlib/ -L$ENV{IPPROOT}/lib/ )
		SET(IPP_LIBS_PATH -L$ENV{IPPROOT}/sharedlib/ -L$ENV{IPPROOT}/lib/ )
		INCLUDE_DIRECTORIES( "$ENV{IPPROOT}/include/")
	ENDIF ($ENV{IPPROOT} MATCHES .*composer.*)
	IF( EXISTS "$ENV{IPPROOT}/include/ippi.h" OR EXISTS "$ENV{IPPROOT}/ipp/include/ippi.h" )
		ADD_DEFINITIONS(-DCOMPILE_IPP=1)
		ADD_DEFINITIONS(-DUSE_IPP=1)
		ADD_DEFINITIONS(-DIPP_FOUND=1)
		SET(IPP_FOUND 1)
		MESSAGE(STATUS "IPP Activated")
	ELSE ( EXISTS "$ENV{IPPROOT}/include/ippi.h" OR EXISTS "$ENV{IPPROOT}/ipp/include/ippi.h" )
		MESSAGE(STATUS "IPPROOT enviornment variable set incorrectly. Check it!!")
	ENDIF ( EXISTS "$ENV{IPPROOT}/include/ippi.h" OR EXISTS "$ENV{IPPROOT}/ipp/include/ippi.h" )
ENDIF( "$ENV{IPPROOT}" STREQUAL "" )

