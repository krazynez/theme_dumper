#include <pspsdk.h>
#include <pspkernel.h>
#include <psprtc.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <stdio.h>

#define printf pspDebugScreenPrintf

#include "libpspexploit.h"

PSP_MODULE_INFO("Theme_Dumper", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(0);
PSP_HEAP_SIZE_KB(20480);

static KernelFunctions _ktbl; KernelFunctions* k_tbl = &_ktbl;

void dump() {
	int k1 = pspSdkSetK1(0);
	int userLevel = pspXploitSetUserLevel(8);
	pspXploitRepairKernel();
	pspXploitScanKernelFunctions(k_tbl);


	
	k_tbl->IoAssign("flash3:", "lflash0:0,3", "flashfat3:", 0, NULL, 0);
	SceUID flash3_wallpaper = k_tbl->KernelIOOpen("flash3:/custom_theme.dat", PSP_O_RDONLY, 0777);
	if(flash3_wallpaper<0) {
		pspDebugScreenClear();
		pspDebugScreenSetXY(2, 2);
		printf("No flash3 theme found only required for MHB Premium Theme ....");	
		k_tbl->IoUnassign("flash3:");
		k_tbl->KernelDelayThread(5000000);
	}

	k_tbl->IoAssign("flash1:", "lflash0:0,1", "flashfat1:", 0, NULL, 0);
	SceUID wallpaper = k_tbl->KernelIOOpen("flash1:/vsh/theme/custom_theme.dat", PSP_O_RDONLY, 0777);
	if(wallpaper<0) {
		k_tbl->IoUnassign("flash1:");
	}

	if ( wallpaper < 0 && flash3_wallpaper < 0) {
		pspDebugScreenClear();
		pspDebugScreenSetTextColor(0x0000FF);
		pspDebugScreenSetXY(2, 2);
		printf("No theme found quiting ....");	
		k_tbl->IoUnassign("flash3:");
		k_tbl->KernelDelayThread(5000000);
		return;
	}

	SceUID theme = k_tbl->KernelIODopen("ms0:/psp/theme");
	if(theme)
		k_tbl->KernelIODclose(theme);
	else 
		k_tbl->KernelIOMkdir("ms0:/psp/theme", 0777);

	u8 buf[512];
	memset(&buf, 0, sizeof(buf));
	int read;
	if(wallpaper>=0) {
		pspDebugScreenClear();
		pspDebugScreenSetXY(2, 2);
		printf("Dumping custom_theme.dat to ms0:/psp/theme/custom_theme.ptf ....");	
		k_tbl->KernelDelayThread(4000000);
		SceUID fd = k_tbl->KernelIOOpen("ms0:/psp/theme/custom_theme.ptf", PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777);
		if(fd<0) {
			k_tbl->KernelIOMkdir("ms0:/psp/theme", 0777);
			fd = k_tbl->KernelIOOpen("ms0:/psp/theme/custom_theme.ptf", PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777);
		}
		while((read = k_tbl->KernelIORead(wallpaper, buf, 512))>0)
			k_tbl->KernelIOWrite(fd, buf, sizeof(buf));

		k_tbl->KernelIOClose(wallpaper);
		k_tbl->KernelIOClose(fd);
		k_tbl->IoUnassign("flash1:");
	}

	if(flash3_wallpaper>=0) {
		memset(&buf, 0, sizeof(buf));
		pspDebugScreenClear();
		pspDebugScreenSetXY(2, 2);
		printf("Dumping flash3:/custom_theme.dat to ms0:/psp/theme/MHP3.ptf ....");	
		k_tbl->KernelDelayThread(4000000);
		SceUID fd = k_tbl->KernelIOOpen("ms0:/psp/theme/MHP3.ptf", PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777);
		if(fd<0) {
			k_tbl->KernelIOMkdir("ms0:/psp/theme", 0777);
			fd = k_tbl->KernelIOOpen("ms0:/psp/theme/MHP3.ptf", PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777);
		}
		while((read = k_tbl->KernelIORead(flash3_wallpaper, buf, 512))>0)
			k_tbl->KernelIOWrite(fd, buf, sizeof(buf));

		k_tbl->KernelIOClose(flash3_wallpaper);
		k_tbl->KernelIOClose(fd);
		k_tbl->IoUnassign("flash3:");
	}

	pspSdkSetK1(k1);
	pspXploitSetUserLevel(userLevel);
	pspDebugScreenClear();
	pspDebugScreenSetXY(2,2);
	printf("Succesfully dumped theme(s) ...");
	k_tbl->KernelDelayThread(2000000);
	pspDebugScreenClear();
	pspDebugScreenSetXY(2,2);
	printf("Quiting back to XMB ...");
	k_tbl->KernelDelayThread(4000000);

}

int main(int argc, char *args[]) {

	SceCtrlData pad;
	pspDebugScreenInit();
	pspDebugScreenSetTextColor(0x00BFFF);
	pspDebugScreenSetXY(2,2);
	printf("Press X to dump");
	for(;;) {
		sceCtrlReadBufferPositive(&pad, 1);
		if((pad.Buttons & PSP_CTRL_CROSS) == PSP_CTRL_CROSS) {
			int res = pspXploitInitKernelExploit();
			if(res == 0) {
				res = pspXploitDoKernelExploit();
				if (res == 0)
					pspXploitExecuteKernel((int)dump);
			}
			else {
				pspDebugScreenClear();
				printf("ERR!");
				sceKernelDelayThread(500000);
			}
			sceKernelExitGame();
		}
	}
	return 0;
}

