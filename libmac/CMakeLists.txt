include_directories(.)

add_library(mac SHARED 
	libintl.c
	mac.c
	mem.c
	pwd.c
	strmode.c
	)
