CC = wcc386
CFLAGS = /oxl+ /j /s /zu -I=mikmod /fpi

OBJS = hello.obj &
       data.obj &
       mdriver.obj &
       load_mod.obj &
       mloader.obj &
       drv_nos.obj &
       drv_sb.obj &
       mdma.obj &
       virtch.obj &
       mirq.obj &
       mmio.obj &
       munitrk.obj &
       mplayer.obj

hello.exe: $(OBJS) hello.lnk
  wlink @hello.lnk > link.err

mdriver.obj: mikmod/mdriver.c
  $(CC) $(CFLAGS) $<
load_mod.obj: mikmod/load_mod.c
  $(CC) $(CFLAGS) $<
mloader.obj: mikmod/mloader.c
  $(CC) $(CFLAGS) $<
drv_nos.obj: mikmod/drv_nos.c
  $(CC) $(CFLAGS) $<
drv_sb.obj: mikmod/drv_sb.c
  $(CC) $(CFLAGS) $<
mdma.obj: mikmod/mdma.c
  $(CC) $(CFLAGS) $<
virtch.obj: mikmod/virtch.c
  $(CC) $(CFLAGS) $<
mirq.obj: mikmod/mirq.c
  $(CC) $(CFLAGS) $<
mmio.obj: mikmod/mmio.c
  $(CC) $(CFLAGS) $<
munitrk.obj: mikmod/munitrk.c
  $(CC) $(CFLAGS) $<
mplayer.obj: mikmod/mplayer.c
  $(CC) $(CFLAGS) $<
.c.obj:
  $(CC) $(CFLAGS) $<
