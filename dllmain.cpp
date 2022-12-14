#include "pch.h"

#define TRAMPOLINE(name) ((decltype(name##_r)*)name##_t->Target())

static Trampoline* EmeraldManager_Main_t = nullptr;

static bool SpatialSounds = false;
static bool Simultaneous = true;
static bool NewColors = true;
static bool NoAlertIcon = false;

VoidFunc(sub_458EA0, 0x458EA0);
VoidFunc(sub_458B90, 0x458B90);
VoidFunc(DrawEmeraldMark, 0x73A450);
VoidFunc(SetDefaultRenderStuff, 0x458EE0);

enum EmeraldHUDs
{
    EmeraldHud_Off = 4,
    EemeraldHud_Lock = 7,
    EmeraldHud_Red = 10,
    EmeraldHud_Pink = 12,
    EmeraldHud_Yellow = 13,
    EmeraldHud_Green = 16,
    EmeraldHud_Blue = 18,
};

enum EmeraldColors
{
    EmeraldColor_Red,
    EmeraldColor_Pink,
    EmeraldColor_Yellow,
    EmeraldColor_Green,
    EmeraldColor_Blue,
};

const float EmeraldDistances[]
{
    100.0f, // Red
    200.0f, // Pink
    300.0f, // Yellow
    400.0f, // Green
    500.0f, // Blue
};

#pragma optimize("", off)
//void __usercall DrawEmeraldRadar(int sprite_id@<ecx>, signed int emerald, signed int frame)
static const void* const DrawEmeraldRadar_ptr = (void*)0x00739FE0;
static inline void DrawEmeraldRadar(int sprite_id, int emerald, int frame)
{
    __asm
    {
        push[frame]
        push[emerald]
        mov ecx, sprite_id
        call DrawEmeraldRadar_ptr
        add esp, 8
    }
}
#pragma optimize("", on)

static void EmeraldRader_DisplayItem(int emerald, float distance)
{
    int sprite;

    if (NewColors == true)
    {
        if (distance < EmeraldDistances[EmeraldColor_Red])
        {
            sprite = EmeraldHud_Red;
        }
        else if (distance < EmeraldDistances[EmeraldColor_Pink])
        {
            sprite = EmeraldHud_Pink;
        }
        else if (distance < EmeraldDistances[EmeraldColor_Yellow])
        {
            sprite = EmeraldHud_Yellow;
        }
        else if (distance < EmeraldDistances[EmeraldColor_Green])
        {
            sprite = EmeraldHud_Green;
        }
        else if (distance < EmeraldDistances[EmeraldColor_Blue])
        {
            sprite = EmeraldHud_Blue;
        }
        else
        {
            sprite = EmeraldHud_Off;
        }
    }
    else
    {
        if (distance < EmeraldDistances[EmeraldColor_Red])
        {
            sprite = EmeraldHud_Red;
        }
        else if (distance < EmeraldDistances[EmeraldColor_Yellow])
        {
            sprite = EmeraldHud_Yellow;
        }
        else if (distance < EmeraldDistances[EmeraldColor_Blue])
        {
            sprite = EmeraldHud_Green;
        }
        else
        {
            sprite = EmeraldHud_Off;
        }
    }
   
    if (NoAlertIcon == false && distance < 30.0f)
    {
        DrawEmeraldMark();
    }

    DrawEmeraldRadar(sprite, emerald, sprite == EmeraldHud_Red ? FrameCountIngame * 2 : FrameCountIngame);
}

static void __cdecl EmeraldRadar_Display_r(ObjectMaster* obj)
{
    if (!*(char*)0x174AFCB || *(char*)0x174B002 == 1 || Pose2PStart_PlayerNum || *(char*)0x174AFF9 == 1)
    {
        return;
    }

    auto wk = EmeraldManagerObj2;

    if (*(char*)0x174AFE0)
    {
        sub_458EA0();
        sub_458B90();
    }

    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    
    njSetTexture(wk->TexList);

    int allow = true;

    for (int i = 0; i < 3; ++i)
    {
        auto emeinfo = wk->byte2C[i];

        if (emeinfo.byte0 == -2 || njScalor(&emeinfo.v) == 0.0f)
        {
            continue;
        }

        if (allow)
        {
            if (TwoPlayerMode == 0)
            {
                EmeraldRader_DisplayItem(i, CheckDistance(&emeinfo.v, &MainCharObj1[0]->Position));
            }
            else
            {
                EmeraldRader_DisplayItem(i, CheckDistance(&emeinfo.v, &MainCharObj1[CurrentScreen]->Position));
            }
        }
        else
        {
            DrawEmeraldRadar(EemeraldHud_Lock, i, FrameCountIngame);
        }
        
        if (Simultaneous == false)
        {
            allow = false;
        }
    }

    if (*(char*)0x174AFE0)
    {
        SetDefaultRenderStuff();
    }
}

static void PlayEmeraldSound(NJS_VECTOR* pos, float distance)
{
    if (distance <= EmeraldDistances[EmeraldColor_Green])
    {
        if (distance <= EmeraldDistances[EmeraldColor_Blue])
        {
            if (distance <= EmeraldDistances[EmeraldColor_Red])
            {
                if ((FrameCountIngame & 0xF) != 0)
                {
                    return;
                }
            }
            else if ((FrameCountIngame & 0x1F) != 0)
            {
                return;
            }
        }
        else if ((FrameCountIngame & 0x3F) != 0)
        {
            return;
        }

        if (SpatialSounds == true)
        {
            Play3DSound_Pos(0x1009, pos, 0, 0, 25);
        }
        else
        {
            PlaySoundProbably(0x1009, 0, 0, 0);
        }
    }
}

static void __cdecl EmeraldManager_Main_r(ObjectMaster* obj)
{
    auto wk = EmeraldManagerObj2;
    
    if (wk->Action == 4)
    {
        for (int i = 0; i < 3; ++i)
        {
            auto emeinfo = wk->byte2C[i];

            if (emeinfo.byte0 == -2 || njScalor(&emeinfo.v) == 0.0f)
            {
                continue;
            }

            if (TwoPlayerMode == 0)
            {
                PlayEmeraldSound(&emeinfo.v, CheckDistance(&emeinfo.v, &MainCharObj1[0]->Position));
            }
            else
            {
                float p1 = CheckDistance(&emeinfo.v, &MainCharObj1[0]->Position);
                float p2 = CheckDistance(&emeinfo.v, &MainCharObj1[1]->Position);

                if (p2 < p1)
                {
                    p1 = p2;
                }

                PlayEmeraldSound(&emeinfo.v, p1);
            }
            
            if (Simultaneous == false)
            {
                break;
            }
        }
    }

    TRAMPOLINE(EmeraldManager_Main)(obj);
}

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
        const IniFile* config = new IniFile(std::string(path) + "\\config.ini");
        const IniGroup* configgrp = config->getGroup("");

        if (configgrp)
        {
            SpatialSounds = configgrp->getBool("SpatialSounds", SpatialSounds);
            Simultaneous = configgrp->getBool("Simultaneous", Simultaneous);
            NewColors = configgrp->getBool("NewColors", NewColors);
            NoAlertIcon = configgrp->getBool("NoAlertIcon", NoAlertIcon);
        }

        delete config;

        if (NewColors == true)
        {
            helperFunctions.ReplaceFile("resource\\gd_PC\\PRS\\kgametex.pak", "resource\\gd_PC\\PRS\\kgametex_BR.pak");
            helperFunctions.ReplaceFile("resource\\gd_PC\\PRS\\kgametexdz.pak", "resource\\gd_PC\\PRS\\kgametexdz_BR.pak");
            helperFunctions.ReplaceFile("resource\\gd_PC\\PRS\\kgametexsh.pak", "resource\\gd_PC\\PRS\\kgametexsh_BR.pak");
        }

        if (Simultaneous == true || NewColors == true)
        {
            WriteJump(EmeraldRadar_Update, EmeraldRadar_Display_r);
        }
        
        if (Simultaneous == true || SpatialSounds == true)
        {
            EmeraldManager_Main_t = new Trampoline(0x739570, 0x739576, EmeraldManager_Main_r, false);
            WriteData<5>((void*)0x739E0D, 0x90);
            WriteData<5>((void*)0x739DF2, 0x90);
        }

        if (NoAlertIcon == true)
        {
            WriteData((uint8_t*)0x73A450, 0xC3ui8);
        }
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}