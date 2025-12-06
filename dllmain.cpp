#include "pch.h"
#include "utils.h"

static Trampoline* EggQuarters_Init_t = nullptr;
static Trampoline* EggQuarters_Free_t = nullptr;
static Trampoline* EmeraldManager_Main_t = nullptr;

static bool SpatialSounds = false;
static bool Simultaneous = true;
static bool NewColors = true;
static bool NoAlertIcon = false;
static bool BeetleRadar = true;

enum kgametex_texid
{
    texid_kgametex_off = 4,
    texid_kgametex_shut = 7,
    texid_kgametex_red = 10,
    texid_kgametex_yellow = 13,
    texid_kgametex_green = 16,
    texid_kgametex_wave = 19,
    texid_kgametex_pink = 23,
    texid_kgametex_blue = 26,
    texid_kgametex_max = 29
};

enum stg26_rader_texid
{
    texid_rader_green,
    texid_rader_off,
    texid_rader_red,
    texid_rader_wave,
    texid_rader_yellow,
};

struct DistToTex
{
    float dist;
    int tex;
    float speed;
};

const DistToTex EmeraldDistMap_Extended[]
{
    { 100.0f, texid_kgametex_red, 2.0f },
    { 200.0f, texid_kgametex_pink, 1.0f },
    { 300.0f, texid_kgametex_yellow, 1.0f },
    { 400.0f, texid_kgametex_green, 0.5f },
    { 500.0f, texid_kgametex_blue, 0.5 },
    { 0.0f }
};

const DistToTex EmeraldDistMap[]
{
    { 100.0f, texid_kgametex_red, 2.0f },
    { 300.0f, texid_kgametex_yellow, 1.0f },
    { 500.0f, texid_kgametex_green, 0.5f },
    { 0.0f }
};

const DistToTex BeetleDistMap[]
{
    { 100.0f, texid_rader_red, 2.0f },
    { 300.0f, texid_rader_yellow, 1.0f },
    { 400.0f, texid_rader_green, 0.5f },
    { 0.0f }
};

DataArray(NJS_TEXTURE_VTX, emerald_vtx, 0xA17090, 4);
DataArray(NJS_TEXTURE_VTX, emewave_vtx, 0xA170F0, 4);
DataArray(int*, beetle_wk, 0x1D1B770, 2);
DataArray(NJS_VECTOR, beetle_pos, 0x1D1B780, 2);

NJS_TEXNAME texname_stg26rader[5];
NJS_TEXLIST texlist_stg26rader = { arrayptrandlength(texname_stg26rader) };

// Decomp of 0x739FE0 with more control
void DrawRadarItem(NJS_TEXLIST* texlist, int texid, int wave_texid, int pos, int id, int frame)
{
    if (texlist)
    {
        njSetTexture(texlist);
    }

    int rframe;
    if (wave_texid == -1)
    {
        rframe = 0;
    }
    else
    {
        rframe = (frame >> 1) & 15;
    }

    if (TwoPlayerMode)
    {
        int posx = pos * 32.0f;
        if (CurrentScreen)
        {
            flt_1AF015C = posx + 432.0f;
            flt_1AF0160 = posx + 464.0f;
            flt_1AF0164 = (HorizontalResolution / 640.0f / (VerticalResolution / 480.0f) - 1.0f) * 160.0f;
        }
        else
        {
            flt_1AF015C = posx + 112.0f;
            flt_1AF0160 = posx + 144.0f;
            flt_1AF0164 = (HorizontalResolution / 640.0f / (VerticalResolution / 480.0f) - 1.0f) * -160.0f;
        }
        if (HorizontalResolution / VerticalResolution <= 1.6f)
        {
            emerald_vtx[1].x = flt_1AF0164 + flt_1AF015C + 30.0f;
            emerald_vtx[0].x = emerald_vtx[1].x;
            emerald_vtx[3].x = flt_1AF0164 + flt_1AF0160 + 30.0f;
        }
        else
        {
            emerald_vtx[1].x = flt_1AF0164 + flt_1AF015C;
            emerald_vtx[0].x = emerald_vtx[1].x;
            emerald_vtx[3].x = flt_1AF0164 + flt_1AF0160;
        }
        emerald_vtx[2].x = emerald_vtx[3].x;
        emerald_vtx[2].y = 408.0f;
        emerald_vtx[0].y = 408.0f;
        emerald_vtx[3].y = 440.0f;
        emerald_vtx[1].y = 440.0f;
        njSetTextureNum(texid + id);
        njDrawTexture(emerald_vtx, 4, pRenderInfo->texparbuf, 1);

        if (wave_texid != -1)
        {
            if (CurrentScreen)
            {
                flt_1AF015C = posx + 432.0f - rframe;
                flt_1AF0160 = posx + 464.0f + rframe;
                flt_1AF0164 = (HorizontalResolution / 640.0f / (VerticalResolution / 480.0f) - 1.0f) * 160.0f;
            }
            else
            {
                flt_1AF015C = posx + 112.0f - rframe;
                flt_1AF0160 = posx + 144.0f + rframe;
                flt_1AF0164 = (HorizontalResolution / 640.0f / (VerticalResolution / 480.0f) - 1.0f) * -160.0f;
            }
            float v25 = flt_1AF0164;
            float v26 = flt_1AF0164 + flt_1AF015C;
            if (HorizontalResolution / VerticalResolution <= 1.6f)
            {
                emewave_vtx[1].x = v26 + 30.0f;
                emewave_vtx[0].x = emewave_vtx[1].x;
                emewave_vtx[3].x = v25 + flt_1AF0160 + 30.0f;
            }
            else
            {
                emewave_vtx[1].x = v26;
                emewave_vtx[0].x = emewave_vtx[1].x;
                emewave_vtx[3].x = v25 + flt_1AF0160;
            }
            emewave_vtx[2].x = emewave_vtx[3].x;
            emewave_vtx[3].col = 0xFFFFFF | ((15 - rframe) << 28);
            emewave_vtx[2].col = 0xFFFFFF | ((15 - rframe) << 28);
            emewave_vtx[1].col = 0xFFFFFF | ((15 - rframe) << 28);
            emewave_vtx[0].col = 0xFFFFFF | ((15 - rframe) << 28);
            emewave_vtx[2].y = 408.0f - rframe;
            emewave_vtx[0].y = emewave_vtx[2].y;
            emewave_vtx[3].y = rframe + 440.0f;
            emewave_vtx[1].y = emewave_vtx[3].y;
            njSetTextureNum(wave_texid + id);
            njDrawTexture(emewave_vtx, 4, pRenderInfo->texparbuf, 1);
        }
    }
    else
    {
        int posx = pos * 40.0f;
        emerald_vtx[1].x = posx + 264.0f;
        emerald_vtx[0].x = emerald_vtx[1].x;
        emerald_vtx[3].x = posx + 296.0f;
        emerald_vtx[2].x = emerald_vtx[3].x;
        emerald_vtx[2].y = 408.0f;
        emerald_vtx[0].y = 408.0f;
        emerald_vtx[3].y = 440.0f;
        emerald_vtx[1].y = 440.0f;
        njSetTextureNum(texid + id);
        njDrawTexture(emerald_vtx, 4, pRenderInfo->texparbuf, 1);

        if (wave_texid != -1)
        {
            emewave_vtx[1].x = emerald_vtx[1].x - rframe;
            emewave_vtx[0].x = emewave_vtx[1].x;
            emewave_vtx[3].x = emerald_vtx[2].x + rframe;
            emewave_vtx[2].x = emewave_vtx[3].x;
            emewave_vtx[3].col = 0xFFFFFF | ((15 - rframe) << 28);
            emewave_vtx[2].col = 0xFFFFFF | ((15 - rframe) << 28);
            emewave_vtx[1].col = 0xFFFFFF | ((15 - rframe) << 28);
            emewave_vtx[0].col = 0xFFFFFF | ((15 - rframe) << 28);
            emewave_vtx[2].y = 408.0f - rframe;
            emewave_vtx[0].y = emewave_vtx[2].y;
            emewave_vtx[3].y = rframe + 440.0f;
            emewave_vtx[1].y = emewave_vtx[3].y;
            njSetTextureNum(wave_texid + id);
            njDrawTexture(emewave_vtx, 4, pRenderInfo->texparbuf, 1);
        }
    }
}

void DrawRadarEmerald(NJS_TEXLIST* texlist, int emerald, float distance)
{
    int texid = texid_kgametex_off;
    float speed = 1.0f;

    const DistToTex* map = NewColors && texlist->nbTexture >= texid_kgametex_max ? EmeraldDistMap_Extended : EmeraldDistMap;
    for (int i = 0; map[i].dist != 0.0f; ++i)
    {
        if (distance < map[i].dist)
        {
            texid = map[i].tex;
            speed = map[i].speed;
            break;
        }
    }

    if (!NoAlertIcon && distance < 30.0f)
    {
        DrawEmeraldMark();
    }

    int wave_texid = texid == texid_kgametex_off ? -1 : texid_kgametex_wave;
    DrawRadarItem(texlist, texid, wave_texid, emerald, emerald, FrameCountIngame * speed);
}

void DrawRadarBeetle()
{
    int texid = texid_rader_off;
    float speed = 1.0f;

    if (!beetle_wk[0] && !beetle_wk[1])
    {
        return;
    }

    for (int i = 0; i < 2; ++i)
    {
        if (beetle_wk[i] && (beetle_wk[i][1] == 2 || beetle_wk[i][1] == 3))
        {
            texid = texid_rader_red;
            break;
        }
    }

    if (texid == texid_rader_off)
    {
        float min_dist = 99999.0f;
        for (int i = 0; i < 2; ++i)
        {
            float x = beetle_pos[0].x - MainCharObj1[0]->Position.x;
            float y = beetle_pos[0].y - MainCharObj1[0]->Position.y;
            float z = beetle_pos[0].z - MainCharObj1[0]->Position.z;
            float dist = sqrtf(x * x + y * y + z * z);
            if (dist < min_dist)
            {
                min_dist = dist;
            }
        }

        const DistToTex* map = BeetleDistMap;
        for (int i = 0; map[i].dist != NULL; ++i)
        {
            if (min_dist < map[i].dist)
            {
                texid = map[i].tex;
                speed = map[i].speed;
                break;
            }
        }
    }

    int wave_texid = texid == texid_rader_off ? -1 : texid_rader_wave;
    DrawRadarItem(&texlist_stg26rader, texid, wave_texid, 4, 0, FrameCountIngame * speed);
}

void __cdecl EmeraldRadar_Display_r(ObjectMaster* obj)
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
            DrawRadarEmerald(wk->TexList, i, CheckDistance(&emeinfo.v, &MainCharObj1[TwoPlayerMode == 0 ? 0 : CurrentScreen]->Position));
        }
        else
        {
            DrawRadarItem(wk->TexList, texid_kgametex_shut, -1, i, i, 0);
        }
        
        if (!Simultaneous)
        {
            allow = false;
        }
    }

    if (CurrentLevel == LevelIDs_EggQuarters)
    {
        DrawRadarBeetle();
    }

    if (*(char*)0x174AFE0)
    {
        SetDefaultRenderStuff();
    }
}

void PlayEmeraldSound(NJS_VECTOR* pos, float distance)
{
    if (distance <= EmeraldDistMap[2].dist)
    {
        if (distance <= EmeraldDistMap[1].dist)
        {
            if (distance <= EmeraldDistMap[0].dist)
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
    
        if (SpatialSounds)
        {
            Play3DSound_Pos(0x1009, pos, 0, 0, 25);
        }
        else
        {
            PlaySoundProbably(0x1009, 0, 0, 0);
        }
    }
}

void __cdecl EmeraldManager_Main_r(ObjectMaster* obj)
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
            
            if (!Simultaneous)
            {
                break;
            }
        }
    }

    TRAMPOLINE(EmeraldManager_Main)(obj);
}

void EggQuarters_Init_r()
{
    TRAMPOLINE(EggQuarters_Init)();
    LoadTextureList("STG26_RADER", &texlist_stg26rader);
}

void EggQuarters_Free_r()
{
    FreeTexList(&texlist_stg26rader);
    TRAMPOLINE(EggQuarters_Free)();
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
            BeetleRadar = configgrp->getBool("BeetleRadar", BeetleRadar);
        }

        delete config;

        if (Simultaneous || NewColors)
        {
            WriteJump(EmeraldRadar_Update, EmeraldRadar_Display_r);
        }
        
        if (Simultaneous || SpatialSounds)
        {
            EmeraldManager_Main_t = new Trampoline(0x739570, 0x739576, EmeraldManager_Main_r, false);
            WriteData<5>((void*)0x739E0D, 0x90);
            WriteData<5>((void*)0x739DF2, 0x90);
        }

        if (NoAlertIcon)
        {
            WriteData((uint8_t*)0x73A450, 0xC3ui8);
        }

        if (BeetleRadar)
        {
            EggQuarters_Init_t = new Trampoline(0x68F8A0, 0x68F8A5, EggQuarters_Init_r, false);
            EggQuarters_Free_t = new Trampoline(0x68FEE0, 0x68FEE5, EggQuarters_Free_r, false);
            helperFunctions.ReplaceFile("resource\\gd_PC\\Message\\mh0026e.prs", "resource\\gd_PC\\Message\\mh0026e_BR.prs");
            helperFunctions.ReplaceFile("resource\\gd_PC\\Message\\mh0026f.prs", "resource\\gd_PC\\Message\\mh0026f_BR.prs");
            helperFunctions.ReplaceFile("resource\\gd_PC\\Message\\mh0026g.prs", "resource\\gd_PC\\Message\\mh0026g_BR.prs");
            helperFunctions.ReplaceFile("resource\\gd_PC\\Message\\mh0026i.prs", "resource\\gd_PC\\Message\\mh0026i_BR.prs");
            helperFunctions.ReplaceFile("resource\\gd_PC\\Message\\mh0026j.prs", "resource\\gd_PC\\Message\\mh0026j_BR.prs");
            helperFunctions.ReplaceFile("resource\\gd_PC\\Message\\mh0026s.prs", "resource\\gd_PC\\Message\\mh0026s_BR.prs");
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