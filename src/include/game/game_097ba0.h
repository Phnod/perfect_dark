#ifndef _IN_GAME_GAME_097BA0_H
#define _IN_GAME_GAME_097BA0_H
#include <ultra64.h>
#include "types.h"

u32 func0f097ba0(void);
u32 func0f097d0c(void);
u32 func0f097d64(void);
u32 func0f097df0(void);
u32 func0f097e74(void);
u32 func0f097f28(void);
u32 func0f098030(void);
u32 func0f09815c(void);
u32 func0f0981e8(void);
u32 func0f098884(void);
void func0f0988e0(u32 *mines, bool arg1, struct hand *hand);
u32 func0f098a44(void);
u32 func0f098bfc(void);
u32 func0f098c0c(void);
u32 func0f098c4c(void);
u32 func0f098ca0(void);
u32 func0f098df8(void);
u32 func0f098f8c(void);
u32 func0f099008(u32 hand);
bool func0f0990b0(struct weaponfunc *basefunc, struct weapon *weapon);
u32 func0f099188(void);
u32 func0f0991e4(void);
u32 func0f099780(void);
u32 func0f099808(void);
u32 func0f099c24(void);
u32 func0f099c48(void);
u32 func0f09a310(void);
u32 func0f09a3f8(void);
u32 func0f09a6f8(void);
u32 func0f09aba4(void);
u32 func0f09afe4(void);
u32 func0f09b260(void);
u32 func0f09b4d8(void);
u32 func0f09b500(void);
u32 func0f09b828(void);
u32 func0f09b8e0(void);
u32 func0f09bc2c(void);
u32 func0f09bd58(void);
u32 func0f09bec8(void);
u32 func0f09bf44(void);
u32 func0f09c01c(void);
u32 func0f09cb44(void);
u32 func0f09cb5c(void);
u32 func0f09cd18(void);
void func0f09cdc4(s32 hand);
void func0f09ce8c(void);
u32 func0f09ceac(void);
f32 func0f09cf58(u32 arg0);
void func0f09cf88(void);
u32 func0f09d140(void);
u32 func0f09d550(void);
u32 func0f09d8dc(void);
u32 func0f09dd7c(void);
u32 func0f09ddcc(void);
u32 func0f09ddec(void);
u32 func0f09ddfc(void);
void func0f09df50(void);
void func0f09df64(s32 arg0);
void func0f09df9c(void);
u32 func0f09e004(void);
u32 func0f09e144(void);
u32 func0f09e4e0(void);
u32 func0f09ea90(void);
u32 func0f09eae4(void);
u32 func0f09ebbc(void);
void func0f09ebcc(struct defaultobj *obj, struct coord *coord, s16 *rooms, f32 *matrix1, f32 *arg4, f32 *matrix2, struct prop *prop, struct coord *pos);
u32 func0f09ed2c(void);
void func0f09ee18(struct chrdata *chr, u8 *arg1, struct coord *pos, s16 *rooms, f32 *arg4, struct coord *arg5);
u32 func0f09f100(void);
u32 func0f09f848(void);
u32 func0f09f974(void);
u32 func0f09fa20(void);
u32 func0f09fa84(void);
void func0f0a0394(f32 autoaimx, f32 autoaimy, f32 arg2, f32 arg3);
void currentPlayerSwivelGunTowards(f32 autoaimx, f32 autoaimy, f32 autoaimdamp);
void func0f0a0b98(f32 arg0, f32 arg1);
void currentPlayerGetCrossPos(f32 *x, f32 *y);
void func0f0a0c08(struct coord *arg0, struct coord *arg1);
u32 func0f0a0c44(void);
u32 func0f0a0cb0(void);
u32 func0f0a0fac(void);
u32 func0f0a12b0(void);
u32 func0f0a1318(void);
u32 func0f0a134c(void);
u32 func0f0a1528(void);
void currentPlayerEquipWeapon(s32 weaponnum);
u32 getCurrentPlayerWeaponId(u32 arg0);
u32 func0f0a1a10(void);
u32 func0f0a1a68(void);
u32 func0f0a1ab0(void);
void func0f0a1b50(void);
void func0f0a1c2c(void);
bool func0f0a1d14(u32 weaponnum);
u32 func0f0a1df4(void);
void currentPlayerEquipWeaponWrapper(bool arg0, s32 weaponnum);
bool func0f0a20fc(u32 arg0);
u32 func0f0a212c(void);
char *weaponGetName(s32 weaponnum);
u16 weaponGetNameId(s32 weaponnum);
char *weaponGetShortName(s32 arg0);
u32 func0f0a2218(void);
u32 func0f0a2290(void);
u32 func0f0a2308(void);
u32 func0f0a233c(void);
u32 func0f0a2498(void);
u32 func0f0a24f0(void);
u32 func0f0a27c8(void);
void func0f0a29c8(void);
u32 func0f0a2ab0(void);
u32 func0f0a2ae4(void);
u32 func0f0a2da8(void);
u32 func0f0a2e94(void);
void playerDetonateRemoteMines(s32 playernum);
u32 func0f0a3160(void);
u32 func0f0a3490(void);
u32 func0f0a37b4(void);
u32 func0f0a3aa4(void);
u32 func0f0a3eac(void);
u32 func0f0a4094(void);
u32 func0f0a419c(void);
u32 func0f0a4334(void);
u32 func0f0a4438(void);
u32 func0f0a4570(void);
u32 func0f0a45d0(void);
u32 func0f0a46a4(void);
u32 func0f0a4e44(void);
u32 func0f0a5300(void);
u32 func0f0a5550(void);
u32 func0f0a695c(void);
u32 func0f0a6c30(void);
u32 func0f0a70f8(void);
void func0f0a7138(Gfx **gdl);
u32 func0f0a7d5c(void);
void func0f0a7d98(u8 *arg0, struct prop *prop, s32 arg2);
u32 func0f0a8404(void);
u32 func0f0a84c8(void);
void func0f0a8910(s32 hand, bool firing);
u32 currentPlayerConsiderToggleGunFunction(s32 usedowntime, bool firing, s32 arg2);
void func0f0a8c50(void);
bool currentPlayerIsUsingSecondaryFunction(void);
void currentPlayerTickInventory(bool triggeron);
void playersSetPassiveMode(bool enable);
void currentPlayerSetAimType(u32 aimtype);
void func0f0a93e0(struct coord *coord);
void func0f0a9464(struct coord *coord);
void func0f0a9494(u32 operation);
void func0f0a94d0(u32 operation, struct coord *a, struct coord *b);
void func0f0a95ec(u32 arg0, bool enable);
void currentPlayerSetAmmoQuantity(s32 ammotype, s32 quantity);
s32 ammoGetQuantity(s32 type);
s32 currentPlayerGetAmmoCount(s32 ammotype);
s32 ammotypeGetMaxCapacity(u32 ammotype);
bool ammotypeAllowsUnlimitedAmmo(u32 ammotype);
void currentPlayerGiveUnlimitedAmmo(bool force);
u32 weaponGetAmmoType(u32 weaponnum, u32 func);
u32 currentPlayerGetAmmoQuantityForWeapon(u32 weaponnum, u32 func);
void currentPlayerGiveAmmoForWeapon(u32 weaponnum, u32 func, u32 quantity);
u32 func0f0a9b10(void);
u32 func0f0a9b68(void);
u32 func0f0a9d2c(void);
u32 func0f0a9d94(void);
u32 func0f0a9da8(void);
u32 func0f0a9fc0(void);
Gfx *hudRenderAmmo(Gfx *gdl);
u32 func0f0abad0(void);
u32 func0f0abb80(void);
u32 func0f0abba8(void);
u32 func0f0abbe8(void);
void func0f0abc28(void);
void func0f0abc74(u32 arg0, s32 arg1);
Gfx *func0f0abcb0(Gfx *gdl);
u32 func0f0abd30(void);
u32 getCurrentPlayerWeaponIdWrapper(u32 arg0);
s8 freeFireslotWrapper(s32 fireslot);
s8 freeFireslot(s32 fireslot);

#endif
