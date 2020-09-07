all: mkfs_t ls_t.o mkdir_t.o external_cp.o cp_t.o cat_t.o call.o
	gcc -o mkfs_t mkfs_t.c
	gcc -o ls_t ls_t.o call.o
	gcc -o mkdir_t mkdir_t.o call.o
	gcc -o external_cp external_cp.o call.o
	gcc -o cp_t cp_t.o call.o
	gcc -o cat_t cat_t.o call.o
ls_t.o:ls_t.c
	gcc -c ls_t.c -o ls_t.o
mkdir_t.o:mkdir_t.c
	gcc -c mkdir_t.c -o mkdir_t.o
external_cp.o:external_cp.c
	gcc -c external_cp.c -o external_cp.o
cp_t.o:cp_t.c
	gcc -c cp_t.c -o cp_t.o
cat_t.o:cat_t.c
	gcc -c cat_t.c -o cat_t.o
call.o:call.c
	gcc -c call.c -o call.o 

