#pragma once

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

//void __usercall njDrawTexture(int count@<eax>, int gbix@<edx>, RectangleInfo *rect@<ecx>, int alpha)
static const void* const njDrawTexture_ptr = (void*)0x0077F510;
static inline void njDrawTexture(NJS_TEXTURE_VTX* polygon, Int count, Int tex, Int flag)
{
    __asm
    {
        push[flag]
        mov edx, tex
        mov eax, count
        mov ecx, polygon
        call njDrawTexture_ptr
        add esp, 4
    }
}

#define TRAMPOLINE(name) ((decltype(name##_r)*)name##_t->Target())
VoidFunc(sub_458EA0, 0x458EA0);
VoidFunc(sub_458B90, 0x458B90);
VoidFunc(DrawEmeraldMark, 0x73A450);
VoidFunc(SetDefaultRenderStuff, 0x458EE0);
DataPointer(float, flt_1AF015C, 0x1AF015C);
DataPointer(float, flt_1AF0160, 0x1AF0160);
DataPointer(float, flt_1AF0164, 0x1AF0164);