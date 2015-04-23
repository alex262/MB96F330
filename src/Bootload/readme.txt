Переход на новый bootloader:
1. Удлалить все файлы из папки Bootload проекта  (Flash.c, Flash2.c, mhx_reader.c и *.h файлы)
2. Добавить в папку Bootload bootloader.c и bootloader.h
2. В global.c полностью удалить часть "Обновление Bootloadera"	и строчку:
	#include "flash2.h"
3. В start.asm директиву COPY_RAMCODE перевести в OFF (не обязательно, но больше не используется)