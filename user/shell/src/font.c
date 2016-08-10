#include <stddef.h>

char const *font[128] = {
	NULL, // 0x00
	NULL, // 0x01
	NULL, // 0x02
	NULL, // 0x03
	NULL, // 0x04
	NULL, // 0x05
	NULL, // 0x06
	NULL, // 0x07
	NULL, // 0x08
	NULL, // 0x09
	NULL, // 0x0A
	NULL, // 0x0B
	NULL, // 0x0C
	NULL, // 0x0D
	NULL, // 0x0E
	NULL, // 0x0F
	NULL, // 0x10
	NULL, // 0x11
	NULL, // 0x12
	NULL, // 0x13
	NULL, // 0x14
	NULL, // 0x15
	NULL, // 0x16
	NULL, // 0x17
	NULL, // 0x18
	NULL, // 0x19
	NULL, // 0x1A
	NULL, // 0x1B
	NULL, // 0x1C
	NULL, // 0x1D
	NULL, // 0x1E
	"a3",
	"a3 M04 P0A M02 d",
	"a4 M0A p0-2 M2A p0-2",
	"a7 M05 p50 M07 p50 M13 P29 M33 P49",
	"a6 M04 p1-1 p20 p11 p01 p-11 p-20 p-11 p01 p11 p20 p1-1 M22 P2A",
	"a6 M09 p11 p1-1 p-1-1 p-11 M23 p11 p1-1 p-1-1 p-11 M02 P4A",
	"a6 M44 p-2-2 p-10 p-11 p01 p33 p01 p-11 p-10 p-1-1 p0-2 P42",
	"a2 M0A p0-2",
	"a5 M3A p-1-1 p-1-2 p0-2 p1-2 p1-1",
	"a5 M0A p1-1 p1-2 p0-2 p-1-2 p-1-1",
	"a5 M06 p40 M24 p04 M08 p4-4 M48 p-4-4",
	"a6 M06 p40 M24 p04",
	"a3 M12 d p-1-2",
	"a5 M06 p30",
	"a2 M02 d",
	"a4 M2A P02",
	"a6 M03 p06 p11 p20 p1-1 p0-6 p-1-1 p-20 p-11 M49 P03",
	"a6 M03 p06 p11 p20 p1-1 p0-6 p-1-1 p-20 p-11 M26 d",
	"a5 M08 p22 P22 M12 p20",
	"a6 M08 p01 p11 p20 p1-1 P47 P36 P02 P42",
	"a6 M09 p11 p20 p1-1 p0-2 p-1-1 P26 M36 p1-1 p0-2 p-1-1 p-20 p-11",
	"a6 M2A P06 P46 M38 P32",
	"a6 M4A P0A P06 p30 p1-1 p0-2 p-1-1 p-20 p-11",
	"a6 M3A P18 P06 P03 p1-1 p20 p11 p02 p-11 p-20 p-1-1",
	"a5 M0A P3A P12 M16 p20",
	"a6 M16 p-11 p02 p11 p20 p1-1 p0-2 p-1-1 p1-1 p0-2 p-1-1 p-20 p-11 p02 p11 p20",
	"a6 M47 p-1-1 p-20 p-11 p02 p11 p20 p1-1 p0-6 p-1-1 p-20 p-11",
	"a3 M14 d M18 d",
	"a4 M14 d p-1-2 M18 d",
	"a5 M37 p-3-1 p3-1",
	"a5 M07 p30 M05 p30",
	"a5 M07 p3-1 p-3-1",
	"a6 M08 p01 p11 p20 p1-1 p0-2 p-2-2 p0-1 m0-2 d",
	"a5 M37 p-20 p0-2 p20 p03 p-11 p-20 p-1-1 p0-4 p1-1 p20",
	"a6 M02 p07 p11 p20 p1-1 p0-7 M06 p40",
	"a6 M02 p08 p30 p1-1 p0-2 p-1-1 p-30 m30 p1-1 p0-2 p-1-1 p-30",
	"a6 M43 p-1-1 p-20 p-11 p06 p11 p20 p1-1",
	"a6 M02 p08 p30 p1-1 p0-6 p-1-1 p-30",
	"a6 M42 p-40 p08 p40 M06 p30",
	"a6 M02 p08 p40 M06 p30",
	"a6 M26 p20 p0-3p-1-1 p-20 p-11 p06 p11 p20 p1-1",
	"a6 M02 p08 M42 p08 M06 p40",
	"a4 M02 p20 M12 p08 M0A p20",
	"a6 M0A p40 p0-7 p-1-1 p-20 p-11",
	"a6 M02 P0A M4A p-4-4 p4-4",
	"a6 M0A P02 P42",
	"a6 M02 P0A p2-4 p24 P42",
	"a6 M02 P0A P42 P4A",
	"a6 M03 p06 p11 p20 p1-1 p0-6 p-1-1 p-20 p-11",
	"a6 M02 p08 p30 p1-1 p0-2 p-1-1 p-30",
	"a6 M03 p06 p11 p20 p1-1 p0-5 p-2-2 p-10 p-11 M24 P42",
	"a6 M02 p08 p30 p1-1 p0-2 p-1-1 p-30 P42",
	"a6 M02 p30 p11 p02 p-11 p-20 p-11 p02 p11 p30",
	"a6 M22 P2A M0A P4A",
	"a6 M0A P03 P12 P32 P43 P4A",
	"a6 M0A P22 P4A",
	"aA M0A P22 P46 P62 P8A",
	"a6 M0A P42 M02 P4A",
	"a6 M22 P26 P0A M26 P4A",
	"a6 M0A P4A P02 P42 M16 P36",
	"a4 M2A P0A P02 P22",
	"a4 M0A P22",
	"a4 M0A P2A P22 P02",
	"a4 M09 p11 p1-1",
	"a6 M02 P42",
	"a3 M0A p1-2",
	"a6 M46 p0-4 p-30 p-11 p02 p11 p20 p1-1",
	"a6 M0A p0-8 p30 p11 p02 p-11 p-20 p-1-1",
	"a6 M45 p-11 p-20 p-1-1 p0-2 p1-1 p20 p11",
	"a6 M4A p0-8 m01 p-1-1 p-20 p-11 p02 p11 p20 p1-1",
	"a6 M04 p40 p01 p-11 p-20 p-1-1 p0-2 p1-1 p20 p11",
	"a4 M12 p07 p11 M06 p20",
	"a6 M43 p-1-1 p-20 p-11 p02 p11 p20 p1-1 p0-4 p-1-1 p-20 p-11",
	"a5 M0A P02 M06 p20 p1-1 p0-3",
	"a2 M02 P06 M08 d",
	"a2 M06 p0-4 p-1-1 M08 d",
	"a5 M02 P0A M03 p33 M14 p2-2",
	"a4 M0A P03 p1-1 p10",
	"a8 M02 P06 M05 p11 p10 p1-1 p0-3 m03 p11 p10 p1-1 p0-3",
	"a6 M02 P06 M05 p11 p10 p1-1 p0-3",
	"a6 M03 p02 p11 p20 p1-1 p0-2 p-1-1 p-20 p-11",
	"a6 M00 P06 M03 p1-1 p20 p11 p02 p-11 p-20 p-1-1",
	"a6 M40 P46 M43 p-1-1 p-20 p-11 p02 p11 p20 p1-1",
	"a5 M02 P06 M05 p11 p10 p1-1",
	"a5 M02 p20 p11 p-11 p-10 p-11 p11 p20",
	"a4 M18 p0-5 p1-1 M06 p20",
	"a6 M06 p0-3 p1-1 p20 p11 p03",
	"a6 M06 p2-4 p24",
	"a6 M06 p1-4 p14 p1-4 p14",
	"a6 M06 P42 M02 P46",
	"a6 M06 p0-3 p1-1 p20 p11 p03 m0-3 p0-2 p-1-1 p-20 p-11",
	"a6 M06 p40 p-4-4 p40 M14 p20",
	"a5 M3A p-10 p-1-1 p0-2 p-1-1 p1-1 p0-2 p1-1 p10",
	"a2 M02 P0A",
	"a5 M0A p10 p1-1 p0-2 p1-1 p-1-1 p0-2 p-1-1 p-10",
	"a6 M06 p11 p2-2 p11",
};