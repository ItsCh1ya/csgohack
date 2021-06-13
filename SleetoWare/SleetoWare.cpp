

#include <iostream>
#include "Memory.h"
#include "offsets.h"

using namespace hazedumper;
using namespace netvars;
using namespace signatures;
PModule clientAddr;
memory mem;

typedef struct GlowObjectDefinition_t {
    float r;
    float g;
    float b;
    float a;
    uint8_t unk1[16];
    bool m_bRenderWhenOccluded;
    bool m_bRenderWhenUnoccluded;
    bool m_bFullBloom;
    int GlowStyle;
    uint8_t unk2[10];
} GlowObjectDefinition_t;

int triggerbot(DWORD LocalPlayer, DWORD LocalTeam, DWORD LocalHealth) {
    DWORD isTriggered = mem.Read<DWORD>(LocalPlayer + m_iCrosshairId);
    if (!isTriggered || isTriggered > 64 || !LocalHealth)
        return 0;
    DWORD EntityBase = mem.Read<int>(clientAddr.dwBase + dwEntityList + (isTriggered - 1) * 0x10);
    if (!EntityBase)
        return 0;
    DWORD EntityHp = mem.Read<DWORD>(EntityBase + m_iHealth);
    DWORD EntityTeam = mem.Read<DWORD>(EntityBase + m_iTeamNum);
    if (!EntityHp || EntityTeam == LocalTeam)
        return 0;
    Sleep(5);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    Sleep(15);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    return 0;
}

int glowesp(DWORD LocalPlayer, DWORD LocalTeam, DWORD LocalHealth) {
    int glowArray = mem.Read<int>(clientAddr.dwBase + dwGlowObjectManager);
    int glowCount = mem.Read<int>(clientAddr.dwBase + dwGlowObjectManager + 0x4);

    for (int i = 0; i < glowCount; i++) {
        DWORD gEntity = mem.Read<DWORD>(glowArray + 0x38 * i);
        if (!gEntity)
            continue;
        int entHp = mem.Read<int>(gEntity + m_iHealth);
        if (!entHp)
            continue;

        int enTeam = mem.Read<int>(gEntity + m_iTeamNum);

        if (enTeam == LocalTeam)
            continue;
        static GlowObjectDefinition_t Glow = mem.Read<GlowObjectDefinition_t>(glowArray + (i * 0x38) + 0x4);
        if (enTeam == 3)
        {
            Glow.r = 0.447058827f;
            Glow.g = 0.607843161f;
            Glow.b = 0.8666667f;
            Glow.a = 0.5f;
            Glow.m_bRenderWhenOccluded = true;
            Glow.m_bRenderWhenUnoccluded = false;
        }

        if (enTeam == 2)
        {
            Glow.r = 0.8784314f;
            Glow.g = 0.6862745f;
            Glow.b = 0.3372549f;
            Glow.a = 0.5f;
            Glow.m_bRenderWhenOccluded = true;
            Glow.m_bRenderWhenUnoccluded = false;
        }
        mem.Write<GlowObjectDefinition_t>(glowArray + (i * 0x38) + 0x4, Glow);
    }
    return 0;

}

int main()
{
    while (!mem.Attach("csgo.exe", PROCESS_ALL_ACCESS)) {}
    clientAddr = mem.GetModule("client.dll");
    std::cout << "Initialized";
    while (true) {
        system("cls");
        DWORD LocalPlayer = mem.Read<DWORD>(clientAddr.dwBase + dwLocalPlayer);
        DWORD LocalTeam = mem.Read<DWORD>(LocalPlayer + m_iTeamNum);
        DWORD LocalHealth = mem.Read<DWORD>(LocalPlayer + m_iHealth);
        if (GetAsyncKeyState(VK_XBUTTON2))
        {
            triggerbot(LocalPlayer, LocalTeam, LocalHealth);
        }
        glowesp(LocalPlayer, LocalTeam, LocalHealth);

    }
    return 0;
}

