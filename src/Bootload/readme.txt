������� �� ����� bootloader:
1. �������� ��� ����� �� ����� Bootload �������  (Flash.c, Flash2.c, mhx_reader.c � *.h �����)
2. �������� � ����� Bootload bootloader.c � bootloader.h
2. � global.c ��������� ������� ����� "���������� Bootloadera"	� �������:
	#include "flash2.h"
3. � start.asm ��������� COPY_RAMCODE ��������� � OFF (�� �����������, �� ������ �� ������������)